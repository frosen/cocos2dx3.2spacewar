#include "llyStream.h"

#ifdef WIN32
	#include <winsock2.h>
#else
	#include<arpa/inet.h>
#endif

using namespace lly;

lly::DataStream::DataStream(int room) : 
	m_pBuf(nullptr),
	m_lenAlloc(0),
	m_nPosBegin(0),
	m_nPosEnd(0)
{
	if (room > 0)
	{
		m_pBuf = (char*)malloc(room);
		m_lenAlloc = room;
	}
}

lly::DataStream::~DataStream()
{
	if (m_pBuf != nullptr) free(m_pBuf);
}

int lly::DataStream::getDataLen()
{
	return m_nPosEnd - m_nPosBegin; //从头到尾的距离
}

bool lly::DataStream::setPosBegin( int nPos )
{
	if (nPos < 0 || nPos > m_lenAlloc) return false;
	m_nPosBegin = nPos;
	return true;
}

bool lly::DataStream::addStream( const char* pBuf, int len )
{
	if (m_nPosBegin == m_nPosEnd) clear();

	if (m_lenAlloc - m_nPosEnd < len) //尾部剩余空间不够，不能直接复制
	{
		lenStream = m_nPosEnd - m_nPosBegin;

		if (m_lenAlloc - lenStream < len * 2) //空间不够则重新分配空间 *2是为了优化效率
		{
			int lenNeed = (m_nPosEnd - m_nPosBegin + len * 2) * 2; //获取所需要的两倍的空间
			char* pTmp = (char*)malloc(lenNeed);
			if (pTmp == nullptr) return false;
		
			memcpy(pTmp, m_pBuf+m_nPosBegin, lenStream); //把原来的流拷贝到新空间

			if (m_pBuf != nullptr) free(m_pBuf);
			m_pBuf = pTmp;
			m_lenAlloc = lenNeed;
		}
		else //空间够则把内容移动到流最前边
		{
			memmove(m_pBuf, m_pBuf+m_nPosBegin, lenStream);
		}		

		m_nPosBegin = 0;
		m_nPosEnd = lenStream;
	}

	memcpy(m_pBuf + m_nPosEnd, pBuf, len);
	m_nPosEnd += len;

	return true;
}

bool lly::DataStream::getStream( char* pBuf, int len )
{
	lenStream = m_nPosEnd - m_nPosBegin;

	if (lenStream == 0) return false;

	len = len < lenStream ? len : lenStream;

	memcpy(pBuf, m_pBuf + m_nPosBegin, len);
	m_nPosBegin += len;
	
	return true;
}

bool lly::DataStream::getChar( char &ch )
{
	lenStream = m_nPosEnd - m_nPosBegin;
	if (lenStream < sizeof(char)) return false;

	ch=*(m_pBuf + m_nPosBegin);
	++m_nPosBegin;

	return true;
}

bool lly::DataStream::getShortInt( short int &n )
{
	lenStream = m_nPosEnd - m_nPosBegin;
	if (lenStream < sizeof(short int)) return false;

	n = ntohs(*(short int *)(m_pBuf + m_nPosBegin));
	m_nPosBegin+=sizeof(short int);

	return true;
}

bool lly::DataStream::getInt( int &n )
{
	lenStream = m_nPosEnd - m_nPosBegin;
	if (lenStream < sizeof(int)) return false;

	n = ntohl(*(int *)(m_pBuf + m_nPosBegin));
	m_nPosBegin += sizeof(int);

	return true;
}

bool lly::DataStream::getFloat( float &f )
{
	lenStream = m_nPosEnd - m_nPosBegin;
	if (lenStream < sizeof(float)) return false;

	f = *(float *)(m_pBuf + m_nPosBegin);
	m_nPosBegin += sizeof(float);

	return true;
}

bool lly::DataStream::getString( std::string &str )
{
	for (int i = m_nPosBegin; i < m_nPosEnd; ++i)
	{
		if (m_pBuf[i] == 0) //获得从开始到第一个\0的字符
		{
			str = m_pBuf + m_nPosBegin;
			m_nPosBegin = i + 1;
			return true;
		}
	}
	return false;
}

bool lly::DataStream::putChar( char ch )
{
	return addStream(&ch, sizeof(char));
}

bool lly::DataStream::putShortInt( short int n )
{
	n=htons(n);
	return addStream((char*)&n, sizeof(short int));
}

bool lly::DataStream::putInt( int n )
{
	n=htonl(n);
	return addStream((char*)&n, sizeof(int));
}

bool lly::DataStream::putFloat( float f )
{
	return addStream((char*)&f, sizeof(float));
}

bool lly::DataStream::putString( const char* pStr )
{
	return addStream(pStr, strlen(pStr)+1);
}



