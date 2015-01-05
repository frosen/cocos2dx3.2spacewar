/*
名称：llyListView.h
内容：新的ListView

			!!!!< 其父类中有些函数要变成虚函数，为了继承，包括
			pushBackDefaultItem to removeAllItems
			updateInnerContainerSize
			remedyLayoutParameter 

			其父类要继承于lly::Scrollview
			>!!!!

			添加动画
			添加对目标控件的操作
			添加移动控件的函数
			添加操作控件后的回调

制作：卢乐颜
日期：2014.9.1
*/

#ifndef _LLY_LIST_VIEW_H__  
#define _LLY_LIST_VIEW_H__

#include "ui/UIListView.h"

//新listview

namespace lly{

class ListView : cocos2d::ui::ListView
{
	DECLARE_CLASS_GUI_INFO

public:
	enum class changeItemType//控件的改变方式
	{
		ADD_ITEM, //增加
		REMOVE_ITEM, //删除
		MOVE_ITEM, //移动
	};

	enum class WidgetMoveType//移动控件的方式
	{
		MOVE_BY, //移动某个距离
		MOVE_TO, //移动到哪里
	};

	enum class WidgetMoveDirection//移动控件的方式
	{
		MOVE_FORWARD, //向前移动
		MOVE_BACKWARD, //向后移动
	};

	typedef std::function<void(cocos2d::Ref*, changeItemType t)> changeItemCallback;

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
	void moveItem(
		cocos2d::ui::Widget* item, 
		WidgetMoveDirection dir, 
		WidgetMoveType t = WidgetMoveType::MOVE_BY, 
		int num = 1);

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
			_direction == cocos2d::ui::ScrollView::Direction::VERTICAL ? item->getContentSize().height : 
			_direction == cocos2d::ui::ScrollView::Direction::VERTICAL ? item->getContentSize().width : 0); 
	}

	//增删移动子控件时候调用 默认空
	void changeWidgetEvent(changeItemType type);

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


