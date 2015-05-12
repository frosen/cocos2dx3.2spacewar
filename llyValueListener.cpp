#include "llyValueListener.h"

USING_NS_CC;
using namespace lly;

ValueListener::ValueListener() : m_pfvar(nullptr), m_fMaxWaitingSecond(-1), m_fTimeDelay(0)
{

}

ValueListener::~ValueListener()
{

}

ValueListener* ValueListener::create( float* var, float aim, std::function<void(void)> doAfter, float maxWaitingTime /*= -1.0f*/ )
{
	ValueListener* pRet = new ValueListener();
	if (pRet && pRet->init(var, nullptr, aim, doAfter, maxWaitingTime))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

ValueListener* ValueListener::create( std::function<float(void)> listen, float aim, std::function<void(void)> doAfter, float maxWaitingTime /*= -1.0f*/ )
{
	ValueListener* pRet = new ValueListener();
	if (pRet && pRet->init(nullptr, listen, aim, doAfter, maxWaitingTime))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

void ValueListener::run()
{
	this->retain();
	Director::getInstance()->getScheduler()->resumeTarget(this);
}

void ValueListener::update( float dt )
{
	if (m_pfvar)
	
		if ((*m_pfvar) * m_nType <= m_fconstant * m_nType)
		{
			onDone();	
			return;
		}
	}
	else
	{
		if (listen_cb() * m_nType <= m_fconstant * m_nType)
		{
			onDone();
			return;
		}
	}

	if (m_fMaxWaitingSecond > 0)
	{
		m_fTimeDelay += dt;
		if (m_fTimeDelay > m_fMaxWaitingSecond) onDone();
	}	
}

//=====================================
bool ValueListener::init( float* var, std::function<float(void)> listen, float aim, std::function<void(void)> doAfter, float maxWaitingTime )
{
	do 
	{
		CC_BREAK_IF(!var && !listen);
		CC_BREAK_IF(!doAfter);

		m_pfvar = var;
		listen_cb = listen;
		m_fconstant = aim;
		doAfter_cb = doAfter;
		m_fMaxWaitingSecond = maxWaitingTime;

		if (listen_cb() > m_fconstant) //最开始的数比要到达的数大还是小
			m_nType = 1;
		else
			m_nType = -1;

		Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, true);

		return true;
	} while (0);

	return false;
}

void ValueListener::onDone()
{
	doAfter_cb(); //执行
	Director::getInstance()->getScheduler()->unscheduleUpdate(this); //停止
	CC_SAFE_RELEASE(this); //释放
}

