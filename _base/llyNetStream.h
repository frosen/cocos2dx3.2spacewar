/*
	名称：llyNetStream.h
	描述：自己写的流，先进先出，用一个字符串放置流，尽量优化效率，有跨平台处理
	作者：乐颜
	日期：2014.8.3
*/

#include <string>

namespace lly{

class NetStream
{
public:
	NetStream(int room = 0);
	virtual ~NetStream();

	//获得数据长度
	int getDataLen();

	//数据起始位置
	int getPosBegin() { return m_nPosBegin; }
	bool setPosBegin(int nPos);

	//添加/获得流
	bool addStream(const char *pBuf,int len);
	bool getStream(char *pBuf,int len);

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
	bool putString(const char *pStr);

	//清除流内内容
	void clear() { m_nPosBegin = 0; m_nPosEnd = 0; }

protected:
	char *m_pBuf; //流
	int m_lenAlloc; //总长度
	int m_nPosBegin; //流头的位置
	int m_nPosEnd; //流尾的位置

	int lenStream; //内容长度
};

//模板类的网络流，设定一个模板，进行流的使用
template <class T>
class NetStreamT : public NetStream
{
public:
	NetStreamT(int room = 0) : NetStream(room) { _nTLen = sizeof(T); }
	virtual ~NetStreamT() {}

	bool putT(T &t) { return addStream((char *)&t, sizeof(T)); }
	bool getT(T &t);

	int getTLen() { return _nTLen; } //模板长度
	int getNumOfT() { return getDataLen() / getTLen(); } //数据中模板数量

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




