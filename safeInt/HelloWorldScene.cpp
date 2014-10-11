#include "HelloWorldScene.h"
#include "SafeInt.h"
#include <random>
#include "llyValueListener.h"
#include "llyModifyVar.h"

USING_NS_CC;
using namespace std;


Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
	/*
	SafeInt d = 26;
	SafeInt c = 78;
	SafeInt de = 1000;

	d += 24;
	c -= 18;
	de *= 1.2f;
	SafeInt f = de / 1.2f;
	SafeInt ee = d;

	CCLOG("safe int is %d %d %d", (int)d, (int)c, (int)de);
	
	log("add is %d %d %d %d", (int)(d), (int)(c ), (int)(de), (int)(ee));

	std::random_device rd ;

	std::default_random_engine e(rd()) ;

	std::uniform_int_distribution<> u (-9999999 , 9999999);

	for ( size_t i = 0 ; i < 100 ; i ++ ) {

		//CCLOG("r is %d", u(e));

	}
	fffff = 800;
	MoveBy* mb = MoveBy::create(10.0f, Vec2(0, -400));
	label->runAction(mb);
	std::function<void(void)> doAfter;
	auto vl = ValueListener::create(&fffff, 500, [&]()
	{
		CCLOG("yyyyyyyyy %f", fffff);	
	});

	vl->excute_cb = [=](float t)
	{
		CCLOG("dd %f", t);
	};

	vl->run();

	auto mfv = ModifyFuncVar::create([&](float t)
	{
		fffff = t;
	}, 8, fffff, 300);

	this->runAction(mfv);
	*/
	vector<vector<string> > vvs;
	vector<string> vs;
	vector<vector<string> > vvs2;
	
	
	CCLOG("begin is %d", (int)(getTime()));
	for(int i = 0; i < 100; ++i)
	{
		for (int j = 0; j < 300; ++j)
		{
			vs.push_back("asdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdf");
		}
		vvs.push_back(vs);
		vs.clear();
	}
	maps.insert(pair<string, vector<vector<string> > >("123", vvs));
	CCLOG("end is %d", (int)(getTime()));
	vvs2 = getvec("123");
	string s = vvs2.at(3).at(3);
	CCLOG("%s", s.c_str());
	CCLOG("end2 is %d", (int)(getTime()));

    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
