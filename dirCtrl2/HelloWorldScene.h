#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
	void menuCloseCallback2(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
	bool bgo;
	float xhalf;
	int ii;

	void setTouchLayer();
	void setUI();

	float sq(float x)
	{
		xhalf = 0.5f*x;
		ii = *(int*)&x; // get bits for floating VALUE
		ii = 0x5f375a86- (ii>>1); // gives initial guess y0
		x = *(float*)&ii; // convert bits BACK to float
		x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
		return x;
	}

};

#endif // __HELLOWORLD_SCENE_H__
