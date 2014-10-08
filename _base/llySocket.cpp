#include "llySocket.h"
#include <stdio.h>
#include <string>

#ifdef WIN32
	#pragma comment(lib, "wsock32")
	#include "ws2ipdef.h"
	#pragma warning( disable : 4996) 
#else
	#include <sys/ioctl.h>
#endif

using namespace lly;

bool Socket::bInited = false;

//默认超时60秒
int Socket::m_nDefaultTimeout = 60000;

Socket::Socket() : m_hSocket(INVALID_SOCKET)
{
	
}

Socket::~Socket()
{
	
}

bool Socket::create(int type,int protocol)
{
	if (initWin() != 0) return false;

	//已经打开过的，先要关闭
	if (m_hSocket != INVALID_SOCKET) close();
	m_hSocket = ::socket(AF_INET, type, protocol);
	if (m_hSocket == INVALID_SOCKET) return false;

	setTimeout(m_nDefaultTimeout);
	return true;
}

int Socket::close()
{
	//socket不存在
	if (m_hSocket == INVALID_SOCKET) return 0;
#ifdef WIN32
	int nRet = ::closesocket(m_hSocket);
#else
	int nRet = ::close(m_hSocket);
#endif
	m_hSocket = INVALID_SOCKET;
	return nRet;
}

int Socket::initWin()
{
#ifdef WIN32
	if (!bInited)
	{
		WSADATA wsaData;
		WORD version = MAKEWORD(2, 0);
		int ret = WSAStartup(version, &wsaData);
		if (ret) return SOCKET_ERROR;

		bInited = true;
	}
#endif
	return 0;
}

int Socket::cleanWin()
{
#ifdef WIN32
	return WSACleanup();
#else
	return 0;
#endif
}

bool Socket::connect(uint32_t nIP, unsigned short port)
{
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = htonl(nIP);
	svraddr.sin_port = htons(port);
	int ret = ::connect(m_hSocket, (struct sockaddr*)&svraddr, sizeof(svraddr));
	if (ret == SOCKET_ERROR) return false;

	return true;
}

bool Socket::connect(const char* ip, unsigned short port)
{
	return connect(ntohl(inet_addr(ip)), port);
}

int Socket::send(const void *pBuf,int len,int flags)
{
	int bytes;
	int count = 0;

	while (count < len)
	{
		bytes = ::send(m_hSocket,((const char *)pBuf)+count,len-count,flags);
		if (bytes == SOCKET_ERROR || bytes == 0) return SOCKET_ERROR;

		count += bytes;
	} 
	return count;
}

int Socket::recv(void *pBuf,int len,int flags)
{
	return (::recv(m_hSocket, (char*)pBuf, len, flags));
}

bool Socket::bind(unsigned short port)
{
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = INADDR_ANY;
	svraddr.sin_port = htons(port);

	int opt = 1;
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) return false;

	int ret = ::bind(m_hSocket, (struct sockaddr*)&svraddr, sizeof(svraddr));
	if (ret == SOCKET_ERROR) return false;

	return true;
}

bool Socket::listen(int backlog)
{
	int ret = ::listen(m_hSocket, backlog);
	if (ret == SOCKET_ERROR) return false;

	return true;
}

bool Socket::accept(SOCKET &s, char* fromip)
{
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	s = ::accept(m_hSocket, (struct sockaddr*)&cliaddr, &addrlen);
	if (s == SOCKET_ERROR) return false;

	if (fromip != nullptr) sprintf(fromip, "%s", inet_ntoa(cliaddr.sin_addr));

	return true;
}

bool Socket::accept(Socket &socket, char *fromip)
{
	SOCKET s;
	if (!accept(s, fromip)) return false;

	socket.attach(s);
	return true;
}

bool Socket::attach(SOCKET hSocket)
{
	if (m_hSocket != INVALID_SOCKET) close();
	m_hSocket = hSocket;
	return true;
}

bool Socket::parseDNS(const char *domain, char *ip)
{
	struct hostent* p = gethostbyname(domain);
	if (p == nullptr) return false;

	sprintf(ip, "%u.%u.%u.%u", 
		(unsigned char)p->h_addr_list[0][0], (unsigned char)p->h_addr_list[0][1], 
		(unsigned char)p->h_addr_list[0][2], (unsigned char)p->h_addr_list[0][3]);
	return true;
}

int Socket::getError()
{
#ifdef WIN32
	return (WSAGetLastError());
#else
	return (errno);
#endif
}

bool Socket::sendTo(void*pData, int len, uint32_t nIP, int nPort)
{
	if (m_hSocket == INVALID_SOCKET) return false;

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	addr.sin_addr.s_addr = htonl(nIP);

	//UDP消息不存在发送一半数据的问题，要么发送成功，要么失败或者阻塞
	int nRet = ::sendto(m_hSocket, (char*)pData, len, 0, (sockaddr*)&addr, sizeof(addr));
	if (nRet == SOCKET_ERROR) return false;

	return true;
}

bool Socket::sendTo(void* pData, int len, const char*pszIP, int nPort)
{
	uint32_t nIP;
	if (pszIP == nullptr) nIP = INADDR_BROADCAST;
	else nIP = ntohl(inet_addr(pszIP));
	return sendTo(pData, len, nIP, nPort);
}

int Socket::recvFrom(char*pData, int len)
{
	int size = sizeof(m_addr);
	int nRet = ::recvfrom(m_hSocket, pData, len, 0, (sockaddr*)&m_addr, &size);
	return nRet;
}

void Socket::getAddrFrom(sockaddr_in* pAddr)
{
	memmove(pAddr, &m_addr, sizeof(m_addr));
}

int Socket::getSocketState(bool* pbReadable, bool* pbWritable, bool* pbException, int cnWaitMS)
{
	if (m_hSocket == INVALID_SOCKET) return SOCKET_ERROR;

	struct timeval tv;
	tv.tv_sec = cnWaitMS / 1000;
	tv.tv_usec = (cnWaitMS % 1000) * 1000;

	fd_set rfs, wfs, efs;
	fd_set *prfs, *pwfs, *pefs;

	int cnSelect = 1;
	if (pbReadable)
	{
		prfs = &rfs;
		FD_ZERO(prfs);
		FD_SET(m_hSocket, prfs);
		*pbReadable = false;
		++cnSelect;
	}
	else prfs = nullptr;

	if (pbWritable)
	{
		pwfs = &wfs;
		FD_ZERO(pwfs);
		FD_SET(m_hSocket, pwfs);
		*pbWritable=false;
		cnSelect++;
	}
	else pwfs=nullptr;

	if (pbException)
	{
		pefs = &efs;
		FD_ZERO(pefs);
		FD_SET(m_hSocket, pefs);
		*pbException = false;
		++cnSelect;
	}
	else pefs = nullptr;

	int nRet;
	nRet = select(cnSelect, prfs, pwfs, pefs, &tv);

	if (nRet == 0) return 0;
	if (nRet == SOCKET_ERROR) return SOCKET_ERROR;

	if (prfs && FD_ISSET(m_hSocket, prfs)) *pbReadable=true;
	if (pwfs && FD_ISSET(m_hSocket, pwfs)) *pbWritable=true;
	if (pefs && FD_ISSET(m_hSocket, pefs)) *pbException=true;

	return 0;
}

int Socket::isReadable(int cnWaitMS)
{
	bool bReadable;
	getSocketState(&bReadable, nullptr, nullptr, cnWaitMS);
	return bReadable;
}

int Socket::isWritable(int cnWaitMS)
{
	bool bWritable;
	getSocketState(nullptr, &bWritable, nullptr, cnWaitMS);
	return bWritable;
}

int Socket::getReadableSize()
{
	unsigned long l;
#ifdef WIN32
	int nRet = ioctlsocket(m_hSocket, FIONREAD, &l);
#else
	int nRet = ioctl(m_hSocket, FIONREAD, &l);
#endif
	if (nRet < 0) return nRet;
	return l;
}

bool Socket::setBufSize(int nBufSize)
{
	if (m_hSocket == INVALID_SOCKET) return false;

	setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nBufSize, sizeof(nBufSize));
	setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nBufSize, sizeof(nBufSize));
	return true;
}

bool Socket::joinGroup(uint32_t nIP)
{
	if (m_hSocket == INVALID_SOCKET) return false;

	struct ip_mreq ipmr;
	ipmr.imr_multiaddr.s_addr = htonl(nIP);
	ipmr.imr_interface.s_addr = INADDR_ANY;

	if (setsockopt(m_hSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&ipmr, sizeof(ipmr)) == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}

bool Socket::joinGroup(const char* pszIP)
{
	return joinGroup(ntohl(inet_addr(pszIP)));
}

bool Socket::leaveGroup(uint32_t nIP)
{
	if (m_hSocket == INVALID_SOCKET) return false;

	struct ip_mreq ipmr;
	ipmr.imr_multiaddr.s_addr = htonl(nIP);
	ipmr.imr_interface.s_addr = INADDR_ANY;

	if (setsockopt(m_hSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&ipmr, sizeof(ipmr)) == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}

bool Socket::leaveGroup(const char* pszIP)
{
	return leaveGroup(ntohl(inet_addr(pszIP)));
}

bool Socket::setLoop(bool bLoop)
{
	if (m_hSocket == INVALID_SOCKET) return false;

	if (setsockopt(m_hSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&bLoop, sizeof(bool)) == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}

bool Socket::setTTL(int nTTL)
{
	if (m_hSocket == INVALID_SOCKET) return false;

	if (setsockopt(m_hSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&nTTL, sizeof(int)) == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}

int Socket::setSocketOption(int level, int optname, const char* pValuePtr, int lenValue)
{
	return setsockopt(m_hSocket, level, optname, pValuePtr, lenValue);
}

int Socket::setSocketOption(int level, int optname, bool bOption)
{
	return setsockopt(m_hSocket, level, optname, (char*)&bOption, sizeof(bool));
}

int Socket::setTimeout(int nTimeout)
{
#ifdef WIN32
	int nRet = setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nTimeout, sizeof(nTimeout));
	if (nRet == 0) nRet = setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeout, sizeof(nTimeout));
#else
	struct timeval tv;
	tv.tv_sec = nTimeout / 1000;
	tv.tv_usec = (nTimeout % 1000) * 1000;
	int nRet = setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));
	if (nRet == 0) nRet = setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
#endif

	return nRet;
}


