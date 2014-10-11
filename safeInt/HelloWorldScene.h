#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;
using namespace std;


class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	int64_t getTime()
	{
		struct timeval tval;
		gettimeofday(&tval, NULL);
		return (int64_t)(tval.tv_sec * 1000LL + tval.tv_usec/1000);
	}

	map<string, vector<vector<string> > > maps;

	vector<vector<string> > getvec(string str) {return maps.at(str);}

	float fffff;
};

#endif // __HELLOWORLD_SCENE_H__
