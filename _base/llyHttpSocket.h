/*
名称：llyHttpSocket.h
内容：处理http消息的socket类
		
制作：卢乐颜
日期：2014.8.21
*/

#ifndef _LLY_HTTP_SOCKET_H_
#define _LLY_HTTP_SOCKET_H_

#include "llySocket.h"
#include "json\document.h"
#include <string>
#include <vector>
#include <memory>

namespace lly{

class HttpRequest;
class HttpResponse;
class Error;

//http协议的socket
class HttpSocket : public lly::Socket
{
public:
	enum class RequestMethod
	{
		UNKNOWN,
		GET,
		POST,
	};

	enum class Data
	{
		MAXLEN = 262144, //http数据不能超过256k（256*1024）
	};

	HttpSocket();
	virtual ~HttpSocket();

	bool sendRequest(std::shared_ptr<HttpRequest> pReq);
	bool sendResponse(std::shared_ptr<HttpResponse> pAck);

	std::shared_ptr<HttpRequest> recvRequest();
	std::shared_ptr<HttpResponse> recvResponse();

	//其他connect、send、recv的超时直接由基类的SetTimeout设置
	void setTimeout(int nTimeout) { m_nTimeout = nTimeout; }
	void setKeepAlive(bool bKeepAlive) { m_bKeepAlive = bKeepAlive; }

	int getErrorCode() {return m_ErrorCode;}

	virtual int close();

protected:
	//从socket字节流中获取一个完整的http消息
	char* recvHttpMsg_needfree(int &out_lenMsg);

protected:
	std::string m_strIP; //IP
	int m_nPort; //端口号

	bool m_bKeepAlive; //是否保持连接，默认为true

	int m_ErrorCode; //错误代码
	int m_nTimeout; //超时，默认30秒

	//上一个消息读取数据时，可能会多读取了下一个消息的内容，在此保存
	int m_lenPreReadBuf;
	char* m_pPreReadBuf;
};

class HttpDataBase
{
public:
	HttpDataBase();
	virtual ~HttpDataBase();

	void setToken(std::string &str) { m_strToken = str; }
	std::string getToken() { return m_strToken; }

	//添加和获取附加内容
	void setJson(rapidjson::Document* pJson);
	rapidjson::Document* getJson();

	bool setExtraData(const char* pExtraData, int lenExtraData);
	bool addExtraData(const char* pExtraData, int lenExtraData);
	char* getExtraData(int &out_lenExtraData);

	//是否保持连接
	void setKeepAlive(bool bKeepAlive) { m_bKeepAlive = bKeepAlive; }
	bool isKeepAlive() { return m_bKeepAlive; }

	//外部数据
	void setUserData(void* pUserData) { m_pUserData = pUserData; }
	void* getUserData() { return m_pUserData; }

	//错误
	void setErrorCode(int n) { m_ErrorCode = n; }
	int getErrorCode() { return m_ErrorCode; }

protected:
	//根据rapidjson文件，获取json的字符串码
	bool getJsonString(rapidjson::Document* json, std::string &out_strJson, int out_lenJsonStr);

protected:
	std::string m_strToken; //发送端由外部设置，接收端由FromData解析获得
	rapidjson::Document* m_pJson;	 //需要作为内容附加的json的指针

	int m_lenExtraData; //其他附加数据长度
	int m_lenExtraDataAlloc; //附加数据缓冲区大小
	char* m_pExtraData; //附加数据缓冲区

	bool m_bKeepAlive; //保持连接

	void* m_pUserData; //附件数据，由外部设置，外部获取

	int m_ErrorCode; //错误代码

	friend lly::HttpSocket;
};

//http请求消息的组织、发送和接收
class HttpRequest : public HttpDataBase
{
public:
	//http请求数据包格式
	//[GET] [URL?KEY=VALUE&KEY2=VALUE2 HTTP/1.1
	//Host: [host]
	//Accept: */*
	//Connection: [live]
	//Json-Length: xx
	//Content-Length: xx
	//[Token: xx]
	//[WaitResponse: 0]
	//
	//[json]
	//[extra data]
#define http_request_data_packet \
	"%s %s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nConnection:%s\r\nnJson-Length: %d\r\nContent-Length: %d\r\n"

	//URL中的参数
	struct struHttpRequsetInfo
	{
		std::string strName;
		std::string strValue;
	};

	HttpRequest();
	~HttpRequest();
	
	//生成本类的智能指针
	static std::shared_ptr<HttpRequest> createSharedPointer();

	//接收到http消息包后，通过这个函数解析，生成对象实体
	bool analyzeData(char* pData, int len);

	//基本设置
	void setServer(const char* pszServerIP, int nPort);
	void setRequestMethod(HttpSocket::RequestMethod eMethod);

	//指定请求URL，允许包含服务器地址，端口地址，及参数
	//例如，http://[www.17xuexi.com[:8080]]/login[?username='szUserName'&password='szPassword']
	bool setURL(const char *pszURL);

	//添加和查找参数
	void addParam(const char* pszName, const char* pszValue);
	const char* findParamStr(const char* pszName); //文本参数
	int findParamInt(const char* pszName, int nDefault); //数字参数

	//生成http数据包
	bool geneData(std::string &str, HttpSocket::RequestMethod eMethod);

	//清理所有内容
	void clear();

protected:
	void clearParam(); //清理参数

public:
	std::string m_strServerIP; //ip
	int m_nServerPort; //端口号

	HttpSocket::RequestMethod m_eRequestMethod; //get还是post

	std::string m_strURL; //连接的url地址，允许直接由参数
	
	std::vector<struHttpRequsetInfo*> m_listParam;	//需要附加的参数

	bool m_bWaitResponse; //是否等待回应
};

//http回馈消息的组织、发送和接收
class HttpResponse : public HttpDataBase
{
public:
	//http回应数据包格式
	//HTTP/1.1 状态码 状态描述
	//Server:llyHttpServer
	//Content-Type:application/octet-stream
	//Connection: [live]
	//Json-Length: xx
	//Content-Length: xx
	//[Token: xx]
	//
	//[json]
	//[extra data]
#define http_response_data_packet \
	"HTTP/1.1 %d %s\r\nServer:llyHttpServer\r\nContent-Type:application/octet-stream\r\nConnection:%s\r\nJson-Length:%d\r\nContent-Length:%d\r\n"

	HttpResponse();
	~HttpResponse();

	//生成本类的智能指针
	static std::shared_ptr<HttpResponse> createSharedPointer();

	//解析数据
	bool analyzeData(char* pData, int len);

	//生成http数据包
	bool geneData(std::string &str);

	//清空内容
	void clear();
};


} //lly

#endif //_LLY_HTTP_SOCKET_H_
