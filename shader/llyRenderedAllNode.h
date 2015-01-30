/*
	���ƣ�llyRenderedAllNode.h
	�������ɼ���һ��shader������openGL��post-processing��ʽ��Ⱦ��ȫ���ӽڵ����ɵ�����
				�̳���renderTexture
	���ߣ�����
	���ڣ�2015.1.21
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

	//����������Ⱦ�ڵ�
	//����Ϊ����Ⱦ����Ŀ�ߣ���Ⱦ����ɫ��������ĸ�ʽ����Ⱦ�����
	static RenderedAllNode*create(
		int w, 
		int h, 
		ShaderEffectBase* effect = nullptr, 
		cocos2d::Texture2D::PixelFormat format = cocos2d::Texture2D::PixelFormat::RGBA4444, 
		GLuint depthStencilFormat = 0);

	//�������û��߻����ȾЧ��
	void setShaderEffect(ShaderEffectBase* effect);
	ShaderEffectBase* getShaderEffect();

	//���أ��ڲ�Ϊ�����侫���ê��
	virtual void setAnchorPoint(const cocos2d::Vec2& anchorPoint);
    virtual const cocos2d::Vec2& getAnchorPoint() const;

protected:
	virtual bool init(
		int w, 
		int h, 
		ShaderEffectBase* effect, 
		cocos2d::Texture2D::PixelFormat format, 
		GLuint depthStencilFormat);

	//����visit���ڴ˰���Ⱦ�����Ⱦ�������ӽڵ�
	virtual void visit( cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags ) override;

	virtual void draw( cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags ) override;

protected:
	ShaderEffectBase* m_effect;

};

} //lly

#endif //_LLY_RENDERED_ALL_NODE_H_




