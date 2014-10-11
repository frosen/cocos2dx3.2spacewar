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

<<<<<<< HEAD
//=================================================
=======
//==========================================
static float xd;
static float yd;
static lly::AngleVec AV;

<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> parent of b3b9793... æ ¹æ®ä»£ç å¤§å…¨ä¸Šçš„å†…å®¹ï¼Œä¼˜åŒ–ä»£ç 
=======
>>>>>>> parent of b3b9793... æ ¹æ®ä»£ç å¤§å…¨ä¸Šçš„å†…å®¹ï¼Œä¼˜åŒ–ä»£ç 
=======
>>>>>>> parent of b3b9793... æ ¹æ®ä»£ç å¤§å…¨ä¸Šçš„å†…å®¹ï¼Œä¼˜åŒ–ä»£ç 
lly::AngleVec lly::getVecFrom2Points( float x, float y, float xOrig, float yOrig )
{
	xd = x - xOrig;
	yd = y - yOrig;
	AV.x = 1 / sqrtf(yd * yd / (xd * xd) + 1); //±¾ÉísqrtµÄĞ§ÂÊÒÑ¾­·Ç³£¸ßÁË
	AV.y = AV.x * yd / xd;
	return AV;
}

lly::AngleVec lly::getVecFrom2Points( float x, float y, float xOrig, float yOrig, float dis )
{
	xd = x - xOrig;
	yd = y - yOrig;
	AV.x = dis / sqrtf(yd * yd / (xd * xd) + 1);  //±¾ÉísqrtµÄĞ§ÂÊÒÑ¾­·Ç³£¸ßÁË
	AV.y = AV.x * yd / xd;
	return AV; 
}



