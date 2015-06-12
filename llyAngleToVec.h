/*
	名称：llyAngleToVec.h
	描述：把角度转换成向量，还可以根据两点计算出此角度向量
				初始化0-360度的矢量值，查表得向量，
				由于矢量由x，y组成，为了节省空间，则载入0-450度，y轴的值等于x轴增加90度的值
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
	AngleVec getVec(int angle);

	//根据角度和距离获得向量
	AngleVec getVec(int angle, float distance);

protected:
	virtual bool init();

protected:
	static AngleToVec* s_A2V; //单例

	float m_fVec[450]; //从0到449度长度为1的矢量值
};

//根据两点位置计算向量
AngleVec getVecFrom2Points(float x, float y, float xOrig, float yOrig);

//根据两点位置和距离计算向量
AngleVec getVecFrom2Points(float x, float y, float xOrig, float yOrig, float dis);

} //lly end

#endif //_ANGLE_TO_VEC_H_



