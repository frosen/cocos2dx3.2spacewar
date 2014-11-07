#include "llyHttpSocket.h"
#include "llyHttpErrorCode.h"

#include "json\stringbuffer.h"
#include "json\writer.h"

#include <regex>

using namespace lly;

//ͳһ����ϵͳ�ķ���
#ifdef WIN32
#define strcasecmp _stricmp
#endif

//ͳһ����ϵͳ�е�sleep����
inline void sleep(int n)
{
#ifdef WIN32
	::Sleep(n);
#else
	::usleep(n*1000);
#endif
}

lly::HttpSocket::HttpSocket() :
	lly::Socket(),
	m_nTimeoutMS(30000),
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
	m_ErrorCode = HTTP_ERROR_UNKNOWN;

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
			m_ErrorCode = HTTP_ERROR_CONNECT_FAIL;
			return false;
		}

		lly::Socket::setTimeout(m_nTimeoutMS);
	}

	std::string strHttpSend; //��http���͵�������
	if (!pReq->geneData(strHttpSend, pReq->m_eRequestMethod)) return false;

	int nRet = send(strHttpSend.c_str(), strHttpSend.size());
	
	if (nRet == strHttpSend.size())
	{
		return true;
	}
	else
	{
		m_ErrorCode = HTTP_ERROR_SEND_FAIL;
		close();
		return false;
	}
}

bool lly::HttpSocket::sendResponse( std::shared_ptr<HttpResponse> pAck )
{
	m_ErrorCode = HTTP_ERROR_UNKNOWN;

	std::string strHttpSend; //��http���͵�������
	if (!pAck->geneData(strHttpSend)) return false;

	int nRet = send(strHttpSend.c_str(), strHttpSend.size());
	if (nRet == strHttpSend.size())
	{
		return true;
	}
	else
	{
		m_ErrorCode = HTTP_ERROR_SEND_FAIL;
		close();
		return false;
	}	
}

std::shared_ptr<HttpRequest> lly::HttpSocket::recvRequest()
{
	int lenBuf;
	char* pBuf = recvHttpMsg_needfree(lenBuf);
	if (pBuf == nullptr) return nullptr;

	auto pReq = new HttpRequest();
	if (pReq == nullptr)
	{
		free(pBuf);
		return nullptr;
	}
	
	pReq->analyzeData(pBuf, lenBuf);
	free(pBuf);

	if (m_ErrorCode == HTTP_ERROR_OK) //�����޴���Ͷ���request�Ĵ���
	{
		m_ErrorCode = pReq->getErrorCode();
	}

	return std::shared_ptr<HttpRequest>(pReq);
}

std::shared_ptr<HttpResponse> lly::HttpSocket::recvResponse()
{
	int lenBuf;
	
	char* pBuf = recvHttpMsg_needfree(lenBuf);
	
	if (pBuf == nullptr) return nullptr;

	HttpResponse* pAck=new HttpResponse();
	if (pAck == nullptr)
	{
		free(pBuf);
		return nullptr;
	}

	pAck->analyzeData(pBuf, lenBuf);
	free(pBuf);

	if (m_ErrorCode == HTTP_ERROR_OK) //�����޴���Ͷ���Response�Ĵ���
	{
		m_ErrorCode = pAck->getErrorCode();
	}

	return std::shared_ptr<HttpResponse>(pAck);
}

int lly::HttpSocket::close()
{
	m_lenPreReadBuf = 0;
	return lly::Socket::close();
}

char* lly::HttpSocket::recvHttpMsg_needfree( int &out_lenMsg ) 
{
	m_ErrorCode = HTTP_ERROR_UNKNOWN;

	out_lenMsg = 0;
	int lenMsgRecv = 0;
	int lenMsgAlloc = 0;
	char* pMsg = nullptr;

	if (m_lenPreReadBuf) //����û�������Ϣ�������ȶ�ȡ
	{
		lenMsgRecv = m_lenPreReadBuf;
		lenMsgAlloc = m_lenPreReadBuf;
		pMsg = m_pPreReadBuf;

		m_lenPreReadBuf = 0;
		m_pPreReadBuf = nullptr;
	}

	time_t timeStartMS = time(nullptr);

	while (true)
	{
		if (out_lenMsg == 0 && lenMsgRecv)
		{
			pMsg[lenMsgRecv] = '\0';

			int lenChar1 = 4; //\r\n\r\n
			int lenChar2 = 15; //Content-Length:

			char* sPos = strstr(pMsg, "\r\n\r\n"); //��õڶ����ַ����ڵ�һ���е�λ��ָ��
			if (sPos == nullptr)
			{
				sPos = strstr(pMsg, "\n\n");
				lenChar1 = 2; //\n\n
			}

			if (sPos != nullptr) //˵����һ������
			{
				char* sPos2 = strstr(pMsg, "Content-Length:");
				if (sPos2 != nullptr)
				{
					//����ͷ���ĳ��ȣ�����ָ������ó�
					int lenHeadContents = sPos + lenChar1 - pMsg; //����Ϊ

					//�������ݵĳ��ȣ���ͷ����¼�����ݵó�
					int lenBodyContents;
					sscanf_s(sPos2 + lenChar2, "%d", &lenBodyContents);

					out_lenMsg = lenHeadContents + lenBodyContents;

					if (out_lenMsg > (int)Data::MAXLEN) break;
				}
			}
		}

		if (0 < out_lenMsg && out_lenMsg <= lenMsgRecv) //?
		{
			m_ErrorCode = HTTP_ERROR_OK;
			break;
		}

		int len = getReadableSize();

		if (len < 0) //����
		{
			m_ErrorCode = HTTP_ERROR_RECV_FAIL;
			break;
		}
		else if (len == 0) //δ�յ���Ϣ
		{
			if (time(nullptr) - timeStartMS > m_nTimeoutMS)
			{
				m_ErrorCode = HTTP_ERROR_TIMEOUT;
				break;
			}
			else //û�г�ʱ�����¶�ȡ
			{
				sleep(10);
				continue;
			}		
		}

		//�յ�����Ϣ	
		int lenNeed = lenMsgRecv + len + 1; //ĩβ���һ��0
		if (lenNeed > lenMsgAlloc)
		{
			char* pTmp = (char*)realloc(pMsg, lenNeed);
			if (pTmp == nullptr)
			{
				m_ErrorCode = HTTP_ERROR_OUT_OF_MEMORY;
				break;
			}
			else
			{
				pMsg = pTmp;
				lenMsgAlloc = lenNeed;
			}		
		}

		int nRet = recv(pMsg + lenMsgRecv, len);
		if (nRet < 0)
		{
			m_ErrorCode = HTTP_ERROR_RECV_FAIL;
			break;
		}

		lenMsgRecv += nRet;

		if (lenMsgRecv > (int)Data::MAXLEN) break; //��Ϣ����

		//����ѭ��
	}

	if (m_ErrorCode != HTTP_ERROR_OK) //ʧ��
	{
		close();
		out_lenMsg = 0;
		free(pMsg);
		return nullptr;
	}
	else if (out_lenMsg < lenMsgRecv) //�ɹ������ǽ��յ�����Ϣ��һ��http��Ϣ����
	{
		int lenMore = lenMsgRecv - out_lenMsg;
		char* pTmp = (char*)realloc(m_pPreReadBuf, lenMore + 1);
		if (pTmp == nullptr)
		{
			m_ErrorCode = HTTP_ERROR_OUT_OF_MEMORY;
		}
		else
		{
			m_pPreReadBuf = pTmp;
			m_lenPreReadBuf = lenMore;
			memmove(m_pPreReadBuf, pMsg + out_lenMsg, lenMore);
		}
	}
	
	pMsg[out_lenMsg] = '\0';
	return pMsg;
}

//==========================================

lly::HttpDataBase::HttpDataBase() :
	m_strToken(""),
	m_pJson(nullptr),
	m_lenExtraData(0),
	m_lenExtraDataAlloc(0),
	m_pExtraData(nullptr),
	m_bKeepAlive(false),
	m_ErrorCode(HTTP_ERROR_OK),
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
		//���û�о�Ҫ����һ���յ�
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
	out_lenJsonStr = out_strJson.size() + 1; //json������һ��0�������ڶ�ȡʱֹͣ

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

std::shared_ptr<HttpRequest> lly::HttpRequest::createSharedPointer()
{
	return std::shared_ptr<HttpRequest>(new HttpRequest());
}

bool lly::HttpRequest::analyzeData( char* pData, int len )
{
	//0.���Ի�ԭĬ��ֵ
	clear();

	//1.����
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

	//ֻ֧��get��post
	if ( m_eRequestMethod == HttpSocket::RequestMethod::UNKNOWN) return false;

	while (*pData == ' ') ++pData; //ȥ���ո�
	if (*pData == '/') ++pData; //ȥ���м��/ ���� GET /index.html?name=xiao&age=14 HTTP1.1\r\n
	if (*pData == '\0') return false;

	//2.url ���index.html
	char* pszCmd = pData; //������ݵ�ǰλ�õ�ָ��
	char ch; //��ʱ�ַ�
	while (true)
	{
		ch = *pData;
		if (ch == ' ' || ch == '?' || ch == 0x0a || ch == 0x0d) break; //0x0a = \r 0x0d = \n
		if (ch == '\0') return false;
		++pData;
	}
	
	*pData = '\0'; //�Ѵ˴����ַ���Ϊ\0����pszCmd�Ͷ����˴�
	m_strURL = pszCmd;

	++pData;

	if (ch == ' ') //����ǿո����ҵ��¸��س����ߣ�����Ϊ\0������
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

	//3.url����Ĳ��� ?name=xiao&age=14
	if (ch == '?') //�ǣ����ȡ����
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

			*pData = '\0'; //�Ѵ˴����ַ���Ϊ\0����pszParamName�Ͷ����˴�
			++pData;

			char* pszParamValue = pData;
			while (true)
			{
				ch = *pData;
				if (ch == ' ' || ch == '&' || ch == 0x0a || ch == 0x0d) break;
				if ( ch == '\0' ) return false;
				++pData;
			}
			
			*pData = '\0'; //�Ѵ˴����ַ���Ϊ\0����pszParamValue�Ͷ����˴�
			++pData;

			if (ch==' ') //����ǿո����ҵ��¸��س�����&��������
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
				//token���������⴦��
				m_strToken = pszParamValue;
			}
			else
			{
				//��������
				auto pParam=new struHttpRequsetInfo();
				pParam->strName = pszParamName;
				pParam->strValue = pszParamValue;
				m_listParam.push_back(pParam);
			}

			//û�в�����
			if (ch != '&') break;
		}
	}

	if (ch == 0x0a) ++pData; //һ����ŵľ���\n

	//��ʱָ���ڵ�һ��\n�ĺ��棬Ҳ���ǵڶ��еĿ�ͷ

	//4.����ͷ��  Host: %s\r\nAccept: */*\r\n������������
	int lenJsonTemp = 0; //��ʱjson����
	int lenContentTemp = 0; //��ʱ�������ݳ���
	while (true)
	{
		//����Ƿ���һ�����У����б�ʾ����ͷ������
		if (*pData == 0x0a) ++pData; //һ����ŵľ���\n
		if (*pData == 0x0d)
		{
			++pData;
			break;
		}
		
		//�����
		char *pszKey = pData;
		while (*pData != '\0' && *pData != ':') ++pData;
		if (*pData == '\0') return false;
		*pData = '\0'; //�Ѵ˴����ַ���Ϊ\0����pszKey�Ͷ����˴�
		++pData;

		while (*pData == ' ') ++pData; //ȥ�ո�
		if (*pData == '\0') return false;

		//����ֵ
		char* pszValue = pData;

		while (*pData != '\0' && *pData != 0x0a || *pData != 0x0d) ++pData;
		if (*pData == '\0') return false;
		*pData = '\0'; //�Ѵ˴����ַ���Ϊ\0����pszKey�Ͷ����˴�
		if (*pData == 0x0a) //ͬʱ���������\n
		{
			++pData;
			++pData;
		}
		else
		{
			++pData;
		}

		if (strcasecmp(pszKey, "Connection") == 0) //������
		{			
			m_bKeepAlive = (strcasecmp(pszValue,"Keep-Alive") == 0);
		}
		else if (strcasecmp(pszKey, "Json-Length") == 0) //json���ݳ���
		{		
			sscanf_s(pszValue, "%d", &lenJsonTemp); //ת����
		}
		else if (strcasecmp(pszKey, "Content-Length") == 0) //�������ݳ���
		{			
			sscanf_s(pszValue, "%d", &lenContentTemp); //ת����
			if (lenContentTemp > (int)HttpSocket::Data::MAXLEN) return false;
		}	
		else if (strcasecmp(pszKey, "Token") == 0)
		{
			m_strToken = pszValue;
		}
		else if (strcasecmp(pszKey, "WaitResponse") == 0)
		{
			int nWait;
			sscanf_s(pszValue, "%d", &nWait); //ת����
			m_bWaitResponse = nWait ? true : false;
		}
	}

	//5.��������
	if (lenJsonTemp)
	{
		//����json
		if(m_pJson) delete m_pJson;
		m_pJson = new rapidjson::Document();
		m_pJson->Parse<0>(pData);
		if (m_pJson->HasParseError()) return false;
		pData += lenJsonTemp;
	}

	if (lenContentTemp > lenJsonTemp)//������������
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
	
	//ת��ΪСд
	char *s=(char *)str.c_str();
	for (int i = 0; i < (int)str.size(); ++i)
	{
		*s=tolower(*s);
		s++;
	}

	int nURLPos = 0; //urlָ��λ��
	int nPosFind = 0;

	//ȥ��http://
	nPosFind = str.find("http://");
	if (nPosFind != std::string::npos)
	{		
		nURLPos += 7;
	}

	//�ҵ�����������ַ
	nPosFind = str.find('/', nURLPos);
	if (nPosFind != std::string::npos && nPosFind > 0)
	{
		//��������ַ����Сд������
		std::string strServer = str.substr(nURLPos, nPosFind);
		
		//���������������ip��ַ����ת��
		if (!std::regex_match(strServer, std::regex("(\\d{1,3}).(\\d{1,3}).(\\d{1,3}).(\\d{1,3})")))
		{
			char ip[256] = {0};
			if (!Socket::parseDNS(strServer.c_str(), ip)) return false; //����ip�������򷵻�false
			m_strServerIP = ip;
		}
		else
		{
			m_strServerIP = strServer;
		}

		//���Ƿ��ж˿�ָ��
		int nSubFind = m_strServerIP.find(':');
		if (nSubFind != std::string::npos)
		{
			sscanf_s(m_strServerIP.substr(nSubFind + 1).c_str(), "%d", &m_nServerPort);
			m_strServerIP.erase(nSubFind, m_strServerIP.size());
		}
		else //����ʹ��Ĭ��
		{
			m_nServerPort = 80;
		}

		nURLPos = nPosFind;
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

	//��ȡjson�Լ�����		
	std::string strJson;
	int lenJson = 0;
	getJsonString(m_pJson, strJson, lenJson);
	
	if (m_listParam.size() > 0)
	{
		//��һ��������?��ʼ���������&�ָ�
		char ch = '?';

		//url���У�˵���Ѿ����ǵ�һ�����������Ըĳ�&
		if (m_strURL.find("?") != std::string::npos) ch = '&'; 

		for (uint32_t i = 0; i < m_listParam.size(); ++i)
		{
			auto pParam = m_listParam.at(i);
			m_strURL += ch;
			m_strURL += pParam->strName;
			m_strURL += '=';
			m_strURL += pParam->strValue;

			//������������&�ָ�
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

	if (!m_bWaitResponse) //Ĭ�ϵȴ��ظ������ֻ�в��õȴ��ظ�ʱ�����������Ϣ
	{
		str += "WaitResponse:0\r\n";
	}

	str += "\r\n"; //���� ����������

	if (lenJson)
	{
		str += strJson;
		//json����һ��Ҫ��һ��0�������ڶ�ȡʱֹͣ
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
	m_ErrorCode = HTTP_ERROR_UNKNOWN;
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

lly::HttpResponse::HttpResponse() : HttpDataBase()
{

}

lly::HttpResponse::~HttpResponse()
{

}

std::shared_ptr<HttpResponse> lly::HttpResponse::createSharedPointer()
{
	return std::shared_ptr<HttpResponse>(new HttpResponse());
}

bool lly::HttpResponse::analyzeData( char* pData, int len )
{
	//��һ��
	if (strncmp(pData, "HTTP/1.1",8) != 0) return false; //�����������ͷ

	pData += 8;

	//״̬��
	sscanf_s(pData, "%d", &m_ErrorCode);

	//�������к����������ݵ��ڶ���
	while (*pData && *pData != 0x0d) ++pData;
	if (*pData==0) return false;
	++pData;

	//�ڶ���
	m_bKeepAlive = false;

	int lenJson = 0; //��ʱjson����
	int lenContent = 0; //��ʱ�������ݳ���

	while (true)
	{
		//����Ƿ���һ�����У����б�ʾ����ͷ������
		if (*pData == 0x0a) ++pData; //һ����ŵľ���\n
		if (*pData == 0x0d)
		{
			++pData;
			break;
		}

		//�����
		char* pszKey = pData;
		while (*pData != '\0' && *pData != ':') ++pData;
		if (*pData == '\0') return false;
		*pData = '\0'; //�Ѵ˴����ַ���Ϊ\0����pszKey�Ͷ����˴�
		++pData;

		while (*pData == ' ') ++pData; //ȥ�ո�
		if (*pData == '\0') return false;

		//����ֵ
		char* pszValue = pData;

		while (*pData != '\0' && *pData != 0x0da || *pData != 0x0d) ++pData;
		if (*pData == '\0') return false;
		*pData = '\0'; //�Ѵ˴����ַ���Ϊ\0����pszKey�Ͷ����˴�
		if (*pData == 0x0a) //ͬʱ���������\n
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
			//������
			m_bKeepAlive = (strcasecmp(pszValue, "Keep-Alive") == 0);
		}
		else if (strcasecmp(pszKey, "Json-Length")==0)
		{
			sscanf_s(pszValue, "%d", &lenJson); //ת����
		}
		else if (strcasecmp(pszKey, "Content-Length") == 0) //��������
		{
			sscanf_s(pszValue, "%d", &lenContent); //ת����
			if (lenContent > (int)HttpSocket::Data::MAXLEN) return false;
		}
		else if (strcasecmp(pszKey, "Token") == 0)
		{
			m_strToken = pszValue;
		}
	}
	
	if (lenJson)
	{
		//����json
		if(m_pJson) delete m_pJson;
		m_pJson = new rapidjson::Document();
		m_pJson->Parse<0>(pData);
		if (m_pJson->HasParseError()) return false;
		pData += lenJson;
	}

	if (lenContent > lenJson)//������������
	{
		setExtraData(pData, lenContent - lenJson);
	}

	return true;
}

bool lly::HttpResponse::geneData( std::string &str )
{
	//��ȡjson�Լ�����
	std::string strJson;
	int lenJson = 0;
	getJsonString(m_pJson, strJson, lenJson);

	char szLine[256];
	sprintf(szLine,
		http_response_data_packet,
		m_ErrorCode, 
		getErrorString(m_ErrorCode),
		m_bKeepAlive ? "Keep-Alive" : "Close",
		lenJson,
		lenJson + m_lenExtraData);
	str = szLine;

	if (!m_strToken.empty())
	{
		sprintf(szLine, "Token:%s\r\n", m_strToken.c_str());
		str += szLine;
	}

	str+="\r\n"; //���� ����������

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




