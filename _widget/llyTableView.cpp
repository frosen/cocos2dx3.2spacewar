#include "llyTableView.h"

using namespace lly;
USING_NS_CC;
using namespace ui;

namespace llyO_for_lly_tableview {

ListView::ListView():
	_model(nullptr),
	_gravity(Gravity::CENTER_VERTICAL),
	_itemsMargin(0.0f),
	_listViewEventListener(nullptr),
	_listViewEventSelector(nullptr),
	_curSelectedIndex(0),
	_refreshViewDirty(true),
	_eventCallback(nullptr),
	//自添加
	m_posMaxMove(Vec2(5, 5)),
	_bMoved(false)
{
	this->setTouchEnabled(true);
}

ListView::~ListView()
{
	_listViewEventListener = nullptr;
	_listViewEventSelector = nullptr;
	_items.clear();
	CC_SAFE_RELEASE(_model);
}

ListView* ListView::create()
{
	ListView* widget = new (std::nothrow) ListView();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

bool ListView::init()
{
	if (ScrollView::init())
	{
		setLayoutType(Type::VERTICAL);
		return true;
	}
	return false;
}

void ListView::setItemModel(Widget *model)
{
	if (!model)
	{
		return;
	}
	CC_SAFE_RELEASE_NULL(_model);
	_model = model;
	CC_SAFE_RETAIN(_model);
}

void ListView::updateInnerContainerSize()
{
	//已经重载于lly::ListView
}

void ListView::remedyLayoutParameter(Node *item)
{
	//已经重载于lly::ListView
}

void ListView::pushBackDefaultItem()
{
	if (!_model)
	{
		return;
	}
	Widget* newItem = _model->clone();
	//remedyLayoutParameter(newItem);
	addChild(newItem);
	_refreshViewDirty = true;
}

void ListView::insertDefaultItem(ssize_t index)
{
	if (!_model)
	{
		return;
	}
	Widget* newItem = _model->clone();

	_items.insert(index, newItem);
	ScrollView::addChild(newItem);

	//remedyLayoutParameter(newItem);

	_refreshViewDirty = true;
}


void ListView::pushBackCustomItem(Node* item)
{
	//remedyLayoutParameter(item);
	addChild(item);
	_refreshViewDirty = true;
}

void ListView::addChild(cocos2d::Node *child, int zOrder, int tag)
{
	ScrollView::addChild(child, zOrder, tag);
	_items.pushBack(child);
}

void ListView::addChild(cocos2d::Node *child)
{
	ListView::addChild(child, child->getLocalZOrder(), child->getName());
}

void ListView::addChild(cocos2d::Node *child, int zOrder)
{
	ListView::addChild(child, zOrder, child->getName());
}

void ListView::addChild(Node* child, int zOrder, const std::string &name)
{
	ScrollView::addChild(child, zOrder, name);
	_items.pushBack(child);
}

void ListView::removeChild(cocos2d::Node *child, bool cleaup)
{
	_items.eraseObject(child);
	ScrollView::removeChild(child, cleaup);
}

void ListView::removeAllChildren()
{
	this->removeAllChildrenWithCleanup(true);
}

void ListView::removeAllChildrenWithCleanup(bool cleanup)
{
	ScrollView::removeAllChildrenWithCleanup(cleanup);
	_items.clear();
}

void ListView::insertCustomItem(Node* item, ssize_t index)
{
	_items.insert(index, item);
	ScrollView::addChild(item);

	//remedyLayoutParameter(item);
	_refreshViewDirty = true;
}

void ListView::removeItem(ssize_t index)
{
	auto* item = getItem(index);
	if (!item)
	{
		return;
	}
	removeChild(item, true);

	_refreshViewDirty = true;
}

void ListView::removeLastItem()
{
	removeItem(_items.size() -1);
}

void ListView::removeAllItems()
{
	removeAllChildren();
}

Node* ListView::getItem(ssize_t index)const
{
	if (index < 0 || index >= _items.size())
	{
		return nullptr;
	}
	return _items.at(index);
}

Vector<Node*>& ListView::getItems()
{
	return _items;
}

ssize_t ListView::getIndex(Node *item) const
{
	if (!item)
	{
		return -1;
	}
	return _items.getIndex(item);
}

void ListView::setGravity(Gravity gravity)
{
	if (_gravity == gravity)
	{
		return;
	}
	_gravity = gravity;
	_refreshViewDirty = true;
}

void ListView::setItemsMargin(float margin)
{
	if (_itemsMargin == margin)
	{
		return;
	}
	_itemsMargin = margin;
	_refreshViewDirty = true;
}

float ListView::getItemsMargin()const
{
	return _itemsMargin;
}

void ListView::setDirection(Direction dir)
{
	switch (dir)
	{
	case Direction::VERTICAL:
		setLayoutType(Type::VERTICAL);
		break;
	case Direction::HORIZONTAL:
		setLayoutType(Type::HORIZONTAL);
		break;
	case Direction::BOTH:
		return;
	default:
		return;
		break;
	}
	ScrollView::setDirection(dir);
}

void ListView::requestRefreshView()
{
	_refreshViewDirty = true;
}

void ListView::refreshView()
{
	ssize_t length = _items.size();
	for (int i=0; i<length; i++)
	{
		auto* item = _items.at(i);
		item->setLocalZOrder(i);
		remedyLayoutParameter(item);
	}
	updateInnerContainerSize();
}

void ListView::doLayout()
{
	Layout::doLayout();

	if (_refreshViewDirty)
	{
		refreshView();
		_refreshViewDirty = false;
	}
}

void ListView::addEventListenerListView(Ref *target, SEL_ListViewEvent selector)
{
	_listViewEventListener = target;
	_listViewEventSelector = selector;
}


void ListView::addEventListener(const ccListViewCallback& callback)
{
	_eventCallback = callback;
}

void ListView::selectedItemEvent(TouchEventType event)
{
	switch (event)
	{
	case TouchEventType::BEGAN:
		{
			if (_listViewEventListener && _listViewEventSelector)
			{
				(_listViewEventListener->*_listViewEventSelector)(this, LISTVIEW_ONSELECTEDITEM_START);
			}
			if (_eventCallback) {
				_eventCallback(this,EventType::ON_SELECTED_ITEM_START);
			}
		}
		break;
	default:
		{
			if (_listViewEventListener && _listViewEventSelector)
			{
				(_listViewEventListener->*_listViewEventSelector)(this, LISTVIEW_ONSELECTEDITEM_END);
			}
			if (_eventCallback) {
				_eventCallback(this, EventType::ON_SELECTED_ITEM_END);
			}
		}
		break;
	}

}

void ListView::interceptTouchEvent(TouchEventType event, Widget *sender, Touch* touch)
{
	ScrollView::interceptTouchEvent(event, sender, touch);
	switch (event)
	{
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		{
			//若移动的距离大于一个量，则视为移动，而不响应touch结束
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

		//没有break

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

ssize_t ListView::getCurSelectedIndex() const
{
	return _curSelectedIndex;
}

void ListView::onSizeChanged()
{
	ScrollView::onSizeChanged();
	_refreshViewDirty = true;
}

std::string ListView::getDescription() const
{
	return "ListView for table";
}

Widget* ListView::createCloneInstance()
{
	return ListView::create();
}

void ListView::copyClonedWidgetChildren(Widget* model)
{
	//不可复制子控件
}

void ListView::copySpecialProperties(Widget *widget)
{
	ListView* listViewEx = dynamic_cast<ListView*>(widget);
	if (listViewEx)
	{
		ScrollView::copySpecialProperties(widget);
		setItemModel(listViewEx->_model);
		setItemsMargin(listViewEx->_itemsMargin);
		setGravity(listViewEx->_gravity);
		_listViewEventListener = listViewEx->_listViewEventListener;
		_listViewEventSelector = listViewEx->_listViewEventSelector;
		_eventCallback = listViewEx->_eventCallback;
	}
}

} //llyO_for_lly_tableview



//================================
using namespace lly;

IMPLEMENT_CLASS_GUI_INFO(lly::TableView)

lly::TableView::TableView() : 
	llyO_for_lly_tableview::ListView(),
	m_eLayoutType(ELayoutType::ABSOLUTE),
	m_eTVG(ETableViewGravity::LEFT_TOP),
	m_eArrangeDir(EArrangementDirection::HORIZONTAL),
	m_fWidgetSpacing(20),
	m_fLineSpacing(20),
	m_fHorizontalMargin(30),
	m_fTopMargin(40),
	m_fBottomMargin(20),
	lastItem(nullptr),
	bneedcheckindex(true),
	m_posMaxMove(Vec2(5, 5)),
	m_bChangeZOrder(false)
{

}

lly::TableView::~TableView()
{

}

TableView* lly::TableView::create()
{
	TableView* widget = new (std::nothrow) TableView();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

void lly::TableView::setTableLayoutType( ELayoutType Ltype )
{
	if (m_eLayoutType == Ltype) return;
	m_eLayoutType = Ltype;
	_refreshViewDirty = true;
}

void lly::TableView::setTableViewGravity( ETableViewGravity TVG )
{
	if (m_eTVG == TVG) return;
	m_eTVG = TVG;
	_refreshViewDirty = true;
}

void lly::TableView::setGravity( Gravity gravity )
{
	if (_gravity == gravity) return;
	_gravity = gravity;

	switch (gravity)
	{
	case llyO_for_lly_tableview::ListView::Gravity::LEFT:
		setTableViewGravity(ETableViewGravity::LEFT_TOP);
		break;
	case llyO_for_lly_tableview::ListView::Gravity::RIGHT:
		setTableViewGravity(ETableViewGravity::RIGHT_TOP);
		break;
	case llyO_for_lly_tableview::ListView::Gravity::CENTER_HORIZONTAL:
		setTableViewGravity(ETableViewGravity::LEFT_TOP);
		break;
	case llyO_for_lly_tableview::ListView::Gravity::TOP:
		setTableViewGravity(ETableViewGravity::LEFT_TOP);
		break;
	case llyO_for_lly_tableview::ListView::Gravity::BOTTOM:
		setTableViewGravity(ETableViewGravity::LEFT_BOTTOM);
		break;
	case llyO_for_lly_tableview::ListView::Gravity::CENTER_VERTICAL:
		setTableViewGravity(ETableViewGravity::LEFT_TOP);
		break;
	default:
		break;
	}
}

void lly::TableView::setArrangementDirection( EArrangementDirection dir )
{
	if (m_eArrangeDir == dir) return;
	m_eArrangeDir = dir;	

	switch (m_eArrangeDir) //滚动方向设置
	{
	case lly::TableView::EArrangementDirection::HORIZONTAL:
		ScrollView::setDirection(ScrollView::Direction::VERTICAL);
		break;
	case lly::TableView::EArrangementDirection::VERTICAL:
		ScrollView::setDirection(ScrollView::Direction::HORIZONTAL);
		break;
	default:
		break;
	}

	_refreshViewDirty = true; //刷新开启
}

void lly::TableView::setDirection( Direction dir )
{
	switch (dir)
	{
	case ScrollView::Direction::VERTICAL:
		setArrangementDirection(EArrangementDirection::HORIZONTAL);
		break;
	case ScrollView::Direction::HORIZONTAL:
		setArrangementDirection(EArrangementDirection::VERTICAL);
		break;
	default:
		break;
	}
}

void lly::TableView::setWidgetSpacing(float fSpacing)
{
	if (m_fWidgetSpacing == fSpacing) return;
	m_fWidgetSpacing = fSpacing;
	_refreshViewDirty = true;
}

void lly::TableView::setLineSpacing(float fLineSpacing)
{
	if (m_fLineSpacing == fLineSpacing) return;
	m_fLineSpacing = fLineSpacing;
	_refreshViewDirty = true;
}

void lly::TableView::setHorizontalMargin(float fMargin)
{
	if (m_fHorizontalMargin == fMargin) return;
	m_fHorizontalMargin = fMargin;
	_refreshViewDirty = true;
}

void lly::TableView::setTopMargin( float fMargin )
{
	if (m_fTopMargin == fMargin) return;
	m_fTopMargin = fMargin;
	_refreshViewDirty = true;
}

void lly::TableView::setBottomMargin( float fMargin )
{
	if (m_fBottomMargin == fMargin) return;
	m_fBottomMargin = fMargin;
	_refreshViewDirty = true;
}

bool lly::TableView::isNewLineSymbol( cocos2d::Node* item )
{
	return item->getName()[0] == '\n';
}

Node* lly::TableView::getLastItem()
{
	return static_cast<cocos2d::Node*>(_items.back());
}

void lly::TableView::refreshView()
{
	m_arHeadNode.clear(); //清空头行记录，在refresh中重新加载
	bneedcheckindex = false; //为了提高效率，不检测
	lastItem = nullptr; //从第一个控件开始刷新，第一个控件的上一个控件为空
	llyO_for_lly_tableview::ListView::refreshView();
	bneedcheckindex = true; //恢复

	m_bChangeZOrder = true;
}

//=====================================
bool lly::TableView::init()
{
	if (ScrollView::init())
	{
		setLayoutType(Type::ABSOLUTE);
		return true;
	}
	else return false;	
}

void lly::TableView::updateInnerContainerSize()
{
	if (!getItem(0) || !getLastItem()) return;

	switch (m_eArrangeDir) //水平排列
	{
	case EArrangementDirection::HORIZONTAL: 
		{
			//第一个控件到最后一个控件的垂直长度加上两边留白的总距离
			float f = abs(getItem(0)->getPositionY() - getLastItem()->getPositionY()) + m_fTopMargin + m_fBottomMargin;
			if (f > _contentSize.height && f != _innerContainer->getContentSize().height )
			{
				setInnerContainerSize(CCSize( _contentSize.width, f ));

				if (m_eTVG == ETableViewGravity::LEFT_TOP || m_eTVG == ETableViewGravity::RIGHT_TOP)
				{
					float fDistance = 0.0f;
					bool first = true;
					for (auto item : _items)
					{
						if (!item) break;

						if (first)
						{
							float fORI = item->getPositionY();
							item->setPositionY(_innerContainer->getContentSize().height - m_fTopMargin);
							fDistance = item->getPositionY() - fORI;
							first  = false;
						}
						else
						{
							item->setPositionY(item->getPositionY() + fDistance);
						}
					}
				}				
			}
		}
		break;
	case EArrangementDirection::VERTICAL:
		{
			//第一个控件到最后一个控件的水平长度加上两边留白的总距离
			float f = abs(getItem(0)->getPositionX() - getLastItem()->getPositionX()) + m_fTopMargin + m_fBottomMargin;

			if (f >= _contentSize.width && f != _innerContainer->getContentSize().width )
			{
				setInnerContainerSize(CCSize( f, _contentSize.height ));

				if (m_eTVG == ETableViewGravity::RIGHT_TOP || m_eTVG == ETableViewGravity::RIGHT_BOTTOM)
				{
					float fDistance = 0.0f;
					bool first = true;
					for (auto item : _items)
					{
						if (!item) break;

						if (first)
						{
							float fORI = item->getPositionX();
							item->setPositionX(_innerContainer->getContentSize().width - m_fTopMargin);
							fDistance = item->getPositionX() - fORI;
							first  = false;
						}
						else
						{
							item->setPositionX(item->getPositionX() + fDistance);
						}
					}
				}			
			}			
		}	
		break;
	default:
		break;
	}
}

void lly::TableView::remedyLayoutParameter( cocos2d::Node* item )
{
	if (!item) return;

	if (bneedcheckindex)
	{
		int nIndex = getIndex(item);
		lastItem = getItem(nIndex - 1); //上一个控件
	}

	switch (m_eArrangeDir) //排版方向
	{
	case EArrangementDirection::HORIZONTAL:

		switch (m_eTVG) //根据停靠方向不同
		{
		case ETableViewGravity::LEFT_TOP: //左上
			if (lastItem == nullptr)
			{
				item->setPosition(Vec2(m_fHorizontalMargin, _innerContainer->getContentSize().height - m_fTopMargin));
				m_arHeadNode.pushBack(item); //行头
			}
			else
			{
				//把位置放到上一个控件往右增加一定距离的地方
				if (m_eLayoutType == ELayoutType::ABSOLUTE)
				{
					item->setPositionX(lastItem->getPositionX() + m_fWidgetSpacing);
				}
				else
				{
					float fAverage = (lastItem->getContentSize().width + item->getContentSize().width) / 2;
					item->setPositionX(lastItem->getPositionX() + fAverage);
				}

				if (item->getPositionX() + m_fHorizontalMargin > _innerContainer->getContentSize().width || 
					isNewLineSymbol(lastItem)) //如果超出了一行的宽度或遇到换行符，就换行
				{
					item->setPosition(Vec2(m_fHorizontalMargin, lastItem->getPositionY() - m_fLineSpacing));
					m_arHeadNode.pushBack(item); //行头
				}
				else
				{
					item->setPositionY(lastItem->getPositionY());
				}
			}
			break;
		case ETableViewGravity::RIGHT_TOP:
			if (lastItem == nullptr)
			{
				item->setPosition(Vec2(_innerContainer->getContentSize().width - m_fHorizontalMargin, _innerContainer->getContentSize().height - m_fTopMargin));
				m_arHeadNode.pushBack(item); //行头
			}
			else
			{
				//把位置放到上一个控件往左边增加一定距离的地方
				if (m_eLayoutType == ELayoutType::ABSOLUTE)
				{
					item->setPositionX(lastItem->getPositionX() - m_fWidgetSpacing);
				}
				else
				{
					float fAverage = (lastItem->getContentSize().width + item->getContentSize().width) / 2;
					item->setPositionX(lastItem->getPositionX() - fAverage);
				}

				if (item->getPositionX() - m_fHorizontalMargin < 0 || isNewLineSymbol(lastItem)) //如果超出了一行的宽度或遇到换行符，就换行
				{
					item->setPosition(Vec2(_innerContainer->getContentSize().width - m_fHorizontalMargin, lastItem->getPositionY() - m_fLineSpacing));
					m_arHeadNode.pushBack(item); //行头
				}
				else
				{
					item->setPositionY(lastItem->getPositionY());
				}
			}
			break;
		case ETableViewGravity::LEFT_BOTTOM:
			if (lastItem == nullptr)
			{
				item->setPosition(Vec2(m_fHorizontalMargin, m_fTopMargin));
				m_arHeadNode.pushBack(item); //行头
			}
			else
			{
				//把位置放到上一个控件往右增加一定距离的地方
				if (m_eLayoutType == ELayoutType::ABSOLUTE)
				{
					item->setPositionX(lastItem->getPositionX() + m_fWidgetSpacing);
				}
				else
				{
					float fAverage = (lastItem->getContentSize().width + item->getContentSize().width) / 2;
					item->setPositionX(lastItem->getPositionX() + fAverage);
				}

				if (item->getPositionX() + m_fHorizontalMargin > _innerContainer->getContentSize().width || 
					isNewLineSymbol(lastItem)) //如果超出了一行的宽度或遇到换行符，就换行
				{
					item->setPosition(Vec2(m_fHorizontalMargin, lastItem->getPositionY() + m_fLineSpacing));
					m_arHeadNode.pushBack(item); //行头
				}
				else
				{
					item->setPositionY(lastItem->getPositionY());
				}
			}
			break;
		case ETableViewGravity::RIGHT_BOTTOM:
			if (lastItem == nullptr)
			{
				item->setPosition(Vec2(_innerContainer->getContentSize().width - m_fHorizontalMargin, m_fTopMargin));
				m_arHeadNode.pushBack(item); //行头
			}
			else
			{
				//把位置放到上一个控件往左边增加一定距离的地方
				if (m_eLayoutType == ELayoutType::ABSOLUTE)
				{
					item->setPositionX(lastItem->getPositionX() - m_fWidgetSpacing);
				}
				else
				{
					float fAverage = (lastItem->getContentSize().width + item->getContentSize().width) / 2;
					item->setPositionX(lastItem->getPositionX() - fAverage);
				}

				if (item->getPositionX() - m_fHorizontalMargin < 0 || isNewLineSymbol(lastItem)) //如果超出了一行的宽度或遇到换行符，就换行
				{
					item->setPosition(Vec2(_innerContainer->getContentSize().width - m_fHorizontalMargin, lastItem->getPositionY() + m_fLineSpacing));
					m_arHeadNode.pushBack(item); //行头
				}
				else
				{
					item->setPositionY(lastItem->getPositionY());
				}
			}
			break;
		default:
			break;
		}
		break;
	case EArrangementDirection::VERTICAL:

		switch (m_eTVG) //根据停靠方向不同
		{
		case ETableViewGravity::LEFT_TOP: //左上
			if (lastItem == nullptr)
			{
				item->setPosition(Vec2(m_fTopMargin, _innerContainer->getContentSize().height - m_fHorizontalMargin));
				m_arHeadNode.pushBack(item); //行头
			}
			else
			{
				//把位置放到上一个控件往下增加一定距离的地方
				if (m_eLayoutType == ELayoutType::ABSOLUTE)
				{
					item->setPositionY(lastItem->getPositionY() - m_fWidgetSpacing);
				}
				else
				{
					float fAverage = (lastItem->getContentSize().height + item->getContentSize().height) / 2;
					item->setPositionY(lastItem->getPositionY() - fAverage);
				}

				if (item->getPositionY() - m_fHorizontalMargin < 0 || isNewLineSymbol(lastItem)) //如果超出了一行的宽度或遇到换行符，就换行
				{
					item->setPosition(Vec2(lastItem->getPositionX() + m_fLineSpacing, _innerContainer->getContentSize().height - m_fHorizontalMargin));
					m_arHeadNode.pushBack(item); //行头
				}
				else
				{
					item->setPositionX(lastItem->getPositionX());
				}
			}
			break;
		case ETableViewGravity::RIGHT_TOP:
			if (lastItem == nullptr)
			{
				item->setPosition(Vec2(_innerContainer->getContentSize().width - m_fTopMargin, _innerContainer->getContentSize().height - m_fHorizontalMargin));
				m_arHeadNode.pushBack(item); //行头
			}
			else
			{
				//把位置放到上一个控件往下边增加一定距离的地方
				if (m_eLayoutType == ELayoutType::ABSOLUTE)
				{
					item->setPositionY(lastItem->getPositionY() - m_fWidgetSpacing);
				}
				else
				{
					float fAverage = (lastItem->getContentSize().height + item->getContentSize().height) / 2;
					item->setPositionY(lastItem->getPositionY() - fAverage);
				}

				if (item->getPositionY() - m_fHorizontalMargin < 0 || isNewLineSymbol(lastItem)) //如果超出了一行的宽度或遇到换行符，就换行
				{
					item->setPosition(Vec2(lastItem->getPositionX() - m_fLineSpacing, _innerContainer->getContentSize().height - m_fHorizontalMargin));
					m_arHeadNode.pushBack(item); //行头
				}
				else
				{
					item->setPositionX(lastItem->getPositionX());
				}
			}
			break;
		case ETableViewGravity::LEFT_BOTTOM:
			if (lastItem == nullptr)
			{
				item->setPosition(Vec2(m_fTopMargin, m_fHorizontalMargin));
				m_arHeadNode.pushBack(item); //行头
			}
			else
			{
				//把位置放到上一个控件往上增加一定距离的地方
				if (m_eLayoutType == ELayoutType::ABSOLUTE)
				{
					item->setPositionY(lastItem->getPositionY() + m_fWidgetSpacing);
				}
				else
				{
					float fAverage = (lastItem->getContentSize().height + item->getContentSize().height) / 2;
					item->setPositionY(lastItem->getPositionY() + fAverage);
				}

				if (item->getPositionY() + m_fHorizontalMargin > _innerContainer->getContentSize().height || 
					isNewLineSymbol(lastItem)) //如果超出了一行的宽度或遇到换行符，就换行
				{
					item->setPosition(Vec2(lastItem->getPositionX() + m_fLineSpacing, m_fHorizontalMargin));
					m_arHeadNode.pushBack(item); //行头
				}
				else
				{
					item->setPositionX(lastItem->getPositionX());
				}
			}
			break;
		case ETableViewGravity::RIGHT_BOTTOM:
			if (lastItem == nullptr)
			{
				item->setPosition(Vec2(_innerContainer->getContentSize().width - m_fTopMargin, m_fHorizontalMargin));
				m_arHeadNode.pushBack(item); //行头
			}
			else
			{
				//把位置放到上一个控件往shang边增加一定距离的地方
				if (m_eLayoutType == ELayoutType::ABSOLUTE)
				{
					item->setPositionY(lastItem->getPositionY() + m_fWidgetSpacing);
				}
				else
				{
					float fAverage = (lastItem->getContentSize().height + item->getContentSize().height) / 2;
					item->setPositionY(lastItem->getPositionY() + fAverage);
				}

				if (item->getPositionY() + m_fHorizontalMargin > _innerContainer->getContentSize().height || 
					isNewLineSymbol(lastItem)) //如果超出了一行的宽度或遇到换行符，就换行
				{
					item->setPosition(Vec2(lastItem->getPositionX() - m_fLineSpacing, m_fHorizontalMargin));
					m_arHeadNode.pushBack(item); //行头
				}
				else
				{
					item->setPositionX(lastItem->getPositionX());
				}
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	//把自己作为下个控件的上一个控件
	lastItem = item;
}

cocos2d::ui::Widget* lly::TableView::createCloneInstance()
{
	return  lly::TableView::create();
}

void lly::TableView::copySpecialProperties( cocos2d::ui::Widget* widget )
{
	lly::TableView* tableview = dynamic_cast<lly::TableView*>(widget);
	if (tableview)
	{
		llyO_for_lly_tableview::ListView::copySpecialProperties(widget);
		setTableLayoutType(tableview->m_eLayoutType);
		setTableViewGravity(tableview->m_eTVG);
		setArrangementDirection(tableview->m_eArrangeDir);
		setWidgetSpacing(tableview->m_fWidgetSpacing);
		setLineSpacing(tableview->m_fLineSpacing);
		setHorizontalMargin(tableview->m_fHorizontalMargin);
		setTopMargin(tableview->m_fTopMargin);
		setBottomMargin(tableview->m_fBottomMargin);
	}
}

void lly::TableView::interceptTouchEvent( cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch )
{
	ScrollView::interceptTouchEvent(event, sender, touch);

	switch (event)
	{
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		{
			//若移动的距离大于一个量，则视为移动，而不响应touch结束
			auto movePoint = touch->getLocation(); 
			if (_bMoved == false && 
				(abs(movePoint.x - m_posBegintouch.x) > m_posMaxMove.x || 
				abs(movePoint.y - m_posBegintouch.y) > m_posMaxMove.y))
				_bMoved = true;
		}		
		break;
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		//获得目标控件
		m_targetItem = _items.at(_curSelectedIndex);

		_bMoved = false;
		m_posBegintouch = touch->getLocation();

		//没有break

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

void lly::TableView::visit( cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, bool parentTransformUpdated )
{
	llyO_for_lly_tableview::ListView::visit(renderer, parentTransform, parentTransformUpdated);

	//改变目标控件的z轴在最上
	if (m_targetItem == nullptr || m_bChangeZOrder == false) return;
	m_bChangeZOrder = false;
	m_targetItem->setLocalZOrder(INT_MAX);
}



