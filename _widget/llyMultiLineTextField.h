/*
名称：llyMultilineTextField.h 
内容：多行文本输入框，继承于llyTableView，每个文字成为一个精灵依次排列
			带光标
			光标可处于任何位置

			安卓中，横屏下，为了让输入法不全屏化遮挡输入区域
				需要在libcocos2d
				中的Cocos2dxGLSurfaceView.java
				中的setCocos2dxEditText
				的if后面，添加
				int oldImeOptions = this.mCocos2dxEditText.getImeOptions();
				this.mCocos2dxEditText.setImeOptions(oldImeOptions | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
		
制作：卢乐颜
日期：2014.4.30
*/

#pragma once

#include "cocos2d.h"
#include "llyTableView.h"

namespace lly{

//人工光标
class Cursor : public cocos2d::LayerColor
{
public:
#define CURSOR_FLASH_INTERVAL (500) //出现消失间的间隔时间（毫秒）

	Cursor();
	virtual ~Cursor();
	CREATE_FUNC(Cursor);

	//重置光标
	void refreshCursor(); 

protected:
	virtual bool init();
	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags);

protected:
	bool m_bCursorOn; //光标是否显示的状态

	struct timeval m_timeNow; //当前系统时间
	struct timeval m_timeLast; //上次光标出现或隐藏的时间
};

class MultilineTextField : public lly::TableView, public cocos2d::IMEDelegate
{
public:
	enum class EventType
	{
		ATTACH_WITH_IME,
		DETACH_WITH_IME,
	};

	typedef std::function<void(EventType)> textFieldCallback;

	MultilineTextField();
	virtual ~MultilineTextField();

	static MultilineTextField* create();

	//文字与其属性
	void setText(const std::string &text);
	std::string getText();
	void clearText(); //清空

	void setFontSize(int size);
	int getFontSize();
	void changeAllWordsFontSize();

	void setFontName(const std::string &name);
	std::string getFontName();
	void changeAllWordsFontName();

	void setFontColor(cocos2d::Color3B &color);
	cocos2d::Color3B getFontColor();
	void changeAllWordsFontColor();

	//开始输入或停止时候的回调
	void addEventListener(const textFieldCallback &callback);

	//控制与输入器的链接（重载CCIMEDelegate）
	virtual bool attachWithIME() override;
	virtual bool detachWithIME() override;

	//设置是否可以开始输入或者关闭输入
	void setCanAttachWithIME(bool can);
	void setCanDetachWithIME(bool can);

	//打开、关闭光标
	void OpenCursor();
	void CloseCursor();

protected:
	virtual bool init();

	//当与输入器链接和断开的时候做什么（重载CCIMEDelegate）
	virtual bool canAttachWithIME() override;
	virtual bool canDetachWithIME() override;
	virtual void didAttachWithIME() override;
	virtual void didDetachWithIME() override;

	//添加文本（重载CCIMEDelegate）
	virtual void insertText( const char * text, size_t len ) override;

	//当点击删除时（重载CCIMEDelegate）
	virtual void deleteBackward() override;

	// IMEDispatcher获取文本时调用（重载CCIMEDelegate）
	virtual const std::string& getContentText() override;

	//软键盘（重载CCIMEDelegate）
	virtual void keyboardWillShow( cocos2d::IMEKeyboardNotificationInfo& info ) override;
	virtual void keyboardDidShow( cocos2d::IMEKeyboardNotificationInfo& info ) override;
	virtual void keyboardWillHide( cocos2d::IMEKeyboardNotificationInfo& info ) override;
	virtual void keyboardDidHide( cocos2d::IMEKeyboardNotificationInfo& info ) override;

	//不可修改以下设置
	void setTableLayoutType(ELayoutType Ltype) {};
	void setTableViewGravity(ETableViewGravity TVG) {};
	virtual void setGravity(Gravity gravity) override {}; //重载
	void setArrangementDirection(EArrangementDirection dir) {};
	virtual void setDirection(Direction dir) override {}; //重载

	//点击时开启输入，点击未中停止输入
	virtual bool onTouchBegan( cocos2d::Touch *pTouch, cocos2d::Event *pEvent ) override;

	//根据点击移动光标
	void moveCursorByTouch(cocos2d::Vec2 &touchBeganPoint);

	//直接跳到scrollview
	virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch) override;

	//获取碰撞体积
	cocos2d::Rect getRect()
	{
		return cocos2d::Rect(
			-getContentSize().width * getAnchorPoint().x, 
			-getContentSize().height * getAnchorPoint().y,
			getContentSize().width,
			getContentSize().height);
	}
 
 	//更新光标位置
 	void UpdateCursor();

protected:
	bool m_bActivate; //是否已经激活

	cocos2d::Label* m_word;

	textFieldCallback m_textFieldCallback; //输入开始和停止的回调

	int m_nFontSize;
	char* m_strFontName;
	cocos2d::Color3B m_FontColor;

	bool m_bCanAttchIME;
	bool m_bCanDetachIME;

	Cursor* m_pCursor; //光标
	std::vector<cocos2d::Node*>::iterator m_itTargetWord; //目标文字的指针，放置光标和插入文字的在这个文字后面
	int m_nIndexTargetWord; //目标文字的索引，放置光标和插入文字的在这个文字后面
	bool m_cursorInHead; //如果光标在某行的开头
};

} //lly end



