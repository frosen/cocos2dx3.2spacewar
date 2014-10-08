/*
	名称：llyAngleToVec.h
	描述：把角度转换成向量，还可以根据两点计算出此角度向量
	作者：乐颜
	日期：2014.8.3
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

	//单例
	static AngleToVec* getInstance();
	static void destroyInstance();

	//根据角度获取向量（从0到359）
	AngleVec getVec(int angle) 
	{ return AngleVec(m_fVec[angle], m_fVec[angle + 90]); }

	//根据角度和距离获得向量
	AngleVec getVec(int angle, float distance) 
	{ return AngleVec(m_fVec[angle] * distance, m_fVec[angle + 90] * distance); }

protected:
	virtual bool init();

protected:
	static AngleToVec* m_A2V; //单例

	float m_fVec[450]; //从0到449度
};

//根据两点位置计算向量
AngleVec getVecFrom2Points(float x, float y, float xOrig, float yOrig);

//根据两点位置和距离计算向量
AngleVec getVecFrom2Points(float x, float y, float xOrig, float yOrig, float dis);

} //lly end

#endif //_ANGLE_TO_VEC_H_



