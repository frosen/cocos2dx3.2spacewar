#include "llyMovableLayer.h"

USING_NS_CC;
using namespace ui;
using namespace lly;

void lly::TouchFunction::setTouchFunction( Node* node, uint32_t flag )
{
	TouchesCallback moveMoved = nullptr;
	TouchesCallback scaleMoved = nullptr;
	TouchesCallback rotateMoved = nullptr;

	//����flag Ϊtouch�����Ӧ�Ĳ���
	if (flag & TouchFunction::move) moveMoved = m_moveMoved;
	if (flag & TouchFunction::scale) scaleMoved = m_scaleMoved;
	if (flag & TouchFunction::rotate) rotateMoved = m_rotateMoved;

	//������㴥�ؼ���
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchAllAtOnce::create();

	//�����ֲ�ͬ�������ֻ�ƶ���ֻ�д�С�仯����ת�������ƶ����б仯���ֱ��Ӧ��ͬ�Ļص�������ص������Ĳ���
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
		TouchFunction::refresh(nullptr); //ˢ��ȫ��
	};

	dispatcher->addEventListenerWithSceneGraphPriority(listener, node);
}

void lly::TouchFunction::refresh( cocos2d::Node* node /*= nullptr*/ )
{
	if (node) 
	{
		auto it = mNodeIsHitted.find(node);
		if (it == mNodeIsHitted.end()) return; //��֤node��map���Ǵ��ڵ�	
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
	if (it == mNodeIsHitted.end()) //�ѵ�һ���Ƿ���е���Ϣͨ��map���ݸ�move����
	{
		if (needChangeAnchor) //��ê���ƶ�������
		{
			Vec2 pointOldAnchor; //��¼ԭ����ê����
			changeAnchorToTouchPoint(node, touchpointInNode, pointOldAnchor);

			mNodeIsHitted[node] = struTouchNodeInfo(1, pointOldAnchor);			
		}
		else
		{			
			mNodeIsHitted[node] = struTouchNodeInfo(1, node->getAnchorPoint());
		}			
	}	
	else if (it->second.nNumHittedPoint == 1) //node����ʱ���ڴ��жϵڶ����Ƿ����У������������жϵ�����
	{
		Vec2 secondPoint = vTouch[0]->getLocation();
		Vec2 secondPointInNode = node->convertToNodeSpace(secondPoint);

		it->second.nNumHittedPoint = 2;

		if (!needChangeAnchor) //��ê���ƶ�������
		{
			changeAnchorBack(node, it->second.anchorOld);

			Vec2 pointOldAnchor; //��¼ԭ����ê����
			Vec2 secondPointInNode_2 = node->convertToNodeSpace(secondPoint); //node�仯����������ڵ���λ��
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
	if (it == mNodeIsHitted.end()) return; //���node��map���Ǵ��ڵģ���ʾ�е����

	//��ȡ��Ҫ�Ĵ���
	Vec2 touchpoint1[2];
	touchpoint1[0] = vTouch[0]->getLocation();
	touchpoint1[1] = vTouch[0]->getPreviousLocation();

	//�ƶ�ֻ��Ҫ����
	moveMoved(touchpoint1, nullptr, event, node);

	if (vTouch.size() <= 1 || it->second.nNumHittedPoint < 2) return; //���ڶ����Ƿ���ڲ��ҵ���

	//��Ҫ����
	//��ȡ�ڶ���
	Vec2 touchpoint2[2];
	touchpoint2[0] = vTouch[1]->getLocation();
	touchpoint2[1] = vTouch[1]->getPreviousLocation();

	if (scaleMoved) scaleMoved(touchpoint1, touchpoint2, event, node);
	if (rotateMoved) rotateMoved(touchpoint1, touchpoint2, event, node);
}

void lly::TouchFunction::touchMoveWithoutWidgetMove( const std::vector<cocos2d::Touch*> &vTouch, cocos2d::Event* event, cocos2d::Node* node, TouchesCallback moveMoved, TouchesCallback scaleMoved, TouchesCallback rotateMoved )
{
	auto it = mNodeIsHitted.find(node);
	if (it == mNodeIsHitted.end()) return; //���node��map���Ǵ��ڵģ���ʾ�е����

	//��ȡ��Ҫ�Ĵ���
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
	if (it == mNodeIsHitted.end()) return; //��֤node��map���Ǵ��ڵ�	

	Vec2 endPoint = vTouch[0]->getLocation();
	Vec2 endPointInNode = node->convertToNodeSpace(endPoint);

	if (!containsPoint(node, endPointInNode)) return;
	
	if (it->second.nNumHittedPoint >= 2) //��n�����
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

		mNodeIsHitted.erase(node);//ֻ��һ��������Ƴ�node��map��	
	}
	else
	{
		mNodeIsHitted.erase(node); //�������������node�Ĵ������
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
	//�µ������ê���ƫ�ƾ���
	auto pointbyOldAnchor = pointInNode - node->getAnchorPointInPoints();

	//��¼ԭ����
	oldAnchor = node->getAnchorPoint(); 

	//�õ���ǰ��λ����Ӧ��ê��
	Vec2 pointNewAnchor;
	Size nodeSize = node->getContentSize();
	pointNewAnchor.x = pointInNode.x / nodeSize.width;
	pointNewAnchor.y = pointInNode.y / nodeSize.height;

	//�ı�ê��
	node->setAnchorPoint(pointNewAnchor);
	
	//����ê���޸�λ��	
	float fangle = node->getRotation() * (float)M_PI / 180 * (-1);
	auto pointChange = (pointbyOldAnchor * node->getScale()).rotateByAngle(Vec2(), fangle);
	node->setPosition(node->getPosition() + pointChange);
}

void lly::TouchFunction::changeAnchorBack( cocos2d::Node* node, cocos2d::Vec2 &oldAnchor )
{
	//��ȡ����ϵ㣬�µ�ƫ�Ƶľ���
	Vec2 pointNewAnchor = node->getAnchorPoint(); 
	Size nodeSize = node->getContentSize();
	Vec2 anchorSub = pointNewAnchor - oldAnchor;
	auto pointbyOldAnchor = Vec2(nodeSize.width * anchorSub.x, nodeSize.height * anchorSub.y);

	//�������ź���ת�µ�ƫ�ƾ���
	float fangle = node->getRotation() * (float)M_PI / 180 * (-1);
	auto pointChangeBack = (pointbyOldAnchor * node->getScale()).rotateByAngle(Vec2(), fangle);

	//�ƶ�
	node->setPosition(node->getPosition() - pointChangeBack);
	
	//�ı�ê��
	node->setAnchorPoint(oldAnchor);
}

//========================================================
lly::TouchFunction::TouchesCallback lly::TouchFunction::m_moveMoved =
	[=](const cocos2d::Vec2* p1, const cocos2d::Vec2* p2, cocos2d::Event* e, cocos2d::Node* node)
{
	auto nodePoint = node->getPosition();
	node->setPosition(nodePoint + p1[0] - p1[1]); //λ�ü������ε�Ĳ�

	//CCLOG("point1 from %f, %f, to %f, %f == %f, %f", point1Pre.x, point1Pre.y, point1.x, point1.y, point1.x - point1Pre.x, point1.y - point1Pre.y);
	//CCLOG("point from %f, %f, to %f, %f == %f, %f", nodePoint.x, nodePoint.y, node->getPosition().x, node->getPosition().y, node->getPosition().x - nodePoint.x, node->getPosition().y - nodePoint.y);
};

lly::TouchFunction::TouchesCallback lly::TouchFunction::m_scaleMoved =
	[=](const cocos2d::Vec2* p1, const cocos2d::Vec2* p2, cocos2d::Event* e, cocos2d::Node* node)
{
	//�����ϴε��͵�ǰ��� //����ƽ���Ż�Ч��
	float distancePre =  p1[1].distanceSquared(p2[1]); 
	float distanceNow = p1[0].distanceSquared(p2[0]);
	//log("pre %f, %f ------ %f, %f ------ %f", point1.x, point1.y, point2.x, point2.y, sqrt(distancePre));
	//log("now %f, %f ------ %f, %f ------ %f", point1.x, point1.y, point2.x, point2.y, sqrt(distanceNow));

	//��õ�ǰ���ϴ��������ı��� //�ڴ˿���
	float rate = sqrt(distanceNow / distancePre);
	
	//����
	node->setScale(node->getScale() * rate); 
};

lly::TouchFunction::TouchesCallback lly::TouchFunction::m_rotateMoved =
	[=](const cocos2d::Vec2* p1, const cocos2d::Vec2* p2, cocos2d::Event* e, cocos2d::Node* node)
{
	auto point1p = p1[1];
	auto point2p = p2[1];
	auto point1 = p1[0];
	auto point2 = p2[0];

	//��Ϊ�¾�����ʸ����ƽ����ʸ����ʹ�¾�ʸ��ԭ����ͬ
	point2 = point2 + point1p - point1;
	point2p  -= point1p; //��ԭ����Ϊ0
	point2 -= point1p;

	//����Ƕ�
	float fangle = Vec2::angle(point2p, point2) / M_PI * 180;

	//����Ƕȵķ��� ��˴���0��Ϊ��ʱ����Ҫ����-1
	if (point2p.cross(point2) > 0) fangle *= -1;
	/*log("%f angel", fangle);*/

	//�ı�node�Ƕ�
	node->setRotation(node->getRotation() + fangle);
};

//��̬�����ĳ�ʼ��
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


