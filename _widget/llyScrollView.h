/*
名称：llyScrollView.h
内容：新的滑动列表控件，
			原始函数修改一些函数成为虚函数：setInnerContainerSize, jumpToDestination
			特性：
            给内层范围超出外层时增加了回调函数，可用于此时显示滑动条，
			给拖动内层时增加了回调函数，可用于更改滑动条
			可直接关联纵向和横向的滑动条
			可不让子层随拖拽移动（windows向）
			增加了设置目标控件
			存在非虚继承

制作：卢乐颜
日期：2014.5.1
*/

#ifndef _LLY_SCROLL_VIEW_H_
#define _LLY_SCROLL_VIEW_H_

#include "ui/UILayout.h"

namespace llyO_for_lly_scrollview{

class EventFocusListener;

typedef enum
{
    SCROLLVIEW_EVENT_SCROLL_TO_TOP,
    SCROLLVIEW_EVENT_SCROLL_TO_BOTTOM,
    SCROLLVIEW_EVENT_SCROLL_TO_LEFT,
    SCROLLVIEW_EVENT_SCROLL_TO_RIGHT,
    SCROLLVIEW_EVENT_SCROLLING,
    SCROLLVIEW_EVENT_BOUNCE_TOP,
    SCROLLVIEW_EVENT_BOUNCE_BOTTOM,
    SCROLLVIEW_EVENT_BOUNCE_LEFT,
    SCROLLVIEW_EVENT_BOUNCE_RIGHT
}ScrollviewEventType;

typedef void (cocos2d::Ref::*SEL_ScrollViewEvent)(cocos2d::Ref*, ScrollviewEventType);
#define scrollvieweventselector(_SELECTOR) (SEL_ScrollViewEvent)(&_SELECTOR)


class ScrollView : public cocos2d::ui::Layout
{
    
    DECLARE_CLASS_GUI_INFO
    
public:
    enum class Direction
    {
        NONE,
        VERTICAL,
        HORIZONTAL,
        BOTH
    };
    
    enum class EventType
    {
        SCROLL_TO_TOP,
        SCROLL_TO_BOTTOM,
        SCROLL_TO_LEFT,
        SCROLL_TO_RIGHT,
        SCROLLING,
        BOUNCE_TOP,
        BOUNCE_BOTTOM,
        BOUNCE_LEFT,
        BOUNCE_RIGHT
    };
    typedef std::function<void(cocos2d::Ref*, EventType)> ccScrollViewCallback;
   
    /**
     * Default constructor
     */
    ScrollView();
    
    /**
     * Default destructor
     */
    virtual ~ScrollView();
    /**
     * Allocates and initializes.
     */
    static ScrollView* create();
    
    /**
     * Changes scroll direction of scrollview.
     *
     * @see Direction      Direction::VERTICAL means vertical scroll, Direction::HORIZONTAL means horizontal scroll
     *
     * @param dir
     */
    virtual void setDirection(Direction dir);
    
    /**
     * Gets scroll direction of scrollview.
     *
     * @see Direction      Direction::VERTICAL means vertical scroll, Direction::HORIZONTAL means horizontal scroll
     *
     * @return Direction
     */
    Direction getDirection()const;
    
    /**
     * Gets inner container of scrollview.
     *
     * Inner container is the container of scrollview's children.
     *
     * @return inner container.
     */
    cocos2d::ui::Layout* getInnerContainer()const;
    
    /**
     * Scroll inner container to bottom boundary of scrollview.
     */
    void scrollToBottom(float time, bool attenuated);
    
    /**
     * Scroll inner container to top boundary of scrollview.
     */
    void scrollToTop(float time, bool attenuated);
    
    /**
     * Scroll inner container to left boundary of scrollview.
     */
    void scrollToLeft(float time, bool attenuated);
    
    /**
     * Scroll inner container to right boundary of scrollview.
     */
    void scrollToRight(float time, bool attenuated);
    
    /**
     * Scroll inner container to top and left boundary of scrollview.
     */
    void scrollToTopLeft(float time, bool attenuated);
    
    /**
     * Scroll inner container to top and right boundary of scrollview.
     */
    void scrollToTopRight(float time, bool attenuated);
    
    /**
     * Scroll inner container to bottom and left boundary of scrollview.
     */
    void scrollToBottomLeft(float time, bool attenuated);
    
    /**
     * Scroll inner container to bottom and right boundary of scrollview.
     */
    void scrollToBottomRight(float time, bool attenuated);
    
    /**
     * Scroll inner container to vertical percent position of scrollview.
     */
    void scrollToPercentVertical(float percent, float time, bool attenuated);
    
    /**
     * Scroll inner container to horizontal percent position of scrollview.
     */
    void scrollToPercentHorizontal(float percent, float time, bool attenuated);
    
    /**
     * Scroll inner container to both direction percent position of scrollview.
     */
    void scrollToPercentBothDirection(const cocos2d::Vec2& percent, float time, bool attenuated);
    
    /**
     * Move inner container to bottom boundary of scrollview.
     */
    void jumpToBottom();
    
    /**
     * Move inner container to top boundary of scrollview.
     */
    void jumpToTop();
    
    /**
     * Move inner container to left boundary of scrollview.
     */
    void jumpToLeft();
    
    /**
     * Move inner container to right boundary of scrollview.
     */
    void jumpToRight();
    
    /**
     * Move inner container to top and left boundary of scrollview.
     */
    void jumpToTopLeft();
    
    /**
     * Move inner container to top and right boundary of scrollview.
     */
    void jumpToTopRight();
    
    /**
     * Move inner container to bottom and left boundary of scrollview.
     */
    void jumpToBottomLeft();
    
    /**
     * Move inner container to bottom and right boundary of scrollview.
     */
    void jumpToBottomRight();
    
    /**
     * Move inner container to vertical percent position of scrollview.
     */
    void jumpToPercentVertical(float percent);
    
    /**
     * Move inner container to horizontal percent position of scrollview.
     */
    void jumpToPercentHorizontal(float percent);
    
    /**
     * Move inner container to both direction percent position of scrollview.
     */
    void jumpToPercentBothDirection(const cocos2d::Vec2& percent);
    
    /**
     * Changes inner container size of scrollview.
     *
     * Inner container size must be larger than or equal scrollview's size.
     *
     * @param inner container size.
     */
    virtual void setInnerContainerSize(const cocos2d::Size &size);
    
    /**
     * Gets inner container size of scrollview.
     *
     * Inner container size must be larger than or equal scrollview's size.
     *
     * @return inner container size.
     */
	const cocos2d::Size& getInnerContainerSize() const;
    
    /**
     * Add call back function called scrollview event triggered
     */
    CC_DEPRECATED_ATTRIBUTE void addEventListenerScrollView(cocos2d::Ref* target, SEL_ScrollViewEvent selector);
    virtual void addEventListener(const ccScrollViewCallback& callback);
    
    //all of these functions are related to innerContainer.
    virtual void addChild(cocos2d::Node* child)override;
    virtual void addChild(cocos2d::Node * child, int localZOrder)override;
    virtual void addChild(cocos2d::Node* child, int zOrder, int tag) override;
    virtual void addChild(cocos2d::Node* child, int zOrder, const std::string &name) override;
    
    virtual void removeAllChildren() override;
    virtual void removeAllChildrenWithCleanup(bool cleanup) override;
	virtual void removeChild(cocos2d::Node* child, bool cleaup = true) override;
    
    virtual cocos2d::Vector<cocos2d::Node*>& getChildren() override;
    virtual const cocos2d::Vector<cocos2d::Node*>& getChildren() const override;
    virtual ssize_t getChildrenCount() const override;
    virtual cocos2d::Node * getChildByTag(int tag) const override;
    virtual cocos2d::Node* getChildByName(const std::string& name)const override;
    
    //handle touch event
    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
    virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
    virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
    
    virtual void update(float dt) override;
    
    void setBounceEnabled(bool enabled);
    
    bool isBounceEnabled() const;
    
    void setInertiaScrollEnabled(bool enabled);
    
    bool isInertiaScrollEnabled() const;
    
    /**
     * Sets LayoutType.
     *
     * @see LayoutType
     *
     * @param LayoutType
     */
    virtual void setLayoutType(cocos2d::ui::Layout::Type type) override;
    
    /**
     * Gets LayoutType.
     *
     * @see LayoutType
     *
     * @return LayoutType
     */
    virtual cocos2d::ui::Layout::Type getLayoutType() const override;
    
    /**
     * Returns the "class name" of widget.
     */
    virtual std::string getDescription() const override;
    
    virtual void onEnter() override;
    
    /**
     *  When a widget is in a layout, you could call this method to get the next focused widget within a specified direction.
     *  If the widget is not in a layout, it will return itself
     *@param dir the direction to look for the next focused widget in a layout
     *@param current  the current focused widget
     *@return the next focused widget in a layout
     */
    virtual cocos2d::ui::Widget* findNextFocusedWidget(cocos2d::ui::Widget::FocusDirection direction, cocos2d::ui::Widget* current) override;

CC_CONSTRUCTOR_ACCESS:
    virtual bool init() override;
    
protected:
    virtual void initRenderer() override;
    
    virtual void onSizeChanged() override;
    virtual void doLayout() override;

    virtual cocos2d::ui::Widget* createCloneInstance() override;
    virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;
    virtual void copyClonedWidgetChildren(cocos2d::ui::Widget* model) override;
    
    
    void moveChildren(float offsetX, float offsetY);
    void autoScrollChildren(float dt);
    void bounceChildren(float dt);
    void checkBounceBoundary();
    bool checkNeedBounce();
    void startAutoScrollChildrenWithOriginalSpeed(const cocos2d::Vec2& dir, float v, bool attenuated, float acceleration);
    void startAutoScrollChildrenWithDestination(const cocos2d::Vec2& des, float time, bool attenuated);
    virtual void jumpToDestination(const cocos2d::Vec2& des);
    void stopAutoScrollChildren();
    void startBounceChildren(float v);
    void stopBounceChildren();
    bool checkCustomScrollDestination(float* touchOffsetX, float* touchOffsetY);
    
    virtual bool scrollChildren(float touchOffsetX, float touchOffsetY);

    bool scrollChildrenVertical(float touchOffsetX, float touchOffsetY);
    bool scrollChildrenHorizontal(float touchOffsetX, float touchOffestY);
    bool scrollChildrenBoth(float touchOffsetX, float touchOffsetY);

    
    bool bounceScrollChildren(float touchOffsetX, float touchOffsetY);
    void startRecordSlidAction();
    virtual void endRecordSlidAction();
    
    //ScrollViewProtocol
    virtual void handlePressLogic(cocos2d::Touch *touch) ;
    virtual void handleMoveLogic(cocos2d::Touch *touch) ;
    virtual void handleReleaseLogic(cocos2d::Touch *touch) ;
    
    virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch *touch) override;
    
    void recordSlidTime(float dt);
    
    void scrollToTopEvent();
    void scrollToBottomEvent();
    void scrollToLeftEvent();
    void scrollToRightEvent();
    void scrollingEvent();
    
    void bounceTopEvent();
    void bounceBottomEvent();
    void bounceLeftEvent();
    void bounceRightEvent();
    
protected:
    cocos2d::ui::Layout* _innerContainer;
    
    Direction _direction;
    cocos2d::Vec2 _autoScrollDir;
    
    float _topBoundary;
    float _bottomBoundary;
    float _leftBoundary;
    float _rightBoundary;
    
    float _bounceTopBoundary;
    float _bounceBottomBoundary;
    float _bounceLeftBoundary;
    float _bounceRightBoundary;

    
    bool _autoScroll;
    float _autoScrollAddUpTime;
    
    float _autoScrollOriginalSpeed;
    float _autoScrollAcceleration;
    bool _isAutoScrollSpeedAttenuated;
    bool _needCheckAutoScrollDestination;
    cocos2d::Vec2 _autoScrollDestination;
    
    bool _bePressed;
    float _slidTime;
    cocos2d::Vec2 _moveChildPoint;
    float _childFocusCancelOffset;
    
    bool _leftBounceNeeded;
    bool _topBounceNeeded;
    bool _rightBounceNeeded;
    bool _bottomBounceNeeded;
    
    bool _bounceEnabled;
    bool _bouncing;
    cocos2d::Vec2 _bounceDir;
    float _bounceOriginalSpeed;
    bool _inertiaScrollEnabled;
    
    cocos2d::Ref* _scrollViewEventListener;
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (push)
#pragma warning (disable: 4996)
#endif
    SEL_ScrollViewEvent _scrollViewEventSelector;
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic warning "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (pop)
#endif
    ccScrollViewCallback _eventCallback;
};

} //llyO_for_lly_scrollview

//======================================
namespace lly {

class Slider;

class ScrollView : public llyO_for_lly_scrollview::ScrollView
{
	DECLARE_CLASS_GUI_INFO

public:
	enum class ChangeInnerSizeType//添加，用于变更内层大小时的回调
	{
		NO_CHANGE,
		INNER_TURN_LONGER_THAN_OUTER,
		INNER_TURN_NOT_LONGER_THAN_OUTER,
	};

	typedef std::function<void(cocos2d::Ref* ob, int x, int y)> ScrollViewCallback; 
	typedef std::function<void(cocos2d::Ref* ob, ChangeInnerSizeType wT, ChangeInnerSizeType hT, int w, int h)> ChangeInnerCallback; 

	ScrollView();
    virtual ~ScrollView();

    static ScrollView* create();

	virtual void setInnerContainerSize(const cocos2d::Size &size) override;

	//===========================================
	//滚动事件，控制滑动条，参数为控件对象，到达X轴%，到达y轴%，-1为内层不大于外层
	//x轴左边是0，y轴上边是0
	void addScrollViewEventListener(const ScrollViewCallback &cb) { m_addScrollViewEventFunc = cb; }

	//添加内层变化的事件，type显示是否突然内层大过外层，或突然不大了，意味着是否显示滑动条，w 和 t 最小为0 最大100，为外层和内层的尺寸比例	
	void addChangeInnerEventListener(const ChangeInnerCallback &cb) { m_addChangeInnerEventFunc = cb; }

	//获取内层是否大于外层
	bool isInnerWider() { return isInnerWidthThanOuter; }
	bool isInnerHigher() { return isInnerHeightThanOuter; }

	//设置关联的滑动条，bool为true表示不需要滑动条的时候会隐藏它
	void setLinkedSlider(lly::Slider* heightSlider, lly::Slider* widthSlider = nullptr, bool canHideSlider = true);

	//设置和获取是否可以移动子层（默认可以）
	void setInnerCanMove(bool b) { _isInnerCanMove = b; }
	bool isInnerCanMove() { return _isInnerCanMove; }

	//设置和得到目标控件
	void setTargetItem(cocos2d::Node* target) { m_targetItem = target; }
	cocos2d::Node* getTargetItem() { return m_targetItem; }

	//===========================================
    /**
     * Returns the "class name" of widget.
     */
    virtual std::string getDescription() const override;

protected:
	virtual void onSizeChanged() override;

	virtual void jumpToDestination(const cocos2d::Vec2& des); //非虚继承

	virtual bool scrollChildren(float touchOffsetX, float touchOffsetY) override;

	virtual void endRecordSlidAction() override;

	virtual void handleMoveLogic(cocos2d::Touch *touch) override;

	//==============================
	//记录百分比和进行回调
	void doAfterChangeInnerContainerPosition(bool runFunction = true);

	//添加删除控件的时候，可能会造成内层控件大小改变，此时更新比例，并回调一个函数
	void changeInnerContainerSizeEvent();

	//===============================
	virtual cocos2d::ui::Widget* createCloneInstance() override;
	virtual void copySpecialProperties(Widget* model) override;

protected:
	//==========================
	ScrollViewCallback m_addScrollViewEventFunc;
	ChangeInnerCallback m_addChangeInnerEventFunc;

	//内层在整个外层的百分比位置
	int m_innerContainerPositionXPercent; //左边是0 默认-1
	int m_innerContainerPositionYPercent; //上边是0 默认-1

	bool isInnerWidthThanOuter; //默认为false，变更size同时变更
	bool isInnerHeightThanOuter; //默认为false，变更size同时变更

	lly::Slider* m_heightSlider; //相关滑动条
	lly::Slider* m_widthSlider;

	bool _isInnerCanMove; //是否可以拖拽移动子层，默认为true

	cocos2d::Node* m_targetItem;
};

}

#endif