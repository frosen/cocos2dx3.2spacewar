/*
	名称：llyModifyVar.h
	描述：创造一个动作，可以让一个变量在一定时间内不断地被赋值，从一个开始的值到最终值变化
	作者：乐颜
	日期：2014.7.1
*/

#ifndef _MODIFY_FUNC_VAR_H_
#define _MODIFY_FUNC_VAR_H_

#include "cocos2d.h"

namespace lly {

class ModifyVar : public cocos2d::ActionInterval
{
public:
	ModifyVar();
	virtual ~ModifyVar();

	//通过一个初始量，变化时间，最终数值来修改一个接收改变后数值的回调函数
	static ModifyVar* create(std::function<void(float)> Modify,  float duration, float start,float destination);

	//override
	virtual ModifyVar* clone() const override;
	virtual ModifyVar* reverse(void) const override;	
	virtual void startWithTarget(cocos2d::Node *pTarget) override;
	virtual void update(float time) override;;

protected:
	bool initWithDuration(std::function<void(float)> Modify, float duration, float start, float destination);

	std::function<void(float)> Modify_cb; //回调

	float m_fDuration; //持续时间

	float m_fStart; //开始数值	
	float m_fDestination; //最终数值

	float m_fDelta; //每次刷新时的变化量
	int nPlusOrMinus; //int 1为正数，-1为负数

private:
	CC_DISALLOW_COPY_AND_ASSIGN(ModifyVar);
};

} //lly

#endif //_MODIFY_FUNC_VAR_H_




