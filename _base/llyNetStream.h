/*
	���ƣ�llyNetStream.h
	�������Լ�д�������Ƚ��ȳ�����һ���ַ����������������Ż�Ч�ʣ��п�ƽ̨����
	���ߣ�����
	���ڣ�2014.8.3
*/

#include <string>

namespace lly{

class NetStream
{
public:
	NetStream(int room = 0);
	virtual ~NetStream();

	//������ݳ���
	int getDataLen();

	//������ʼλ��
	int getPosBegin() { return m_nPosBegin; }
	bool setPosBegin(int nPos);

	//���/�����
	bool addStream(const char *pBuf,int len);
	bool getStream(char *pBuf,int len);

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
	bool putString(const char *pStr);

	//�����������
	void clear() { m_nPosBegin = 0; m_nPosEnd = 0; }

protected:
	char *m_pBuf; //��
	int m_lenAlloc; //�ܳ���
	int m_nPosBegin; //��ͷ��λ��
	int m_nPosEnd; //��β��λ��

	int lenStream; //���ݳ���
};

//ģ��������������趨һ��ģ�壬��������ʹ��
template <class T>
class NetStreamT : public NetStream
{
public:
	NetStreamT(int room = 0) : NetStream(room) { _nTLen = sizeof(T); }
	virtual ~NetStreamT() {}

	bool putT(T &t) { return addStream((char *)&t, sizeof(T)); }
	bool getT(T &t);

	int getTLen() { return _nTLen; } //ģ�峤��
	int getNumOfT() { return getDataLen() / getTLen(); } //������ģ������

protected:
	int _nTLen;
};

template <class T>
bool lly::NetStreamT<T>::getT( T &t )
{
	if (m_nPosEnd - m_nPosBegin < _nTLen) return false;
	t = *(T*)(m_pBuf + m_nPosBegin);
	m_nPosBegin += _nTLen;
	return true;
}

} //lly




