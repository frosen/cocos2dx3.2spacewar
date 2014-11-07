/*
���ƣ�llyHttpSocket.h
���ݣ�����http��Ϣ��socket��
		
������¬����
���ڣ�2014.8.21
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

//httpЭ���socket
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

	int getErrorCode() {return m_ErrorCode;}

	virtual int close();

protected:
	//��socket�ֽ����л�ȡһ��������http��Ϣ
	char* recvHttpMsg_needfree(int &out_lenMsg);

protected:
	std::string m_strIP; //IP
	int m_nPort; //�˿ں�

	bool m_bKeepAlive; //�Ƿ񱣳����ӣ�Ĭ��Ϊtrue

	int m_ErrorCode; //�������
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

	//�Ƿ񱣳�����
	void setKeepAlive(bool bKeepAlive) { m_bKeepAlive = bKeepAlive; }
	bool isKeepAlive() { return m_bKeepAlive; }

	//�ⲿ����
	void setUserData(void* pUserData) { m_pUserData = pUserData; }
	void* getUserData() { return m_pUserData; }

	//����
	void setErrorCode(int n) { m_ErrorCode = n; }
	int getErrorCode() { return m_ErrorCode; }

protected:
	//����rapidjson�ļ�����ȡjson���ַ�����
	bool getJsonString(rapidjson::Document* json, std::string &out_strJson, int out_lenJsonStr);

protected:
	std::string m_strToken; //���Ͷ����ⲿ���ã����ն���FromData�������
	rapidjson::Document* m_pJson;	 //��Ҫ��Ϊ���ݸ��ӵ�json��ָ��

	int m_lenExtraData; //�����������ݳ���
	int m_lenExtraDataAlloc; //�������ݻ�������С
	char* m_pExtraData; //�������ݻ�����

	bool m_bKeepAlive; //��������

	void* m_pUserData; //�������ݣ����ⲿ���ã��ⲿ��ȡ

	int m_ErrorCode; //�������

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
	//Content-Length: xx
	//[Token: xx]
	//[WaitResponse: 0]
	//
	//[json]
	//[extra data]
#define http_request_data_packet \
	"%s %s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nConnection:%s\r\nnJson-Length: %d\r\nContent-Length: %d\r\n"

	//URL�еĲ���
	struct struHttpRequsetInfo
	{
		std::string strName;
		std::string strValue;
	};

	HttpRequest();
	~HttpRequest();
	
	//���ɱ��������ָ��
	static std::shared_ptr<HttpRequest> createSharedPointer();

	//���յ�http��Ϣ����ͨ������������������ɶ���ʵ��
	bool analyzeData(char* pData, int len);

	//��������
	void setServer(const char* pszServerIP, int nPort);
	void setRequestMethod(HttpSocket::RequestMethod eMethod);

	//ָ������URL�����������������ַ���˿ڵ�ַ��������
	//���磬http://[www.17xuexi.com[:8080]]/login[?username='szUserName'&password='szPassword']
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
	void clearParam(); //�������

public:
	std::string m_strServerIP; //ip
	int m_nServerPort; //�˿ں�

	HttpSocket::RequestMethod m_eRequestMethod; //get����post

	std::string m_strURL; //���ӵ�url��ַ������ֱ���ɲ���
	
	std::vector<struHttpRequsetInfo*> m_listParam;	//��Ҫ���ӵĲ���

	bool m_bWaitResponse; //�Ƿ�ȴ���Ӧ
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
	//Content-Length: xx
	//[Token: xx]
	//
	//[json]
	//[extra data]
#define http_response_data_packet \
	"HTTP/1.1 %d %s\r\nServer:llyHttpServer\r\nContent-Type:application/octet-stream\r\nConnection:%s\r\nJson-Length:%d\r\nContent-Length:%d\r\n"

	HttpResponse();
	~HttpResponse();

	//���ɱ��������ָ��
	static std::shared_ptr<HttpResponse> createSharedPointer();

	//��������
	bool analyzeData(char* pData, int len);

	//����http���ݰ�
	bool geneData(std::string &str);

	//�������
	void clear();
};


} //lly

#endif //_LLY_HTTP_SOCKET_H_
