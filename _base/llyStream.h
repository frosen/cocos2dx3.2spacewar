/*
	���ƣ�llyStream.h
	�������Լ�д�������Ƚ��ȳ�����һ���ַ����������������Ż�Ч�ʣ��п�ƽ̨����
	���ߣ�����
	���ڣ�2014.8.3
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

	//������ݳ���
	int getDataLen();

	//������ʼλ��
	int getPosBegin() { return m_nPosBegin; }
	bool setPosBegin(int nPos);

	//���/�����
	bool addStream(const char* pBuf, int len);
	bool getStream(char* pBuf, int len);

	//�����
	bool getChar(char &ch);
	bool getShortInt(short int &n);
	bool getInt(int &n);
	bool getFloat(float &f);
	bool getString(std::string &str);

	//�����
	bool putChar(char ch);
	bool putShortInt(short int n);
	bool putInt(int n);
	bool putFloat(float f);
	bool putString(const char* pStr);

	//�����������
	void clear() { m_nPosBegin = 0; m_nPosEnd = 0; }

protected:
	char* m_pBuf; //��
	int m_lenAlloc; //�ܳ���
	int m_nPosBegin; //��ͷ��λ��
	int m_nPosEnd; //��β��λ��

	int lenStream; //���ݳ���
};

//ģ��������������趨һ��ģ�壬��������ʹ�ã����Է����κεĶ���
template <class T>
class StructStream
{
public:
	StructStream(int room = 0);
	virtual ~StructStream();

	int getNumOfStruct() { return m_nPosEnd - m_nPosBegin; } //������ģ������

	//������ʼλ��
	int getPosBegin() { return m_nPosBegin; }
	bool setPosBegin(int nPos);

	bool putStruct(T &t);
	bool getStruct(T &t);

	//�����������
	void clear() { m_nPosBegin = 0; m_nPosEnd = 0; }

protected:
	T* m_pBuf; //�ṹ��ָ�룬ָ��һ������
	int m_lenAlloc; //�ܳ���
	int m_nPosBegin; //��ͷ��λ��
	int m_nPosEnd; //��β��λ��

	int lenStream; //���ݳ���
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

	if (m_nPosEnd == m_lenAlloc) //β��û��ʣ���λ������ֱ�Ӹ���
	{
		lenStream = m_nPosEnd - m_nPosBegin;

		if (m_lenAlloc - lenStream < 2) //�ռ䲻�������·���ռ䣬��2��Ϊ���Ż�Ч��
		{
			int lenNeed = (m_nPosEnd - m_nPosBegin + 1) * 2; //��ȡ����Ҫ�������Ŀռ�
			T* pTmp = new T[lenNeed];
			if (pTmp == nullptr) return false;

			if (m_nPosBegin != m_nPosEnd)
			{
				int n = 0;
				for (int i = m_nPosBegin; i <= m_nPosEnd; ++i) //��ԭ�������������¿ռ�
				{
					pTmp[n] = m_pBuf[i];
					++n;
				}
			}
			
			if (m_pBuf != nullptr) delete [] m_pBuf;
			m_pBuf = pTmp;
			m_lenAlloc = lenNeed;
		}
		else //�ռ乻��������ƶ�������ǰ��
		{
			int n = 0;
			for (int i = m_nPosBegin; i <= m_nPosEnd; ++i) //��ԭ�������������¿ռ�
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


