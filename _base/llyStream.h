/*
	名称：llyStream.h
	描述：自己写的流，先进先出，用一个字符串放置流，尽量优化效率，有跨平台处理
	作者：乐颜
	日期：2014.8.3
*/

#ifndef _LLY_STREAM_H_
#define _LLY_STREAM_H_

#include <string>

namespace lly{

class DataStream
{
public:
	DataStream(int room = 0);
	virtual ~DataStream();

	//获得数据长度
	int getDataLen();

	//数据起始位置
	int getPosBegin() { return m_nPosBegin; }
	bool setPosBegin(int nPos);

	//添加/获得流
	bool addStream(const char* pBuf, int len);
	bool getStream(char* pBuf, int len);

	//获得流
	bool getChar(char &ch);
	bool getShortInt(short int &n);
	bool getInt(int &n);
	bool getFloat(float &f);
	bool getString(std::string &str);

	//添加流
	bool putChar(char ch);
	bool putShortInt(short int n);
	bool putInt(int n);
	bool putFloat(float f);
	bool putString(const char* pStr);

	//清除流内内容
	void clear() { m_nPosBegin = 0; m_nPosEnd = 0; }

protected:
	char* m_pBuf; //流
	int m_lenAlloc; //总长度
	int m_nPosBegin; //流头的位置
	int m_nPosEnd; //流尾的位置

	int lenStream; //内容长度
};

//模板类的网络流，设定一个模板，进行流的使用，可以放置任何的东西
template <class T>
class StructStream
{
public:
	StructStream(int room = 0);
	virtual ~StructStream();

	int getNumOfStruct() { return m_nPosEnd - m_nPosBegin; } //数据中模板数量

	//数据起始位置
	int getPosBegin() { return m_nPosBegin; }
	bool setPosBegin(int nPos);

	bool putStruct(T &t);
	bool getStruct(T &t);

	//清除流内内容
	void clear() { m_nPosBegin = 0; m_nPosEnd = 0; }

protected:
	T* m_pBuf; //结构的指针，指向一个数组
	int m_lenAlloc; //总长度
	int m_nPosBegin; //流头的位置
	int m_nPosEnd; //流尾的位置

	int lenStream; //内容长度
};

template <class T>
lly::StructStream<T>::StructStream( int room /*= 0*/ ) :
	m_pBuf(nullptr),
	m_lenAlloc(0),
	m_nPosBegin(0),
	m_nPosEnd(0)
{
	if (room > 0)
	{
		m_pBuf = new T[room];
		m_lenAlloc = room;
	}
}

template <class T>
lly::StructStream<T>::~StructStream()
{
	if (m_pBuf) delete [] m_pBuf;
}

template <class T>
bool lly::StructStream<T>::setPosBegin( int nPos )
{
	if (nPos < 0 || nPos > m_lenAlloc) return false;
	m_nPosBegin = nPos;
	return true;
}

template <class T>
bool lly::StructStream<T>::putStruct( T &t )
{
	if (m_nPosBegin == m_nPosEnd) clear();

	if (m_nPosEnd == m_lenAlloc) //尾部没有剩余空位，不能直接复制
	{
		lenStream = m_nPosEnd - m_nPosBegin;

		if (m_lenAlloc - lenStream < 2) //空间不够则重新分配空间，用2是为了优化效率
		{
			int lenNeed = (m_nPosEnd - m_nPosBegin + 1) * 2; //获取所需要的两倍的空间
			T* pTmp = new T[lenNeed];
			if (pTmp == nullptr) return false;

			if (m_nPosBegin != m_nPosEnd)
			{
				int n = 0;
				for (int i = m_nPosBegin; i <= m_nPosEnd; ++i) //把原来的流拷贝到新空间
				{
					pTmp[n] = m_pBuf[i];
					++n;
				}
			}
			
			if (m_pBuf != nullptr) delete [] m_pBuf;
			m_pBuf = pTmp;
			m_lenAlloc = lenNeed;
		}
		else //空间够则把内容移动到流最前边
		{
			int n = 0;
			for (int i = m_nPosBegin; i <= m_nPosEnd; ++i) //把原来的流拷贝到新空间
			{
				m_pBuf[n] = m_pBuf[i];
				++n;
			}
		}		

		m_nPosBegin = 0;
		m_nPosEnd = lenStream;
	}

	m_pBuf[m_nPosEnd] = t;
	++m_nPosEnd;

	return true;
}

template <class T>
bool lly::StructStream<T>::getStruct( T &t )
{
	if (m_nPosBegin == m_nPosEnd) return false;
	t = m_pBuf[m_nPosBegin];
	++m_nPosBegin;
	return true;
}

} //lly

#endif //_LLY_NET_STREAM_H_


