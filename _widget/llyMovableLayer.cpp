#include "llyMovableLayer.h"

USING_NS_CC;
using namespace ui;
using namespace lly;

void lly::TouchFunction::setTouchFunction( Node* node, uint32_t flag )
{
	TouchesCallback moveMoved = nullptr;
	TouchesCallback scaleMoved = nullptr;
	TouchesCallback rotateMoved = nullptr;

	//根据flag 为touch添加相应的操作
	if (flag & TouchFunction::move) moveMoved = m_moveMoved;
	if (flag & TouchFunction::scale) scaleMoved = m_scaleMoved;
	if (flag & TouchFunction::rotate) rotateMoved = m_rotateMoved;

	//建立多点触控监听
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchAllAtOnce::create();

	//有三种不同的情况，只移动，只有大小变化或旋转，既有移动又有变化，分别对应不同的回调函数或回调函数的参数
	if ( flag & TouchFunction::move && !(flag & TouchFunction::scale || flag & TouchFunction::rotate))
	{
		listener->onTouchesBegan = std::bind(TouchFunction::touchBeginWithChangeAnchor, 
			std::placeholders::_1, std::placeholders::_2, node, false);

		listener->onTouchesMoved = std::bind(TouchFunction::touchMoveHasWidgetMove, 
			std::placeholders::_1, std::placeholders::_2, node, moveMoved, scaleMoved, rotateMoved);

		listener->onTouchesEnded = std::bind(TouchFunction::touchEndWithChangeAnchor, 
			std::placeholders::_1, std::placeholders::_2, node, false);
	}
	else if (!(flag & TouchFunction::move) && (flag & TouchFunction::scale || flag & TouchFunction::rotate))
	{
		listener->onTouchesBegan = std::bind(TouchFunction::touchBeginWithChangeAnchor, 
			std::placeholders::_1, std::placeholders::_2, node, false);

		listener->onTouchesMoved = std::bind(TouchFunction::touchMoveWithoutWidgetMove, 
			std::placeholders::_1, std::placeholders::_2, node, moveMoved, scaleMoved, rotateMoved);

		listener->onTouchesEnded = std::bind(TouchFunction::touchEndWithChangeAnchor, 
			std::placeholders::_1, std::placeholders::_2, node, false);
	}
	else
	{
		listener->onTouchesBegan = std::bind(TouchFunction::touchBeginWithChangeAnchor, 
			std::placeholders::_1, std::placeholders::_2, node, true);

		listener->onTouchesMoved = std::bind(TouchFunction::touchMoveHasWidgetMove, 
			std::placeholders::_1, std::placeholders::_2, node, moveMoved, scaleMoved, rotateMoved);

		listener->onTouchesEnded = std::bind(TouchFunction::touchEndWithChangeAnchor, 
			std::placeholders::_1, std::placeholders::_2, node, true);
	}

	listener->onTouchesCancelled = [=](const std::vector<Touch*> &vTouch, Event* event)
	{
		TouchFunction::refresh(nullptr); //刷新全部
	};

	dispatcher->addEventListenerWithSceneGraphPriority(listener, node);
}

void lly::TouchFunction::refresh( cocos2d::Node* node /*= nullptr*/ )
{
	if (node) 
	{
		auto it = mNodeIsHitted.find(node);
		if (it == mNodeIsHitted.end()) return; //保证node在map里是存在的	
		node->setAnchorPoint(it->second.anchorOld);
		node->setScale(1);
		node->setRotation(0);
		mNodeIsHitted.erase(it);
	}
	else 
	{
		for (auto spair : mNodeIsHitted)
		{
			if (!spair.first) continue;
			
			spair.first->setAnchorPoint(spair.second.anchorOld);
			spair.first->setScale(1);
			spair.first->setRotation(0);
		}
		mNodeIsHitted.clear();
	}
}

void lly::TouchFunction::touchBeginWithChangeAnchor( const std::vector<cocos2d::Touch*> &vTouch, cocos2d::Event* event, cocos2d::Node* node, bool needChangeAnchor )
{
	Vec2 firstpoint = vTouch[0]->getLocation(); 
	Vec2 touchpointInNode = node->convertToNodeSpace(firstpoint);

	if (!containsPoint(node, touchpointInNode)) return;

	auto it = mNodeIsHitted.find(node);
	if (it == mNodeIsHitted.end()) //把第一点是否点中的消息通过map传递给move函数
	{
		if (needChangeAnchor) //把锚点移动到触点
		{
			Vec2 pointOldAnchor; //记录原来的锚点用
			changeAnchorToTouchPoint(node, touchpointInNode, pointOldAnchor);

			mNodeIsHitted[node] = struTouchNodeInfo(1, pointOldAnchor);			
		}
		else
		{			
			mNodeIsHitted[node] = struTouchNodeInfo(1, node->getAnchorPoint());
		}			
	}	
	else if (it->second.nNumHittedPoint == 1) //node存在时，在此判断第二点是否点击中，若存在则不再判断第三点
	{
		Vec2 secondPoint = vTouch[0]->getLocation();
		Vec2 secondPointInNode = node->convertToNodeSpace(secondPoint);

		it->second.nNumHittedPoint = 2;

		if (!needChangeAnchor) //把锚点移动到触点
		{
			changeAnchorBack(node, it->second.anchorOld);

			Vec2 pointOldAnchor; //记录原来的锚点用
			Vec2 secondPointInNode_2 = node->convertToNodeSpace(secondPoint); //node变化，重新求出节点中位置
			changeAnchorToTouchPoint(node, secondPointInNode_2, pointOldAnchor);

			it->second.anchorOld = pointOldAnchor;
		}
	}
	else
	{
		it->second.nNumHittedPoint += 1;
	}
}

void lly::TouchFunction::touchMoveHasWidgetMove( const std::vector<cocos2d::Touch*> &vTouch, cocos2d::Event* event, cocos2d::Node* node, TouchesCallback moveMoved, TouchesCallback scaleMoved, TouchesCallback rotateMoved )
{
	auto it = mNodeIsHitted.find(node);
	if (it == mNodeIsHitted.end()) return; //如果node在map里是存在的，表示有点点中

	//获取需要的触点
	Vec2 touchpoint1[2];
	touchpoint1[0] = vTouch[0]->getLocation();
	touchpoint1[1] = vTouch[0]->getPreviousLocation();

	//移动只需要单点
	moveMoved(touchpoint1, nullptr, event, node);

	if (vTouch.size() <= 1 || it->second.nNumHittedPoint < 2) return; //检测第二点是否存在并且点中

	//需要两点
	//获取第二点
	Vec2 touchpoint2[2];
	touchpoint2[0] = vTouch[1]->getLocation();
	touchpoint2[1] = vTouch[1]->getPreviousLocation();

	if (scaleMoved) scaleMoved(touchpoint1, touchpoint2, event, node);
	if (rotateMoved) rotateMoved(touchpoint1, touchpoint2, event, node);
}

void lly::TouchFunction::touchMoveWithoutWidgetMove( const std::vector<cocos2d::Touch*> &vTouch, cocos2d::Event* event, cocos2d::Node* node, TouchesCallback moveMoved, TouchesCallback scaleMoved, TouchesCallback rotateMoved )
{
	auto it = mNodeIsHitted.find(node);
	if (it == mNodeIsHitted.end()) return; //如果node在map里是存在的，表示有点点中

	//获取需要的触点
	Vec2 touchpoint1[2];
	touchpoint1[0] = vTouch[0]->getLocation();
	touchpoint1[1] = vTouch[0]->getPreviousLocation();

	Vec2 touchpoint2[2];
	touchpoint2[0] = node->convertToWorldSpace(node->getAnchorPointInPoints());
	touchpoint2[1] = touchpoint2[0];

	if (scaleMoved) scaleMoved(touchpoint2, touchpoint1, event, node);
	if (rotateMoved) rotateMoved(touchpoint2, touchpoint1, event, node);
}

void lly::TouchFunction::touchEndWithChangeAnchor( const std::vector<cocos2d::Touch*> &vTouch, cocos2d::Event* event, cocos2d::Node* node, bool needChangeAnchor )
{
	auto it = mNodeIsHitted.find(node);
	if (it == mNodeIsHitted.end()) return; //保证node在map里是存在的	

	Vec2 endPoint = vTouch[0]->getLocation();
	Vec2 endPointInNode = node->convertToNodeSpace(endPoint);

	if (!containsPoint(node, endPointInNode)) return;
	
	if (it->second.nNumHittedPoint >= 2) //第n点点中
	{
		it->second.nNumHittedPoint -= 1;
	}
	else if (it->second.nNumHittedPoint == 1)
	{
		//===================
		if (needChangeAnchor)
		{
			Vec2 oldAnchor = it->second.anchorOld;
			changeAnchorBack(node, oldAnchor);
		}
		//=====================

		mNodeIsHitted.erase(node);//只有一点击中则移除node在map中	
	}
	else
	{
		mNodeIsHitted.erase(node); //其他情况则重置node的触点控制
	}

}

bool lly::TouchFunction::containsPoint(cocos2d::Node* node, cocos2d::Vec2 point)
{
	auto r = Rect(0, /*node->getContentSize().width * (-node->getAnchorPoint().x)*/
							0, /*node->getContentSize().height * (-node->getAnchorPoint().y)*/
							node->getContentSize().width, 
							node->getContentSize().height);

	//CCLOG("%f, %f, %f, %f point in node %f, %f", r.origin.x, r.origin.y, r.size.width, r.size.height, point1.x, point1.y);
	if (r.containsPoint(point)) return true;
	else return false;
}

void lly::TouchFunction::changeAnchorToTouchPoint( cocos2d::Node* node, const cocos2d::Vec2 &pointInNode, cocos2d::Vec2 &oldAnchor )
{
	//新点相对老锚点的偏移距离
	auto pointbyOldAnchor = pointInNode - node->getAnchorPointInPoints();

	//记录原数据
	oldAnchor = node->getAnchorPoint(); 

	//得到当前点位所对应的锚点
	Vec2 pointNewAnchor;
	Size nodeSize = node->getContentSize();
	pointNewAnchor.x = pointInNode.x / nodeSize.width;
	pointNewAnchor.y = pointInNode.y / nodeSize.height;

	//改变锚点
	node->setAnchorPoint(pointNewAnchor);
	
	//根据锚点修改位置	
	float fangle = node->getRotation() * (float)M_PI / 180 * (-1);
	auto pointChange = (pointbyOldAnchor * node->getScale()).rotateByAngle(Vec2(), fangle);
	node->setPosition(node->getPosition() + pointChange);
}

void lly::TouchFunction::changeAnchorBack( cocos2d::Node* node, cocos2d::Vec2 &oldAnchor )
{
	//获取相对老点，新点偏移的距离
	Vec2 pointNewAnchor = node->getAnchorPoint(); 
	Size nodeSize = node->getContentSize();
	Vec2 anchorSub = pointNewAnchor - oldAnchor;
	auto pointbyOldAnchor = Vec2(nodeSize.width * anchorSub.x, nodeSize.height * anchorSub.y);

	//计算缩放和旋转下的偏移距离
	float fangle = node->getRotation() * (float)M_PI / 180 * (-1);
	auto pointChangeBack = (pointbyOldAnchor * node->getScale()).rotateByAngle(Vec2(), fangle);

	//移动
	node->setPosition(node->getPosition() - pointChangeBack);
	
	//改变锚点
	node->setAnchorPoint(oldAnchor);
}

//========================================================
lly::TouchFunction::TouchesCallback lly::TouchFunction::m_moveMoved =
	[=](const cocos2d::Vec2* p1, const cocos2d::Vec2* p2, cocos2d::Event* e, cocos2d::Node* node)
{
	auto nodePoint = node->getPosition();
	node->setPosition(nodePoint + p1[0] - p1[1]); //位置加上两次点的差

	//CCLOG("point1 from %f, %f, to %f, %f == %f, %f", point1Pre.x, point1Pre.y, point1.x, point1.y, point1.x - point1Pre.x, point1.y - point1Pre.y);
	//CCLOG("point from %f, %f, to %f, %f == %f, %f", nodePoint.x, nodePoint.y, node->getPosition().x, node->getPosition().y, node->getPosition().x - nodePoint.x, node->getPosition().y - nodePoint.y);
};

lly::TouchFunction::TouchesCallback lly::TouchFunction::m_scaleMoved =
	[=](const cocos2d::Vec2* p1, const cocos2d::Vec2* p2, cocos2d::Event* e, cocos2d::Node* node)
{
	//计算上次点距和当前点距 //带着平方优化效率
	float distancePre =  p1[1].distanceSquared(p2[1]); 
	float distanceNow = p1[0].distanceSquared(p2[0]);
	//log("pre %f, %f ------ %f, %f ------ %f", point1.x, point1.y, point2.x, point2.y, sqrt(distancePre));
	//log("now %f, %f ------ %f, %f ------ %f", point1.x, point1.y, point2.x, point2.y, sqrt(distanceNow));

	//获得当前和上次两点距离的比例 //在此开方
	float rate = sqrt(distanceNow / distancePre);
	
	//缩放
	node->setScale(node->getScale() * rate); 
};

lly::TouchFunction::TouchesCallback lly::TouchFunction::m_rotateMoved =
	[=](const cocos2d::Vec2* p1, const cocos2d::Vec2* p2, cocos2d::Event* e, cocos2d::Node* node)
{
	auto point1p = p1[1];
	auto point2p = p2[1];
	auto point1 = p1[0];
	auto point2 = p2[0];

	//视为新旧两条矢量，平移新矢量，使新旧矢量原点相同
	point2 = point2 + point1p - point1;
	point2p  -= point1p; //将原点设为0
	point2 -= point1p;

	//计算角度
	float fangle = Vec2::angle(point2p, point2) / M_PI * 180;

	//计算角度的方向 叉乘大于0则为逆时针需要乘以-1
	if (point2p.cross(point2) > 0) fangle *= -1;
	/*log("%f angel", fangle);*/

	//改变node角度
	node->setRotation(node->getRotation() + fangle);
};

//静态变量的初始化
std::map <cocos2d::Node*, TouchFunction::struTouchNodeInfo> lly::TouchFunction::mNodeIsHitted = std::map <cocos2d::Node*, TouchFunction::struTouchNodeInfo>(); 

//===========================================================
lly::MovableLayer::MovableLayer() : Layer()
{

}

lly::MovableLayer::~MovableLayer()
{

}

MovableLayer* lly::MovableLayer::create( uint32_t flag )
{
	MovableLayer* pRet = new MovableLayer();
	if (pRet && pRet->init(flag))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool lly::MovableLayer::init( uint32_t flag )
{
	if (!Layer::init()) return false;

	this->ignoreAnchorPointForPosition(false);
	TouchFunction::setTouchFunction(this, flag);

	return true;
}

//============================================================
IMPLEMENT_CLASS_GUI_INFO(lly::MovableLayout)

lly::MovableLayout::MovableLayout() : Layout()
{

}

lly::MovableLayout::~MovableLayout()
{

}

MovableLayout* lly::MovableLayout::create( uint32_t flag )
{
	MovableLayout* pRet = new MovableLayout();
	if (pRet && pRet->init(flag))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool lly::MovableLayout::init()
{
	return Layout::init();
}

bool lly::MovableLayout::init( uint32_t flag )
{
	if (!MovableLayout::init()) return false;
	setTouchFunction(flag);

	return true;
}

void lly::MovableLayout::setTouchFunction( uint32_t flag )
{
	this->ignoreAnchorPointForPosition(false);
	TouchFunction::setTouchFunction(this, flag);
}


