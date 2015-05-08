#include "llyCallPerFrame.h"

USING_NS_CC;
using namespace lly;

CallPerFrame::CallPerFrame()
{

}

CallPerFrame::~CallPerFrame()
{

}

CallPerFrame* CallPerFrame::create( float duration, std::function<void(float)> call )
{
    auto p = new CallPerFrame();
    if (p && p->initWithDuration(duration,call))
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

CallPerFrame* CallPerFrame::clone() const
{
    auto a = new CallPerFrame();
    a->initWithDuration(m_fDuration, m_call);
    a->autorelease();
    return a;
}

void CallPerFrame::startWithTarget( Node *pTarget )
{
    ActionInterval::startWithTarget(pTarget);
}

void CallPerFrame::update( float time )
{
    if (_target) m_call(time); //回调
}

bool CallPerFrame::initWithDuration( float duration, std::function<void(float)> call )
{
    CCASSERT(call != nullptr, "call is nullptr");

    if (ActionInterval::initWithDuration(duration))
    {
        m_call = call;

        return true;
    }
    return false;
}
