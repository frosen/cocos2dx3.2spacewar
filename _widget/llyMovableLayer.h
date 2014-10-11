/*
名称：llyMovableLayer.h
内容：包括两个类，分别继承CCLayer和ui::layout，给两个类添加一个多点触控的功能
			可以在创建时选择实现：
			1. 拖拽平移
			2. 两点放大缩小
			3. 两点旋转

制作：卢乐颜
日期：2014.5.1
*/

#ifndef _LLY_MOVABLE_LAYER_H_
#define _LLY_MOVABLE_LAYER_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

namespace lly{

class TouchFunction
{
public:
	typedef std::function<void(const cocos2d::Vec2*, const cocos2d::Vec2*, cocos2d::Event*, cocos2d::Node*)> TouchesCallback;

	enum //可以使用的操作的标识
	{
		move = 1,
		scale = 2,
		rotate = 4,
	};

	struct isHitted //记录触点是否点中
	{
		char nNumHittedPoint;
		cocos2d::Vec2 anchorOld;
		cocos2d::Vec2 posNewAnchor;
		isHitted() {}
		isHitted(char NumHittedPoint, cocos2d::Vec2 oldAnchor, cocos2d::Vec2 newAnchorPoint) : 
			nNumHittedPoint(NumHittedPoint),
			anchorOld(oldAnchor), 
			posNewAnchor(newAnchorPoint) {}
	};
	
	//给node增加功能
	static void setTouchFunction(cocos2d::Node* node, uint32_t flag); 

	//刷新node null为刷新全部
	static void refresh(cocos2d::Node* node = nullptr);

private:
	static void touchBeginWithChangeAnchor(
		const std::vector<cocos2d::Touch*> &vTouch, 
		cocos2d::Event* event, 
		cocos2d::Node* node, 
		bool needChangeAnchor);

	static void touchMoveHasWidgetMove(
		const std::vector<cocos2d::Touch*> &vTouch, 
		cocos2d::Event* event, 
		cocos2d::Node* node, 
		TouchesCallback moveMoved, 
		TouchesCallback scaleMoved, 
		TouchesCallback rotateMoved);
												  
	static void touchMoveWithoutWidgetMove(
		const std::vector<cocos2d::Touch*> &vTouch, 
		cocos2d::Event* event,
		cocos2d::Node* node, 
		TouchesCallback moveMoved, 
		TouchesCallback scaleMoved, 
		TouchesCallback rotateMoved);

	static void touchEndWithChangeAnchor(
		const std::vector<cocos2d::Touch*> &vTouch, 
		cocos2d::Event* event, 
		cocos2d::Node* node, 
		bool needChangeAnchor);

	//触点是否击中了node
	static bool containsPoint(cocos2d::Node* node, cocos2d::Vec2 point);

	//改变锚点位置到第一触点，缩放和旋转以此为基点，记录原有锚点
	static void changeAnchorToTouchPoint(
		cocos2d::Node* node, 
		const cocos2d::Vec2 &pointInNode, 
		cocos2d::Vec2 &oldAnchor_out);

	static void changeAnchorBack(
		cocos2d::Node* node, 
		const cocos2d::Vec2 &pointInNode, 
		cocos2d::Vec2 &oldAnchor);

private:
	//三种触摸功能
	static TouchesCallback m_moveMoved; //点击移动
	static TouchesCallback m_scaleMoved; //两点开合改变大小
	static TouchesCallback m_rotateMoved; //两点扭错旋转图	

	//记录当前node和是否点击，在bengin，move，end间传递消息
	static std::map <cocos2d::Node*, isHitted> mNodeIsHitted; 
};

class MovableLayer : public cocos2d::Layer
{
public:
	MovableLayer();
	virtual ~MovableLayer();

	static MovableLayer* create(uint32_t flag);

protected:
	virtual bool init(uint32_t flag);
};

class MovableLayout : public cocos2d::ui::Layout
{
	DECLARE_CLASS_GUI_INFO

public:
	MovableLayout();
	virtual ~MovableLayout();

	CREATE_FUNC(MovableLayout);
	static MovableLayout* create(uint32_t flag);

	void setTouchFunction(uint32_t flag);

protected:
	virtual bool init();
	virtual bool init(uint32_t flag);

	virtual std::string getDescription() const override { return "lly Movable Layer"; };

};


} // namespace lly

#endif //_LLY_MOVABLE_LAYER_H_


