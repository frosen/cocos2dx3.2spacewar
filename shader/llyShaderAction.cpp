#include "llyShaderAction.h"
#include "llyShaderEffectBase.h"

using namespace lly;

ShaderAction* lly::ShaderAction::create( float duration, ShaderEffectBase* effect )
{
	ShaderAction* ret = new ShaderAction();
	ret->initWithDuration(duration, effect);
	ret->autorelease();

	return ret;
}

ShaderAction* lly::ShaderAction::clone() const 
{
	auto a = new ShaderAction();
	a->initWithDuration(_duration, m_effect);
	a->autorelease();
	return a;
}

ShaderAction* lly::ShaderAction::reverse( void ) const 
{
	return ShaderAction::create(_duration, m_effect);
}

void lly::ShaderAction::startWithTarget( cocos2d::Node *pTarget )
{
	ActionInterval::startWithTarget(pTarget);

	auto sp = dynamic_cast<cocos2d::Sprite*>(pTarget);
	if (!sp) return;
	
	oldGLProgramState = sp->getGLProgramState();
	m_effect->setTarget(sp);
}

void lly::ShaderAction::update( float time )
{
	getTarget()->getGLProgramState()->setUniformFloat("ActionTime", time);
}

void lly::ShaderAction::stop()
{
	cocos2d::Node* node = getTarget();
	
	if(node) m_effect->restoreTarget(oldGLProgramState);

	ActionInterval::stop();
}

bool lly::ShaderAction::initWithDuration( float duration, ShaderEffectBase* effect )
{
	if (ActionInterval::initWithDuration(duration))
	{
		m_effect = effect;
		return true;
	}

	return false;
}

