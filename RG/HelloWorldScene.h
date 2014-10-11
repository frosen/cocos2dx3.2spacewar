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
    void menuCloseCallback1(cocos2d::Ref* pSender);
	void menuCloseCallback2(cocos2d::Ref* pSender);
	void menuCloseCallback3(cocos2d::Ref* pSender);
	void menuCloseCallback4(cocos2d::Ref* pSender);
	void menuCloseCallback5(cocos2d::Ref* pSender);
	void menuCloseCallback6(cocos2d::Ref* pSender);
	void menuCloseCallback7(cocos2d::Ref* pSender);

	void menuCloseCallbacka(cocos2d::Ref* pSender);
	void menuCloseCallbacks(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	cocos2d::LabelTTF* label;

	std::string m_musicPath;

	int64_t m_time;
};

#endif // __HELLOWORLD_SCENE_H__
