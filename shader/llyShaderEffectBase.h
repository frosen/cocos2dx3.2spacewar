/*
名称：llyShaderEffectBase.h
内容：可以通过载入渲染文件名生成渲染效果，
			通过设置目标改变一个sprite的渲染管线
			可以通过设置目标及其子节点，改变一个node本身及其子节点中所有sprite的渲染管线
			可以还原sprite（或包括其所有子sprite）的渲染程序为默认
			可以通过继承，为渲染增加设置值

制作：卢乐颜
日期：2014.10.19
*/

#ifndef _LLY_SHADER_MANAGER_H_
#define _LLY_SHADER_MANAGER_H_

#include "cocos2d.h"

//默认值
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

	//新建shader效果，vert可以为""，表示使用默认的vert shader
	static ShaderEffectBase* createWithShaderFileName(const std::string &vert, const std::string &frag);

	//将精灵的渲染管线改为本渲染效果
	virtual void setTarget(cocos2d::Sprite* sprite);

	//将节点以及节点所有的精灵的渲染管线改为本渲染效果
	virtual void setTargetWithChild(cocos2d::Node* node);

	//还原目标，将精灵（或者节点所有的精灵）的渲染管线变为原值或者默认值
	void restoreTarget(cocos2d::GLProgramState* glProgramState = nullptr);

protected:
	virtual bool init(const std::string &vert, const std::string &frag);

	void setShaderWithChild_IfIsSprite(cocos2d::Node* node);
	void setShaderDefaultWithChild_IfIsSprite(cocos2d::Node* node, cocos2d::GLProgramState* GLState);

protected:
	cocos2d::GLProgramState *_glprogramstate; //载入的渲染程序

	cocos2d::Node* m_target;

	ETargetType m_eTargetType; //目标的类型

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string _vertSource;
	std::string _fragSource;
	cocos2d::EventListenerCustom* _backgroundListener;
#endif
};

} // namespace lly

#endif //_LLY_SHADER_MANAGER_H_


