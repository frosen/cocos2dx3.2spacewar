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

	//设定标识
	bool bMove = flag & TouchFunction::move;
	bool bChange = flag & TouchFunction::scale || flag & TouchFunction::rotate;

	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchAllAtOnce::create();

	listener->onTouchesBegan = [=](const std::vector<Touch*> &vTouch, Event* event)
	{
		////////////////////////////测试锚点位置随触点改变
// 		auto point = vTouch[0]->getLocation();
// 		//记录原数据
// 		_oldAnchor = node->getAnchorPoint(); 
// 		_oldIgnore = node->isIgnoreAnchorPointForPosition();
// 
// 		//获得本地坐标下的点位
// 		auto pointInNode = node->convertToNodeSpaceAR(point);
// 
// 		//得到当前点位所对应的锚点
// 		Vec2 pointNewAnchor;
// 		pointNewAnchor.x = node->getAnchorPoint().x + (pointInNode.x / node->getContentSize().width);
// 		pointNewAnchor.y = node->getAnchorPoint().y + (pointInNode.y / node->getContentSize().height);
// 
// 		//改变锚点
// 		node->ignoreAnchorPointForPosition(false);
// 		node->setAnchorPoint(pointNewAnchor);
// 
// 		//根据锚点修改位置
// 		float fangle = node->getRotation() * (float)M_PI / 180 * (-1);
// 		auto pointChange = (pointInNode * node->getScale()).rotateByAngle(Vec2(), fangle);
// 		node->setPosition(node->getPosition() + pointChange);
// 
// 		//缩放
// 		//node->setScale(node->getScale() * rate); 
// 
// 		//还原锚点并移动至相应位置
// 		fangle = node->getRotation() * (float)M_PI / 180 * (-1);
// 		auto pointChangeBack = (pointInNode * node->getScale()).rotateByAngle(Vec2(), fangle);
// 		node->setPosition(node->getPosition() - pointChangeBack);
// 
// 		node->setAnchorPoint(_oldAnchor);
// 		node->ignoreAnchorPointForPosition(_oldIgnore);
		////////////////////////////////

		auto it = mNodeIsHitted.find(node);
		if (it == mNodeIsHitted.end()) //把第一点是否点中的消息通过map传递给move函数
		{
			Vec2 point1 = vTouch[0]->getLocation();
			Vec2 touchpoint = node->convertToNodeSpace(point1);
			if (containsPoint(node, touchpoint))
			{
				if (bMove && bChange) //又移动又变化才需要变更锚点
				{
					//把锚点移动到触点
					Vec2 pointOldAnchor; //记录原来的锚点用
					changeAnchorToTouchPoint(node, touchpoint, pointOldAnchor);

					//第一点点中则在map里加入node，并且记录原始锚点
					mNodeIsHitted[node] = isHitted(false, false, pointOldAnchor, point1, Vec2::ZERO); 	
				}
				else
				{
					mNodeIsHitted[node] = isHitted(false, false, node->getAnchorPoint(), point1, Vec2::ZERO); 	
				}
					
			}					
		}
		//node存在时，在此判断第二点是否点击中，若存在则不再判断第三点
		else if (it->second.bPoint2exist == false) 
		{
			it->second.bPoint2exist = true; //有第二点
			Vec2 secondPoint = vTouch[0]->getLocation();
			Vec2 pointInNode = node->convertToNodeSpace(secondPoint);
			if (containsPoint(node, pointInNode))
			{
				it->second.bPoint2Hitted = true; //第二点点中
				it->second.secondTouchPoint = secondPoint; //记录
			}
			else it->second.bPoint2Hitted = false;
		}
	};

	listener->onTouchesMoved = [=](const std::vector<Touch*> &vTouch, Event* event)
	{
		auto it = mNodeIsHitted.find(node);
		if (it == mNodeIsHitted.end()) return; //如果node在map里是存在的，表示有点点中

		//获取需要的触点
		Vec2 touchpoint1[2];
		touchpoint1[0] = vTouch[0]->getLocation();
		touchpoint1[1] = vTouch[0]->getPreviousLocation();

		if (bMove) //只移动或者移动加变化
		{
			//移动只需要单点
			moveMoved(touchpoint1, nullptr, event, node);
			
			if (vTouch.size() <= 1 || !it->second.bPoint2Hitted) return; //检测第二点是否存在并且点中

			//需要两点
			//获取第二点
			Vec2 touchpoint2[2];
			touchpoint2[0] = vTouch[1]->getLocation();
			touchpoint2[1] = vTouch[1]->getPreviousLocation();

			if (scaleMoved) scaleMoved(touchpoint1, touchpoint2, event, node);
			if (rotateMoved) rotateMoved(touchpoint1, touchpoint2, event, node);

			//需要记录两点作为end中修改锚点用
			it->second.firstTouchPoint = touchpoint1[0];
			it->second.secondTouchPoint = touchpoint2[0];
		}
		else //不移动只变化的话把本身锚点作为固定点，只需要单点
		{
			Vec2 touchpoint2[2];
			touchpoint2[0] = node->convertToWorldSpace(node->getAnchorPointInPoints());
			touchpoint2[1] = touchpoint2[0];

			if (scaleMoved) scaleMoved(touchpoint2, touchpoint1, event, node);
			if (rotateMoved) rotateMoved(touchpoint2, touchpoint1, event, node);

			if (vTouch.size() <= 1 || !it->second.bPoint2Hitted) return; //检测第二点是否存在并且点中

			//需要记录两点作为end中修改锚点用
			it->second.firstTouchPoint = touchpoint1[0];
			it->second.secondTouchPoint = vTouch[1]->getLocation();
		}		
	};

	listener->onTouchesEnded = [=](const std::vector<Touch*> &vTouch, Event* event)
	{
		auto it = mNodeIsHitted.find(node);
		if (it == mNodeIsHitted.end()) return; //保证node在map里是存在的	

		if (it->second.bPoint2exist == true) 
		{
			auto endPoint = vTouch[0]->getLocation();

			//不为两个点中的一个 说明取消的是第三个点
			if (it->second.firstTouchPoint == endPoint || it->second.secondTouchPoint == endPoint)
			{
				//取消记录第二点的存在
				it->second.bPoint2exist = false;

				if (it->second.bPoint2Hitted) //第二点击中
				{
					it->second.bPoint2Hitted = false;

					//有第二点，则检测end的是哪个点，如果是第一个点，则把锚点改到第二个点
					if (it->second.firstTouchPoint == endPoint) 
					{
						if (bMove && bChange) //又移动又变化才需要变更锚点
						{
							Vec2 pointInNode = node->convertToNodeSpace(endPoint); //本层相对位置
							changeAnchorBack(node, pointInNode, it->second.oldAnchor);

							Vec2 pointOldAnchor; //记录原来的锚点用
							Vec2 pointInNode2 = node->convertToNodeSpace(it->second.secondTouchPoint);
							changeAnchorToTouchPoint(node, pointInNode2, pointOldAnchor);

							it->second.oldAnchor = pointOldAnchor;
						}
						
						it->second.firstTouchPoint = it->second.secondTouchPoint;
					}
				}				
			}			
		}		 		
		else
		{
			if (bMove && bChange)
			{
				auto endPoint = vTouch[0]->getLocation();
				Vec2 pointInNode = node->convertToNodeSpace(endPoint); //本层相对位置
				Vec2 oldAnchor = it->second.oldAnchor;

				changeAnchorBack(node, pointInNode, oldAnchor);
			}
			mNodeIsHitted.erase(node);//只有一点击中则移除node在map中	
		}				
	};

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
		node->setAnchorPoint(it->second.oldAnchor);
		node->setScale(1);
		node->setRotation(0);
		mNodeIsHitted.erase(it);
	}
	else 
	{
		for (auto spair : mNodeIsHitted)
		{
			if (!spair.first) continue;
			
			spair.first->setAnchorPoint(spair.second.oldAnchor);
			spair.first->setScale(1);
			spair.first->setRotation(0);
		}
		mNodeIsHitted.clear();
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

void lly::TouchFunction::changeAnchorBack( cocos2d::Node* node, const cocos2d::Vec2 &pointInNode, cocos2d::Vec2 &oldAnchor )
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
std::map <cocos2d::Node*, TouchFunction::isHitted> lly::TouchFunction::mNodeIsHitted = std::map <cocos2d::Node*, TouchFunction::isHitted>(); 

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


