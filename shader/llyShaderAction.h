/*
	名称：llyShaderAction.h
	描述：创造一个动作，可以在一段时间内使用一个渲染，结束后变回原来的渲染
				渲染要封装成effect效果类
				载入的渲染文件，必须接受一个名为“ActionTime"的float uniform，调用行动的update函数中对时间的控制
	作者：乐颜
	日期：2014.7.1
*/

#ifndef _LLY_SHADER_ACTION_H_
#define _LLY_SHADER_ACTION_H_

#include "cocos2d.h"

namespace lly {

class ShaderEffectBase;

class ShaderAction : public cocos2d::ActionInterval
{
public:
	//在一段时间内使用一个渲染，结束后变回原来的渲染，渲染会随时间变化
	static ShaderAction* create(float duration, ShaderEffectBase* effect);

	//override
	virtual ShaderAction* clone() const override;
	virtual ShaderAction* reverse(void) const override;	
	virtual void startWithTarget(cocos2d::Node *pTarget) override;
	virtual void update(float time) override;
	virtual void stop();

CC_CONSTRUCTOR_ACCESS:
	ShaderAction() {}
	virtual ~ShaderAction() {}

	bool initWithDuration(float duration, ShaderEffectBase* effect);

protected:
	ShaderEffectBase* m_effect;

	cocos2d::GLProgramState* oldGLProgramState;

private:
	CC_DISALLOW_COPY_AND_ASSIGN(ShaderAction);
};

} //lly

#endif //_LLY_SHADER_ACTION_H_




