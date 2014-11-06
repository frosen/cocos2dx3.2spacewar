/*
���ƣ�llyHttpSocket.h
���ݣ�����http��Ϣ��socket��
		
������¬����
���ڣ�2014.8.21
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

//httpЭ���socket
class HttpSocket : public lly::Socket
{
public:
	enum class Error
	{
		OK = 200,
		EMPTY = 204,
		CHANGE_SERVER = 210,
		FILE_EXIST = 240, //�ļ��Ѿ�����

		UNKNOWN = 400, //һ���Դ���

		CONNECT_FAIL = 470,
		TIMEOUT = 471,
		SEND_FAIL = 472,
		RECV_FAIL = 473,
		
		INVALID_TOKEN = 480, //������token����
		INVALID_USER_NAME = 481, //�û����Ҳ���
		INVALID_PASSWORD = 482, //�������
		FILE_NOT_FOUND = 483, //�ļ�������
		FILE_SIZE_ERROR = 484, //�ļ��ߴ粻��
		ANOTHER_LOGIN = 485, //��һ��login����������

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
		MAXLEN = 262144, //http���ݲ��ܳ���256k��256*1024��
	};

	HttpSocket();
	virtual ~HttpSocket();

	bool sendRequest(std::shared_ptr<HttpRequest> pReq);
	bool sendResponse(std::shared_ptr<HttpResponse> pAck);

	std::shared_ptr<HttpRequest> recvRequest();
	std::shared_ptr<HttpResponse> recvResponse();

	//����connect��send��recv�ĳ�ʱֱ���ɻ����SetTimeout����
	void setTimeout(int nTimeout) { m_nTimeout = nTimeout; }
	void setKeepAlive(bool bKeepAlive) { m_bKeepAlive = bKeepAlive; }

	Error getErrorCode() {return m_ErrorCode;}

	virtual int close();

protected:
	//��socket�ֽ����л�ȡһ��������http��Ϣ
	char* recvHttpMsg(int &out_lenMsg);

protected:
	std::string m_strIP; //IP
	int m_nPort; //�˿ں�

	bool m_bKeepAlive; //�Ƿ񱣳����ӣ�Ĭ��Ϊtrue

	Error m_ErrorCode; //�������
	int m_nTimeout; //��ʱ��Ĭ��30��

	//��һ����Ϣ��ȡ����ʱ�����ܻ���ȡ����һ����Ϣ�����ݣ��ڴ˱���
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

	//��Ӻͻ�ȡ��������
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
	//�������ݣ����ⲿ���ã��ⲿ��ȡ
	CC_SYNTHESIZE(void*, m_pUserData, UserData); 

	//�������
	CC_SYNTHESIZE(HttpSocket::Error, m_ErrorCode, ErrorCode);

protected:
	std::string m_strToken; //���Ͷ����ⲿ���ã����ն���FromData�������
	rapidjson::Document* m_pJson;	 //��Ҫ��Ϊ���ݸ��ӵ�json��ָ��

	int m_lenExtraData; //�����������ݳ���
	int m_lenExtraDataAlloc; //�������ݻ�������С
	char* m_pExtraData; //�������ݻ�����

	bool m_bKeepAlive;

	friend lly::HttpSocket;
};

//http������Ϣ����֯�����ͺͽ���
class HttpRequest : public HttpDataBase
{
public:
	//http�������ݰ���ʽ
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

	//URL�еĲ���
	struct struHttpRequsetInfo
	{
		std::string strName;
		std::string strValue;
	};

	HttpRequest();
	~HttpRequest();
	
	static std::unique_ptr<HttpRequest> createUniquePointer();

	//���յ�http��Ϣ����ͨ������������������ɶ���ʵ��
	bool analyzeData(char* pData, int len);

	//��������
	void setServer(const char* pszServerIP, int nPort);
	void setRequestMethod(HttpSocket::RequestMethod eMethod);

	//ָ������URL�����������������ַ���˿ڵ�ַ��������
	//http://[www.17xuexi.com[:8080]]/login[?username='szUserName'&password='szPassword']
	bool setURL(const char *pszURL);

	//��ӺͲ��Ҳ���
	void addParam(const char* pszName, const char* pszValue);

	const char* findParamStr(const char* pszName); //�ı�����
	int findParamInt(const char* pszName, int nDefault); //���ֲ���

	//����http���ݰ�
	bool geneData(std::string &str, HttpSocket::RequestMethod eMethod);

	//������������
	void clear();

protected:
	void clearParam();

public:
	std::string m_strServerIP; //ip
	int m_nServerPort; //�˿ں�

	HttpSocket::RequestMethod m_eRequestMethod; //get����post

	std::string m_strURL; //���ӵ�url��ַ������ֱ���ɲ���
	
	std::vector<struHttpRequsetInfo*> m_listParam;	//��Ҫ���ӵĲ���

	bool m_bWaitResponse;
};

//http������Ϣ����֯�����ͺͽ���
class HttpResponse : public HttpDataBase
{
public:
	//http��Ӧ���ݰ���ʽ
	//HTTP/1.1 ״̬�� ״̬����
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

	//��������
	bool analyzeData(char* pData, int len);

	//����http���ݰ�
	bool geneData(std::string &str);

	//�������
	void clear();
};


} //lly

#endif //_LLY_HTTP_SOCKET_H_
