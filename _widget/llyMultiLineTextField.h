/*
���ƣ�llyMultilineTextField.h 
���ݣ������ı�����򣬼̳���llyTableView��ÿ�����ֳ�Ϊһ��������������
			�����
			���ɴ����κ�λ��

			��׿�У������£�Ϊ�������뷨��ȫ�����ڵ���������
				��Ҫ��libcocos2d
				�е�Cocos2dxGLSurfaceView.java
				�е�setCocos2dxEditText
				��if���棬���
				int oldImeOptions = this.mCocos2dxEditText.getImeOptions();
				this.mCocos2dxEditText.setImeOptions(oldImeOptions | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
		
������¬����
���ڣ�2014.4.30
*/

#pragma once

#include "cocos2d.h"
#include "llyTableView.h"

namespace lly{

//�˹����
class Cursor : public cocos2d::LayerColor
{
public:
#define CURSOR_FLASH_INTERVAL (500) //������ʧ��ļ��ʱ�䣨���룩

	Cursor();
	virtual ~Cursor();
	CREATE_FUNC(Cursor);

	//���ù��
	void refreshCursor(); 

protected:
	virtual bool init();
	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags);

protected:
	bool m_bCursorOn; //����Ƿ���ʾ��״̬

	struct timeval m_timeNow; //��ǰϵͳʱ��
	struct timeval m_timeLast; //�ϴι����ֻ����ص�ʱ��
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

	//������������
	void setText(const std::string &text);
	std::string getText();
	void clearText(); //���

	void setFontSize(int size);
	int getFontSize();
	void changeAllWordsFontSize();

	void setFontName(const std::string &name);
	std::string getFontName();
	void changeAllWordsFontName();

	void setFontColor(cocos2d::Color3B &color);
	cocos2d::Color3B getFontColor();
	void changeAllWordsFontColor();

	//��ʼ�����ֹͣʱ��Ļص�
	void addEventListener(const textFieldCallback &callback);

	//�����������������ӣ�����CCIMEDelegate��
	virtual bool attachWithIME() override;
	virtual bool detachWithIME() override;

	//�����Ƿ���Կ�ʼ������߹ر�����
	void setCanAttachWithIME(bool can);
	void setCanDetachWithIME(bool can);

	//�򿪡��رչ��
	void OpenCursor();
	void CloseCursor();

protected:
	virtual bool init();

	//�������������ӺͶϿ���ʱ����ʲô������CCIMEDelegate��
	virtual bool canAttachWithIME() override;
	virtual bool canDetachWithIME() override;
	virtual void didAttachWithIME() override;
	virtual void didDetachWithIME() override;

	//����ı�������CCIMEDelegate��
	virtual void insertText( const char * text, size_t len ) override;

	//�����ɾ��ʱ������CCIMEDelegate��
	virtual void deleteBackward() override;

	// IMEDispatcher��ȡ�ı�ʱ���ã�����CCIMEDelegate��
	virtual const std::string& getContentText() override;

	//����̣�����CCIMEDelegate��
	virtual void keyboardWillShow( cocos2d::IMEKeyboardNotificationInfo& info ) override;
	virtual void keyboardDidShow( cocos2d::IMEKeyboardNotificationInfo& info ) override;
	virtual void keyboardWillHide( cocos2d::IMEKeyboardNotificationInfo& info ) override;
	virtual void keyboardDidHide( cocos2d::IMEKeyboardNotificationInfo& info ) override;

	//�����޸���������
	void setTableLayoutType(ELayoutType Ltype) {};
	void setTableViewGravity(ETableViewGravity TVG) {};
	virtual void setGravity(Gravity gravity) override {}; //����
	void setArrangementDirection(EArrangementDirection dir) {};
	virtual void setDirection(Direction dir) override {}; //����

	//���ʱ�������룬���δ��ֹͣ����
	virtual bool onTouchBegan( cocos2d::Touch *pTouch, cocos2d::Event *pEvent ) override;

	//���ݵ���ƶ����
	void moveCursorByTouch(cocos2d::Vec2 &touchBeganPoint);

	//ֱ������scrollview
	virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch) override;

	//��ȡ��ײ���
	cocos2d::Rect getRect()
	{
		return cocos2d::Rect(
			-getContentSize().width * getAnchorPoint().x, 
			-getContentSize().height * getAnchorPoint().y,
			getContentSize().width,
			getContentSize().height);
	}
 
 	//���¹��λ��
 	void UpdateCursor();

protected:
	bool m_bActivate; //�Ƿ��Ѿ�����

	cocos2d::Label* m_word;

	textFieldCallback m_textFieldCallback; //���뿪ʼ��ֹͣ�Ļص�

	int m_nFontSize;
	char* m_strFontName;
	cocos2d::Color3B m_FontColor;

	bool m_bCanAttchIME;
	bool m_bCanDetachIME;

	Cursor* m_pCursor; //���
	std::vector<cocos2d::Node*>::iterator m_itTargetWord; //Ŀ�����ֵ�ָ�룬���ù��Ͳ������ֵ���������ֺ���
	int m_nIndexTargetWord; //Ŀ�����ֵ����������ù��Ͳ������ֵ���������ֺ���
	bool m_cursorInHead; //��������ĳ�еĿ�ͷ
};

} //lly end



