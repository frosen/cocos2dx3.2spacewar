#include "HelloWorldScene.h"
#include "llyRhythmGenerator.h"

using namespace lly;

USING_NS_CC;

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

	m_musicPath = "\0";
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem1 = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback1, this));
    
	closeItem1->setPosition(Vec2(origin.x + visibleSize.width - closeItem1->getContentSize().width/2 ,
                                origin.y + closeItem1->getContentSize().height/2));

	auto closeItem2 = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback2, this));

	closeItem2->setPosition(Vec2(origin.x + visibleSize.width - closeItem2->getContentSize().width/2 - 100 ,
		origin.y + closeItem2->getContentSize().height/2));

	auto closeItem3 = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback3, this));

	closeItem3->setPosition(Vec2(origin.x + visibleSize.width - closeItem3->getContentSize().width/2 - 200 ,
		origin.y + closeItem3->getContentSize().height/2));

	auto closeItem4 = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback4, this));

	closeItem4->setPosition(Vec2(origin.x + visibleSize.width - closeItem4->getContentSize().width/2 - 300,
		origin.y + closeItem4->getContentSize().height/2));

	//===================
	auto closeItema = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallbacka, this));

	closeItema->setPosition(Vec2(origin.x + visibleSize.width - closeItema->getContentSize().width/2 - 400,
		origin.y + closeItema->getContentSize().height/2));

	auto closeItems = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallbacks, this));

	closeItems->setPosition(Vec2(origin.x + visibleSize.width - closeItems->getContentSize().width/2 - 500,
		origin.y + closeItems->getContentSize().height/2));
	//=====================

	auto closeItem5 = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback5, this));
	
	closeItem5->setPosition(Vec2(origin.x + visibleSize.width - closeItem5->getContentSize().width/2,
		origin.y + closeItem5->getContentSize().height/2 + 100));
	
	auto closeItem6 = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback6, this));

	closeItem6->setPosition(Vec2(origin.x + visibleSize.width - closeItem6->getContentSize().width/2 - 100,
		origin.y + closeItem6->getContentSize().height/2 + 100));

	auto closeItem7 = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback7, this));

	closeItem7->setScale(2.0f);

	closeItem7->setPosition(Vec2(origin.x + visibleSize.width - closeItem7->getContentSize().width/2 - 100,
		origin.y + closeItem7->getContentSize().height/2 + 400));

	

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem1, closeItem2, closeItem3, closeItem4, closeItem5, closeItem6, closeItem7, closeItema, closeItems, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
	auto num = RhythmGenerator::getInstance()->getMSecondPerBeat();
	char ch[100];
    sprintf_s(ch, "second per beat %d", num);
    label = LabelTTF::create(ch, "Arial", 24);
    
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

	RhythmGenerator::getInstance()->eventPerBeat = [=](int nBeat)
	{
		Color3B color = sprite->getColor();
		color.r += 24;
		color.g += 60;
		color.b += 100;
		sprite->setColor(color);

		int b = (int)RhythmGenerator::getInstance()->getBeginTime();
		int a = (int)RhythmGenerator::getInstance()->getCurrentTime();
		int c = (int)RhythmGenerator::getInstance()->getMSecondPerBeat();
		log("now : %d ----- %d = %d //// %d x %d = %d", 
			a, b, a-b, nBeat, c, c*nBeat);
	};

	m_musicPath = "fc.mp3";

	//RhythmGenerator::getInstance()->start();
    
    return true;
}


void HelloWorld::menuCloseCallback1(Ref* pSender)
{
	RhythmGenerator::getInstance()->setMSecondPerBeat(RhythmGenerator::getInstance()->getMSecondPerBeat() - 1);
	auto num = RhythmGenerator::getInstance()->getMSecondPerBeat();
	char ch[100];
	sprintf_s(ch, "second per beat %d", num);
	label->setString(ch);
}

void HelloWorld::menuCloseCallback2(Ref* pSender)
{
	RhythmGenerator::getInstance()->setMSecondPerBeat(RhythmGenerator::getInstance()->getMSecondPerBeat() + 1);
	auto num = RhythmGenerator::getInstance()->getMSecondPerBeat();
	char ch[100];
	sprintf_s(ch, "second per beat %d", num);
	label->setString(ch);
}

void HelloWorld::menuCloseCallback3(Ref* pSender)
{
	RhythmGenerator::getInstance()->setMSecondPerBeat(RhythmGenerator::getInstance()->getMSecondPerBeat() - 10);
	auto num = RhythmGenerator::getInstance()->getMSecondPerBeat();
	char ch[100];
	sprintf_s(ch, "second per beat %d", num);
	label->setString(ch);
}

void HelloWorld::menuCloseCallback4(Ref* pSender)
{
	RhythmGenerator::getInstance()->setMSecondPerBeat(RhythmGenerator::getInstance()->getMSecondPerBeat() + 10);
	auto num = RhythmGenerator::getInstance()->getMSecondPerBeat();
	char ch[100];
	sprintf_s(ch, "second per beat %d", num);
	label->setString(ch);
}

void HelloWorld::menuCloseCallback5( cocos2d::Ref* pSender )
{
	RhythmGenerator::getInstance()->stopBackgroundMusic(true);
	CCLOG("stop");
}

void HelloWorld::menuCloseCallback6( cocos2d::Ref* pSender )
{
	if (m_musicPath == "\0")
	{
		CCLOG("%s", m_musicPath.c_str());
		return;
	}
	
	if (CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
	{
		RhythmGenerator::getInstance()->rewindBackgroundMusic();
		CCLOG("rewind");
	}
	else
	{
		RhythmGenerator::getInstance()->playBackgroundMusic(m_musicPath.c_str());
		CCLOG("play");
		m_time = RhythmGenerator::getInstance()->getCurrentMSecond();
	}
	
}

void HelloWorld::menuCloseCallback7( cocos2d::Ref* pSender )
{
	int64_t a = RhythmGenerator::getInstance()->getCurrentMSecond();
	int64_t b = a - m_time;
	m_time = a;
	CCLOG("------------------------------------------------------------------------------------------%d", (int)b);
}

void HelloWorld::menuCloseCallbacka( cocos2d::Ref* pSender )
{
	RhythmGenerator::getInstance()->setMSecondPerBeat(RhythmGenerator::getInstance()->getMSecondPerBeat() - 100);
	auto num = RhythmGenerator::getInstance()->getMSecondPerBeat();
	char ch[100];
	sprintf_s(ch, "second per beat %d", num);
	label->setString(ch);
}

void HelloWorld::menuCloseCallbacks( cocos2d::Ref* pSender )
{
	RhythmGenerator::getInstance()->setMSecondPerBeat(RhythmGenerator::getInstance()->getMSecondPerBeat() + 100);
	auto num = RhythmGenerator::getInstance()->getMSecondPerBeat();
	char ch[100];
	sprintf_s(ch, "second per beat %d", num);
	label->setString(ch);
}










