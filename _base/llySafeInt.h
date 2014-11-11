/*
���ƣ�llySafeInt.h
��������װһ��int���͵����ݲ��Ҽ��ܣ����ԭʼ��ֵȡ��ֵ������������ֵ�Ľ�����棬��ֹ�޸��ڴ�
		�����Ӽ��˳�һ��int��ֵ�ļ��㣬����ת������ͨint
nPrivate = n * (-1) + nPasswords
���ߣ�����
���ڣ�2014.2.2
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
	
	//��ֵ��ʼ��
	SafeInt(int n) { SafeInt(); nPrivate = nPasswords - n; }

	//�޸���ֵ
	SafeInt& operator =(int n) { nPrivate = nPasswords - n; return *this; } 

	//�Ӽ�����
	SafeInt& operator +(int n) { nPrivate -= n; return *this; }//�ӷ�
	SafeInt& operator -(int n) { nPrivate += n; return *this; }//����

	void operator +=(int n) { nPrivate -= n; }//�ӷ�
	void operator -=(int n) { nPrivate += n; }//������

	//�˳�
	SafeInt& operator *(int n) { nPrivate = (nPrivate - nPasswords)*n + nPasswords; return *this; }//�˷�
	SafeInt& operator /(int n) { nPrivate = (nPrivate - nPasswords)/n + nPasswords; return *this; }//������

	void operator *=(float n) { nPrivate = (nPrivate - nPasswords)*n + nPasswords; }//�˷�
	void operator /=(float n) { nPrivate = (nPrivate - nPasswords)/n + nPasswords; }//������

	//ת����int
	operator int() { return nPasswords - nPrivate; } //�����ֵ

private:
	int nPrivate; //int��ֵ
	static int nPasswords; //������ֹ�Ƿ���ȡnPrivate������
};

int SafeInt::nPasswords = getRandomInt();

} //lly

#endif //_SAFE_INT_H_





