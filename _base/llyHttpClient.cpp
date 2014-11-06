#include "llyHttpClient.h"
#include <thread>
#include <regex>
#include "llyHttpSocket.h"
#include "cocostudio\DictionaryHelper.h"

USING_NS_CC;
using namespace lly;
using namespace cocostudio;

//ͳһ����ϵͳ�е�sleep����
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
	m_nTimeout(30),
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
	if (nThread >= 2) m_nRunThread =  nThread; //�����г�����Ҫ�������������Ҫ����
	else
	{
		CCLOG("nThread must more than 2");
	}
}

void lly::HttpClient::run()
{
	assert(m_strServerIP != "", "wrong ip");
	
	for (int i = 0; i < m_nRunThread; ++i) //�������߳�
	{
		auto t = std::thread(&HttpClient::runHttpInteraction, this);
		t.detach();
	}
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
	struCBInfo cb; //�շ���Ϣ�Ľṹ��
	bool bProcessingSyncReq; //�Ƿ���ͬ������

	HttpSocket::Error eErrorCode; //�������
	int nSocketErrorCode;
	std::shared_ptr<HttpResponse> pAck;

	lly::HttpSocket socket; //�½�socket
	socket.setTimeout(m_nTimeout);

	int nRetry = 1; //���Դ���
	int nWaitMS = 100; //�ȴ�������

	while (true) 
	{
		if (m_bNeedQuit) break;

		cb.pReq = nullptr;
		bProcessingSyncReq = false;

		//1.����������÷�������
		mtx.lock(); //����

		do 
		{
			//�ȼ�����ȶ��У��ɹ����cb�򲻽��������жӵļ��
			if (m_listReqQuick.getT(cb)) break;

			//�ڼ��ͬ�����У��ɹ����cb�򲻽��������жӵļ��
			if (!m_bProcessingSyncReq && m_listReqSync.getT(cb))
			{
				m_bProcessingSyncReq = true;
				bProcessingSyncReq = true;
				break;
			}

			//���һ�����
			if (m_listReqNormal.getT(cb)) break;
			
		} while (0);
		
		mtx.unlock(); //����

		//û����Ϣ��Ҫ���ͣ���Ϣ��������
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

		//2.��ʼ���з��ͺͽ���
		//Ϊ�˴�����������������ԣ��������������ѭ��
		do 
		{
			//�ڵ�½��ǰm_strTokenΪ�գ�login�ɹ������ֵ���Զ������ˣ���Ϊ��֤�õ�
			cb.pReq->setToken(m_strToken);

			//�Ƿ���Ҫ��������
			bool bKeepAlive = cb.pReq->isKeepAlive();

			//�Ƿ���Ҫ�ȴ���Ӧ
			bool bWaitResponse = cb.pReq->m_bWaitResponse;

			bool bRet = socket.sendRequest(cb.pReq);

			if (!bRet) //����ʧ��
			{
				if (nRetry < m_nMaxRetry)
				{
					++nRetry;
					socket.close();
					sleep(nWaitMS);

					//���ȴ������������
					nWaitMS = MIN(nWaitMS * 2, 30 * 1000);
					continue;
				}

				//��ӡ�������
				eErrorCode = socket.getErrorCode();
				nSocketErrorCode = (int)socket.getError();

				CCLOG("wrong http = %d; socket = %d",(int)eErrorCode, nSocketErrorCode);

				//��ʧ�ܴ���
				socket.close();

				break;
			}

			if (!bWaitResponse) //����ȴ�����Ϣ
			{
				break;
			}

			//3.���������Ҫ������Ϣ
			pAck = socket.recvResponse();

			//�������
			eErrorCode = socket.getErrorCode();
			nSocketErrorCode = (int)socket.getError();

			if (pAck) //�з�����Ϣ
			{
				if (eErrorCode == HttpSocket::Error::CHANGE_SERVER) //Ҫ�����������
				{				
					rapidjson::Document* pJson = pAck->getJson();
					const char*pszServerIP = DICTOOL->getStringValue_json(pJson, "Server");
					int nPort = DICTOOL->getIntValue_json(pJson, "Port");
					if (*pszServerIP != '\0' || nPort != 0)
					{
						m_strServerIP = pszServerIP;
						m_nServerPort = nPort;

						continue; //���·���cb.Req
					}
					else //Ӧ�ò���������������
					{
						eErrorCode = HttpSocket::Error::UNKNOWN;
					}				
				}
				else //������Ϣ
				{
					eErrorCode = pAck->getErrorCode();

					//������Ҫ��ر�����
					if (!pAck->isKeepAlive()) socket.close();

					//ֻ��login�Ż᷵��token
					if (!pAck->getToken().empty())
					{
						m_strToken = pAck->getToken();

						//��¼ʱ��
						struct timeval tval;
						gettimeofday(&tval, NULL);
						m_timeLogin64 = (long int)(tval.tv_sec * 1000000LL + tval.tv_usec); 
					}
				}
			}

			if (!pAck || !bKeepAlive) socket.close();

		} while (0);

		//4.���ݻص������ͽ��ս�����л���
		mtx.lock();
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]()
		{
			cb.callback((int)eErrorCode, nSocketErrorCode, pAck);
		});
		mtx.unlock();

		if (bProcessingSyncReq) 
		{
			mtx.lock();
			m_bProcessingSyncReq = false;
			mtx.unlock();
		}

		//����ѭ��
	}

	//�߳̽���
	mtx.lock();
	--m_nRunThread;
	mtx.unlock();
}

void lly::HttpClient::runHttpRetainInteraction(struCBInfo cb)
{
	HttpSocket::Error eErrorCode; //�������
	int nSocketErrorCode;
	std::shared_ptr<HttpResponse> pAck;

	lly::HttpSocket socket;

	//�־�������Ҫ��ʱ��Ҫ������������60��ᷢ��һ����Ϣ
	socket.setTimeout(80);

	int nWaitTimeForRetry_ms = 10;
	int64_t timeLogin64 = m_timeLogin64;

	bool bEndThisThread = false;

	while (true) 
	{
		if (m_bNeedQuit) break;
		if (bEndThisThread) break;

		//token���ˣ�������
		if (timeLogin64 != m_timeLogin64) break;

		cb.pReq->setToken(m_strToken);
		cb.pReq->setKeepAlive(true);

		//���Ϳ�ʼ
		if (socket.sendRequest(cb.pReq))
		{
			//��������ɣ�֮��һֱ�ȴ���֪�����ر�
			nWaitTimeForRetry_ms = 10;
		}
		else
		{
			socket.close();
			sleep(nWaitTimeForRetry_ms);

			//���粻�õ�ʱ�����ȴ�30��
			nWaitTimeForRetry_ms = MIN(nWaitTimeForRetry_ms * 2, 30 * 1000);
			continue;
		}	

		//���տ�ʼ
		do
		{
			pAck = socket.recvResponse();

			eErrorCode = pAck->getErrorCode();
			nSocketErrorCode = socket.getError();

			//token���ˣ�������
			if (timeLogin64 != m_timeLogin64) break;

			//���ִ���Ĵ���
			if (!pAck) //����ʧ�ܣ����߷������Ͽ�
			{
				CCLOG("retain msg, pAck is null");

				socket.close();
				break; //�ص���ͷ
			}
			else if (eErrorCode == HttpSocket::Error::EMPTY) //����Ϣ���൱��������Ϣ����������
			{
				CCLOG("retain msg, empty msg");
				continue; //�ص���ʼ����
			}
			else if (eErrorCode == HttpSocket::Error::ANOTHER_LOGIN) //��һ����½
			{
				bEndThisThread = true;
				break;
			}
			else if (pAck->getErrorCode() != HttpSocket::Error::OK) //�����������Ϣ
			{
				CCLOG("retain msg, not ok");

				socket.close();
				break; //�ص���ͷ
			}

			if (!pAck->isKeepAlive()) //���ñ��֣��˳��߳�
			{
				CCLOG("retain msg, not keep alive");
				bEndThisThread = true;
				break;
			}
	
		}while(0);
		//ѭ��, �����ȴ���Ϣ

		mtx.lock();
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]()
		{
			cb.callback((int)eErrorCode, nSocketErrorCode, pAck);
		});
		mtx.unlock();
	}

	//�˳��߳�
	mtx.lock();
	--m_nRunThread;
	mtx.unlock();
}




