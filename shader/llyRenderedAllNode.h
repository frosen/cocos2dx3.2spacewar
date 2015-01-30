/*
	名称：llyRenderedAllNode.h
	描述：可加载一个shader，运用openGL的post-processing方式渲染其全部子节点生成的纹理
				继承于renderTexture
	作者：乐颜
	日期：2015.1.21
*/

#ifndef _LLY_RENDERED_ALL_NODE_H_
#define _LLY_RENDERED_ALL_NODE_H_

#include "cocos2d.h"
#include "llyShaderEffectBase.h"

namespace lly {

class ShaderEffectBase;

class RenderedAllNode : public cocos2d::RenderTexture
{
public:
	RenderedAllNode();
	virtual ~RenderedAllNode();

	//创建后处理渲染节点
	//参数为，渲染纹理的宽高，渲染的着色程序，纹理的格式，渲染的深度
	static RenderedAllNode*create(
		int w, 
		int h, 
		ShaderEffectBase* effect = nullptr, 
		cocos2d::Texture2D::PixelFormat format = cocos2d::Texture2D::PixelFormat::RGBA4444, 
		GLuint depthStencilFormat = 0);

	//重新设置或者获得渲染效果
	void setShaderEffect(ShaderEffectBase* effect);
	ShaderEffectBase* getShaderEffect();

	//重载，内部为设置其精灵的锚点
	virtual void setAnchorPoint(const cocos2d::Vec2& anchorPoint);
    virtual const cocos2d::Vec2& getAnchorPoint() const;

protected:
	virtual bool init(
		int w, 
		int h, 
		ShaderEffectBase* effect, 
		cocos2d::Texture2D::PixelFormat format, 
		GLuint depthStencilFormat);

	//重载visit，在此把渲染变成渲染其所有子节点
	virtual void visit( cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags ) override;

	virtual void draw( cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags ) override;

protected:
	ShaderEffectBase* m_effect;

};

} //lly

#endif //_LLY_RENDERED_ALL_NODE_H_




