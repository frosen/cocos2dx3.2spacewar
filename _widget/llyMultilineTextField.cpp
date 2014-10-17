#include "llyMultiLineTextField.h"

USING_NS_CC;
using namespace lly;

lly::Cursor::Cursor()
{
	 
}

lly::Cursor::~Cursor()
{

}

void lly::Cursor::refreshCursor()
{
	//��ԭ��������
	setOpacity(255);
	m_timeLast = m_timeNow;
	m_bCursorOn = true;
}

bool lly::Cursor::init()
{
	if (!LayerColor::init()) return false;

	//���ù��Ļ�����״����ɫ
	setContentSize(Size(2,40));
	setAnchorPoint(Vec2(0,0));
	setColor(Color3B::BLACK); //��ɫΪ��ɫ

	//���ó�ʼֵ
	setOpacity(0);
	m_timeLast.tv_sec = 0;
	m_timeLast.tv_usec = 0;

	m_bCursorOn=false;

	return true;
}

void lly::Cursor::visit( cocos2d::Renderer* renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags )
{
	gettimeofday(&m_timeNow, nullptr);

	//ʱ�䳬������Ͳ�ͣ�ĳ��ֺ���ʧ
	if (m_timeNow.tv_usec - m_timeLast.tv_usec > CURSOR_FLASH_INTERVAL)
	{
		if (!m_bCursorOn)
		{
			//�ɼ���ɫ���
			setOpacity(255);
			m_timeLast = m_timeNow;
			m_bCursorOn = true;
		}
		else
		{
			//���ع��
			setOpacity(0);
			m_timeLast = m_timeNow;
			m_bCursorOn = false;
		}
	}
	
	LayerColor::visit();
}

//=======================================
lly::MultilineTextField::MultilineTextField() :
	lly::TableView(),
	m_bActivate(false),
	m_nFontSize(18),
	m_FontColor(Color3B::BLACK),
	m_strFontName(""),
	m_pCursor(nullptr),
	m_textFieldCallback(nullptr),
	m_bCanAttchIME(true),
	m_bCanDetachIME(true),
	m_nIndexTargetWord(-1),
	m_cursorInHead(false)
{

}

lly::MultilineTextField::~MultilineTextField()
{
	CC_SAFE_RELEASE(m_pCursor);
}

MultilineTextField* lly::MultilineTextField::create()
{
	MultilineTextField* widget = new MultilineTextField();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	else
	{
		CC_SAFE_DELETE(widget);
		return nullptr;
	}	
}

void lly::MultilineTextField::setText( const std::string &text )
{

}

std::string lly::MultilineTextField::getText()
{
	return "";
}

void lly::MultilineTextField::clearText()
{

}

void lly::MultilineTextField::setFontSize( int size )
{
	m_nFontSize = size;
}

int lly::MultilineTextField::getFontSize()
{
	return m_nFontSize;
}

void lly::MultilineTextField::changeAllWordsFontSize()
{
// 	ccArray* arrayItems = _items->data;
// 	int length = arrayItems->num;
// 	for (int i=0; i<length; ++i)
// 	{
// 		static_cast<CCLabelTTF*>(arrayItems->arr[i])->setFontSize(m_nFontSize);		
// 	}
// 	_refreshViewDirty = true;
}

void lly::MultilineTextField::setFontName( const std::string& name )
{
	m_strFontName = const_cast<char*>(name.c_str());
}

std::string lly::MultilineTextField::getFontName()
{
	return m_strFontName;
}

void lly::MultilineTextField::changeAllWordsFontName()
{
// 	ccArray* arrayItems = _items->data;
// 	int length = arrayItems->num;
// 	for (int i=0; i<length; ++i)
// 	{
// 		static_cast<CCLabelTTF*>(arrayItems->arr[i])->setFontName(m_strFontName);		
// 	}
// 	_refreshViewDirty = true;
}

void lly::MultilineTextField::setFontColor( cocos2d::Color3B &color )
{

}

cocos2d::Color3B lly::MultilineTextField::getFontColor()
{
	return m_FontColor;
}

void lly::MultilineTextField::changeAllWordsFontColor()
{
// 	ccArray* arrayItems = _items->data;
// 	int length = arrayItems->num;
// 	for (int i=0; i<length; ++i)
// 	{
// 		static_cast<CCLabelTTF*>(arrayItems->arr[i])->setColor(m_FontColor);		
// 	}
// 	_refreshViewDirty = true;
}

void lly::MultilineTextField::addEventListener( const textFieldCallback &callback )
{
	m_textFieldCallback = callback;
}

bool lly::MultilineTextField::attachWithIME()
{
	bool ret = IMEDelegate::attachWithIME();
	if (ret)
	{
		// open keyboard
		GLView * pGlView = Director::getInstance()->getOpenGLView();
		if (pGlView)
		{
			pGlView->setIMEKeyboardState(true);
		}
	}
	return ret;
}

bool lly::MultilineTextField::detachWithIME()
{
	bool ret = IMEDelegate::detachWithIME();
	if (ret)
	{
		// close keyboard
		GLView * glView = Director::getInstance()->getOpenGLView();
		if (glView)
		{
			glView->setIMEKeyboardState(false);
		}
	}
	return ret;
}

void lly::MultilineTextField::setCanAttachWithIME( bool can )
{
	m_bCanAttchIME = can;
}

void lly::MultilineTextField::setCanDetachWithIME( bool can )
{
	m_bCanDetachIME = can;
}

void lly::MultilineTextField::OpenCursor()
{
	//�������
	if (m_pCursor == nullptr)
	{
		m_pCursor=Cursor::create();
		CC_SAFE_RETAIN(m_pCursor);

		//�˴��޸ģ����ù��߶�������һ�£����������ı���
		m_pCursor->setContentSize(Size(2, m_nFontSize));

		//�趨�����ɫ������һ��
		m_pCursor->setColor(m_FontColor);
	}

	//��ʾ���
	m_pCursor->setVisible(true);

	//�����ʼ�������һ���ֵĺ������û�оͷ����ʼ
	m_nIndexTargetWord = (int)getItems().size() - 1;

	UpdateCursor();	
}

void lly::MultilineTextField::CloseCursor()
{
	if (m_pCursor != nullptr) m_pCursor->setVisible(false);
}

//=======================================
bool lly::MultilineTextField::init()
{
	bool b = TableView::init();

	//������֮����Ϊ�ֵĿ��
	setTableLayoutType(lly::TableView::ELayoutType::RELATIVE);

	return b;
}

bool lly::MultilineTextField::canAttachWithIME()
{
	return m_bCanAttchIME;
}

bool lly::MultilineTextField::canDetachWithIME()
{
	return m_bCanDetachIME;
}

void lly::MultilineTextField::didAttachWithIME()
{
	OpenCursor();
	m_bActivate = true;

	if (m_textFieldCallback) m_textFieldCallback(EventType::ATTACH_WITH_IME);
}

void lly::MultilineTextField::didDetachWithIME()
{
	CloseCursor();
	m_bActivate = false;

	if (m_textFieldCallback) m_textFieldCallback(EventType::DETACH_WITH_IME);
}

void lly::MultilineTextField::insertText( const char* text, size_t len )
{
	if (!isEnabled() || !isVisible()) return;

	//���������½�һ�����־���
// 	auto word = Label::create(text, m_strFontName, m_nFontSize,
// 		CCSizeZero, kCCTextAlignmentCenter, kCCVerticalTextAlignmentTop);
// 	m_word->setColor(m_FontColor);
// 	m_word->setAnchorPoint(ccp(0, 0.5f));
// 
// 	if (m_nIndexTargetWord + 1 >= (int)_items->count())
// 	{
// 		pushBackCustomItem(m_word);
// 	}
// 	else
// 	{
// 		insertCustomItem(m_word, m_nIndexTargetWord + 1);
// 	}
// 
// 	//�����Ŀ�����ֱ����������Ǹ�
// 	++m_nIndexTargetWord;
// 
// 	//��Ŀ�����ֲ��ܳ�����㷶Χ
// 	fTemp = _innerContainer->getPositionY() - getItem(m_nIndexTargetWord)->getPositionY() + getItem(m_nIndexTargetWord)->getContentSize().height / 2;
// 	CCLog("%f, %f, %f, %f, %f", _innerContainer->getSize().height, getSize().height, _innerContainer->getPositionY(), getItem(m_nIndexTargetWord)->getPositionY(), getItem(m_nIndexTargetWord)->getContentSize().height / 2);
// 	if (fTemp > 0)
// 	{
// 		_innerContainer->setSize(CCSizeMake(_innerContainer->getSize().width, _innerContainer->getSize().height + fTemp)); 
// 	}

	//���¹��
	UpdateCursor();
}

void lly::MultilineTextField::deleteBackward()
{

}

const std::string& lly::MultilineTextField::getContentText()
{
	return "";
}

void lly::MultilineTextField::keyboardWillShow( IMEKeyboardNotificationInfo& info )
{

}

void lly::MultilineTextField::keyboardDidShow( IMEKeyboardNotificationInfo& info )
{

}

void lly::MultilineTextField::keyboardWillHide( IMEKeyboardNotificationInfo& info )
{

}

void lly::MultilineTextField::keyboardDidHide( IMEKeyboardNotificationInfo& info )
{

}

bool lly::MultilineTextField::onTouchBegan( cocos2d::Touch *pTouch, cocos2d::Event *pEvent )
{
	bool pass = Widget::onTouchBegan(pTouch, pEvent);

	if (_hitted) //�����˾Ϳ�ʼ���룬�㲻�о�ֹͣ
	{
		//�ж��Ƿ��Ѿ�������Ǿͼ�����
		if (!m_bActivate) attachWithIME();

		Vec2 touchBeganPoint = getTouchBeganPosition();
		moveCursorByTouch(touchBeganPoint);
	} 
	else 
	{
		//�ж��Ƿ��Ѿ�����Ǿ�ֹͣ
		if (m_bActivate) detachWithIME();
	}

	return pass;
}

void lly::MultilineTextField::moveCursorByTouch( cocos2d::Vec2 &touchBeganPoint )
{
	if (m_arHeadNode.empty()) //ͷ����Ϊ�� ˵��û������������
	{
		m_nIndexTargetWord = -1;
	}
	else
	{
		//�е�λ��
		int nLineIndex; //�����

		//��һ�е�λ�ã�Ҳ���ǵ�һ�е׵�λ�ã���ΪanchorpointΪ0,0
		float fYFirstNode = m_arHeadNode.front()->getPositionY();

		//����͵�һ�е�λ�ò�
		float fTouchToFirstNode = touchBeganPoint.y - fYFirstNode;
		if (fTouchToFirstNode <= 0) 
		{
			nLineIndex = 0; //��������ڵ�һ�е׵����棬�����һ��
		}
		else
		{
			//��������о�������ڵ���������Ϊ���������룬��������1
			nLineIndex = fTouchToFirstNode / m_fLineSpacing + 1;

			//��Ŵ�����������ȥ1�Ļ����ͱ���Ϊ��������
			int nLineIndexMax = (int)m_arHeadNode.size() - 1;
			if (nLineIndex > nLineIndexMax) nLineIndex = nLineIndexMax;
		}

		//��ȡ��ͷ�Ŀؼ������
		Node* node = m_arHeadNode.at(nLineIndex);
		m_nIndexTargetWord = getIndex(node);
		
		//�����ڱ��е�λ��
		if (touchBeganPoint.x < node->getPositionX())
		{
			m_cursorInHead = true;
		}
		else
		{
			float fX = 0.0f;

			while (true)
			{
				 //�ж��ǲ����ں�һ�������ǰ��
				node = getItem(m_nIndexTargetWord + 1);

				if (!node) break;//����û�ж�����

				fX = node->getPositionX(); //��һ�������λ��
				
				//���λ���ں�һ�������ǰ�棬���������˻��з��������Ѿ�����һ����
				if (touchBeganPoint.x < fX + node->getContentSize().width / 2 || 
					isNewLineSymbol(node) || 
					fX == m_fHorizontalMargin) break;

				++m_nIndexTargetWord;
			}					
		}
	}

	UpdateCursor();
}

void lly::MultilineTextField::interceptTouchEvent( cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch )
{
	llyO_for_lly_scrollview::ScrollView::interceptTouchEvent(event, sender, touch);
}

void lly::MultilineTextField::UpdateCursor()
{
	if (m_nIndexTargetWord == -1) //��ʾ��û���κε�����
	{
		//�������ڵ�
		if (m_pCursor->getParent()) m_pCursor->removeFromParentAndCleanup(false);
		_innerContainer->CCNode::addChild(m_pCursor, 1, 1);

		m_pCursor->setPosition(Vec2(m_fHorizontalMargin, _innerContainer->getContentSize().height - m_fTopMargin));
	}
	else
	{
		//���Ŀ������
		Node* m_tempNode = getItem(m_nIndexTargetWord);

		//���������ڵ㵽Ŀ������
		if (m_pCursor->getParent()) m_pCursor->removeFromParentAndCleanup(false); 
		m_tempNode->Node::addChild(m_pCursor, 1, 1);

		if (m_cursorInHead) //����ù������ǰ��
		{
			m_cursorInHead = false;
			m_pCursor->setPosition(Vec2(0, 0));
		}
		else //������ĩβ
		{			
			m_pCursor->setPosition(Vec2(m_tempNode->getContentSize().width, 0));
		}		
	}
	m_pCursor->refreshCursor();
}



