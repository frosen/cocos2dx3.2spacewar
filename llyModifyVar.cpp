#include "llyModifyVar.h"

USING_NS_CC;
using namespace lly;

ModifyVar::ModifyVar() : nPlusOrMinus(1)
{

}

ModifyVar::~ModifyVar()
{

}

ModifyVar* ModifyVar::create( std::function<void(float)> Modify, float duration, float start,float destination )
{
	auto p = new ModifyVar();
	if (p && p->initWithDuration(Modify, duration, start, destination))
	{
		p->autorelease();
		return p;
	}
	else
	{
		CC_SAFE_DELETE(p);
		return nullptr;
	}
}

ModifyVar* ModifyVar::clone() const 
{
	auto a = new ModifyVar();
	a->initWithDuration(Modify_cb, m_fDuration, m_fStart, m_fDestination);
	a->autorelease();
	return a;
}

ModifyVar* ModifyVar::reverse( void ) const 
{
	return ModifyVar::create(Modify_cb, m_fDuration, m_fDestination, m_fStart);
}

void ModifyVar::startWithTarget( Node *pTarget )
{
	ActionInterval::startWithTarget(pTarget);

	//先准备好每帧变化的数值
	m_fDelta = (m_fDestination - m_fStart) / (m_fDuration / Director::getInstance()->getAnimationInterval()); 

	//准备好对结束的检测
	if (m_fStart < m_fDestination) nPlusOrMinus = -1;
}

void ModifyVar::update( float time )
{
	if (_target)
	{
		m_fStart += m_fDelta; //每帧变化一个数值

		if ((m_fStart - m_fDestination) * nPlusOrMinus < 0.0f) m_fStart = m_fDestination; //结束时
			
		Modify_cb(m_fStart); //回调
	}
}

bool ModifyVar::initWithDuration( std::function<void(float)> Modify, float duration, float start, float destination )
{
	if (ActionInterval::initWithDuration(duration))
	{
		Modify_cb = Modify;
		m_fDuration = duration;

		m_fStart = start;		
		m_fDestination = destination;

		return true;
	}
	return false;
}
