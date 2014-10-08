/*
名称：llyCheckBox.h
内容：新复选框控件，继承于原checkbox，
            新增加
            可以选择响应触摸的时机，点击开始时候（windows向）或者结束时候（触摸屏向）
            可以加入复选框管理器中，实现单选，额定数量选择等
            增加外部控制状态的方法
			增加lambda表达式的回调

制作：卢乐颜
日期：2014.5.1
*/

#ifndef _LLY_CHECK_BOX_H_
#define _LLY_CHECK_BOX_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

namespace lly{

enum CHECKBOX_ACT_TYPE
{
    WHEN_TOUCH_BEGAN,
    WHEN_TOUCH_ENDED,
};

class CheckBox : public cocos2d::ui::CheckBox
{
	DECLARE_CLASS_GUI_INFO

public:
	CheckBox();
	virtual ~CheckBox();

	static CheckBox* create();
    
	static CheckBox* create(const std::string& backGround,
                            const std::string& backGroundSeleted,
                            const std::string& cross,
                            const std::string& backGroundDisabled,
                            const std::string& frontCrossDisabled,
                           TextureResType texType = TextureResType::LOCAL);

    //获得单选管理器
    cocos2d::Ref* getManager() {return m_CKBmgr;}

	//设置响应时机
    void setActType(CHECKBOX_ACT_TYPE t) { m_actType = t; }

    //可实现外部改变复选框状态
    void turnToSelectedState(); //变成选中状态
    void turnToUnselectedState(); //变成非选中状态

    //重载触摸机制，可以选择响应触摸的时机，点击时候响应，或者弹起时候响应
    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unusedEvent);
	virtual void releaseUpEvent();

	virtual std::string getDescription() { return "lly CheckBox";  }

	void setCheckBoxManager(cocos2d::Ref* mgr, std::function <void(lly::CheckBox*, bool)> func);
protected:
	virtual Widget* createCloneInstance() override;
	virtual void copySpecialProperties(Widget* model) override;

protected:
    cocos2d::Ref* m_CKBmgr;
    CHECKBOX_ACT_TYPE m_actType;

	//如果有管理器，则在这里把点击事件传输给管理器
	std::function <void(lly::CheckBox*, bool)> excuteManagerEvent;
};

} // namespace lly

#endif //_LLY_CHECK_BOX_H_


