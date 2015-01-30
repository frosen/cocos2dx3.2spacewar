#include "llyRenderedAllNode.h"

USING_NS_CC;
using namespace lly;

lly::RenderedAllNode::RenderedAllNode() : RenderTexture()
{

}

lly::RenderedAllNode::~RenderedAllNode()
{
	CC_SAFE_RELEASE(m_effect);
}

RenderedAllNode* lly::RenderedAllNode::create( int w, int h, ShaderEffectBase* effect/* = nullptr*/, cocos2d::Texture2D::PixelFormat format /*= cocos2d::Texture2D::PixelFormat::RGBA4444*/, GLuint depthStencilFormat /*= 0*/ )
{
	auto node = new (std::nothrow) RenderedAllNode();
	if (node && node->init(w, h, effect, format, depthStencilFormat))
	{
		node->autorelease();
		return node;
	}
	else
	{
		CC_SAFE_DELETE(node);
		return nullptr;
	}
}

void lly::RenderedAllNode::setShaderEffect( ShaderEffectBase* effect )
{
	if (!effect) return;

	effect->setTarget(getSprite());

	if (m_effect) m_effect->release();
	m_effect = effect;
	effect->retain();
}

ShaderEffectBase* lly::RenderedAllNode::getShaderEffect()
{
	return m_effect;
}

void lly::RenderedAllNode::setAnchorPoint( const Vec2& anchorPoint )
{
	getSprite()->setAnchorPoint(anchorPoint);
}

const Vec2& lly::RenderedAllNode::getAnchorPoint() const
{
	return getSprite()->getAnchorPoint();
}

//===========================================================

bool lly::RenderedAllNode::init( int w, int h, ShaderEffectBase* effect, cocos2d::Texture2D::PixelFormat format, GLuint depthStencilFormat )
{
	if (!RenderTexture::initWithWidthAndHeight(w, h, format, depthStencilFormat))
	{
		return false;
	}

	//保持效果存在，并加载到使用自己渲染成的纹理的精灵上
	m_effect = effect;
	CC_SAFE_RETAIN(m_effect);
	if (m_effect) m_effect->setTarget(getSprite());

	//初始锚点为（0,0）位置
	getSprite()->setAnchorPoint(Vec2::ZERO);

	return true;
}

void lly::RenderedAllNode::visit( cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags )
{
	if (!_visible) return;

	uint32_t flags = processParentFlags(parentTransform, parentFlags);

	//Director* director = Director::getInstance();
	// IMPORTANT:
	// To ease the migration to v3.0, we still support the Mat4 stack,
	// but it is deprecated and your code should not rely on it
	//director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	//director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

	draw(renderer, Mat4::IDENTITY, true); //修改，载入IDENTITY
	_sprite->visit(renderer, _modelViewTransform, flags); //位置从draw上面变到下面

	//director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	// FIX ME: Why need to set _orderOfArrival to 0??
	// Please refer to https://github.com/cocos2d/cocos2d-x/pull/6920
	// setOrderOfArrival(0);
}

void lly::RenderedAllNode::draw( Renderer *renderer, const Mat4 &transform, uint32_t flags )
{
	//不进行AutoDraw的判断，无论如何会进行

	//Begin will create a render group using new render target
	beginWithClear(0.0, 0.0, 0.0, 0.0); //修改，每次渲染前要clear

	//clear screen
	_clearCommand.init(_globalZOrder);
	_clearCommand.func = CC_CALLBACK_0(RenderedAllNode::onClear, this);
	renderer->addCommand(&_clearCommand);

	//! make sure all children are drawn
	sortAllChildren();

	for(const auto &child: _children)
	{
		if (child != _sprite)
			child->visit(renderer, transform, flags);
	}

	//End will pop the current render group
	end();
}







