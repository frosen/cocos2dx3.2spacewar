/*
名称：llyTableView.h
内容：表格控件，继承于listview，只是排序方式与listview不同

制作：卢乐颜
日期：2014.5.1
*/

#ifndef _LLY_TABLE_VIEW_H_
#define _LLY_TABLE_VIEW_H_

#include "llyListView.h"

namespace lly{

class TableView : llyO::ListView
{
public:
	typedef enum  //布局方式，绝对布局比较横平竖直，相对布局用于文字等
	{
		ABSOLUTE,
		RELATIVE,
	}ELayoutType;

	typedef enum  //停靠方向
	{
		LEFT_TOP,
		RIGHT_TOP,
		LEFT_BOTTOM,
		RIGHT_BOTTOM,
	}ETableViewGravity;

	typedef enum //书写方向
	{
		HORIZONTAL, //先横着排，到头换行
		VERTICAL,	//先竖着排，到头换列
	}EArrangementDirection;

	TableView();
	virtual ~TableView();

	static TableView* create();

	//设置布局方式（默认绝对布局）
	void setTableLayoutType(ELayoutType Ltype);

	//设置停靠方向
	void setTableViewGravity(ETableViewGravity TVG);

	virtual void setGravity(Gravity gravity) override; //重载

	//设置书写方向（默认从左到右）
	void setWriteOrder(EWriteOrder wo);

	//设置排版方向
	void setArrangementDirection(EArrangementDirection dir);

	virtual void setDirection(Direction dir) override; //重载

	//设置控件间距（绝对布局就是基点到基点，相对则是控件之间的距离）（默认为20）
	void setWidgetSpacing(float fSpacing);

	//设置行间距 基点到基点（默认为20）
	void setLineSpacing(float fLineSpacing);

	//设置到左右边儿的距离（默认为30）
	void setHorizontalMargin(float fMargin);

	//设置到上或下边儿的距离（默认为30）
	void setVerticalMargin(float fMargin);

	//自添加==================
	cocos2d::ui::Widget* getLastItem(); 
	
	//==========================

	virtual void refreshView() override;

CC_CONSTRUCTOR_ACCESS:
	virtual bool init() override;

protected:
	virtual void updateInnerContainerSize() override;
	virtual void remedyLayoutParameter(cocos2d::ui::Widget* item) override;

	virtual cocos2d::ui::Widget* createCloneInstance() override;
	virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;

protected:
	cocos2d::Vector<cocos2d::Node*> m_arHeadNode; //每行头的widget
	
	ELayoutType m_eLayoutType;
	ETableViewGravity m_eTVG;
	EArrangementDirection m_eArrangeDir;

	float m_fWidgetSpacing; //默认20
	float m_fLineSpacing; //默认20

	float m_fHorizontalMargin; //默认30
	float m_fVerticalMargin; //默认30

	cocos2d::ui::Widget* lastItem; //refresh中，为提高效率用
	bool bneedcheckindex; //refresh中，为提高效率用
};

} //lly

#endif // _LLY_TABLE_VIEW_H_




