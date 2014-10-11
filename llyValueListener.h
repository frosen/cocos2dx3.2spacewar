/*
名称：llyValueListener.h
描述：监听一个数值的变化，到一个规定数值后进行一个回调
作者：乐颜
日期：2014.7.2
*/

#ifndef _VALUE_LISTENER_H_
#define _VALUE_LISTENER_H_

#include "cocos2d.h"

namespace lly {

class ValueListener : public cocos2d::Ref
{
public:
	ValueListener();
	virtual ~ValueListener();

	//接收一个float的引用，监听指针指向数值的变化，然后运行一个回调函数
	static ValueListener* create(
		float* var, float aim, 
		std::function<void(void)> doAfter, float maxWaitingTime = -1.0f); 

	//接收一个回调函数，回调函数提供一个float的返回值，监听这个返回值，然后运行一个回调函数
	static ValueListener* create(
		std::function<float(void)> listen, float aim, 
		std::function<void(void)> doAfter, float maxWaitingTime = -1.0f); 

	//开始后的最长等待时间，如果数值大于0，则没到达规定数值而到达时间也会执行回调
	void setMaxWaitingSecond(float fTime) { m_fMaxWaitingSecond = fTime; }

	void run(); //开始运行

	void update(float dt); //检测变化的大循环

protected:
	virtual bool init(float* var, std::function<float(void)> listen, float aim, std::function<void(void)> doAfter, float maxWaitingTime);

	void onDone();

public:
	std::function<void(float)> excute_cb; //执行中的回调

protected:
	float* m_pfvar; //被监听的数值
	std::function<float(void)> listen_cb; //被监听的函数

	float m_fconstant; //常数
	int m_nType; //常数大 = -1；常数小 = 1；
	float fTemp; //临时变量，接受回调返回的float
	
	std::function<void(void)> doAfte_cb; //回调函数

	float m_fMaxWaitingSecond; //最长持续时间
	float m_fTimeDelay; //总持续时间
};

} //lly

#endif //_VALUE_LISTENER_H_





