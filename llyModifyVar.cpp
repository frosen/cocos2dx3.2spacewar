#include "llyModifyVar.h"

USING_NS_CC;
using namespace lly;

ModifyVar::ModifyVar() :
	nPlusOrMinus(1)
{

}

ModifyVar::~ModifyVar()
{

}

ModifyVar* ModifyVar::create( float duration, float destination, std::function<float()> getStart, std::function<void(float)> Modify )
{
	auto p = new ModifyVar();
	if (p && p->initWithDuration(duration, destination, getStart, Modify))
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
	a->initWithDuration(m_fDuration, m_fDestination, getStart_cb, Modify_cb);
	a->autorelease();
	return a;
}

void ModifyVar::startWithTarget( Node *pTarget )
{
	ActionInterval::startWithTarget(pTarget);

	//先准备好每帧变化的数值
    m_fValue = getStart_cb();
	m_fDelta = (m_fDestination - m_fValue) / (m_fDuration / Director::getInstance()->getAnimationInterval());

	//准备好对结束的检测
	if (m_fValue < m_fDestination) nPlusOrMinus = -1;
}

void ModifyVar::update( float time )
{
	if (_target)
	{
		m_fValue += m_fDelta; //每帧变化一个数值

		if ((m_fValue - m_fDestination) * nPlusOrMinus <= 0.0f) m_fValue = m_fDestination; //结束时

		Modify_cb(m_fValue); //回调
	}
}

bool ModifyVar::initWithDuration( float duration, float destination,
		std::function<float()> getStart, std::function<void(float)> Modify )
{
	CCASSERT(getStart != nullptr, "getStart is nullptr");
	CCASSERT(Modify != nullptr, "Modify is nullptr");

	if (ActionInterval::initWithDuration(duration))
	{
		m_fDestination = destination;

		getStart_cb = getStart;
		Modify_cb = Modify;

		m_fDuration = duration;

		return true;
	}
	return false;
}
