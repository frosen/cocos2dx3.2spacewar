/*
	���ƣ�llyAcceDirCtrler.h
	�������������ٷ���������������ֻ�������Ӧ�������Ʒ���
				����ʼ��ʱ���ȡһ��ƽ��ֵ�����ζ��ֻ������ݵ�ǰƽ��ֵ��ԭֵ�Ĳ������ǰ�ƶ��ķ���
				�����Ϊ16�򣬿ɻ�ȡһ�������XY��ı�ֵ
	���ߣ�����
	���ڣ�2014.7.30
*/

#ifndef _ACCE_DIR_CTRLER_H_
#define _ACCE_DIR_CTRLER_H_

#include "cocos2d.h"

namespace lly{

//16����
enum EnDirection
{
	D0, D22p5, D45, D67p5,
	D90, D112p5, D135, D157p5,
	D180, D202p5, D225, D247p5,
	D270, D292p5, D315, D337p5,
	Dnone,
};

//ȡֵ0-1����ֵ��Ϊ����Ϊ1������·�����XY��ӳ���ֵ
class DirectionRatio
{
public:
	DirectionRatio(float xx, float yy);
	float x;
	float y;
};

class AcceDirCtrler
{
public:
#define cut22p5 (0.001915f)
#define cut45 (0.003535f)
#define cut67p5 (0.00462f)
#define cut90 (0.005f)

	AcceDirCtrler();
	virtual ~AcceDirCtrler();

	//����
	static AcceDirCtrler* getInstance();
	static void destroyInstance();

	//��ʼ���㷽��
	void start();

	//��������
	void stop();

	//��ͣ���㣬������һ��ʹ���������
	void pause();

	//���¼���
	void resume();

	//���¼��㣬������һ��ʹ���������
	void restrat();

	//��ȡ����
	EnDirection getDirection() { return m_CurDirction; }

	//��ȡ�����ֵ
	DirectionRatio getDirectionRatio() { return m_vDirRatio[m_CurDirction]; }

	//���������� n��0��20
	void setPrecision(int n);
	int getPrecision() { return m_nPrecison; }

	void getControlDirectionEvent(std::function<void(AcceDirCtrler*)> cb)
	{
		controlDirection_cb = cb;
	}

protected:
	virtual bool init();

protected:
	static AcceDirCtrler* s_ADCer; //����

	cocos2d::EventListenerAcceleration* listener;

	enum EnState //��������״̬
	{
		BEGIN, //��һ�μ�����٣����ó�ʼֵ
		MOVE, //����������٣����Ƽ������ƶ�
		PAUSE, //��ͣ״̬
	} m_enState; 

	float _fXorignal; //ԭֵ
	float _fYorignal; 

	float _fXbuffer; //����ֵ����ֹ���󶶶�
	float _fYbuffer;

	EnDirection m_CurDirction; //��ǰ����

	std::vector<DirectionRatio> m_vDirRatio; //�����б�

	struct value_Dir //x,y�������һ����Χ�ڶ�Ӧ��ʸ��
	{
		EnDirection e1;
		EnDirection e2;
		float rateMax; ///x,y����������С��������ʸ�����������ֵ	
		value_Dir() {}
		value_Dir(float f, EnDirection e1, EnDirection e2) : rateMax(f), e1(e1), e2(e2) {}
	} v_vD[10]; 

	int m_nPrecison; //������
	float m_fPrecision22p5; //�������з���
	float m_fPrecision45;
	float m_fPrecision67p5;
	float m_fPrecision90;

	float xD; //temp
	float yD;
	float rate;

	std::function<void(AcceDirCtrler*)> controlDirection_cb;
};

} //lly


#endif //_ACCE_DIR_CTRLER_H_