/*
    名称：llyCallPerFrame.h
    描述：创造一个动作，在一定时间里每帧执行一个回调函数
    作者：乐颜
    日期：2014.7.1
*/

#ifndef _CALL_PER_FRAME_H_
#define _CALL_PER_FRAME_H_

#include "cocos2d.h"

namespace lly {

class CallPerFrame : public cocos2d::ActionInterval
{
public:
    CallPerFrame();
    virtual ~CallPerFrame();

    //通过一个变化时间，来修改一个接收改变后数值的回调函数
    static CallPerFrame* create(float duration, std::function<void(float)> call);

    //override
    virtual CallPerFrame* clone() const override;
    virtual void startWithTarget(cocos2d::Node *pTarget) override;
    virtual void update(float time) override;

protected:
    bool initWithDuration(float duration, std::function<void(float)> call);

    float m_fDuration; //持续时间
    std::function<void(float)> m_call; //每帧的回调

private:
    CC_DISALLOW_COPY_AND_ASSIGN(CallPerFrame);
};

} //lly

#endif //_CALL_PER_FRAME_H_