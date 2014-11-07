#include "llyHttpClient.h"
#include "llyHttpSocket.h"
#include "llyHttpErrorCode.h"

#include "cocostudio\DictionaryHelper.h"

#include <thread>
#include <regex>

USING_NS_CC;
using namespace lly;
using namespace cocostudio;

//统一两种系统中的sleep函数
inline void sleep(int n)
{
#ifdef WIN32
	::Sleep(n);
#else
	::usleep(n*1000);
#endif
}

HttpClient* lly::HttpClient::s_httpClient = nullptr;

lly::HttpClient::HttpClient() : 
	m_strServerIP(""),
	m_nRunThread(5),
	m_bNeedQuit(false),
	m_bProcessingSyncReq(false),
	m_timeLogin64(0),
	m_nTimeoutMS(30000),
	m_nMaxRetry(5)
{

}

lly::HttpClient::~HttpClient()
{

}

HttpClient* lly::HttpClient::getInstance()
{
	if (!s_httpClient)
	{
		s_httpClient = new HttpClient();	
	}
	return s_httpClient;
}

void lly::HttpClient::destoryInstance()
{
	if (s_httpClient)
	{
		CC_SAFE_DELETE(s_httpClient);
	}		
}

void lly::HttpClient::setServer( const char* pszServerIP, int nPort )
{
	if (std::regex_match(pszServerIP, std::regex("(\\d{1,3}).(\\d{1,3}).(\\d{1,3}).(\\d{1,3})")))
	{
		m_strServerIP = pszServerIP;
		m_nServerPort = nPort;
	}
	else
	{
		CCLOG("wrong ip input");
	}
}

void lly::HttpClient::setThreadCount( int nThread )
{
	if (nThread >= 2) m_nRunThread =  nThread; //由于有长连接要处理，因此至少需要两个
	else
	{
		CCLOG("nThread must more than 2");
	}
}

bool lly::HttpClient::run()
{
	if(m_strServerIP == "") return false;
	
	for (int i = 0; i < m_nRunThread; ++i) //创建新线程
	{
		auto t = std::thread(&HttpClient::runHttpInteraction, this);
		t.detach();
	}

	return true;
}

void lly::HttpClient::terminate()
{
	m_bNeedQuit = true;
}

bool lly::HttpClient::send( lly::HttpRequest* pReq, HttpClientCallback callback )
{
	if (pReq->m_strServerIP.empty()) pReq->setServer(m_strServerIP.c_str(), m_nServerPort);

	struCBInfo cb;
	cb.pReq = std::shared_ptr<HttpRequest>(pReq);
	cb.callback = callback;

	mtx.lock();
	m_listReqNormal.putT(cb);
	mtx.unlock();

	return true;
}

bool lly::HttpClient::send( const char* pszURL, HttpClientCallback callback, rapidjson::Document* pJson /*= nullptr*/, char *pExtraData /*= nullptr*/, int lenExtraData /*= 0*/, bool bWaitResponse/*=true*/ )
{
	auto pReq = new lly::HttpRequest();
	if (pReq == nullptr) return false;

	pReq->setServer(m_strServerIP.c_str(), m_nServerPort);
	pReq->setURL(pszURL);
	pReq->m_bWaitResponse = bWaitResponse;

	if (pJson != nullptr) pReq->setJson(pJson);
	if (pExtraData != nullptr) pReq->setExtraData(pExtraData, lenExtraData);
	return send(pReq, callback);
}

bool lly::HttpClient::sendQuick( lly::HttpRequest* pReq, HttpClientCallback callback )
{
	if (pReq->m_strServerIP.empty()) pReq->setServer(m_strServerIP.c_str(), m_nServerPort);

	struCBInfo cb;
	cb.pReq = std::shared_ptr<HttpRequest>(pReq);
	cb.callback = callback;

	mtx.lock();
	m_listReqQuick.putT(cb);
	mtx.unlock();

	return true;
}

bool lly::HttpClient::sendQuick( const char* pszURL, HttpClientCallback callback, rapidjson::Document* pJson /*= nullptr*/ )
{
	auto pReq = new lly::HttpRequest();
	if (pReq == nullptr) return false;

	pReq->setServer(m_strServerIP.c_str(), m_nServerPort);
	pReq->setURL(pszURL);
	if (pJson != nullptr) pReq->setJson(pJson);
	
	return sendQuick(pReq, callback);
}

bool lly::HttpClient::sendSync( lly::HttpRequest* pReq, HttpClientCallback callback )
{
	if (pReq->m_strServerIP.empty()) pReq->setServer(m_strServerIP.c_str(), m_nServerPort);

	struCBInfo cb;
	cb.pReq = std::shared_ptr<HttpRequest>(pReq);
	cb.callback = callback;

	mtx.lock();
	m_listReqSync.putT(cb);
	mtx.unlock();

	return true;
}

bool lly::HttpClient::sendSync( const char* pszURL, HttpClientCallback callback, rapidjson::Document* pJson /*= nullptr*/ )
{
	auto pReq = new lly::HttpRequest();
	if (pReq == nullptr) return false;

	pReq->setServer(m_strServerIP.c_str(), m_nServerPort);
	pReq->setURL(pszURL);
	if (pJson != nullptr) pReq->setJson(pJson);

	return sendSync(pReq, callback);
}

bool lly::HttpClient::addRetainChannel( lly::HttpRequest *pReq, HttpClientCallback callback )
{
	if (pReq->m_strServerIP.empty()) pReq->setServer(m_strServerIP.c_str(), m_nServerPort);

	struCBInfo cb;

	cb.pReq = std::shared_ptr<HttpRequest>(pReq);
	cb.callback = callback;

	auto t = std::thread(&HttpClient::runHttpRetainInteraction, this, cb);
	t.detach();

	mtx.lock();
	++m_nRunThread;
	mtx.unlock();

	return true;
}

bool lly::HttpClient::addRetainChannel( const char *pszURL, HttpClientCallback callback, rapidjson::Document* pJson /*= nullptr*/ )
{
	auto pReq = new lly::HttpRequest();
	if (pReq == nullptr) return false;

	pReq->setServer(m_strServerIP.c_str(), m_nServerPort);
	pReq->setURL(pszURL);
	if (pJson != nullptr) pReq->setJson(pJson);

	return addRetainChannel(pReq, callback);
}

//======================================
void lly::HttpClient::runHttpInteraction()
{
	struCBInfo cb; //收发信息的结构体
	bool bProcessingSyncReq; //是否处理同步需求

	int nHttpErrorCode = HTTP_ERROR_OK; //错误代码
	int nSocketErrorCode = -100;
	std::shared_ptr<HttpResponse> pAck;

	lly::HttpSocket socket; //新建socket
	socket.setTimeoutMS(m_nTimeoutMS);

	int nRetry = 1; //尝试次数
	int nWaitMS = 100; //等待毫秒数

	while (true) 
	{
		if (m_bNeedQuit) break;

		cb.pReq = nullptr;
		bProcessingSyncReq = false;

		//1.从主函数获得发送请求
		mtx.lock(); //上锁

		do 
		{
			//先检查优先队列，成功获得cb则不进行下面列队的检测
			if (m_listReqQuick.getT(cb)) break;

			//在检查同步队列，成功获得cb则不进行下面列队的检测
			if (!m_bProcessingSyncReq && m_listReqSync.getT(cb))
			{
				m_bProcessingSyncReq = true;
				bProcessingSyncReq = true;
				break;
			}

			//检查一般队列
			if (m_listReqNormal.getT(cb)) break;
			
		} while (0);
		
		mtx.unlock(); //解锁

		//没有消息需要发送，稍息后继续检查
		if (cb.pReq == nullptr) 
		{
			
			if (bProcessingSyncReq) 
			{
				mtx.lock();
				m_bProcessingSyncReq = false;
				mtx.unlock();
			}
						
			sleep(50);
			continue;
		}

		//2.开始进行发送和接收
		//为了处理更换服务器和重试，所以引入了这个循环
		do 
		{
			//在登陆以前m_strToken为空，login成功后，这个值就自动存在了，作为验证用的
			cb.pReq->setToken(m_strToken);

			//是否需要保持连接
			bool bKeepAlive = cb.pReq->isKeepAlive();

			//是否需要等待回应
			bool bWaitResponse = cb.pReq->m_bWaitResponse;

			bool bRet = socket.sendRequest(cb.pReq);

			if (!bRet) //发送失败
			{
				if (nRetry < m_nMaxRetry)
				{
					++nRetry;
					socket.close();
					sleep(nWaitMS);

					//最多等待不超过半分钟
					nWaitMS = MIN(nWaitMS * 2, 30 * 1000);
					continue;
				}

				//打印错误代码
				nHttpErrorCode = socket.getErrorCode();
				nSocketErrorCode = socket.getError();

				CCLOG("wrong http = %d; socket = %d",nHttpErrorCode, nSocketErrorCode);

				//按失败处理
				socket.close();

				break;
			}

			if (!bWaitResponse) //无需等待的消息
			{
				break;
			}

			//3.发送完后需要接受消息
			pAck = socket.recvResponse();

			//错误代码
			nHttpErrorCode = socket.getErrorCode();
			nSocketErrorCode = socket.getError();

			if (!pAck) //无返回消息
			{
				socket.close();
				break;
			}

			if (nHttpErrorCode == HTTP_ERROR_CHANGE_SERVER) //要求更换服务器
			{				
				rapidjson::Document* pJson = pAck->getJson();
				const char* pszServerIP = DICTOOL->getStringValue_json(pJson, "Server");
				int nPort = DICTOOL->getIntValue_json(pJson, "Port");
				if (*pszServerIP != '\0' || nPort != 0)
				{
					m_strServerIP = pszServerIP;
					m_nServerPort = nPort;

					continue; //重新发生cb.Req
				}
				else //应该不会出现这样的情况
				{
					nHttpErrorCode = HTTP_ERROR_UNKNOWN;
					break;
				}				
			}

			if (!pAck->getToken().empty()) //只有login才会返回token
			{
				m_strToken = pAck->getToken();

				//登录时间
				struct timeval tval;
				gettimeofday(&tval, NULL);
				m_timeLogin64 = (long int)(tval.tv_sec * 1000000LL + tval.tv_usec); 
			}

			//服务器要求关闭连接
			if (!pAck->isKeepAlive()) socket.close();

			//自己要求关闭链接
			if (!bKeepAlive) socket.close();

		} while (0);

		//4.根据回调函数和接收结果进行回馈
		mtx.lock();
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]()
		{
			cb.callback(nHttpErrorCode, nSocketErrorCode, pAck);
		});
		mtx.unlock();

		if (bProcessingSyncReq) 
		{
			mtx.lock();
			m_bProcessingSyncReq = false;
			mtx.unlock();
		}

		//继续循环
	}

	//线程结束
	mtx.lock();
	--m_nRunThread;
	mtx.unlock();
}

void lly::HttpClient::runHttpRetainInteraction(struCBInfo cb)
{
	int nHttpErrorCode; //错误代码
	int nSocketErrorCode;
	std::shared_ptr<HttpResponse> pAck;

	lly::HttpSocket socket;

	//持久连接需要的时间要长，服务器端60秒会发送一个消息
	socket.setTimeout(m_nTimeoutMS * 3);

	int nWaitTimeForRetry_ms = 10;
	int64_t timeLogin64 = m_timeLogin64;

	bool bEndThisThread = false;

	while (true) 
	{
		if (m_bNeedQuit) break;
		if (bEndThisThread) break;

		//token换了，不干了
		if (timeLogin64 != m_timeLogin64) break;

		cb.pReq->setToken(m_strToken);
		cb.pReq->setKeepAlive(true);

		//发送开始
		if (socket.sendRequest(cb.pReq))
		{
			//请求发送完成，之后一直等待，知道被关闭
			nWaitTimeForRetry_ms = 10;
		}
		else
		{
			socket.close();
			sleep(nWaitTimeForRetry_ms);

			//网络不好的时候最多等待30秒
			nWaitTimeForRetry_ms = MIN(nWaitTimeForRetry_ms * 2, 30 * 1000);
			continue;
		}	

		//接收开始
		do
		{
			pAck = socket.recvResponse();

			nHttpErrorCode = pAck->getErrorCode();
			nSocketErrorCode = socket.getError();

			//token换了，不干了
			if (timeLogin64 != m_timeLogin64) break;

			//各种错误的处理
			if (!pAck) //接收失败，或者服务器断开
			{
				CCLOG("retain msg, pAck is null");

				socket.close();
				break; //回到开头
			}

			if (nHttpErrorCode == HTTP_ERROR_EMPTY) //空消息，相当于心跳消息，继续接收
			{
				CCLOG("retain msg, empty msg");
				continue; //回到开始接收
			}

			if (nHttpErrorCode != HTTP_ERROR_OK) //其他错误的消息
			{
				CCLOG("retain msg, not ok");

				socket.close();
				break; //回到开头
			}

			if (!pAck->isKeepAlive()) //不用保持，退出线程
			{
				CCLOG("retain msg, not keep alive");
				bEndThisThread = true;
				break;
			}
	
		}while(0);

		mtx.lock();
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]()
		{
			cb.callback(nHttpErrorCode, nSocketErrorCode, pAck);
		});
		mtx.unlock();

		//循环, 继续等待消息
	}

	//退出线程
	mtx.lock();
	--m_nRunThread;
	mtx.unlock();
}




