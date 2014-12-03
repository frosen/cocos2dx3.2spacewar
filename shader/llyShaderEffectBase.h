/*
���ƣ�llyShaderEffectBase.h
���ݣ�����ͨ��������Ⱦ�ļ���������ȾЧ����
			ͨ������Ŀ��ı�һ��sprite����Ⱦ����
			����ͨ������Ŀ�꼰���ӽڵ㣬�ı�һ��node�������ӽڵ�������sprite����Ⱦ����
			���Ի�ԭsprite���������������sprite������Ⱦ����ΪĬ��
			����ͨ���̳У�Ϊ��Ⱦ��������ֵ

������¬����
���ڣ�2014.10.19
*/

#ifndef _LLY_SHADER_MANAGER_H_
#define _LLY_SHADER_MANAGER_H_

#include "cocos2d.h"

//Ĭ��ֵ
#define LLY_SHADER_VERT_DEFAULT cocos2d::ccPositionTextureColor_noMVP_vert
#define LLY_SHADER_FRAG_DEFAULT cocos2d::ccPositionTextureColor_noMVP_frag

namespace lly{

class ShaderEffectBase : public cocos2d::Ref
{
public:
	enum class ETargetType
	{
		NONE_TARGET,
		SPRITE,
		NODE_WITH_CHILD,
	};

	ShaderEffectBase();
	virtual ~ShaderEffectBase();

	//�½�shaderЧ����vert����Ϊ""����ʾʹ��Ĭ�ϵ�vert shader
	static ShaderEffectBase* createWithShaderFileName(const std::string &vert, const std::string &frag);

	//���������Ⱦ���߸�Ϊ����ȾЧ��
	virtual void setTarget(cocos2d::Sprite* sprite);

	//���ڵ��Լ��ڵ����еľ������Ⱦ���߸�Ϊ����ȾЧ��
	virtual void setTargetWithChild(cocos2d::Node* node);

	//��ԭĿ�꣬�����飨���߽ڵ����еľ��飩����Ⱦ���߱�Ϊԭֵ����Ĭ��ֵ
	void restoreTarget(cocos2d::GLProgramState* glProgramState = nullptr);

protected:
	virtual bool init(const std::string &vert, const std::string &frag);

	void setShaderWithChild_IfIsSprite(cocos2d::Node* node);
	void setShaderDefaultWithChild_IfIsSprite(cocos2d::Node* node, cocos2d::GLProgramState* GLState);

protected:
	cocos2d::GLProgramState *_glprogramstate; //�������Ⱦ����

	cocos2d::Node* m_target;

	ETargetType m_eTargetType; //Ŀ�������

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string _vertSource;
	std::string _fragSource;
	cocos2d::EventListenerCustom* _backgroundListener;
#endif
};

} // namespace lly

#endif //_LLY_SHADER_MANAGER_H_


