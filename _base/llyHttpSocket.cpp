#include "llyHttpSocket.h"
#include "json\stringbuffer.h"
#include "json\writer.h"
using namespace lly;

//统一两个系统的方法
#ifdef WIN32
#define strcasecmp _stricmp
#endif

//统一两种系统中的sleep函数
inline void sleep(int n)
{
#ifdef WIN32
	::Sleep(n);
#else
	::usleep(n*1000);
#endif
}

const char* lly::HttpSocket::getErrorMsg( Error e )
{
	switch (e)
	{
	case lly::HttpSocket::Error::OK:
		return "OK";
	case lly::HttpSocket::Error::EMPTY:
		return "EMPTY";
	case lly::HttpSocket::Error::CHANGE_SERVER:
		return "CHANGE_SERVER";
	case lly::HttpSocket::Error::FILE_EXIST:
		return "FILE_EXIST";
	case lly::HttpSocket::Error::UNKNOWN:
		return "UNKNOWN";
	case lly::HttpSocket::Error::CONNECT_FAIL:
		return "CONNECT_FAIL";
	case lly::HttpSocket::Error::TIMEOUT:
		return "TIMEOUT";
	case lly::HttpSocket::Error::SEND_FAIL:
		return "SEND_FAIL";
	case lly::HttpSocket::Error::RECV_FAIL:
		return "RECV_FAIL";
	default:
		return "unknown error";
	}
}

lly::HttpSocket::HttpSocket() :
	lly::Socket(),
	m_nTimeout(30),
	m_bKeepAlive(true),
	m_lenPreReadBuf(0),
	m_pPreReadBuf(nullptr)
{

}

lly::HttpSocket::~HttpSocket()
{
	if (m_pPreReadBuf != nullptr) free(m_pPreReadBuf);
}

bool lly::HttpSocket::sendRequest( std::shared_ptr<HttpRequest> pReq )
{
	m_ErrorCode = HttpSocket::Error::UNKNOWN;

	if (strcmp(m_strIP.c_str(), pReq->m_strServerIP.c_str()) != 0 || 
		m_nPort != pReq->m_nServerPort || 
		m_hSocket == INVALID_SOCKET)
	{
		close();

		m_strIP = pReq->m_strServerIP;
		m_nPort = pReq->m_nServerPort;

		if (!create(SOCK_STREAM) || !connect(m_strIP.c_str(), m_nPort))
		{
			//CCLog("ip %s, port %d, is connect no", m_strServer.c_str(), (int)m_nPort);
			m_ErrorCode = HttpSocket::Error::CONNECT_FAIL;
			return false;
		}

		lly::Socket::setTimeout(m_nTimeout);
	}

	std::string strHttpSend; //用http发送的数据流
	if (!pReq->geneData(strHttpSend, pReq->m_eRequestMethod)) return false;

	int nRet = send(strHttpSend.c_str(), strHttpSend.size());
	
	if (nRet == strHttpSend.size())
	{
		return true;
	}
	else
	{
		m_ErrorCode = HttpSocket::Error::SEND_FAIL;
		close();
		return false;
	}
}

bool lly::HttpSocket::sendResponse( std::shared_ptr<HttpResponse> pAck )
{
	m_ErrorCode = HttpSocket::Error::UNKNOWN;

	std::string strHttpSend; //用http发送的数据流
	if (!pAck->geneData(strHttpSend)) return false;

	int nRet = send(strHttpSend.c_str(), strHttpSend.size());
	if (nRet == strHttpSend.size())
	{
		return true;
	}
	else
	{
		m_ErrorCode = HttpSocket::Error::SEND_FAIL;
		close();
		return false;
	}	
}

std::shared_ptr<HttpRequest> lly::HttpSocket::recvRequest()
{
	int lenBuf;
	char* pBuf = recvHttpMsg(lenBuf);
	if (pBuf == nullptr) return nullptr;

	auto pReq = new HttpRequest();
	if (pReq == nullptr)
	{
		free(pBuf);
		return nullptr;
	}

	pReq->analyzeData(pBuf, lenBuf);
	free(pBuf);

	return std::shared_ptr<HttpRequest>(pReq);
}

std::shared_ptr<HttpResponse> lly::HttpSocket::recvResponse()
{
	int lenBuf;
	
	char* pBuf = recvHttpMsg(lenBuf);
	
	if (pBuf == nullptr) return nullptr;

	HttpResponse* pAck=new HttpResponse();
	if (pAck == nullptr)
	{
		free(pBuf);
		return nullptr;
	}

	pAck->analyzeData(pBuf, lenBuf);
	free(pBuf);

	return std::shared_ptr<HttpResponse>(pAck);
}

int lly::HttpSocket::close()
{
	m_lenPreReadBuf = 0;
	return lly::Socket::close();
}

char* lly::HttpSocket::recvHttpMsg( int &out_lenMsg )
{
	m_ErrorCode = Error::UNKNOWN;

	out_lenMsg = 0;
	int lenMsgRecv = 0;
	int lenMsgAlloc = 0;
	char* pMsg = nullptr;

	if (m_lenPreReadBuf)
	{
		lenMsgRecv = m_lenPreReadBuf;
		lenMsgAlloc = m_lenPreReadBuf;
		pMsg = m_pPreReadBuf;

		m_lenPreReadBuf = 0;
		m_pPreReadBuf = nullptr;
	}

	time_t timeStart = time(nullptr);

	while (true)
	{
		if (out_lenMsg == 0 && lenMsgRecv)
		{
			pMsg[lenMsgRecv] = 0;

			char* s = strstr(pMsg, "\r\n\r\n");
			if (s!=NULL)
			{
				char *ss=strstr(pMsg,"Content-Length:");
				if (ss!=NULL)
				{
					lenMsg=((s+4)-pMsg)+atoi(ss+15);
					if (lenMsg>MAXLEN_HTTPDATA) break;
				}
			}
		}

		if (out_lenMsg && lenMsgRecv >= out_lenMsg)
		{
			m_nErrorCode=ERROR_HTTP_OK;
			break;
		}

		int len = getReadableSize();

		if (len < 0) //错误
		{
			m_ErrorCode = Error::RECV_FAIL;
			break;
		}
		else if (len == 0) //未收到信息
		{
			if (time(nullptr) - timeStart > m_nTimeout)
			{
				m_ErrorCode = Error::TIMEOUT;
				break;
			}
			else //没有超时，重新读取
			{
				sleep(10);
				continue;
			}		
		}

		//末尾添加一个0
		int lenNeed = lenMsgRecv + len + 1;
		if (lenNeed > lenMsgAlloc)
		{
			char* pTmp = (char*)realloc(pMsg, lenNeed);
			if (pTmp == nullptr)
			{
				m_ErrorCode = Error::OUT_OF_MEMORY;
				break;
			}

			pMsg = pTmp;
			lenMsgAlloc = lenNeed;
		}
		int nRet = recv(pMsg + lenMsgRecv, len);
		if (nRet < 0)
		{
			m_ErrorCode = Error::RECV_FAIL;
			break;
		}

		lenMsgRecv += nRet;

		if (lenMsgRecv > (int)Data::MAXLEN) break;
	}

	if (m_nErrorCode==ERROR_HTTP_OK && lenMsg<lenMsgRecv)
	{
		int lenMore=lenMsgRecv-lenMsg;
		char *pTmp=(char *)realloc(m_pPreReadBuf,lenMore+1);
		if (pTmp==NULL)
		{
			m_nErrorCode=ERROR_GENERAL_OUTOFMEMORY;
		}
		else
		{
			m_pPreReadBuf=pTmp;
			m_lenPreReadBuf=lenMore;
			memmove(m_pPreReadBuf,pMsg+lenMsg,lenMore);
		}
	}
	if (m_nErrorCode!=ERROR_HTTP_OK)
	{
		Close();
		lenMsg=0;
		free(pMsg);
		return NULL;
	}
	pMsg[lenMsg]=0;
	return pMsg;
}

//==========================================

lly::HttpDataBase::HttpDataBase() :
	m_pJson(nullptr),
	m_lenExtraData(0),
	m_lenExtraDataAlloc(0),
	m_pExtraData(nullptr),
	m_bKeepAlive(false),
	m_ErrorCode(HttpSocket::Error::OK),
	m_pUserData(nullptr)
{

}

lly::HttpDataBase::~HttpDataBase()
{
	if (m_pJson != nullptr) delete m_pJson;
	if (m_pExtraData != nullptr) free(m_pExtraData);
}

void lly::HttpDataBase::setJson( rapidjson::Document* pJson )
{
	if (m_pJson != nullptr) delete m_pJson;
	m_pJson = pJson;
}

rapidjson::Document* lly::HttpDataBase::getJson()
{
	if (m_pJson == nullptr)
	{
		//如果没有就要生成一个空的
		m_pJson = new rapidjson::Document();
	}
	return m_pJson;
}

bool lly::HttpDataBase::setExtraData( const char* pExtraData, int lenExtraData )
{
	m_lenExtraData = 0;
	return addExtraData(pExtraData, lenExtraData);
}

bool lly::HttpDataBase::addExtraData( const char* pExtraData, int lenExtraData )
{
	int lenNeed = m_lenExtraData + lenExtraData;
	if (lenNeed > m_lenExtraDataAlloc)
	{
		char* pBuf = (char*)realloc(m_pExtraData, lenNeed);
		if (pBuf == nullptr) return false;

		m_pExtraData = pBuf;
		m_lenExtraDataAlloc = lenNeed;
	}
	memmove(m_pExtraData + m_lenExtraData, pExtraData, lenExtraData);
	m_lenExtraData += lenExtraData;
	return true;
}

char* lly::HttpDataBase::getExtraData( int &out_lenExtraData )
{
	out_lenExtraData = m_lenExtraData;
	return m_pExtraData;
}

bool lly::HttpDataBase::getJsonString( rapidjson::Document* json, std::string &out_strJson, int out_lenJsonStr )
{
	if (json == nullptr || json->IsNull()) return false;

	std::string strJson;
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	json->Accept(writer);

	out_strJson = buffer.GetString();
	out_lenJsonStr = out_strJson.size() + 1; //json后面会加一个0，用于在读取时停止

	return true;
}

//===========================================

lly::HttpRequest::HttpRequest() :
	HttpDataBase(),
	m_nServerPort(80),
	m_eRequestMethod(lly::HttpSocket::RequestMethod::GET),
	m_bWaitResponse(true)	
{

}

lly::HttpRequest::~HttpRequest()
{
	clearParam();
}

std::unique_ptr<HttpRequest> lly::HttpRequest::createUniquePointer()
{
	return std::unique_ptr<HttpRequest>(new HttpRequest());
}

bool lly::HttpRequest::analyzeData( char* pData, int len )
{
	//0.属性还原默认值
	clear();

	//1.方法
	m_eRequestMethod = HttpSocket::RequestMethod::UNKNOWN;
	if (memcmp(pData, "GET ", 4) == 0)
	{
		pData+=4;
		m_eRequestMethod = HttpSocket::RequestMethod::GET;
	}
	else if (memcmp(pData, "POST ", 5) == 0)
	{
		pData += 5;
		m_eRequestMethod = HttpSocket::RequestMethod::POST;
	}

	//只支持get和post
	if ( m_eRequestMethod == HttpSocket::RequestMethod::UNKNOWN) return false;

	while (*pData == ' ') ++pData; //去掉空格
	if (*pData == '/') ++pData; //去掉中间的/ 例如 GET /index.html?name=xiao&age=14 HTTP1.1\r\n
	if (*pData == '\0') return false;

	//2.url 获得index.html
	char* pszCmd = pData; //获得数据当前位置的指针
	char ch; //临时字符
	while (true)
	{
		ch = *pData;
		if (ch == ' ' || ch == '?' || ch == 0x0a || ch == 0x0d) break; //0x0a = \r 0x0d = \n
		if (ch == '\0') return false;
		++pData;
	}
	
	*pData = '\0'; //把此处的字符设为\0，则pszCmd就读到此处
	m_strURL = pszCmd;

	++pData;

	if (ch == ' ') //如果是空格，则找到下个回车或者？，设为\0并跳过
	{
		while (true)
		{
			ch = *pData;
			if (ch == '?' || ch == 0x0a || ch == 0x0d) break;
			if (ch == 0) return false;
			++pData;
		}
	
		*pData = '\0';
		++pData;
	}

	//3.url后跟的参数 ?name=xiao&age=14
	if (ch == '?') //是？则读取参数
	{
		while (true)
		{
			char* pszParamName = pData;
			while (true)
			{
				ch = *pData;
				if (ch == '=') break;
				if ( ch == '\0' ) return false;
				++pData;
			}

			*pData = '\0'; //把此处的字符设为\0，则pszParamName就读到此处
			++pData;

			char* pszParamValue = pData;
			while (true)
			{
				ch = *pData;
				if (ch == ' ' || ch == '&' || ch == 0x0a || ch == 0x0d) break;
				if ( ch == '\0' ) return false;
				++pData;
			}
			
			*pData = '\0'; //把此处的字符设为\0，则pszParamValue就读到此处
			++pData;

			if (ch==' ') //如果是空格，则找到下个回车或者&，并跳过
			{
				while (true)
				{
					ch = *pData;
					if ( ch == '&' || ch == 0x0a || ch == 0x0d) break;
					if (ch == 0) return false;
					++pData;
				}				
				++pData;
			}

			if (strcasecmp(pszParamName, "Token") == 0)
			{
				//token参数，特殊处理
				m_strToken = pszParamValue;
			}
			else
			{
				//其他参数
				auto pParam=new struHttpRequsetInfo();
				pParam->strName = pszParamName;
				pParam->strValue = pszParamValue;
				m_listParam.push_back(pParam);
			}

			//没有参数了
			if (ch != '&') break;
		}
	}

	if (ch == 0x0a) ++pData; //一般跟着的就是\n

	//此时指针在第一行\n的后面，也就是第二行的开头

	//4.请求头部  Host: %s\r\nAccept: */*\r\n。。。。。。
	int lenJsonTemp = 0; //临时json长度
	int lenContentTemp = 0; //临时额外内容长度
	while (true)
	{
		//检测是否是一个空行，空行表示请求头部结束
		if (*pData == 0x0a) ++pData; //一般跟着的就是\n
		if (*pData == 0x0d)
		{
			++pData;
			break;
		}
		
		//请求键
		char *pszKey = pData;
		while (*pData != '\0' && *pData != ':') ++pData;
		if (*pData == '\0') return false;
		*pData = '\0'; //把此处的字符设为\0，则pszKey就读到此处
		++pData;

		while (*pData == ' ') ++pData; //去空格
		if (*pData == '\0') return false;

		//请求值
		char* pszValue = pData;

		while (*pData != '\0' && *pData != 0x0a || *pData != 0x0d) ++pData;
		if (*pData == '\0') return false;
		*pData = '\0'; //把此处的字符设为\0，则pszKey就读到此处
		if (*pData == 0x0a) //同时跳过后面的\n
		{
			++pData;
			++pData;
		}
		else
		{
			++pData;
		}

		if (strcasecmp(pszKey, "Connection") == 0) //长链接
		{			
			m_bKeepAlive = (strcasecmp(pszValue,"Keep-Alive") == 0);
		}
		else if (strcasecmp(pszKey, "Json-Length") == 0) //json内容长度
		{		
			sscanf_s(pszValue, "%d", &lenJsonTemp); //转数字
		}
		else if (strcasecmp(pszKey, "Content-Length") == 0) //附带内容长度
		{			
			sscanf_s(pszValue, "%d", &lenContentTemp); //转数字
			if (lenContentTemp > (int)HttpSocket::Data::MAXLEN) return false;
		}	
		else if (strcasecmp(pszKey, "Token") == 0)
		{
			m_strToken = pszValue;
		}
		else if (strcasecmp(pszKey, "WaitResponse") == 0)
		{
			int nWait;
			sscanf_s(pszValue, "%d", &nWait); //转数字
			m_bWaitResponse = nWait ? true : false;
		}
	}

	//5.请求内容
	if (lenJsonTemp)
	{
		//生成json
		if(m_pJson) delete m_pJson;
		m_pJson = new rapidjson::Document();
		m_pJson->Parse<0>(pData);
		if (m_pJson->HasParseError()) return false;
		pData += lenJsonTemp;
	}

	if (lenContentTemp > lenJsonTemp)//其他附加数据
	{
		setExtraData(pData, lenContentTemp - lenJsonTemp);
	}

	return true;
}

void lly::HttpRequest::setServer( const char* pszServerIP, int nPort )
{
	m_strServerIP = pszServerIP;
	m_nServerPort = nPort;
}

void lly::HttpRequest::setRequestMethod( HttpSocket::RequestMethod eMethod )
{
	m_eRequestMethod = eMethod;
}

bool lly::HttpRequest::setURL( const char *pszURL )
{
	std::string str = pszURL;
	
	//转换为小写
	char *s=(char *)str.c_str();
	for (int i = 0; i < (int)str.size(); ++i)
	{
		*s=tolower(*s);
		s++;
	}

	int nURLPos = 0; //url指针位置
	int nPosFind = 0;

	//去掉http://
	nPosFind = str.find("http://");
	if (nPosFind != std::string::npos)
	{		
		nURLPos += 7;
	}

	//找到域名结束地址
	nPosFind = str.find('/', nURLPos);
	if (nPosFind != std::string::npos && nPosFind > 0)
	{
		//服务器地址，大小写不敏感
		m_strServerIP = str.substr(nURLPos, nPosFind);
		m_nServerPort = 80;

		//看是否有端口指定
		int nSubFind = m_strServerIP.find(':');
		if (nSubFind != std::string::npos)
		{
			sscanf_s(m_strServerIP.substr(nSubFind + 1).c_str(), "%d", &m_nServerPort);
			m_strServerIP.erase(nSubFind, m_strServerIP.size());
		}
		nURLPos == nPosFind;
	}

	m_strURL = pszURL + nURLPos;
	return true;
}

void lly::HttpRequest::addParam( const char* pszName, const char* pszValue )
{
	auto pParam=new struHttpRequsetInfo();
	pParam->strName = pszName;
	pParam->strValue = pszValue;
	m_listParam.push_back(pParam);
}

const char* lly::HttpRequest::findParamStr( const char* pszName )
{
	for (uint32_t i = 0; i < m_listParam.size(); ++i)
	{
		auto pParam=m_listParam.at(i);
		if (strcasecmp(pParam->strName.c_str(), pszName) == 0) return pParam->strValue.c_str();
	}
	return nullptr;
}

int lly::HttpRequest::findParamInt( const char* pszName, int nDefault )
{
	const char *pParam=findParamStr(pszName);
	if (pParam == nullptr) return nDefault;
	int nResult;
	sscanf_s(pParam, "%d", &nResult);
	return nResult;
}

bool lly::HttpRequest::geneData( std::string &str, HttpSocket::RequestMethod eMethod )
{
	if (eMethod == HttpSocket::RequestMethod::UNKNOWN) eMethod = m_eRequestMethod;

	char* pszMethod;
	if (eMethod == HttpSocket::RequestMethod::GET) pszMethod = "GET ";
	else if (eMethod == HttpSocket::RequestMethod::POST) pszMethod = "POST ";
	else return false;

	//获取json以及长度		
	std::string strJson;
	int lenJson = 0;
	getJsonString(m_pJson, strJson, lenJson);
	
	if (m_listParam.size() > 0)
	{
		//第一个参数以?开始，后面的以&分隔
		char ch = '?';

		//url中有？说明已经不是第一个参数，所以改成&
		if (m_strURL.find("?") != std::string::npos) ch = '&'; 

		for (uint32_t i = 0; i < m_listParam.size(); ++i)
		{
			auto pParam = m_listParam.at(i);
			m_strURL += ch;
			m_strURL += pParam->strName;
			m_strURL += '=';
			m_strURL += pParam->strValue;

			//后续参数均以&分隔
			ch = '&';
		}
	}

	char szLine[1024];
	sprintf(szLine, 
		http_request_data_packet,
		pszMethod, m_strURL.c_str(), 
		m_strServerIP.c_str(), 
		m_bKeepAlive ? "Keep-Alive" : "Close",
		lenJson,
		m_lenExtraData + lenJson);
	str = szLine;

	if (!m_strToken.empty())
	{
		sprintf(szLine, "Token:%s\r\n", m_strToken.c_str());
		str += szLine;
	}

	if (!m_bWaitResponse) //默认等待回复，因此只有不用等待回复时才添加这条信息
	{
		str += "WaitResponse:0\r\n";
	}

	str += "\r\n"; //空行 后面是数据

	if (lenJson)
	{
		str += strJson;
		//json后面一定要加一个0，用于在读取时停止
		str.append("\0", 1);
	}

	if (m_lenExtraData)
	{
		str.append(m_pExtraData, m_lenExtraData);
	}

	return true;
}

void lly::HttpRequest::clear()
{
	m_ErrorCode = HttpSocket::Error::UNKNOWN;
	m_bKeepAlive = false;
	m_bWaitResponse = true;

	m_strToken.clear();
	clearParam();
	if (m_pJson != nullptr) delete m_pJson;
	m_lenExtraData = 0;
}

//========
void lly::HttpRequest::clearParam()
{
	for (uint32_t i = 0; i < m_listParam.size(); ++i)
	{
		delete m_listParam.at(i);
	}
}


//=====================================================

lly::HttpResponse::HttpResponse()
{

}

lly::HttpResponse::~HttpResponse()
{

}

std::unique_ptr<HttpResponse> lly::HttpResponse::createUniquePointer()
{
	return std::unique_ptr<HttpResponse>(new HttpResponse());
}

bool lly::HttpResponse::analyzeData( char* pData, int len )
{
	//第一行
	if (strncmp(pData, "HTTP/1.1",8) != 0) return false; //必须是这个开头

	pData += 8;

	//状态码
	int nCode;
	sscanf_s(pData, "%d", &nCode);
	m_ErrorCode = (HttpSocket::Error)nCode;

	//跳过本行后面所有内容到第二行
	while (*pData && *pData != 0x0d) ++pData;
	if (*pData==0) return false;
	++pData;

	//第二行
	m_bKeepAlive = false;

	int lenJson = 0; //临时json长度
	int lenContent = 0; //临时额外内容长度

	while (true)
	{
		//检测是否是一个空行，空行表示请求头部结束
		if (*pData == 0x0a) ++pData; //一般跟着的就是\n
		if (*pData == 0x0d)
		{
			++pData;
			break;
		}

		//请求键
		char* pszKey = pData;
		while (*pData != '\0' && *pData != ':') ++pData;
		if (*pData == '\0') return false;
		*pData = '\0'; //把此处的字符设为\0，则pszKey就读到此处
		++pData;

		while (*pData == ' ') ++pData; //去空格
		if (*pData == '\0') return false;

		//请求值
		char* pszValue = pData;

		while (*pData != '\0' && *pData != 0x0da || *pData != 0x0d) ++pData;
		if (*pData == '\0') return false;
		*pData = '\0'; //把此处的字符设为\0，则pszKey就读到此处
		if (*pData == 0x0a) //同时跳过后面的\n
		{
			++pData;
			++pData;
		}
		else
		{
			++pData;
		}

		if (strcasecmp(pszKey, "Connection") == 0)
		{
			//长链接
			m_bKeepAlive = (strcasecmp(pszValue, "Keep-Alive") == 0);
		}
		else if (strcasecmp(pszKey, "Json-Length")==0)
		{
			sscanf_s(pszValue, "%d", &lenJson); //转数字
		}
		else if (strcasecmp(pszKey, "Content-Length") == 0) //附带内容
		{
			sscanf_s(pszValue, "%d", &lenContent); //转数字
			if (lenContent > (int)HttpSocket::Data::MAXLEN) return false;
		}
		else if (strcasecmp(pszKey, "Token") == 0)
		{
			m_strToken = pszValue;
		}
	}
	
	if (lenJson)
	{
		//生成json
		if(m_pJson) delete m_pJson;
		m_pJson = new rapidjson::Document();
		m_pJson->Parse<0>(pData);
		if (m_pJson->HasParseError()) return false;
		pData += lenJson;
	}

	if (lenContent > lenJson)//其他附加数据
	{
		setExtraData(pData, lenContent - lenJson);
	}

	return true;
}

bool lly::HttpResponse::geneData( std::string &str )
{
	//获取json以及长度
	std::string strJson;
	int lenJson = 0;
	getJsonString(m_pJson, strJson, lenJson);

	char szLine[256];
	sprintf(szLine,
		http_response_data_packet,
		(int)m_ErrorCode, 
		HttpSocket::getErrorMsg(m_ErrorCode),
		m_bKeepAlive ? "Keep-Alive" : "Close",
		lenJson,
		lenJson + m_lenExtraData,);
	str = szLine;

	if (!m_strToken.empty())
	{
		sprintf(szLine, "Token:%s\r\n", m_strToken.c_str());
		str += szLine;
	}

	str+="\r\n"; //空行 后面是数据

	if (lenJson)
	{
		str += strJson;
		str.append("\0",1);
	}

	if (m_lenExtraData)
	{
		str.append(m_pExtraData, m_lenExtraData);
	}

	return true;
}

void lly::HttpResponse::clear()
{
	if (m_pJson != nullptr) delete m_pJson;
	m_lenExtraData = 0;
}




