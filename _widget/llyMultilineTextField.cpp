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
	//还原基本设置
	setOpacity(255);
	m_timeLast = m_timeNow;
	m_bCursorOn = true;
}

bool lly::Cursor::init()
{
	if (!LayerColor::init()) return false;

	//设置光标的基本形状和颜色
	setContentSize(Size(2,40));
	setAnchorPoint(Vec2(0,0));
	setColor(Color3B::BLACK); //颜色为黑色

	//设置初始值
	setOpacity(0);
	m_timeLast.tv_sec = 0;
	m_timeLast.tv_usec = 0;

	m_bCursorOn=false;

	return true;
}

void lly::Cursor::visit( cocos2d::Renderer* renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags )
{
	gettimeofday(&m_timeNow, nullptr);

	//时间超过间隔就不停的出现和消失
	if (m_timeNow.tv_usec - m_timeLast.tv_usec > CURSOR_FLASH_INTERVAL)
	{
		if (!m_bCursorOn)
		{
			//可见黑色光标
			setOpacity(255);
			m_timeLast = m_timeNow;
			m_bCursorOn = true;
		}
		else
		{
			//隐藏光标
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
	//创建光标
	if (m_pCursor == nullptr)
	{
		m_pCursor=Cursor::create();
		CC_SAFE_RETAIN(m_pCursor);

		//此处修改，设置光标高度与文字一致，而不是与文本框
		m_pCursor->setContentSize(Size(2, m_nFontSize));

		//设定光标颜色与文字一致
		m_pCursor->setColor(m_FontColor);
	}

	//显示光标
	m_pCursor->setVisible(true);

	//光标起始放在最后一个字的后面如果没有就放在最开始
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

	//字与字之间宽度为字的宽度
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

	//根据文字新建一个文字精灵
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
// 	//输入后目标文字变成新输入的那个
// 	++m_nIndexTargetWord;
// 
// 	//让目标文字不能超出外层范围
// 	fTemp = _innerContainer->getPositionY() - getItem(m_nIndexTargetWord)->getPositionY() + getItem(m_nIndexTargetWord)->getContentSize().height / 2;
// 	CCLog("%f, %f, %f, %f, %f", _innerContainer->getSize().height, getSize().height, _innerContainer->getPositionY(), getItem(m_nIndexTargetWord)->getPositionY(), getItem(m_nIndexTargetWord)->getContentSize().height / 2);
// 	if (fTemp > 0)
// 	{
// 		_innerContainer->setSize(CCSizeMake(_innerContainer->getSize().width, _innerContainer->getSize().height + fTemp)); 
// 	}

	//更新光标
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

	if (_hitted) //点中了就开始输入，点不中就停止
	{
		//判断是否已经激活，不是就激活它
		if (!m_bActivate) attachWithIME();

		Vec2 touchBeganPoint = getTouchBeganPosition();
		moveCursorByTouch(touchBeganPoint);
	} 
	else 
	{
		//判断是否已经激活，是就停止
		if (m_bActivate) detachWithIME();
	}

	return pass;
}

void lly::MultilineTextField::moveCursorByTouch( cocos2d::Vec2 &touchBeganPoint )
{
	if (m_arHeadNode.empty()) //头行组为空 说明没有文字在里面
	{
		m_nIndexTargetWord = -1;
	}
	else
	{
		//行的位置
		int nLineIndex; //行序号

		//第一行的位置，也就是第一行底的位置，因为anchorpoint为0,0
		float fYFirstNode = m_arHeadNode.front()->getPositionY();

		//触点和第一行的位置差
		float fTouchToFirstNode = touchBeganPoint.y - fYFirstNode;
		if (fTouchToFirstNode <= 0) 
		{
			nLineIndex = 0; //如果触点在第一行底的上面，就算第一行
		}
		else
		{
			//距离除以行距就是所在的行数，因为会四舍五入，所以最后加1
			nLineIndex = fTouchToFirstNode / m_fLineSpacing + 1;

			//序号大于总数量减去1的话，就保持为最大行序号
			int nLineIndexMax = (int)m_arHeadNode.size() - 1;
			if (nLineIndex > nLineIndexMax) nLineIndex = nLineIndexMax;
		}

		//获取行头的控件的序号
		Node* node = m_arHeadNode.at(nLineIndex);
		m_nIndexTargetWord = getIndex(node);
		
		//文字在本行的位置
		if (touchBeganPoint.x < node->getPositionX())
		{
			m_cursorInHead = true;
		}
		else
		{
			float fX = 0.0f;

			while (true)
			{
				 //判断是不是在后一个对象的前面
				node = getItem(m_nIndexTargetWord + 1);

				if (!node) break;//后面没有对象了

				fX = node->getPositionX(); //后一个对象的位置
				
				//如果位置在后一个对象的前面，或者遇到了换行符，或者已经到下一行了
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
	if (m_nIndexTargetWord == -1) //表示还没有任何的文字
	{
		//更换父节点
		if (m_pCursor->getParent()) m_pCursor->removeFromParentAndCleanup(false);
		_innerContainer->CCNode::addChild(m_pCursor, 1, 1);

		m_pCursor->setPosition(Vec2(m_fHorizontalMargin, _innerContainer->getContentSize().height - m_fTopMargin));
	}
	else
	{
		//获得目标文字
		Node* m_tempNode = getItem(m_nIndexTargetWord);

		//光标更换父节点到目标文字
		if (m_pCursor->getParent()) m_pCursor->removeFromParentAndCleanup(false); 
		m_tempNode->Node::addChild(m_pCursor, 1, 1);

		if (m_cursorInHead) //如果让光标在最前边
		{
			m_cursorInHead = false;
			m_pCursor->setPosition(Vec2(0, 0));
		}
		else //否则在末尾
		{			
			m_pCursor->setPosition(Vec2(m_tempNode->getContentSize().width, 0));
		}		
	}
	m_pCursor->refreshCursor();
}



