/*
名称：llySafeInt.h
描述：封装一个int类型的数据并且加密，获得原始数值取负值并加上密码数值的结果储存，防止修改内存
		可做加减乘除一个int数值的计算，可以转换成普通int
nPrivate = n * (-1) + nPasswords
作者：乐颜
日期：2014.2.2
*/

#ifndef _SAFE_INT_H_
#define _SAFE_INT_H_

#include <random>

namespace lly {

static int getRandomInt()
{
	std::random_device rd ;
	std::default_random_engine e(rd()) ;
	std::uniform_int_distribution<> u (-9999999 , 9999999);
	return u(e);
}

class SafeInt
{
public:
	SafeInt() : nPrivate(0) {}
	~SafeInt() {}
	
	//赋值初始化
	SafeInt(int n) { SafeInt(); nPrivate = nPasswords - n; }

	//修改数值
	SafeInt& operator =(int n) { nPrivate = nPasswords - n; return *this; } 

	//加减运算
	SafeInt& operator +(int n) { nPrivate -= n; return *this; }//加法
	SafeInt& operator -(int n) { nPrivate += n; return *this; }//减法

	void operator +=(int n) { nPrivate -= n; }//加法
	void operator -=(int n) { nPrivate += n; }//减法·

	//乘除
	SafeInt& operator *(int n) { nPrivate = (nPrivate - nPasswords)*n + nPasswords; return *this; }//乘法
	SafeInt& operator /(int n) { nPrivate = (nPrivate - nPasswords)/n + nPasswords; return *this; }//除法·

	void operator *=(float n) { nPrivate = (nPrivate - nPasswords)*n + nPasswords; }//乘法
	void operator /=(float n) { nPrivate = (nPrivate - nPasswords)/n + nPasswords; }//除法·

	//转换成int
	operator int() { return nPasswords - nPrivate; } //获得数值

private:
	int nPrivate; //int数值
	static int nPasswords; //放置阻止非法获取nPrivate的密码
};

int SafeInt::nPasswords = getRandomInt();

} //lly

#endif //_SAFE_INT_H_





