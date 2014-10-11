/*
llySocket.h
有跨平台处理
乐颜
2014.7.30
*/

#ifndef _LLY_SOCKET_H_
#define _LLY_SOCKET_H_

typedef unsigned int uint32_t;

#define MAX_UDP_PACKET			(65535-28)			//UDP数据包的最大限制

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

	//创建socket type：字节流还是数据报
	virtual bool create(int type, int protocol = 0);
	virtual int close(); //关闭socket

	static int initWin(); //初始化windows
	static int cleanWin();

	//链接服务器
	virtual bool connect(uint32_t nIP, unsigned short port);
	virtual bool connect(const char* ip, unsigned short port);

	//发送、接受数据
	virtual int send(const void* pBuf, int len, int flags = 0);
	virtual int recv(void* pBuf, int len, int flags = 0);

	//服务器绑定
	virtual bool bind(unsigned short port);
	virtual bool listen(int backlog = 5); 
	virtual bool accept(Socket &socket, char* fromip = nullptr);
	virtual bool accept(SOCKET &s, char *fromip = nullptr);

	virtual bool attach(SOCKET hSocket);

	//域名解析
	static bool parseDNS(const char* domain,char* ip);

	SOCKET getSocket() { return m_hSocket; }

	int getError();

	//UDP发送数据
	//pData:			要发送的数据
	//len:				数据长度
	//dwIP:				IP地址，允许INADDR_BROADCAST，host格式
	virtual bool sendTo(void* pData, int len, uint32_t nIP, int nPort);
	bool sendTo(void* pData, int len, const char* pszIP, int nPort); //pszIP: IP地址，为空表示发送广播

	//UDP接收
	int recvFrom(char* pData, int len);
	void getAddrFrom(sockaddr_in* pAddr); //数据来源

	//select
	int getSocketState(bool* pbReadable, bool* pbWritable, bool* pbException, int cnWaitMS = 0);
	int isReadable(int cnWaitMS = 0);
	int isWritable(int cnWaitMS = 0);

	//recv时可读取到数据的大小
	int getReadableSize();

	bool setBufSize(int nBufSize);

	//加入组播组，符合的组播地址范围：224.0.0.0--239.255.255.255
	bool joinGroup(uint32_t nIP);
	bool joinGroup(const char* pszIP);

	//退出组播
	bool leaveGroup(uint32_t nIP);
	bool leaveGroup(const char* pszIP);

	//设置数据回送
	//bLoop:			是否接收自己发出的广播数据包
	//注意：此设置仅对组播有效，如果是广播数据包无论如何都是会收到的
	//返回值：			true成功，false失败，调用GetLastError()检查错误码
	bool setLoop(bool bLoop);

	//设置TTL
	//nTTL:				数据包生命周期，每经过一个路由减一，默认1表示仅子网内播发
	//返回值：			true成功，false失败，调用GetLastError()检查错误码
	bool setTTL(int nTTL);

	//设置
	int setSocketOption(int level, int optname, const char* pValuePtr, int lenValue);
	int setSocketOption(int level, int optname, bool bOption);

	int setTimeout(int nTimeout);
	static void setDefaultTimeout(int nDefaultTimeout) { m_nDefaultTimeout = nDefaultTimeout; }

protected:
	static bool bInited; //是否已经初始化

	SOCKET m_hSocket;
	sockaddr_in m_addr;

	static int m_nDefaultTimeout;
};


} //namespace lly end

#endif //_LLY_SOCKET_H_

