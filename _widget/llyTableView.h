/*
名称：llyTableView.h
内容：表格容器，重载listview，但是排列方式和其不同
		llyO中的listview和原版一致，其中有些函数变成虚函数为了继承，包括
			setGravity
			updateInnerContainerSize
			remedyLayoutParameter
		llyO中的listview可以放置非widget控件，便于作为输入框使用，因此：
			所有的widget为参数的非重载函数参数改为node
			不可复制其子控件

		

制作：卢乐颜
日期：2014.5.1
*/

#ifndef _LLY_TABLE_VIEW_H_
#define _LLY_TABLE_VIEW_H_

#include "llyScrollView.h"  //修改
#include "cocos2d.h"  //修改

namespace llyO_for_lly_tableview{

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
    void setItemModel(cocos2d::ui::Widget* model); //wi still
    
    /**
     * Push back a default item(create by a cloned model) into listview.
     */
    void pushBackDefaultItem();
    
    /**
     * Insert a default item(create by a cloned model) into listview.
     */
    void insertDefaultItem(ssize_t index);
    
    /**
     * Push back custom item into listview.
     */
    void pushBackCustomItem(cocos2d::Node* item); //修改
    
    /**
     * Insert custom item into listview.
     */
    void insertCustomItem(cocos2d::Node* item, ssize_t index); //修改
    
    /**
     *  Removes the last item of listview.
     */
    void removeLastItem();
    
    /**
     * Removes a item whose index is same as the parameter.
     *
     * @param index of item.
     */
    void removeItem(ssize_t index); //修改
    
    void removeAllItems();
    
    /**
     * Returns a item whose index is same as the parameter.
     *
     * @param index of item.
     *
     * @return the item widget.
     */
    cocos2d::Node* getItem(ssize_t index)const; //修改
    
    /**
     * Returns the item container.
     */
    cocos2d::Vector<cocos2d::Node*>& getItems(); //修改
    
    /**
     * Returns the index of item.
     *
     * @param item  the item which need to be checked.
     *
     * @return the index of item.
     */
    ssize_t getIndex(cocos2d::Node* item) const; //修改
    
    /**
     * Changes the gravity of listview.
     * @see ListViewGravity
     */
    virtual void setGravity(Gravity gravity);
    
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
    virtual void addChild(cocos2d::Node* child, int zOrder, int tag) override; //修改，不判断widget
    virtual void addChild(cocos2d::Node* child, int zOrder, const std::string &name) override; //修改，不判断widget
    virtual void removeAllChildren() override;
    virtual void removeAllChildrenWithCleanup(bool cleanup) override;
	virtual void removeChild(cocos2d::Node* child, bool cleaup = true) override; //修改，不判断widget
    
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
    virtual void updateInnerContainerSize();
    virtual void remedyLayoutParameter(cocos2d::Node* item);
    virtual void onSizeChanged() override;
    virtual cocos2d::ui::Widget* createCloneInstance() override; //wi still
    virtual void copySpecialProperties(cocos2d::ui::Widget* model) override; //wi still
    virtual void copyClonedWidgetChildren(cocos2d::ui::Widget* model) override; //wi still  修改
    void selectedItemEvent(TouchEventType event);
    virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch) override; //wi still
protected:
    cocos2d::ui::Widget* _model;
    
    cocos2d::Vector<cocos2d::Node*> _items;
    
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

} //llyO_for_lly_tableview


//new widget tableview========================

namespace lly{

class TableView : public llyO_for_lly_tableview::ListView
{
public:
	DECLARE_CLASS_GUI_INFO

	enum class ELayoutType //布局方式，绝对布局比较横平竖直，相对布局用于文字等
	{
		ABSOLUTE,
		RELATIVE,
	};

	enum class ETableViewGravity //停靠方向
	{
		LEFT_TOP,
		RIGHT_TOP,
		LEFT_BOTTOM,
		RIGHT_BOTTOM,
	};

	enum class EArrangementDirection//书写方向
	{
		HORIZONTAL, //先横着排，到头换行
		VERTICAL,	//先竖着排，到头换列
	};

	TableView();
	virtual ~TableView();

	static TableView* create();

	//设置布局方式（默认绝对布局）
	void setTableLayoutType(ELayoutType Ltype);

	//设置停靠方向
	void setTableViewGravity(ETableViewGravity TVG);

	virtual void setGravity(Gravity gravity) override; //重载

	//设置排版方向
	void setArrangementDirection(EArrangementDirection dir);

	virtual void setDirection(Direction dir) override; //重载

	//设置控件间距（绝对布局就是基点到基点，相对则是控件之间的距离）（默认为20）
	void setWidgetSpacing(float fSpacing);

	//设置行间距 基点到基点（默认为20）
	void setLineSpacing(float fLineSpacing);

	//设置到左右边儿的距离（默认为30）
	void setHorizontalMargin(float fMargin);

	//设置到上边儿的距离（默认为40）
	void setTopMargin(float fMargin);

	//设置到上边儿的距离（默认为20）
	void setBottomMargin(float fMargin);

	//此控件是否是一个换行符号
	bool isNewLineSymbol(cocos2d::Node* item); 

	//自添加==================
	cocos2d::Node* getLastItem();
	
	//==========================

	virtual void refreshView() override;

CC_CONSTRUCTOR_ACCESS:
	virtual bool init() override;

protected:
	virtual void updateInnerContainerSize() override;
	virtual void remedyLayoutParameter(cocos2d::Node* item) override;

	virtual cocos2d::ui::Widget* createCloneInstance() override;
	virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;

	virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch) override; //响应点中后移动
	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, bool parentTransformUpdated); //目标控件在最上

protected:
	cocos2d::Vector<cocos2d::Node*> m_arHeadNode; //每行头的widget
	
	ELayoutType m_eLayoutType;
	ETableViewGravity m_eTVG;
	EArrangementDirection m_eArrangeDir;

	float m_fWidgetSpacing; //默认20
	float m_fLineSpacing; //默认20

	float m_fHorizontalMargin; //默认30
	float m_fTopMargin; //默认40
	float m_fBottomMargin; //默认20

	cocos2d::Vec2 m_posMaxMove; //最大移动距离，大于这个距离不响应触控

	//temp
	bool m_bChangeZOrder; //刷新后 目标控件修改z轴到最上
	bool _bMoved;
	cocos2d::Vec2 m_posBegintouch; 

	//temp
	cocos2d::Node* lastItem; //refresh中，为提高效率用
	bool bneedcheckindex; //refresh中，为提高效率用
};

} //lly

#endif // _LLY_TABLE_VIEW_H_




