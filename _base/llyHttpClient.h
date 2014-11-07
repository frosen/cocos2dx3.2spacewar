/*
���ƣ�llyHttpClient.h
���ݣ�httpʽ����ͨ�ſͻ�����
			���̷߳�ʽ
		
������¬����
���ڣ�2014.8.21
*/

#ifndef _LLY_HTTP_CLIENT_H_
#define _LLY_HTTP_CLIENT_H_

#include "cocos2d.h"
#include "llyNetStream.h"
#include "llyHttpSocket.h"

#include <mutex>

namespace lly{

class HttpClient
{
public:
	typedef std::function<
		void(int nHttpError, int nSocketError, std::shared_ptr<lly::HttpResponse>)> HttpClientCallback;

	struct struCBInfo
	{
		std::shared_ptr<lly::HttpRequest> pReq;
		HttpClientCallback callback;
	};

	HttpClient();
	virtual ~HttpClient();

	//�����ͻ���
	static HttpClient* getInstance();
	static void destoryInstance();

	//��������
	void setServer(const char* pszServerIP, int nPort = 80);
	void setThreadCount(int nThread);
	void setTimeoutMS(int nMS) { m_nTimeoutMS = nMS; }
	void setRetryCount(int cnRetry){ m_nMaxRetry = cnRetry; }
	void setToken(const char* token) { m_strToken = token; }

	//��ʼִ��
	bool run();

	//����
	void terminate();

	//������Ϣ
	bool send(lly::HttpRequest* pReq, HttpClientCallback callback);
	bool send(const char* pszURL, 
		HttpClientCallback callback, 
		rapidjson::Document* pJson = nullptr, 
		char *pExtraData = nullptr, 
		int lenExtraData = 0, 
		bool bWaitResponse=true);

	//�����ж�
	bool sendQuick(lly::HttpRequest* pReq, HttpClientCallback callback);
	bool sendQuick(const char* pszURL, 
		HttpClientCallback callback, 
		rapidjson::Document* pJson = nullptr);

	//ͬ���ж�
	bool sendSync(lly::HttpRequest* pReq, HttpClientCallback callback);
	bool sendSync(const char* pszURL, 
		HttpClientCallback callback, 
		rapidjson::Document* pJson = nullptr);

	//����ͨ��
	bool addRetainChannel(lly::HttpRequest* pReq, HttpClientCallback callback);
	bool addRetainChannel(const char* pszURL, 
		HttpClientCallback callback, 
		rapidjson::Document* pJson = nullptr);

protected:
	//ִ��http����
	void runHttpInteraction();

	void runHttpRetainInteraction(struCBInfo cb);

protected:
	static HttpClient* s_httpClient;

	std::string m_strServerIP; //������ip 
	int m_nServerPort; //�������˿ں�

	int m_nRunThread; //��ת���߳��������߳����ݣ�

	std::mutex mtx; //�߳���

	bool m_bNeedQuit; //��Ҫ����
	bool m_bProcessingSyncReq; //�Ƿ���ͬ���ж�

	NetStreamT<struCBInfo> m_listReqNormal; //�������Ͷ���
	NetStreamT<struCBInfo> m_listReqQuick; //���ȷ��Ͷ���
	NetStreamT<struCBInfo> m_listReqSync; //˳���Ͷ���
	
	std::string m_strToken;
	int64_t m_timeLogin64; //��¼ʱ��
	int m_nTimeoutMS; //��ʱ���룬���ڽ����߳�ʱ������lly::socket�ĳ�ʱ

	int m_nMaxRetry; //���Դ���
};

} //lly

#endif //_LLY_HTTP_CLIENT_H_




