#include "llyListView.h"
#include "ui/UIHelper.h"
#include "extensions/GUI/CCControlExtension/CCScale9Sprite.h"

USING_NS_CC;
using namespace ui;

namespace llyO_for_lly_listview {
    
IMPLEMENT_CLASS_GUI_INFO(ListView)

ListView::ListView():
_model(nullptr),
_gravity(Gravity::CENTER_VERTICAL),
_itemsMargin(0.0f),
_listViewEventListener(nullptr),
_listViewEventSelector(nullptr),
_curSelectedIndex(0),
_refreshViewDirty(true),
_eventCallback(nullptr)
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
    ListView* widget = new ListView();
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

void ListView::remedyLayoutParameter(Widget *item)
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
    remedyLayoutParameter(newItem);
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

    remedyLayoutParameter(newItem);
    
    _refreshViewDirty = true;
}


void ListView::pushBackCustomItem(Widget* item)
{
    remedyLayoutParameter(item);
    addChild(item);
    _refreshViewDirty = true;
}
    
void ListView::addChild(cocos2d::Node *child, int zOrder, int tag)
{
    ScrollView::addChild(child, zOrder, tag);

    Widget* widget = dynamic_cast<Widget*>(child);
    if (widget)
    {
        _items.pushBack(widget);
    }
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
    
    Widget* widget = dynamic_cast<Widget*>(child);
    if (widget)
    {
        _items.pushBack(widget);
    }
}
    
void ListView::removeChild(cocos2d::Node *child, bool cleaup)
{
    Widget* widget = dynamic_cast<Widget*>(child);
    if (widget) {
        _items.eraseObject(widget);
    }
   
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

void ListView::insertCustomItem(Widget* item, ssize_t index)
{
    _items.insert(index, item);
    ScrollView::addChild(item);

    remedyLayoutParameter(item);
    _refreshViewDirty = true;
}

void ListView::removeItem(ssize_t index)
{
    Widget* item = getItem(index);
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

Widget* ListView::getItem(ssize_t index)const
{
    if (index < 0 || index >= _items.size())
    {
        return nullptr;
    }
    return _items.at(index);
}

Vector<Widget*>& ListView::getItems()
{
    return _items;
}

ssize_t ListView::getIndex(Widget *item) const
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
        Widget* item = _items.at(i);
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
    if (event != TouchEventType::MOVED)
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
    return "ListView";
}

Widget* ListView::createCloneInstance()
{
    return ListView::create();
}

void ListView::copyClonedWidgetChildren(Widget* model)
{
    auto& arrayItems = static_cast<ListView*>(model)->getItems();
    for (auto& item : arrayItems)
    {
        pushBackCustomItem(item->clone());
    }
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

} //llyO



//================================
using namespace lly;

IMPLEMENT_CLASS_GUI_INFO(lly::ListView)

lly::ListView::ListView() : llyO_for_lly_listview::ListView(), 
	//自添加 ============
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

ListView* lly::ListView::create()
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
	llyO_for_lly_listview::ListView::pushBackDefaultItem();
	changeWidgetEvent(LISTVIEW_ADD_ITEM); //增删改控件时候的回调
}

void lly::ListView::insertDefaultItem(ssize_t index)
{
	if (!_model) return;

	//===============================
	if (m_bAnimation) //自添加
	{
		//处于更新状态时候，不得有其他的操作
		if (_refreshViewDirty) return;

		//判断要插入的点是否存在
		if (index > (int)_items.size())
		{
			CCLOG("count %d small than %d", _items.size(), index);
			return;
		}

		//如果后面还有控件的话，则此控件为目标控件
		auto item2 = getItem(index);
		if (item2)
		{
			m_itemMove = item2;

			//计算删除后下面控件或者底边与上面控件的距离：注意此处有个负号
			m_fDistance = -getDistance(_model);
		}
	}
	//===========================

	llyO_for_lly_listview::ListView::insertDefaultItem(index);

	changeWidgetEvent(LISTVIEW_ADD_ITEM); //增删改控件时候的回调
}

void lly::ListView::pushBackCustomItem(Widget* item)
{
	llyO_for_lly_listview::ListView::pushBackCustomItem(item);
	changeWidgetEvent(LISTVIEW_ADD_ITEM); //增删改控件时候的回调
}

void lly::ListView::insertCustomItem(Widget* item, ssize_t index)
{
	//===============================
	if (m_bAnimation) //自添加
	{
		//处于更新状态时候，不得有其他的操作
		if (_refreshViewDirty) return;

		//判断要插入的点是否存在
		if (index > (int)_items.size())
		{
			CCLOG("count %d small than %d", _items.size(), index);
			return;
		}

		//如果后面还有控件的话，则此控件为目标控件
		auto item2 = getItem(index);
		if (item2)
		{
			m_itemMove = item2;

			//计算删除后下面控件或者底边与上面控件的距离：注意此处有个负号
			m_fDistance = -getDistance(_model);
		}
	}
	//===========================

	llyO_for_lly_listview::ListView::insertCustomItem(item, index);

	changeWidgetEvent(LISTVIEW_ADD_ITEM); //增删改控件时候的回调
}

void ListView::removeItem(ssize_t index)
{
	Widget* item = getItem(index);
	if (!item) return;

	//===========================
	if (m_targetItem == item) m_targetItem = nullptr; //取消目标控件

	if (m_bAnimation)
	{
		//处于更新状态时候，不得有其他的操作
		if (_refreshViewDirty == true) return;

		//如果后面还有控件的话，则此控件为目标控件
		auto item2 = getItem(index + 1);
		if (item2)
		{
			m_itemMove = item2;

			//计算删除后下面控件或者底边与上面控件的距离
			m_fDistance = getDistance(item);
		}		
	}
	//===========================

	llyO_for_lly_listview::ListView::removeItem(index);

	changeWidgetEvent(LISTVIEW_REMOVE_ITEM); //增删改控件时候的回调
}

void ListView::removeLastItem()
{
	if (m_targetItem == getLastItem()) m_targetItem = nullptr; //取消目标控件
	llyO_for_lly_listview::ListView::removeLastItem();
	changeWidgetEvent(LISTVIEW_REMOVE_ITEM); //增删改控件时候的回调
}

void ListView::removeAllItems()
{
	if (m_targetItem) m_targetItem = nullptr; //取消目标控件
	llyO_for_lly_listview::ListView::removeAllItems();
	changeWidgetEvent(LISTVIEW_REMOVE_ITEM); //增删改控件时候的回调
}

void lly::ListView::doLayout()
{
	lly::ScrollView::doLayout();
	if (_refreshViewDirty)
	{
		refreshView();
		if (m_bAnimation) //修改===============
		{			
			//当距离不是0的时候，则让它变化到0，是0的话就让更新结束
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

				//更新结束时在visit里面更改目标控件的z轴到最上层
				m_bChangeZOrder = true; 
			}  
		}
		else
		{
			_refreshViewDirty = false;
		}	      
	}
}

//=========================================自添加
void lly::ListView::removeItem( Widget* item )
{
	if (!item) return;

	if (m_targetItem == item) m_targetItem = nullptr; //取消目标控件

	if (m_bAnimation)
	{
		//处于更新状态时候，不得有其他的操作
		if (_refreshViewDirty == true) return;

		//如果后面还有控件的话，则此控件为目标控件
		auto item2 = getItem(getIndex(item) + 1);
		if (item2)
		{
			m_itemMove = item2;

			//计算删除后下面控件或者底边与上面控件的距离
			m_fDistance = getDistance(item);
		}		
	}
	//===========================

	removeChild(item, true);
	_refreshViewDirty = true;

	changeWidgetEvent(LISTVIEW_REMOVE_ITEM); //增删改控件时候的回调
}

void lly::ListView::moveItem( cocos2d::ui::Widget* item, WidgetMoveDirection dir, WidgetMoveType t /*= MOVE_BY*/, int num /*= 1*/ )
{
	if (!item || !_items.contains(item))
	{
		CCLOG("@no item can move");
		return;
	}

	int nAimIndex;
	if (t == MOVE_BY)
	{
		if (dir == MOVE_FORWARD)
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

	changeWidgetEvent(LISTVIEW_MOVE_ITEM); //增删改控件时候的回调
}

//===============================保护
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
			//加上动画时变化的距离
			totalHeight += m_fDistance;

			//加上头空白，尾空白
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
			//加上动画时变化的距离
			totalWidth += m_fDistance;

			//加上头空白，尾空白
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
					if (m_itemMove == item) //如果是目标控件则需要加上距离
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
					if (m_itemMove == item) //如果是目标控件则需要加上距离
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
					if (m_itemMove == item) //如果是目标控件则需要加上距离
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
					if (m_itemMove == item) //如果是目标控件则需要加上距离
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
					if (m_itemMove == item) //如果是目标控件则需要加上变化值
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
					if (m_itemMove == item) //如果是目标控件则需要加上变化值
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
					if (m_itemMove == item) //如果是目标控件则需要加上变化值
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
					if (m_itemMove == item) //如果是目标控件则需要加上变化值
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
		llyO_for_lly_listview::ListView::copySpecialProperties(listViewEx);
		
		m_bAnimation = listViewEx->m_bAnimation; //是否开启动画（默认不开启）
		m_fWidgetMoveSpeed = listViewEx->m_fWidgetMoveSpeed; //控件动画移动的速度（默认每帧1像素）
		m_fMarginToTop = listViewEx->m_fMarginToTop; //第一个控件到容器顶部的距离
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

//=============================================自添加
void lly::ListView::changeWidgetEvent( ListViewchangeItem type )
{
	if (addChangeItemEventFunc)
		addChangeItemEventFunc(this, type);
}

void ListView::visit( Renderer *renderer, const Mat4& parentTransform, bool parentTransformUpdated )
{
	llyO_for_lly_listview::ListView::visit(renderer, parentTransform, parentTransformUpdated);

	//改变目标控件的z轴在最上
	if (m_targetItem == nullptr || m_bChangeZOrder == false) return;
	m_bChangeZOrder = false;
	m_targetItem->setLocalZOrder(INT_MAX);
}



