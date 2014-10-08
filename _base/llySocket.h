/*
llySocket.h
�п�ƽ̨����
����
2014.7.30
*/

#ifndef _LLY_SOCKET_H_
#define _LLY_SOCKET_H_

typedef unsigned int uint32_t;

#define MAX_UDP_PACKET			(65535-28)			//UDP���ݰ����������

#ifdef WIN32
	#include <winsock2.h>
	typedef int socklen_t;
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <arpa/inet.h>

	#include <errno.h>

	typedef int SOCKET;

	#define INVALID_SOCKET	-1
	#define SOCKET_ERROR	-1
#endif

namespace lly{

class Socket
{
public:
	Socket();
	~Socket();

	//����socket type���ֽ����������ݱ�
	virtual bool create(int type, int protocol = 0);
	virtual int close(); //�ر�socket

	static int initWin(); //��ʼ��windows
	static int cleanWin();

	//���ӷ�����
	virtual bool connect(uint32_t nIP, unsigned short port);
	virtual bool connect(const char* ip, unsigned short port);

	//���͡���������
	virtual int send(const void* pBuf, int len, int flags = 0);
	virtual int recv(void* pBuf, int len, int flags = 0);

	//��������
	virtual bool bind(unsigned short port);
	virtual bool listen(int backlog = 5); 
	virtual bool accept(Socket &socket, char* fromip = nullptr);
	virtual bool accept(SOCKET &s, char *fromip = nullptr);

	virtual bool attach(SOCKET hSocket);

	//��������
	static bool parseDNS(const char* domain,char* ip);

	SOCKET getSocket() { return m_hSocket; }

	int getError();

	//UDP��������
	//pData:			Ҫ���͵�����
	//len:				���ݳ���
	//dwIP:				IP��ַ������INADDR_BROADCAST��host��ʽ
	virtual bool sendTo(void* pData, int len, uint32_t nIP, int nPort);
	bool sendTo(void* pData, int len, const char* pszIP, int nPort); //pszIP: IP��ַ��Ϊ�ձ�ʾ���͹㲥

	//UDP����
	int recvFrom(char* pData, int len);
	void getAddrFrom(sockaddr_in* pAddr); //������Դ

	//select
	int getSocketState(bool* pbReadable, bool* pbWritable, bool* pbException, int cnWaitMS = 0);
	int isReadable(int cnWaitMS = 0);
	int isWritable(int cnWaitMS = 0);

	//recvʱ�ɶ�ȡ�����ݵĴ�С
	int getReadableSize();

	bool setBufSize(int nBufSize);

	//�����鲥�飬���ϵ��鲥��ַ��Χ��224.0.0.0--239.255.255.255
	bool joinGroup(uint32_t nIP);
	bool joinGroup(const char* pszIP);

	//�˳��鲥
	bool leaveGroup(uint32_t nIP);
	bool leaveGroup(const char* pszIP);

	//�������ݻ���
	//bLoop:			�Ƿ�����Լ������Ĺ㲥���ݰ�
	//ע�⣺�����ý����鲥��Ч������ǹ㲥���ݰ�������ζ��ǻ��յ���
	//����ֵ��			true�ɹ���falseʧ�ܣ�����GetLastError()��������
	bool setLoop(bool bLoop);

	//����TTL
	//nTTL:				���ݰ��������ڣ�ÿ����һ��·�ɼ�һ��Ĭ��1��ʾ�������ڲ���
	//����ֵ��			true�ɹ���falseʧ�ܣ�����GetLastError()��������
	bool setTTL(int nTTL);

	//����
	int setSocketOption(int level, int optname, const char* pValuePtr, int lenValue);
	int setSocketOption(int level, int optname, bool bOption);

	int setTimeout(int nTimeout);
	static void setDefaultTimeout(int nDefaultTimeout) { m_nDefaultTimeout = nDefaultTimeout; }

protected:
	static bool bInited; //�Ƿ��Ѿ���ʼ��

	SOCKET m_hSocket;
	sockaddr_in m_addr;

	static int m_nDefaultTimeout;
};


} //namespace lly end

#endif //_LLY_SOCKET_H_

