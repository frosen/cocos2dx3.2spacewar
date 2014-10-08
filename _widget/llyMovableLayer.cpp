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

	//��ͬ������touchlistener���ز�ͬ�Ļص�����
	typedef std::function<void(const std::vector<Touch*>&, Event*)> TouchListenerCallback;

	TouchListenerCallback _BeginNormal = [=](const std::vector<Touch*> &vTouch, Event* event)
	{
		auto it = mNodeIsHitted.find(node);
		if (it == mNodeIsHitted.end()) //�ѵ�һ���Ƿ���е���Ϣͨ��map���ݸ�move����
		{
			Vec2 point1 = vTouch[0]->getLocation();
			Vec2 touchpoint = node->convertToNodeSpace(point1);
			if (containsPoint(node, touchpoint))
			{
				mNodeIsHitted[node] = isHitted(false, false, node->getAnchorPoint(), point1, Vec2::ZERO);
			}					
		}
		//node����ʱ���ڴ��жϵڶ����Ƿ����У������������жϵ�����
		else if (it->second.bPoint2exist == false) 
		{
			it->second.bPoint2exist = true; //�еڶ���
			Vec2 secondPoint = vTouch[0]->getLocation();
			Vec2 pointInNode = node->convertToNodeSpace(secondPoint);
			if (containsPoint(node, pointInNode))
			{
				it->second.bPoint2Hitted = true; //�ڶ������
				it->second.secondTouchPoint = secondPoint; //��¼
			}
			else it->second.bPoint2Hitted = false;
		}
	};
	TouchListenerCallback _BeginWithChangeAnchor = [=](const std::vector<Touch*> &vTouch, Event* event)
	{
		auto it = mNodeIsHitted.find(node);
		if (it == mNodeIsHitted.end()) //�ѵ�һ���Ƿ���е���Ϣͨ��map���ݸ�move����
		{
			Vec2 point1 = vTouch[0]->getLocation();
			Vec2 touchpoint = node->convertToNodeSpace(point1);
			if (containsPoint(node, touchpoint))
			{
				//��ê���ƶ�������
				Vec2 pointOldAnchor; //��¼ԭ����ê����
				changeAnchorToTouchPoint(node, touchpoint, pointOldAnchor);

				//��һ���������map�����node�����Ҽ�¼ԭʼê��
				mNodeIsHitted[node] = isHitted(false, false, pointOldAnchor, point1, Vec2::ZERO); 	
			}					
		}
		//node����ʱ���ڴ��жϵڶ����Ƿ����У������������жϵ�����
		else if (it->second.bPoint2exist == false) 
		{
			it->second.bPoint2exist = true; //�еڶ���
			Vec2 secondPoint = vTouch[0]->getLocation();
			Vec2 pointInNode = node->convertToNodeSpace(secondPoint);
			if (containsPoint(node, pointInNode))
			{
				it->second.bPoint2Hitted = true; //�ڶ������
				it->second.secondTouchPoint = secondPoint; //��¼
			}
			else it->second.bPoint2Hitted = false;
		}
	};

	TouchListenerCallback _MoveHasWidgetMove = [=](const std::vector<Touch*> &vTouch, Event* event)
	{
		auto it = mNodeIsHitted.find(node);
		if (it == mNodeIsHitted.end()) return; //���node��map���Ǵ��ڵģ���ʾ�е����

		//��ȡ��Ҫ�Ĵ���
		Vec2 touchpoint1[2];
		touchpoint1[0] = vTouch[0]->getLocation();
		touchpoint1[1] = vTouch[0]->getPreviousLocation();

		//�ƶ�ֻ��Ҫ����
		moveMoved(touchpoint1, nullptr, event, node);

		if (vTouch.size() <= 1 || !it->second.bPoint2Hitted) return; //���ڶ����Ƿ���ڲ��ҵ���

		//��Ҫ����
		//��ȡ�ڶ���
		Vec2 touchpoint2[2];
		touchpoint2[0] = vTouch[1]->getLocation();
		touchpoint2[1] = vTouch[1]->getPreviousLocation();

		if (scaleMoved) scaleMoved(touchpoint1, touchpoint2, event, node);
		if (rotateMoved) rotateMoved(touchpoint1, touchpoint2, event, node);

		//��Ҫ��¼������Ϊend���޸�ê����
		it->second.firstTouchPoint = touchpoint1[0];
		it->second.secondTouchPoint = touchpoint2[0];
	};
	TouchListenerCallback _MoveWithoutWidgetMove = [=](const std::vector<Touch*> &vTouch, Event* event)
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

		if (vTouch.size() <= 1 || !it->second.bPoint2Hitted) return; //���ڶ����Ƿ���ڲ��ҵ���

		//��Ҫ��¼������Ϊend���޸�ê����
		it->second.firstTouchPoint = touchpoint1[0];
		it->second.secondTouchPoint = vTouch[1]->getLocation();
	};

	TouchListenerCallback _EndNormal = [=](const std::vector<Touch*> &vTouch, Event* event)
	{
		auto it = mNodeIsHitted.find(node);
		if (it == mNodeIsHitted.end()) return; //��֤node��map���Ǵ��ڵ�	

		if (it->second.bPoint2exist == true) 
		{
			auto endPoint = vTouch[0]->getLocation();

			//��Ϊ�������е�һ�� ˵��ȡ�����ǵ�������
			if (it->second.firstTouchPoint == endPoint || it->second.secondTouchPoint == endPoint)
			{
				//ȡ����¼�ڶ���Ĵ���
				it->second.bPoint2exist = false;

				if (it->second.bPoint2Hitted) //�ڶ������
				{
					it->second.bPoint2Hitted = false;

					//�еڶ��㣬����end�����ĸ��㣬����ǵ�һ���㣬���ê��ĵ��ڶ�����
					if (it->second.firstTouchPoint == endPoint) 
						it->second.firstTouchPoint = it->second.secondTouchPoint;
				}				
			}			
		}		 		
		else
		{
			mNodeIsHitted.erase(node);//ֻ��һ��������Ƴ�node��map��	
		}				
	};
	TouchListenerCallback _EndWithChangeAnchor = [=](const std::vector<Touch*> &vTouch, Event* event)
	{
		auto it = mNodeIsHitted.find(node);
		if (it == mNodeIsHitted.end()) return; //��֤node��map���Ǵ��ڵ�	

		if (it->second.bPoint2exist == true) 
		{
			auto endPoint = vTouch[0]->getLocation();

			//��Ϊ�������е�һ�� ˵��ȡ�����ǵ�������
			if (it->second.firstTouchPoint == endPoint || it->second.secondTouchPoint == endPoint)
			{
				//ȡ����¼�ڶ���Ĵ���
				it->second.bPoint2exist = false;

				if (it->second.bPoint2Hitted) //�ڶ������
				{
					it->second.bPoint2Hitted = false;

					//�еڶ��㣬����end�����ĸ��㣬����ǵ�һ���㣬���ê��ĵ��ڶ�����
					if (it->second.firstTouchPoint == endPoint) 
					{
						Vec2 pointInNode = node->convertToNodeSpace(endPoint); //�������λ��
						changeAnchorBack(node, pointInNode, it->second.oldAnchor);

						Vec2 pointOldAnchor; //��¼ԭ����ê����
						Vec2 pointInNode2 = node->convertToNodeSpace(it->second.secondTouchPoint);
						changeAnchorToTouchPoint(node, pointInNode2, pointOldAnchor);

						it->second.oldAnchor = pointOldAnchor;

						it->second.firstTouchPoint = it->second.secondTouchPoint;
					}
				}				
			}			
		}		 		
		else
		{
			auto endPoint = vTouch[0]->getLocation();
			Vec2 pointInNode = node->convertToNodeSpace(endPoint); //�������λ��
			Vec2 oldAnchor = it->second.oldAnchor;

			changeAnchorBack(node, pointInNode, oldAnchor);

			mNodeIsHitted.erase(node);//ֻ��һ��������Ƴ�node��map��	
		}				
	};	

	//������㴥�ؼ���
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchAllAtOnce::create();

	//�����ֲ�ͬ�������ֻ�ƶ���ֻ�д�С�仯����ת�������ƶ����б仯
	if ( flag & TouchFunction::move && !(flag & TouchFunction::scale || flag & TouchFunction::rotate))
	{
		listener->onTouchesBegan = _BeginNormal;
		listener->onTouchesMoved = _MoveHasWidgetMove;
		listener->onTouchesEnded = _EndNormal;
	}
	else if (!(flag & TouchFunction::move) && (flag & TouchFunction::scale || flag & TouchFunction::rotate))
	{
		listener->onTouchesBegan = _BeginNormal;
		listener->onTouchesMoved = _MoveWithoutWidgetMove;
		listener->onTouchesEnded = _EndNormal;
	}
	else
	{
		listener->onTouchesBegan = _BeginWithChangeAnchor;
		listener->onTouchesMoved = _MoveHasWidgetMove;
		listener->onTouchesEnded = _EndWithChangeAnchor;
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

void lly::TouchFunction::changeAnchorBack( cocos2d::Node* node, const cocos2d::Vec2 &pointInNode, cocos2d::Vec2 &oldAnchor )
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


