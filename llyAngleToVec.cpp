#include "llyAngleToVec.h"
#include <math.h>

using namespace lly;

AngleToVec* lly::AngleToVec::m_A2V = nullptr;


lly::AngleToVec::AngleToVec()
{
	
}

lly::AngleToVec::~AngleToVec()
{

}

AngleToVec* lly::AngleToVec::getInstance()
{
	if (m_A2V == nullptr)
	{
		m_A2V = new AngleToVec();
		if (!m_A2V || !m_A2V->init())
		{
			if(m_A2V) delete m_A2V;
			m_A2V = nullptr;
			return nullptr;
		}
	}
	return m_A2V;
}

void lly::AngleToVec::destroyInstance()
{
	if(m_A2V) delete m_A2V;
	m_A2V = nullptr;
}

bool lly::AngleToVec::init()
{
	for (int i = 0; i < 450; ++i)
	{
		m_fVec[i] = sin(i * 3.14159f / 180);
	}
	return true;
}

//==========================================
static float xd;
static float yd;
static lly::AngleVec AV;

lly::AngleVec lly::getVecFrom2Points( float x, float y, float xOrig, float yOrig )
{
	xd = x - xOrig;
	yd = y - yOrig;
	AV.x = 1 / sqrtf(yd * yd / (xd * xd) + 1); //本身sqrt的效率已经非常高了
	AV.y = AV.x * yd / xd;
	return AV;
}

lly::AngleVec lly::getVecFrom2Points( float x, float y, float xOrig, float yOrig, float dis )
{
	xd = x - xOrig;
	yd = y - yOrig;
	AV.x = dis / sqrtf(yd * yd / (xd * xd) + 1);  //本身sqrt的效率已经非常高了
	AV.y = AV.x * yd / xd;
	return AV; 
}



