#include "llyCheckBox.h"

USING_NS_CC;
namespace lly {

IMPLEMENT_CLASS_GUI_INFO(lly::CheckBox)

lly::CheckBox::CheckBox() : ui::CheckBox(), m_CKBmgr(nullptr)
{

}

lly::CheckBox::~CheckBox()
{

}

CheckBox* lly::CheckBox::create()
{
	CheckBox* widget = new CheckBox();
	if (widget && widget->init())
	{

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 //响应触摸的时机
		widget->m_actType = WHEN_TOUCH_BEGAN; 
#else
		widget->m_actType = WHEN_TOUCH_ENDED;
#endif
        
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

CheckBox* lly::CheckBox::create( const std::string& backGround, const std::string& backGroundSeleted, const std::string& cross, const std::string& backGroundDisabled, const std::string& frontCrossDisabled, TextureResType texType /*= LOCAL*/ )
{
	CheckBox *pWidget = new CheckBox;
	if (pWidget && pWidget->init(backGround,
		backGroundSeleted,
		cross,
		backGroundDisabled,
		frontCrossDisabled,
		texType))
	{

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 //响应触摸的时机
		pWidget->m_actType = WHEN_TOUCH_BEGAN; 
#else
		pWidget->m_actType = WHEN_TOUCH_ENDED;
#endif

		pWidget->autorelease();
		return pWidget;
	}
	CC_SAFE_DELETE(pWidget);
	return nullptr;
}

void CheckBox::turnToSelectedState()
{
    if (_isSelected == true) return;

    setSelectedState(true);
    selectedEvent();

	//查看是否在管理器中，是的话则使其他控件取消激活
	if (excuteManagerEvent) 
	{
		excuteManagerEvent(this, true);
	}
}
		

void CheckBox::turnToUnselectedState()
{
    if (_isSelected == false) return;
    setSelectedState(false);
    unSelectedEvent();

	if (excuteManagerEvent) excuteManagerEvent(this, false);
}

bool CheckBox::onTouchBegan( cocos2d::Touch *touch, cocos2d::Event *unusedEvent )
{
    bool b = Widget::onTouchBegan(touch, unusedEvent);

    if (m_actType == WHEN_TOUCH_BEGAN && _hitted)
    {
        if (_isSelected)
        {
            turnToUnselectedState();
        }
        else
        {
            turnToSelectedState();
        }
    }

    return b;
}

void CheckBox::releaseUpEvent()
{
	Widget::releaseUpEvent();

	if (m_actType == WHEN_TOUCH_BEGAN) //响应时机是开始的话就在此跳过
	{
		//跳过
	}
	else if (_isSelected)
	{
		turnToUnselectedState();
	}
	else
	{
		turnToSelectedState();
	}
}

void CheckBox::setCheckBoxManager( cocos2d::Ref* mgr, std::function <void(lly::CheckBox*, bool)> func )
{
	m_CKBmgr = mgr;
	excuteManagerEvent = func;
}

ui::Widget* CheckBox::createCloneInstance()
{
	return CheckBox::create();
}

void CheckBox::copySpecialProperties( ui::Widget* model )
{
	CheckBox* checkBox = dynamic_cast<CheckBox*>(model);
	if (checkBox)
	{
		cocos2d::ui::CheckBox::copySpecialProperties(model);

		//
		m_CKBmgr = checkBox->m_CKBmgr;
		m_actType = checkBox->m_actType;
		excuteManagerEvent = checkBox->excuteManagerEvent;
	}
}

} //namespace lly end


