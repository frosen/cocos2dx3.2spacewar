/*
名称：llyListView.h
内容：新的ListView
			llyO中的listview和原版一致，其中有些函数变成虚函数为了继承，包括
			pushBackDefaultItem to removeAllItems
			updateInnerContainerSize
			remedyLayoutParameter
			添加动画
			添加对目标控件的操作
			添加移动控件的函数
			添加操作控件后的回调

制作：卢乐颜
日期：2014.9.1
*/

#ifndef _LLY_LIST_VIEW_H__  
#define _LLY_LIST_VIEW_H__

#include "llyScrollView.h"  //修改
#include "cocos2d.h"  //修改

namespace llyO_for_lly_listview{

typedef enum
{
    LISTVIEW_ONSELECTEDITEM_START,
    LISTVIEW_ONSELECTEDITEM_END
}ListViewEventType;

typedef void (cocos2d::Ref::*SEL_ListViewEvent)(cocos2d::Ref*,ListViewEventType);
#define listvieweventselector(_SELECTOR) (SEL_ListViewEvent)(&_SELECTOR)

class ListView : public lly::ScrollView
{
 
    DECLARE_CLASS_GUI_INFO
    
public:
    enum class Gravity
    {
        LEFT,
        RIGHT,
        CENTER_HORIZONTAL,
        TOP,
        BOTTOM,
        CENTER_VERTICAL
    };
    
    enum class EventType
    {
        ON_SELECTED_ITEM_START,
        ON_SELECTED_ITEM_END
    };
    
    typedef std::function<void(cocos2d::Ref*, EventType)> ccListViewCallback;
    
    /**
     * Default constructor
     */
    ListView();
    
    /**
     * Default destructor
     */
    virtual ~ListView();
    
    /**
     * Allocates and initializes.
     */
    static ListView* create();
    
    /**
     * Sets a item model for listview
     *
     * A model will be cloned for adding default item.
     *
     * @param model  item model for listview
     */
    void setItemModel(cocos2d::ui::Widget* model);
    
    /**
     * Push back a default item(create by a cloned model) into listview.
     */
    virtual void pushBackDefaultItem();
    
    /**
     * Insert a default item(create by a cloned model) into listview.
     */
    virtual void insertDefaultItem(ssize_t index);
    
    /**
     * Push back custom item into listview.
     */
    virtual void pushBackCustomItem(cocos2d::ui::Widget* item);
    
    /**
     * Insert custom item into listview.
     */
    virtual void insertCustomItem(cocos2d::ui::Widget* item, ssize_t index);
    
    /**
     *  Removes the last item of listview.
     */
    virtual void removeLastItem();
    
    /**
     * Removes a item whose index is same as the parameter.
     *
     * @param index of item.
     */
    virtual void removeItem(ssize_t index);
    
    virtual void removeAllItems();
    
    /**
     * Returns a item whose index is same as the parameter.
     *
     * @param index of item.
     *
     * @return the item widget.
     */
    cocos2d::ui::Widget* getItem(ssize_t index)const;
    
    /**
     * Returns the item container.
     */
    cocos2d::Vector<cocos2d::ui::Widget*>& getItems();
    
    /**
     * Returns the index of item.
     *
     * @param item  the item which need to be checked.
     *
     * @return the index of item.
     */
    ssize_t getIndex(cocos2d::ui::Widget* item) const;
    
    /**
     * Changes the gravity of listview.
     * @see ListViewGravity
     */
    void setGravity(Gravity gravity);
    
    /**
     * Changes the margin between each item.
     *
     * @param margin
     */
    void setItemsMargin(float margin);
    
    float getItemsMargin()const;
    
    virtual void doLayout() override;
    
    virtual void addChild(cocos2d::Node* child)override;
    virtual void addChild(cocos2d::Node * child, int localZOrder)override;
    virtual void addChild(cocos2d::Node* child, int zOrder, int tag) override;
    virtual void addChild(cocos2d::Node* child, int zOrder, const std::string &name) override;
    virtual void removeAllChildren() override;
    virtual void removeAllChildrenWithCleanup(bool cleanup) override;
	virtual void removeChild(cocos2d::Node* child, bool cleaup = true) override;
    
    ssize_t getCurSelectedIndex() const;
    
    CC_DEPRECATED_ATTRIBUTE void addEventListenerListView(cocos2d::Ref* target, SEL_ListViewEvent selector);
    void addEventListener(const ccListViewCallback& callback);
    using lly::ScrollView::addEventListener;

    /**
     * Changes scroll direction of scrollview.
     *
     * @see Direction Direction::VERTICAL means vertical scroll, Direction::HORIZONTAL means horizontal scroll
     *
     * @param dir, set the list view's scroll direction
     */
    virtual void setDirection(Direction dir) override;
    
    virtual std::string getDescription() const override;
    
    void requestRefreshView();
    virtual void refreshView();

CC_CONSTRUCTOR_ACCESS:
    virtual bool init() override;
    
protected:    
    virtual void updateInnerContainerSize(); //修改
    virtual void remedyLayoutParameter(cocos2d::ui::Widget* item); //修改
    virtual void onSizeChanged() override;
    virtual cocos2d::ui::Widget* createCloneInstance() override;
    virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;
    virtual void copyClonedWidgetChildren(cocos2d::ui::Widget* model) override;
    void selectedItemEvent(TouchEventType event);
    virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch) override;
protected:
    cocos2d::ui::Widget* _model;
    
    cocos2d::Vector<cocos2d::ui::Widget*> _items;
    
    Gravity _gravity;
    
    float _itemsMargin;
    
    ssize_t _curSelectedIndex;
    bool _refreshViewDirty;
    
    cocos2d::Ref* _listViewEventListener;
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (push)
#pragma warning (disable: 4996)
#endif
    SEL_ListViewEvent    _listViewEventSelector;
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic warning "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (pop)
#endif
    ccListViewCallback _eventCallback;
};

} //llyO_for_lly_listview


//新listview

namespace lly{

class ListView : llyO_for_lly_listview::ListView
{
	DECLARE_CLASS_GUI_INFO

public:
	typedef enum //控件的改变方式
	{
		LISTVIEW_ADD_ITEM, //增加
		LISTVIEW_REMOVE_ITEM, //删除
		LISTVIEW_MOVE_ITEM, //移动
	}ListViewchangeItem;

	typedef enum //移动控件的方式
	{
		MOVE_BY, //移动某个距离
		MOVE_TO, //移动到哪里
	}WidgetMoveType;

	typedef enum //移动控件的方式
	{
		MOVE_FORWARD, //向前移动
		MOVE_BACKWARD, //向后移动
	}WidgetMoveDirection;

	typedef std::function<void(Ref*, ListViewchangeItem t)> changeItemCallback;

	ListView();
	virtual ~ListView();

	static ListView* create();

	virtual void pushBackDefaultItem(); //自添加virtual
    virtual void insertDefaultItem(ssize_t index); //自添加virtual
    virtual void pushBackCustomItem(Widget* item); //自添加virtual
    virtual void insertCustomItem(Widget* item, ssize_t index); //自添加virtual

	virtual void removeItem(ssize_t index); //自添加virtual
	virtual void removeLastItem(); //自添加virtual
    virtual void removeAllItems(); //自添加virtual

	virtual void doLayout() override;

	virtual std::string getDescription() const override { return "lly ListView"; }

	//自添加public=================================================

	//直接通过对象删除控件，优化只能通过索引号删除的方式，并且也带动画
	void removeItem(cocos2d::ui::Widget* item);

	//方便与pushbackdefault结合使用，读取最后一个控件
	cocos2d::ui::Widget* getLastItem() { return static_cast<cocos2d::ui::Widget*>(_items.back()); }

	//是否开启动画（默认关闭）=========================================
	void setAnimationEnabled(bool b) { m_bAnimation = b; }
	bool isAnimationEnabled() { return m_bAnimation; }

	//设置动画的速度（默认每帧1个像素）
	void setWidgetSpeedWhenMove(float len) { m_fWidgetMoveSpeed = len; }

	//改变控件时的回调
	void addChangeItemEventListener(changeItemCallback call) { addChangeItemEventFunc = call; }

	//设置控件到顶部的留白
	void setMarginToTop(float fMargin) { m_fMarginToTop = fMargin; }

	//移动控件位置，参数分别为要移动的控件，移动方式（是移动多少个，还是移动到某位置）移动单位（位置），如果超过最大限度则移动到最大位置
	void moveItem(cocos2d::ui::Widget* item, WidgetMoveDirection dir, WidgetMoveType t = MOVE_BY, int num = 1);

	//获取当前是否在变更
	bool isRefreshing() { return _refreshViewDirty; }	

	//设置最大移动距离，超过距离不响应触控（touch end）
	void setListMaxMovePoint(const cocos2d::Vec2 &pos) { m_posMaxMove = pos; }
	bool isMoveList() { return _bMoved; } //移动了列表，用于触摸回调中禁止动作
	//========================================

protected:
	virtual void updateInnerContainerSize(); //自添加virtual
	virtual void remedyLayoutParameter(cocos2d::ui::Widget* item); //自添加virtual

	virtual cocos2d::ui::Widget* createCloneInstance() override;
	virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;
	virtual void copyClonedWidgetChildren(cocos2d::ui::Widget* model) override;
	virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch) override; //响应点中后移动

	//动画时获取初始距离=======================
	float getDistance(cocos2d::ui::Widget* item) 
	{ 
		return _itemsMargin + (
			_direction == llyO::ScrollView::Direction::VERTICAL ? item->getContentSize().height : 
			_direction == llyO::ScrollView::Direction::VERTICAL ? item->getContentSize().width : 0); 
	}

	//增删移动子控件时候调用 默认空
	void changeWidgetEvent(ListViewchangeItem type);

	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, bool parentTransformUpdated);

	//================================

protected:
	//==========================
	bool m_bAnimation; //是否开启动画（默认不开启）
	float m_fWidgetMoveSpeed; //控件动画移动的速度（默认每帧1像素）
	float m_fMarginToTop; //第一个控件到容器顶部的距离
	changeItemCallback addChangeItemEventFunc;
	cocos2d::Vec2 m_posMaxMove; //最大移动距离，大于这个距离不响应触控

	//temp
	cocos2d::ui::Widget* m_itemMove; //动画中需要移动的那个控件
	float m_fDistance; //动画中控件移动的距离
	bool m_bChangeZOrder; //更新结束时在visit里面更改目标控件的z轴到最上层
	bool _bMoved; //是否移动了列表
	cocos2d::Vec2 m_posBegintouch; 
};

} //lly

#endif /* defined(__ListView__) */


