/*
名称：llyScrollView.h
内容：新的滑动列表控件，

			!!!!!< 原始函数修改一些函数成为虚函数：setInnerContainerSize, jumpToDestination >!!!!!!

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

#include "ui/UIScrollView.h"

namespace lly {

class Slider;

class ScrollView : public cocos2d::ui::ScrollView
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