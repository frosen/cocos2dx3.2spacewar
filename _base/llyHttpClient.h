/*
名称：llyHttpClient.h
内容：http式网络通信客户端类
			多线程方式
		
制作：卢乐颜
日期：2014.8.21
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

	//创建客户端
	static HttpClient* getInstance();
	static void destoryInstance();

	//各种设置
	void setServer(const char* pszServerIP, int nPort = 80);
	void setThreadCount(int nThread);
	void setTimeoutMS(int nMS) { m_nTimeoutMS = nMS; }
	void setRetryCount(int cnRetry){ m_nMaxRetry = cnRetry; }
	void setToken(const char* token) { m_strToken = token; }

	//开始执行
	bool run();

	//结束
	void terminate();

	//发送消息
	bool send(lly::HttpRequest* pReq, HttpClientCallback callback);
	bool send(const char* pszURL, 
		HttpClientCallback callback, 
		rapidjson::Document* pJson = nullptr, 
		char *pExtraData = nullptr, 
		int lenExtraData = 0, 
		bool bWaitResponse=true);

	//快速列队
	bool sendQuick(lly::HttpRequest* pReq, HttpClientCallback callback);
	bool sendQuick(const char* pszURL, 
		HttpClientCallback callback, 
		rapidjson::Document* pJson = nullptr);

	//同步列队
	bool sendSync(lly::HttpRequest* pReq, HttpClientCallback callback);
	bool sendSync(const char* pszURL, 
		HttpClientCallback callback, 
		rapidjson::Document* pJson = nullptr);

	//持续通道
	bool addRetainChannel(lly::HttpRequest* pReq, HttpClientCallback callback);
	bool addRetainChannel(const char* pszURL, 
		HttpClientCallback callback, 
		rapidjson::Document* pJson = nullptr);

protected:
	//执行http交互
	void runHttpInteraction();

	void runHttpRetainInteraction(struCBInfo cb);

protected:
	static HttpClient* s_httpClient;

	std::string m_strServerIP; //服务器ip 
	int m_nServerPort; //服务器端口号

	int m_nRunThread; //运转的线程数（跨线程数据）

	std::mutex mtx; //线程锁

	bool m_bNeedQuit; //需要结束
	bool m_bProcessingSyncReq; //是否处理同步列队

	NetStreamT<struCBInfo> m_listReqNormal; //正常发送队列
	NetStreamT<struCBInfo> m_listReqQuick; //优先发送队列
	NetStreamT<struCBInfo> m_listReqSync; //顺序发送队列
	
	std::string m_strToken;
	int64_t m_timeLogin64; //登录时间
	int m_nTimeoutMS; //超时毫秒，会在进入线程时候设置lly::socket的超时

	int m_nMaxRetry; //尝试次数
};

} //lly

#endif //_LLY_HTTP_CLIENT_H_




