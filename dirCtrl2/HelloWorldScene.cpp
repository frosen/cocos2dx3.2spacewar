#include "HelloWorldScene.h"
#include "llyAcceDirCtrler.h"
#include "llyNetStream.h"
#include "llyAngleToVec.h"
#include "widget\llyCheckBox.h"
#include "widget\llyCheckBoxManager.h"
#include "llyFileUtils.h"
#include "llyCharCodeConversion.h"
#include "llyMovableLayer.h"
#include "llySlider.h"

USING_NS_CC;
using namespace lly;

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


	//////////////////////////////
	auto closeItem2 = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback2, this));

	closeItem2->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
		origin.y + closeItem->getContentSize().height/2 + 400));

	closeItem2->setScale(1.6f);


    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, closeItem2, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Hello World", "Arial", 24, Size(960, 640));
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2, visibleSize.height/2 + origin.y));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	sprite->setScale(0.2f);

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);

	float speed = 10;
	AcceDirCtrler::getInstance()->start();
	AcceDirCtrler::getInstance()->getControlDirectionEvent([=](AcceDirCtrler* adc)
	{
 		EnDirection edir = adc->getDirection();
		if (edir == Dnone) return;
		
		float angle = (int)edir * 22.5f;
 		sprite->setRotation(angle);	

		sprite->setPosition(
			sprite->getPositionX() + adc->getDirectionRatio().x * speed, 
			sprite->getPositionY() + adc->getDirectionRatio().y * speed);

		if (sprite->getPositionX() < 0) sprite->setPositionX(0);
		if (sprite->getPositionX() > visibleSize.width) sprite->setPositionX(visibleSize.width);
		if (sprite->getPositionY() < 0) sprite->setPositionY(0);
		if (sprite->getPositionY() > visibleSize.height) sprite->setPositionY(visibleSize.height);
		
	});

	auto ddd = DirectionRatio(234, 34);
	Size pos = reinterpret_cast<Size&>(ddd);
	log("%d......%f,,%f,,%f", sizeof(pos), pos.width, pos.height, ddd.x);


	AcceDirCtrler::getInstance()->pause();
    bgo = false;

	NetStream* ns = new NetStream();
	char* ch = "ni shi wo de";
	char ccc = 'g';
	ns->putChar(ccc);
	ns->putInt(4897);
	ns->addStream(ch, 8);
	
	char cdd;
	char result[100];
	ns->getChar(cdd);
	int nn;
	ns->getInt(nn);
	ns->getStream(result, 8);
	
	
	log("%s", result);
	log("%c", cdd);
	log("%d", nn);
	Sprite* spd = nullptr;

	log("* %d", sizeof(spd));

	Sprite* sp2 = Sprite::create("HelloWorld.png");
	sp2->setPosition(12, 34);
	Sprite* sp3 = Sprite::create("HelloWorld.png");;
	Node* node = Node::create();
	
	auto nsT = new NetStreamT<Sprite*>();
	auto nsT2 = new NetStreamT<Node*>();
	nsT2->putT(node);

	nsT->putT(sp2);
	nsT->putT(sp3);

	Sprite* sp4;
	
	nsT->getT(sp4);
	log("%f, %fdddddddddddddd", sp4->getPosition().x, sp4->getPosition().y);

	AngleVec pps = AD2V(180, 10.0f);
	log("pps is %f ,,,%f", pps.x, pps.y);

	float x = 1873.264f;
	float y = 0;
	
	float arx[5] = {12, 34, 23, 43, 33};

	AngleVec ppss = PPD2V(1300, 100, 200, 800, 5);
	log("%f isisisis %f", ppss.x, ppss.y);

	struct timeval tval;
	gettimeofday(&tval, NULL);
	long int ln = (long int)(tval.tv_sec * 1000LL + tval.tv_usec/1000);
	log("time %ld", ln); 

	for (int i = 0; i < 10; ++i)
	{
		//x = 15 + x;
		//y = 1 / sqrtf(1873.64f);
		//x = 1 / sqrtf(x);
		//x = sq(x);
		//x = getSquareRoot(1873.264f);
		//ppss = PPD2V(1300, i, 200, 800, 5);
		//x = arx[2];
	}
	
	gettimeofday(&tval, NULL);
	long int ln2 = (long int)(tval.tv_sec * 1000LL + tval.tv_usec/1000);
	log("time end %ld ......%ld", ln2, ln2 - ln); 
	log("re is %f ... %f", ppss.x, ppss.y);

	auto cb = lly::CheckBox::create("sliderballnormal.png", "sliderballnormal.png", "sliderballpressed.png", "sliderballpressed.png", "sliderballpressed.png");
	cb->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 - 400,
		origin.y + closeItem->getContentSize().height/2 + 100));
	this->addChild(cb);

	auto cb2 = lly::CheckBox::create("sliderballnormal.png", "sliderballnormal.png", "sliderballpressed.png", "sliderballpressed.png", "sliderballpressed.png");
	cb2->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 - 500,
		origin.y + closeItem->getContentSize().height/2 + 100));
	this->addChild(cb2);

	auto cb3 = lly::CheckBox::create("sliderballnormal.png", "sliderballnormal.png", "sliderballpressed.png", "sliderballpressed.png", "sliderballpressed.png");
	cb3->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 - 600,
		origin.y + closeItem->getContentSize().height/2 + 100));
	this->addChild(cb3);

	auto cb4 = lly::CheckBox::create("sliderballnormal.png", "sliderballnormal.png", "sliderballpressed.png", "sliderballpressed.png", "sliderballpressed.png");
	cb4->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 - 700,
		origin.y + closeItem->getContentSize().height/2 + 100));
	this->addChild(cb4);

	auto cb5 = lly::CheckBox::create("sliderballnormal.png", "sliderballnormal.png", "sliderballpressed.png", "sliderballpressed.png", "sliderballpressed.png");
	cb5->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 - 800,
		origin.y + closeItem->getContentSize().height/2 + 100));
	this->addChild(cb5);

	auto cb6 = lly::CheckBox::create("sliderballnormal.png", "sliderballnormal.png", "sliderballpressed.png", "sliderballpressed.png", "sliderballpressed.png");
	cb6->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 - 900,
		origin.y + closeItem->getContentSize().height/2 + 100));
	this->addChild(cb6);

// 	cb->turnToSelectedState();
// 	cb2->turnToSelectedState();
// 	cb3->turnToSelectedState();
// 	cb4->turnToSelectedState();
// 	cb5->turnToSelectedState();
// 	cb6->turnToSelectedState();

	auto cbM = lly::CheckBoxManager::createWithCheckBox(cb, cb2, cb3, cb4, cb5, cb6, nullptr);
	cbM->retain();
	cbM->setMaxNumSelected(3);
	
	std::map<std::string, bool> m_map;

	std::string strPath = cocos2d::FileUtils::getInstance()->getWritablePath();
	//strPath = "/data/";
#if defined(ANDROID)
	log("android");
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	log("android2");
#endif
	//lly::FileUtils::deleteFolder(strPath.c_str());

// 	log("%s", strPath.c_str());
// 	char d = strPath[strPath.size() - 1];
// 	log("%c", d);
// 	//strPath.erase(strPath.end() - 1);
// 	log("%s", strPath.c_str());
// 	bool bda = lly::FileUtils::searchFolder(strPath.c_str(), m_map);
// 	log("%d", bda ? 1 : 0);
// 
// 	for (auto &data : m_map)
// 	{
		//std::string str8 = convertGBKToUTF8(data.first.c_str());
		//log("%s is a %s", data.first.c_str(), data.second ? "folder" : "file");
//		bool isok = lly::FileUtils::isFileExist(str8.c_str());
//		str8 = convertUTF8ToGBK(str8);
//		bool isok2 = lly::FileUtils::isFileExist(str8.c_str());
//		log("%d, %d...", (isok ? 1 : 0), (isok2 ? 1: 0));
// 		strPath += "\n";
// 		strPath += str8.c_str();
// 		strPath += "--------";
// 		strPath += data.second ? "folder" : "file";
// 	}
// 
// 	label->setString(strPath);

// 	std::string a = strPath + "ftr.txt";
// 	bda = lly::FileUtils::isFileExist(a.c_str());
// 	log("isFileExist %d", bda ? 1 : 0);
// 
// 	std::string b = strPath + "dd/";
// 	bda = lly::FileUtils::isFolderExist(b.c_str());
// 	log("isFolderExist %d", bda ? 1 : 0);

// 	std::string c = strPath + "new/";
// 	bda = lly::FileUtils::createFolder(c.c_str());
// 	log("createFolder %d", bda ? 1 : 0);

//	
// 
// 	int nnum;
// 	std::string sst = lly::FileUtils::readFile(a.c_str(), (uint32_t*)&nnum, 0, 0, 3, 4);
// 	int nsst = lly::FileUtils::getFileLength(a.c_str());
// 	log("read:%s, %d, nnum %d", sst.c_str(), nsst, nnum);
// 
// 	std::string cw = strPath + "write.txt";
// 	bda = lly::FileUtils::writeFile(cw.c_str(), sst.c_str(), nnum);
// 	log("writeFile %d", bda ? 1 : 0);
// 
// 	bda = lly::FileUtils::deleteFile(cw.c_str());
// 	log("deleteFile %d", bda ? 1 : 0);

//	bda = lly::FileUtils::copy(a.c_str(), b.c_str());
/*/	log("move %d", bda ? 1 : 0);

 	std::string cw2 = strPath + "dd/aa/";
 	std::string cw1 = strPath;
 	log("%s %s", cw2.c_str(), cw1.c_str());
 	bda = lly::FileUtils::copy(cw2.c_str(), cw1.c_str());
 	log("copy %d", bda ? 1 : 0); //*/

// #define LUJING "D:\\text\\text4"
// #define LUJINGX "D:\\text\\text7"
// 	bool b = true;
// 	int nnum;
// 	char chch[100];
// 	strcpy_s(chch, "woshi");
// 	char* chch = lly::FileUtils::readFile(LUJING, (uint32_t*)&nnum, 0, 0, 3, 4);
// 	
// 	if (b)
// 	{
// 		log("%s %d chch", chch, nnum);
// 	}

//	lly::FileUtils::writeFile(LUJINGX, chch, nnum);
// 	free(chch);
// 	const char* cg = lly::FileUtils::getLastCharacters("D:\\text2\\deg.txt");
// 	log("%s is type", cg);
// 
// 	b = lly::FileUtils::deleteFile(LUJING);

//	bool bc = lly::FileUtils::move(LUJING, LUJINGX);
//	bool bc = lly::FileUtils::copy(LUJING, LUJINGX);

	setTouchLayer();

	setUI();


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

void HelloWorld::menuCloseCallback2( cocos2d::Ref* pSender )
{
	if (bgo)
	{
		AcceDirCtrler::getInstance()->pause();
		bgo = false;

	}
	else
	{
		AcceDirCtrler::getInstance()->restrat();
		bgo = true;
	}
	
}

void HelloWorld::setTouchLayer()
{
	Vec2 point1p = Vec2(10, 10);
	Vec2 point2p = Vec2(50, 50);
	Vec2 point1 = Vec2(20, 30);
	Vec2 point2 = Vec2(110, 30);

	//视为新旧两条矢量，平移新矢量，使新旧矢量原点相同
	point2 = point2 + point1p - point1;
	point2p  -= point1p;
	point2 -= point1p;

	//计算角度
	float fangle = Vec2::angle(point2p, point2) / M_PI * 180;
	int n = point2p.cross(point2);
	log("%f angel %d", fangle, n);

	//float fangle = Vec2::angle(Vec2(100, 100), Vec2(100, 0));
	//log("%f angel", fangle / M_PI * 180);
	auto la = MovableLayout::create(TouchFunction::move | TouchFunction::scale | TouchFunction::rotate);
	//la->setSize(Size(300, 300));

	la->setScale(2);
	la->setAnchorPoint(Vec2(0.5f, 0.5f));
	la->setRotation(45);

	la->setContentSize(Size(200, 200));
	la->setBackGroundColorType(ui::Layout::BackGroundColorType::GRADIENT);

	la->setBackGroundColorVector(Vec2(1, 1));
	la->setBackGroundColor(Color3B(152, 23, 99), Color3B(45, 144, 8));
	log("adsdf %f %f", la->getAnchorPoint().x, la->getAnchorPoint().y);
	if (la) this->addChild(la, -11);
	auto la2 = Layer::create();
	log("asfesa %f %f", la2->getAnchorPoint().x, la2->getAnchorPoint().y);

	auto sprite = Sprite::create("HelloWorld.png");
	sprite->setPosition(Vec2(0, 0));
	sprite->setAnchorPoint(Vec2::ZERO);
	sprite->setScale(0.4f);
	la->addChild(sprite, 12);

	la->setPosition(Vec2(500, 300));
	auto sprite2 = Sprite::create("HelloWorld.png");
	sprite2->setPosition(Vec2(500, 300));
	sprite2->setAnchorPoint(Vec2::ZERO);
	sprite2->setScale(0.2f);
	sprite2->setColor(Color3B(234, 12, 34));

	this->addChild(sprite2, 10);
}

void HelloWorld::setUI()
{
auto sl = lly::Slider::create();

sl->loadSlidBallTextures("sliderballnormal.png", "sliderballpressed.png", "sliderballpressed.png");
sl->loadBarTexture("loadingbar.png");

sl->setPosition(Vec2(600, 630));
sl->setBallWidthPercent(40);
this->addChild(sl);
}









