/*
名称：llyHttpSocket.h
内容：处理http消息的socket类
		
制作：卢乐颜
日期：2014.8.21
*/

#ifndef _LLY_HTTP_SOCKET_H_
#define _LLY_HTTP_SOCKET_H_

#include "llySocket.h"
#include "cocos2d.h"
#include <string>
#include <vector>
#include "json\document.h"

namespace lly{

class HttpRequest;
class HttpResponse;

//http协议的socket
class HttpSocket : public lly::Socket
{
public:
	enum class Error
	{
		OK = 200,
		EMPTY = 204,
		CHANGE_SERVER = 210,
		FILE_EXIST = 240, //文件已经存在

		UNKNOWN = 400, //一般性错误

		CONNECT_FAIL = 470,
		TIMEOUT = 471,
		SEND_FAIL = 472,
		RECV_FAIL = 473,
		
		INVALID_TOKEN = 480, //附带的token错误
		INVALID_USER_NAME = 481, //用户名找不到
		INVALID_PASSWORD = 482, //密码错误
		FILE_NOT_FOUND = 483, //文件不存在
		FILE_SIZE_ERROR = 484, //文件尺寸不对
		ANOTHER_LOGIN = 485, //另一个login把它给顶了

		OUT_OF_MEMORY = 900,		
	};

	static const char* getErrorMsg(Error e);

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

	Error getErrorCode() {return m_ErrorCode;}

	virtual int close();

protected:
	//从socket字节流中获取一个完整的http消息
	char* recvHttpMsg(int &out_lenMsg);

protected:
	std::string m_strIP; //IP
	int m_nPort; //端口号

	bool m_bKeepAlive; //是否保持连接，默认为true

	Error m_ErrorCode; //错误代码
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

	void setKeepAlive(bool bKeepAlive) { m_bKeepAlive = bKeepAlive; }
	bool isKeepAlive() { return m_bKeepAlive; }	

protected:
	bool getJsonString(rapidjson::Document* json, std::string &out_strJson, int out_lenJsonStr);

public:
	//附件数据，由外部设置，外部获取
	CC_SYNTHESIZE(void*, m_pUserData, UserData); 

	//错误代码
	CC_SYNTHESIZE(HttpSocket::Error, m_ErrorCode, ErrorCode);

protected:
	std::string m_strToken; //发送端由外部设置，接收端由FromData解析获得
	rapidjson::Document* m_pJson;	 //需要作为内容附加的json的指针

	int m_lenExtraData; //其他附加数据长度
	int m_lenExtraDataAlloc; //附加数据缓冲区大小
	char* m_pExtraData; //附加数据缓冲区

	bool m_bKeepAlive;

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
	//Content: xx
	//[Token: xx]
	//[WaitResponse: 0]
	//
	//[json]
	//[extra data]
#define http_request_data_packet \
	"%s %s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nConnection:%s\r\nnJson-Length: %d\r\nContent: %d\r\n"

	//URL中的参数
	struct struHttpRequsetInfo
	{
		std::string strName;
		std::string strValue;
	};

	HttpRequest();
	~HttpRequest();
	
	static std::unique_ptr<HttpRequest> createUniquePointer();

	//接收到http消息包后，通过这个函数解析，生成对象实体
	bool analyzeData(char* pData, int len);

	//基本设置
	void setServer(const char* pszServerIP, int nPort);
	void setRequestMethod(HttpSocket::RequestMethod eMethod);

	//指定请求URL，允许包含服务器地址，端口地址，及参数
	//http://[www.17xuexi.com[:8080]]/login[?username='szUserName'&password='szPassword']
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
	void clearParam();

public:
	std::string m_strServerIP; //ip
	int m_nServerPort; //端口号

	HttpSocket::RequestMethod m_eRequestMethod; //get还是post

	std::string m_strURL; //连接的url地址，允许直接由参数
	
	std::vector<struHttpRequsetInfo*> m_listParam;	//需要附加的参数

	bool m_bWaitResponse;
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
	//Content: xx
	//[Token: xx]
	//
	//[json]
	//[extra data]
#define http_response_data_packet \
	"HTTP/1.1 %d %s\r\nServer:llyHttpServer\r\nContent-Type:application/octet-stream\r\nConnection:%s\r\nJson-Length:%d\r\nContent-Length:%d\r\n"

	HttpResponse();
	~HttpResponse();

	static std::unique_ptr<HttpResponse> createUniquePointer();

	//解析数据
	bool analyzeData(char* pData, int len);

	//生成http数据包
	bool geneData(std::string &str);

	//清空内容
	void clear();
};


} //lly

#endif //_LLY_HTTP_SOCKET_H_
