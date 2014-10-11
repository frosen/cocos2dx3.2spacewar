/*
	名称：llyAcceDirCtrler.h
	描述：重力加速方向控制器，利用手机重力感应器来控制方向
				当开始的时候获取一个平衡值，随后晃动手机，根据当前平衡值和原值的差决定当前移动的方向
				方向分为16向，可获取一个方向和XY轴的比值
	作者：乐颜
	日期：2014.7.30
*/

#ifndef _ACCE_DIR_CTRLER_H_
#define _ACCE_DIR_CTRLER_H_

#include "cocos2d.h"

namespace lly{

//16方向
enum EnDirection
{
	D0, D22p5, D45, D67p5,
	D90, D112p5, D135, D157p5,
	D180, D202p5, D225, D247p5,
	D270, D292p5, D315, D337p5,
	Dnone,
};

//取值0-1的数值，为距离为1的情况下方向在XY上映射的值
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

	//单例
	static AcceDirCtrler* getInstance();
	static void destroyInstance();

	//开始计算方向
	void start();

	//结束计算
	void stop();

	//暂停计算，过程中一般使用这个函数
	void pause();

	//重新计算
	void resume();

	//重新计算，过程中一般使用这个函数
	void restrat();

	//获取方向
	EnDirection getDirection() { return m_CurDirction; }

	//获取方向比值
	DirectionRatio getDirectionRatio() { return m_vDirRatio[m_CurDirction]; }

	//设置灵敏度 n从0到20
	void setPrecision(int n);
	int getPrecision() { return m_nPrecison; }

	void getControlDirectionEvent(std::function<void(AcceDirCtrler*)> cb)
	{
		controlDirection_cb = cb;
	}

protected:
	virtual bool init();

protected:
	static AcceDirCtrler* s_ADCer; //单例

	cocos2d::EventListenerAcceleration* listener;

	enum EnState //加速器的状态
	{
		BEGIN, //第一次计算加速，设置初始值
		MOVE, //正常计算加速，控制监听者移动
		PAUSE, //暂停状态
	} m_enState; 

	float _fXorignal; //原值
	float _fYorignal; 

	float _fXbuffer; //缓冲值，防止对象抖动
	float _fYbuffer;

	EnDirection m_CurDirction; //当前方向

	std::vector<DirectionRatio> m_vDirRatio; //比率列表

	struct value_Dir //x,y轴比例在一定范围内对应的矢量
	{
		EnDirection e1;
		EnDirection e2;
		float rateMax; ///x,y轴比例比这个小，则两轴矢量等于上面的值	
		value_Dir() {}
		value_Dir(float f, EnDirection e1, EnDirection e2) : rateMax(f), e1(e1), e2(e2) {}
	} v_vD[10]; 

	int m_nPrecison; //灵敏度
	float m_fPrecision22p5; //灵敏度切分量
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