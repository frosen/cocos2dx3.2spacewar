/*
	���ƣ�llyShaderAction.h
	����������һ��������������һ��ʱ����ʹ��һ����Ⱦ����������ԭ������Ⱦ
				��ȾҪ��װ��effectЧ����
				�������Ⱦ�ļ����������һ����Ϊ��ActionTime"��float uniform�������ж���update�����ж�ʱ��Ŀ���
	���ߣ�����
	���ڣ�2014.7.1
*/

#ifndef _LLY_SHADER_ACTION_H_
#define _LLY_SHADER_ACTION_H_

#include "cocos2d.h"

namespace lly {

class ShaderEffectBase;

class ShaderAction : public cocos2d::ActionInterval
{
public:
	//��һ��ʱ����ʹ��һ����Ⱦ����������ԭ������Ⱦ����Ⱦ����ʱ��仯
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




