#include "llyAngleToVec.h"
#include <math.h>

using namespace lly;

AngleToVec* lly::AngleToVec::s_A2V = nullptr;


lly::AngleToVec::AngleToVec()
{
	
}

lly::AngleToVec::~AngleToVec()
{

}

AngleToVec* lly::AngleToVec::getInstance()
{
	if (s_A2V == nullptr)
	{
		s_A2V = new (std::nothrow) AngleToVec();
		if (!s_A2V || !s_A2V->init())
		{
			if(s_A2V) delete s_A2V;
			s_A2V = nullptr;
			return nullptr;
		}
	}
	return s_A2V;
}

void lly::AngleToVec::destroyInstance()
{
	if(s_A2V) delete s_A2V;
	s_A2V = nullptr;
}

lly::AngleVec lly::AngleToVec::getVec( int angle )
{
	return AngleVec(m_fVec[angle], m_fVec[angle + 90]);
}

lly::AngleVec lly::AngleToVec::getVec( int angle, float distance )
{
	return AngleVec(m_fVec[angle] * distance, m_fVec[angle + 90] * distance);
}

bool lly::AngleToVec::init()
{
	//��ʼ��0-360�ȵ�ʸ��ֵ������ʸ����x��y��ɣ�Ϊ�˽�ʡ�ռ䣬������0-450�ȣ�y���ֵ����x������90�ȵ�ֵ
	for (int i = 0; i < 450; ++i) 
	{
		m_fVec[i] = sin(i * 3.14159f / 180);
	}
	return true;
}

//=================================================
lly::AngleVec lly::getVecFrom2Points( float x, float y, float xOrig, float yOrig )
{
	float xd = x - xOrig;
	float yd = y - yOrig;
	lly::AngleVec AV;
	AV.x = 1 / sqrtf(yd * yd / (xd * xd) + 1); //����sqrt��Ч���Ѿ��ǳ�����
	AV.y = AV.x * yd / xd;
	return AV;
}

lly::AngleVec lly::getVecFrom2Points( float x, float y, float xOrig, float yOrig, float dis )
{
	float xd = x - xOrig;
	float yd = y - yOrig;
	lly::AngleVec AV;
	AV.x = dis / sqrtf(yd * yd / (xd * xd) + 1);  //����sqrt��Ч���Ѿ��ǳ�����
	AV.y = AV.x * yd / xd;
	return AV; 
}



