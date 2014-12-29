/*
	���ƣ�llyAngleToVec.h
	�������ѽǶ�ת���������������Ը������������˽Ƕ�����
				��ʼ��0-360�ȵ�ʸ��ֵ������������
				����ʸ����x��y��ɣ�Ϊ�˽�ʡ�ռ䣬������0-450�ȣ�y���ֵ����x������90�ȵ�ֵ
	���ߣ�����
	���ڣ�2014.8.3
*/

#ifndef _ANGLE_TO_VEC_H_
#define _ANGLE_TO_VEC_H_

#ifndef A2V
#define A2V(angle) AngleToVec::getInstance()->getVec(angle)
#endif

#ifndef AD2V
#define AD2V(angle, dis) AngleToVec::getInstance()->getVec(angle, dis)
#endif

#ifndef PP2V
#define PP2V(x, y, xOrig, yOrig) getVecFrom2Points(x, y, xOrig, yOrig)
#endif

#ifndef PPD2V
#define PPD2V(x, y, xOrig, yOrig, dis) getVecFrom2Points(x, y, xOrig, yOrig, dis)
#endif

namespace lly {

class AngleVec
{
public:
	AngleVec(float xx = 0, float yy = 0) : x(xx), y(yy) {}
	float x;
	float y;
};

class AngleToVec
{
public:
	AngleToVec();
	~AngleToVec();

	//����
	static AngleToVec* getInstance();
	static void destroyInstance();

	//���ݽǶȻ�ȡ��������0��359��
	AngleVec getVec(int angle);

	//���ݽǶȺ;���������
	AngleVec getVec(int angle, float distance);

protected:
	virtual bool init();

protected:
	static AngleToVec* s_A2V; //����

	float m_fVec[450]; //��0��449�ȳ���Ϊ1��ʸ��ֵ
};

//��������λ�ü�������
AngleVec getVecFrom2Points(float x, float y, float xOrig, float yOrig);

//��������λ�ú;����������
AngleVec getVecFrom2Points(float x, float y, float xOrig, float yOrig, float dis);

} //lly end

#endif //_ANGLE_TO_VEC_H_



