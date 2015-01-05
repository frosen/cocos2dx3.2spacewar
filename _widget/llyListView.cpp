#include "llyListView.h"
#include "ui/UIHelper.h"
#include "extensions/GUI/CCControlExtension/CCScale9Sprite.h"

USING_NS_CC;

using namespace lly;

IMPLEMENT_CLASS_GUI_INFO(lly::ListView)

lly::ListView::ListView() : cocos2d::ui::ListView(), 
	//����� ============
	m_bAnimation(false),
	m_fWidgetMoveSpeed(1),
	m_itemMove(nullptr),
	m_fDistance(0),
	m_bChangeZOrder(false),
	m_fMarginToTop(0),
	addChangeItemEventFunc(nullptr),
	m_posMaxMove(Vec2(5, 5)),
	_bMoved(false)
{

}

lly::ListView::~ListView()
{

}

lly::ListView* lly::ListView::create()
{
	auto wi = new lly::ListView();
	if (wi && wi->init())
	{
		wi->autorelease();
		return wi;
	}
	else
	{
		CC_SAFE_DELETE(wi);
		return nullptr;
	}
	
}

void lly::ListView::pushBackDefaultItem()
{
	ui::ListView::pushBackDefaultItem();
	changeWidgetEvent(changeItemType::ADD_ITEM); //��ɾ�Ŀؼ�ʱ��Ļص�
}

void lly::ListView::insertDefaultItem(ssize_t index)
{
	if (!_model) return;

	//===============================
	if (m_bAnimation) //�����
	{
		//���ڸ���״̬ʱ�򣬲����������Ĳ���
		if (_refreshViewDirty) return;

		//�ж�Ҫ����ĵ��Ƿ����
		if (index > (int)_items.size())
		{
			CCLOG("count %d small than %d", _items.size(), index);
			return;
		}

		//������滹�пؼ��Ļ�����˿ؼ�ΪĿ��ؼ�
		auto item2 = getItem(index);
		if (item2)
		{
			m_itemMove = item2;

			//����ɾ��������ؼ����ߵױ�������ؼ��ľ��룺ע��˴��и�����
			m_fDistance = -getDistance(_model);
		}
	}
	//===========================

	ui::ListView::insertDefaultItem(index);

	changeWidgetEvent(changeItemType::ADD_ITEM); //��ɾ�Ŀؼ�ʱ��Ļص�
}

void lly::ListView::pushBackCustomItem(Widget* item)
{
	ui::ListView::pushBackCustomItem(item);
	changeWidgetEvent(changeItemType::ADD_ITEM); //��ɾ�Ŀؼ�ʱ��Ļص�
}

void lly::ListView::insertCustomItem(Widget* item, ssize_t index)
{
	//===============================
	if (m_bAnimation) //�����
	{
		//���ڸ���״̬ʱ�򣬲����������Ĳ���
		if (_refreshViewDirty) return;

		//�ж�Ҫ����ĵ��Ƿ����
		if (index > (int)_items.size())
		{
			CCLOG("count %d small than %d", _items.size(), index);
			return;
		}

		//������滹�пؼ��Ļ�����˿ؼ�ΪĿ��ؼ�
		auto item2 = getItem(index);
		if (item2)
		{
			m_itemMove = item2;

			//����ɾ��������ؼ����ߵױ�������ؼ��ľ��룺ע��˴��и�����
			m_fDistance = -getDistance(_model);
		}
	}
	//===========================

	ui::ListView::insertCustomItem(item, index);

	changeWidgetEvent(changeItemType::ADD_ITEM); //��ɾ�Ŀؼ�ʱ��Ļص�
}

void ListView::removeItem(ssize_t index)
{
	Widget* item = getItem(index);
	if (!item) return;

	//===========================
	if (m_targetItem == item) m_targetItem = nullptr; //ȡ��Ŀ��ؼ�

	if (m_bAnimation)
	{
		//���ڸ���״̬ʱ�򣬲����������Ĳ���
		if (_refreshViewDirty == true) return;

		//������滹�пؼ��Ļ�����˿ؼ�ΪĿ��ؼ�
		auto item2 = getItem(index + 1);
		if (item2)
		{
			m_itemMove = item2;

			//����ɾ��������ؼ����ߵױ�������ؼ��ľ���
			m_fDistance = getDistance(item);
		}		
	}
	//===========================

	ui::ListView::removeItem(index);

	changeWidgetEvent(changeItemType::REMOVE_ITEM); //��ɾ�Ŀؼ�ʱ��Ļص�
}

void ListView::removeLastItem()
{
	if (m_targetItem == getLastItem()) m_targetItem = nullptr; //ȡ��Ŀ��ؼ�
	ui::ListView::removeLastItem();
	changeWidgetEvent(changeItemType::REMOVE_ITEM); //��ɾ�Ŀؼ�ʱ��Ļص�
}

void ListView::removeAllItems()
{
	if (m_targetItem) m_targetItem = nullptr; //ȡ��Ŀ��ؼ�
	ui::ListView::removeAllItems();
	changeWidgetEvent(changeItemType::REMOVE_ITEM); //��ɾ�Ŀؼ�ʱ��Ļص�
}

void lly::ListView::doLayout()
{
	lly::ScrollView::doLayout();
	if (_refreshViewDirty)
	{
		refreshView();
		if (m_bAnimation) //�޸�===============
		{			
			//�����벻��0��ʱ���������仯��0����0�Ļ����ø��½���
			if (m_fDistance > 0)
			{
				m_fDistance -= m_fWidgetMoveSpeed;

				if (m_fDistance <= 0) m_fDistance = 0;
			}
			else if (m_fDistance < 0)
			{
				m_fDistance += m_fWidgetMoveSpeed;

				if (m_fDistance >= 0) m_fDistance = 0;	
			}
			else
			{
				_refreshViewDirty = false;

				//���½���ʱ��visit�������Ŀ��ؼ���z�ᵽ���ϲ�
				m_bChangeZOrder = true; 
			}  
		}
		else
		{
			_refreshViewDirty = false;
		}	      
	}
}

//=========================================�����
void lly::ListView::removeItem( Widget* item )
{
	if (!item) return;

	if (m_targetItem == item) m_targetItem = nullptr; //ȡ��Ŀ��ؼ�

	if (m_bAnimation)
	{
		//���ڸ���״̬ʱ�򣬲����������Ĳ���
		if (_refreshViewDirty == true) return;

		//������滹�пؼ��Ļ�����˿ؼ�ΪĿ��ؼ�
		auto item2 = getItem(getIndex(item) + 1);
		if (item2)
		{
			m_itemMove = item2;

			//����ɾ��������ؼ����ߵױ�������ؼ��ľ���
			m_fDistance = getDistance(item);
		}		
	}
	//===========================

	removeChild(item, true);
	_refreshViewDirty = true;

	changeWidgetEvent(changeItemType::REMOVE_ITEM); //��ɾ�Ŀؼ�ʱ��Ļص�
}

void lly::ListView::moveItem( cocos2d::ui::Widget* item, WidgetMoveDirection dir, WidgetMoveType t /*= MOVE_BY*/, int num /*= 1*/ )
{
	if (!item || !_items.contains(item))
	{
		CCLOG("@no item can move");
		return;
	}

	int nAimIndex;
	if (t == WidgetMoveType::MOVE_BY)
	{
		if (dir == WidgetMoveDirection::MOVE_FORWARD)
			nAimIndex = _items.getIndex(item) + num;
		else
			nAimIndex = _items.getIndex(item) - num;
	}
	else
		nAimIndex = num;

	if (nAimIndex >= (int)_items.size() || nAimIndex < 0)
	{
		CCLOG("can not move to %d", nAimIndex);
		nAimIndex = 0;			
	}

	//===================================
	Widget* widget = dynamic_cast<Widget*>(item);
	if (widget) {
		_items.eraseObject(widget);
		_items.insert(nAimIndex, widget);
	}	
	_refreshViewDirty = true;
	//======================================

	changeWidgetEvent(changeItemType::MOVE_ITEM); //��ɾ�Ŀؼ�ʱ��Ļص�
}

//===============================����
void lly::ListView::updateInnerContainerSize()
{
	switch (_direction)
	{
	case Direction::VERTICAL:
		{
			size_t length = _items.size();
			float totalHeight = (length - 1) * _itemsMargin;
			for (auto& item : _items)
			{
				totalHeight += item->getContentSize().height;
			}

			//=============================
			//���϶���ʱ�仯�ľ���
			totalHeight += m_fDistance;

			//����ͷ�հף�β�հ�
			totalHeight += 2 * m_fMarginToTop;
			//=============================

			float finalWidth = _contentSize.width;
			float finalHeight = totalHeight;
			setInnerContainerSize(Size(finalWidth, finalHeight));
			break;
		}
	case Direction::HORIZONTAL:
		{
			size_t length = _items.size();
			float totalWidth = (length - 1) * _itemsMargin;
			for (auto& item : _items)
			{
				totalWidth += item->getContentSize().width;
			}

			//=============================
			//���϶���ʱ�仯�ľ���
			totalWidth += m_fDistance;

			//����ͷ�հף�β�հ�
			totalWidth += 2 * m_fMarginToTop;
			//=============================

			float finalWidth = totalWidth;
			float finalHeight = _contentSize.height;
			setInnerContainerSize(Size(finalWidth, finalHeight));
			break;
		}
	default:
		break;
	}
}

void lly::ListView::remedyLayoutParameter(Widget* item)
{
	if (!item)
	{
		return;
	}
	switch (_direction) {
	case Direction::VERTICAL:
		{
			ui::LinearLayoutParameter* llp = (ui::LinearLayoutParameter*)(item->getLayoutParameter());
			if (!llp)
			{
				ui::LinearLayoutParameter* defaultLp = ui::LinearLayoutParameter::create();
				switch (_gravity) {
				case Gravity::LEFT:
					defaultLp->setGravity(ui::LinearLayoutParameter::LinearGravity::LEFT);
					break;
				case Gravity::RIGHT:
					defaultLp->setGravity(ui::LinearLayoutParameter::LinearGravity::RIGHT);
					break;
				case Gravity::CENTER_HORIZONTAL:
					defaultLp->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL);
					break;
				default:
					break;
				}
				//=====================================
				if (getIndex(item) == 0)
				{
					if (m_itemMove == item) //�����Ŀ��ؼ�����Ҫ���Ͼ���
					{
						defaultLp->setMargin(ui::Margin(
							0.0f, m_fMarginToTop + m_fDistance, 0.0f, 0.0f));
					}
					else
					{
						defaultLp->setMargin(ui::Margin(
							0.0f, m_fMarginToTop, 0.0f, 0.0f));
					}
				}
				else
				{
					if (m_itemMove == item) //�����Ŀ��ؼ�����Ҫ���Ͼ���
					{
						defaultLp->setMargin(ui::Margin(
							0.0f, _itemsMargin + m_fDistance, 0.0f, 0.0f));
					}
					else
					{
						defaultLp->setMargin(ui::Margin(
							0.0f, _itemsMargin, 0.0f, 0.0f));
					}
				}
				//====================================
				item->setLayoutParameter(defaultLp);
			}
			else
			{
				//======================================
				if (getIndex(item) == 0)
				{
					if (m_itemMove == item) //�����Ŀ��ؼ�����Ҫ���Ͼ���
					{
						llp->setMargin(ui::Margin(
							0.0f, m_fMarginToTop + m_fDistance, 0.0f, 0.0f));
					}
					else
					{
						llp->setMargin(ui::Margin(
							0.0f, m_fMarginToTop, 0.0f, 0.0f));
					}
				}
				else
				{
					if (m_itemMove == item) //�����Ŀ��ؼ�����Ҫ���Ͼ���
					{
						llp->setMargin(ui::Margin(
							0.0f, _itemsMargin + m_fDistance, 0.0f, 0.0f));
					}
					else
					{
						llp->setMargin(ui::Margin(
							0.0f, _itemsMargin, 0.0f, 0.0f));
					}
				}
				//====================================
				switch (_gravity) {
				case Gravity::LEFT:
					llp->setGravity(ui::LinearLayoutParameter::LinearGravity::LEFT);
					break;
				case Gravity::RIGHT:
					llp->setGravity(ui::LinearLayoutParameter::LinearGravity::RIGHT);
					break;
				case Gravity::CENTER_HORIZONTAL:
					llp->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL);
					break;
				default:
					break;
				}
			}
			break;
		}
	case Direction::HORIZONTAL:
		{
			ui::LinearLayoutParameter* llp = (ui::LinearLayoutParameter*)(item->getLayoutParameter());
			if (!llp)
			{
				ui::LinearLayoutParameter* defaultLp = ui::LinearLayoutParameter::create();
				switch (_gravity) {
				case Gravity::TOP:
					defaultLp->setGravity(ui::LinearLayoutParameter::LinearGravity::TOP);
					break;
				case Gravity::BOTTOM:
					defaultLp->setGravity(ui::LinearLayoutParameter::LinearGravity::BOTTOM);
					break;
				case Gravity::CENTER_VERTICAL:
					defaultLp->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_VERTICAL);
					break;
				default:
					break;
				}
				//=========================================
				if (getIndex(item) == 0)
				{
					if (m_itemMove == item) //�����Ŀ��ؼ�����Ҫ���ϱ仯ֵ
					{
						defaultLp->setMargin(ui::Margin(
							m_fMarginToTop + m_fDistance, 0.0f, 0.0f, 0.0f));
					}
					else
					{
						defaultLp->setMargin(ui::Margin(
							m_fMarginToTop, 0.0f, 0.0f, 0.0f));
					}
				}
				else
				{
					if (m_itemMove == item) //�����Ŀ��ؼ�����Ҫ���ϱ仯ֵ
					{
						defaultLp->setMargin(ui::Margin(
							_itemsMargin + m_fDistance, 0.0f, 0.0f, 0.0f));
					}
					else
					{
						defaultLp->setMargin(ui::Margin(
							_itemsMargin, 0.0f, 0.0f, 0.0f));
					}
				}
				item->setLayoutParameter(defaultLp);
			}
			else
			{
				if (getIndex(item) == 0)
				{
					if (m_itemMove == item) //�����Ŀ��ؼ�����Ҫ���ϱ仯ֵ
					{
						llp->setMargin(ui::Margin(
							m_fMarginToTop + m_fDistance, 0.0f, 0.0f, 0.0f));
					}
					else
					{
						llp->setMargin(ui::Margin(
							m_fMarginToTop, 0.0f, 0.0f, 0.0f));
					}
				}
				else
				{
					if (m_itemMove == item) //�����Ŀ��ؼ�����Ҫ���ϱ仯ֵ
					{
						llp->setMargin(ui::Margin(
							_itemsMargin + m_fDistance, 0.0f, 0.0f, 0.0f));
					}
					else
					{
						llp->setMargin(ui::Margin(
							_itemsMargin, 0.0f, 0.0f, 0.0f));
					}
				}
				//========================================
				switch (_gravity) {
				case Gravity::TOP:
					llp->setGravity(ui::LinearLayoutParameter::LinearGravity::TOP);
					break;
				case Gravity::BOTTOM:
					llp->setGravity(ui::LinearLayoutParameter::LinearGravity::BOTTOM);
					break;
				case Gravity::CENTER_VERTICAL:
					llp->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_VERTICAL);
					break;
				default:
					break;
				}
			}
			break;
		}
	default:
		break;
	}
}

ui::Widget* lly::ListView::createCloneInstance()
{
	return lly::ListView::create();
}

void lly::ListView::copySpecialProperties(Widget *widget)
{
	lly::ListView* listViewEx = dynamic_cast<lly::ListView*>(widget);
	if (listViewEx)
	{
		ui::ListView::copySpecialProperties(listViewEx);
		
		m_bAnimation = listViewEx->m_bAnimation; //�Ƿ���������Ĭ�ϲ�������
		m_fWidgetMoveSpeed = listViewEx->m_fWidgetMoveSpeed; //�ؼ������ƶ����ٶȣ�Ĭ��ÿ֡1���أ�
		m_fMarginToTop = listViewEx->m_fMarginToTop; //��һ���ؼ������������ľ���
		addChangeItemEventFunc = listViewEx->addChangeItemEventFunc;
		m_posMaxMove = listViewEx->m_posMaxMove;
	}
}

void lly::ListView::copyClonedWidgetChildren( cocos2d::ui::Widget* model )
{
	auto listview = static_cast<ListView*>(model);
	auto& arrayItems = listview->getItems();
	bool bAnim = listview->isAnimationEnabled();
	setAnimationEnabled(false);
	for (auto& item : arrayItems)
	{		
		pushBackCustomItem(item->clone());
	}
	setAnimationEnabled(bAnim);
}

void lly::ListView::interceptTouchEvent(TouchEventType event, Widget *sender, Touch* touch)
{
	ScrollView::interceptTouchEvent(event, sender, touch);

	switch (event)
	{
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		{
			//���ƶ��ľ������һ����������Ϊ�ƶ���������Ӧtouch����
			auto movePoint = touch->getLocation(); 
			if (_bMoved == false && 
				(abs(movePoint.x - m_posBegintouch.x) > m_posMaxMove.x || 
				abs(movePoint.y - m_posBegintouch.y) > m_posMaxMove.y))
				_bMoved = true;
		}		
		break;
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		_bMoved = false;
		m_posBegintouch = touch->getLocation();

		//û��break

	case cocos2d::ui::Widget::TouchEventType::ENDED:

		if (_bMoved) break;

		//============orig
		{
			Widget* parent = sender;
			while (parent)
			{
				if (parent && parent->getParent() == _innerContainer)
				{
					_curSelectedIndex = getIndex(parent);
					break;
				}
				parent = dynamic_cast<Widget*>(parent->getParent());
			}
			if (sender->isHighlighted()) {
				selectedItemEvent(event);
			}
		}	
		//===============
		break;
	default:
		break;
	}
}

//=============================================�����
void lly::ListView::changeWidgetEvent( changeItemType type )
{
	if (addChangeItemEventFunc)
		addChangeItemEventFunc(this, type);
}

void ListView::visit( Renderer *renderer, const Mat4& parentTransform, bool parentTransformUpdated )
{
	ui::ListView::visit(renderer, parentTransform, parentTransformUpdated);

	//�ı�Ŀ��ؼ���z��������
	if (m_targetItem == nullptr || m_bChangeZOrder == false) return;
	m_bChangeZOrder = false;
	m_targetItem->setLocalZOrder(INT_MAX);
}



