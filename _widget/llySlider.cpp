#include "llySlider.h"

USING_NS_CC;
using namespace lly;
    
IMPLEMENT_CLASS_GUI_INFO(Slider)
    
Slider::Slider():
	cocos2d::ui::Slider(),
	//===========================
	m_bIsTouchBall(false),
	m_fBallWidthPercent(-1),
	m_fNormalBallOriginalSize(Size::ZERO),
	m_fPressedBallOriginalSize(Size::ZERO),
	m_fDisabledBallOriginalSize(Size::ZERO)
{
    
}

Slider::~Slider()
{
    
}

Slider* Slider::create()
{
    Slider* widget = new (std::nothrow) Slider();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

void Slider::loadSlidBallTextureNormal(const std::string& normal,TextureResType texType)
{
	if (normal.empty())
	{
		return;
	}
	_slidBallNormalTextureFile = normal;
	_ballNTexType = texType;
	switch (_ballNTexType)
	{
	case TextureResType::LOCAL:
		_slidBallNormalRenderer->initWithFile(normal); //改
		break;
	case TextureResType::PLIST:
		_slidBallNormalRenderer->initWithSpriteFrameName(normal); //改
		break;
	default:
		break;
	}

	//记录原始尺寸 //改
	m_fNormalBallOriginalSize = _slidBallNormalRenderer->getContentSize();

	barRendererScaleChangedWithSize(); //自添加，//改变
}

void Slider::loadSlidBallTexturePressed(const std::string& pressed,TextureResType texType)
{
	if (pressed.empty())
	{
		return;
	}
	_slidBallPressedTextureFile = pressed;
	_ballPTexType = texType;
	switch (_ballPTexType)
	{
	case TextureResType::LOCAL:
		_slidBallPressedRenderer->initWithFile(pressed);  //改
		break;
	case TextureResType::PLIST:
		_slidBallPressedRenderer->initWithSpriteFrameName(pressed); //改
		break;
	default:
		break;
	}

	//记录原始尺寸 //改
	m_fPressedBallOriginalSize = _slidBallPressedRenderer->getContentSize();
}

void Slider::loadSlidBallTextureDisabled(const std::string& disabled,TextureResType texType)
{
	if (disabled.empty())
	{
		return;
	}
	_slidBallDisabledTextureFile = disabled;
	_ballDTexType = texType;
	switch (_ballDTexType)
	{
	case TextureResType::LOCAL:
		_slidBallDisabledRenderer->initWithFile(disabled); //改
		break;
	case TextureResType::PLIST:
		_slidBallDisabledRenderer->initWithSpriteFrameName(disabled); //改
		break;
	default:
		break;
	}

	//记录原始尺寸 //改
	m_fDisabledBallOriginalSize = _slidBallDisabledRenderer->getContentSize();
}

void Slider::setPercent(int percent)
{
	if (percent > 100)
	{
		percent = 100;
	}
	if (percent < 0)
	{
		percent = 0;
	}
	_percent = percent;
	float res = percent / 100.0f;
	float dis = _barLength * res;
	_slidBallRenderer->setPosition(Point(
		dis + m_halfRendererWidth, //增加了半个滑块的距离，为了不超出范围 //改
		_contentSize.height / 2.0f));
	if (_scale9Enabled)
	{
		static_cast<extension::Scale9Sprite*>(_progressBarRenderer)->setPreferredSize(Size(dis,_progressBarTextureSize.height));
	}
	else
	{
		Sprite* spriteRenderer = static_cast<Sprite*>(_progressBarRenderer);
		Rect rect = spriteRenderer->getTextureRect();
		rect.size.width = _progressBarTextureSize.width * res;
		spriteRenderer->setTextureRect(rect, spriteRenderer->isTextureRectRotated(), rect.size);
	}
}

bool Slider::onTouchBegan(Touch *touch, Event *unusedEvent)
{
	bool pass = Widget::onTouchBegan(touch, unusedEvent);
	if (_hitted)
	{
		Vec2 nsp = convertToNodeSpace(_touchBeganPosition);
		//改
		m_ballBeginX = _slidBallRenderer->getPositionX() - m_halfRendererWidth;

		//如果点击住了滑动块，则记录，并在move拖拽时移动，否则直接移动到点击位置
		if (nsp.x > m_ballBeginX	&& nsp.x < m_ballBeginX + m_halfRendererWidth * 2) //左右都有半个滑块距离
		{
			m_touchPreX = nsp.x;
			m_bIsTouchBall = true;
		}
		else
		{
			setPercent(getPercentWithBallPos(nsp.x - _slidBallNormalRenderer->getContentSize().width / 2));
			percentChangedEvent();
		}
	}
	return pass;
}

void Slider::onTouchMoved(Touch *touch, Event *unusedEvent)
{
	if (m_bIsTouchBall == false) return; //改

	_touchMovePosition = touch->getLocation();
	Vec2 nsp = convertToNodeSpace(_touchMovePosition);

	float f = m_ballBeginX + nsp.x - m_touchPreX; //改

	_slidBallRenderer->setPosition( Point(f, _contentSize.height * 0.5f)); //改
	setPercent(getPercentWithBallPos(f));

	percentChangedEvent();
}

void Slider::onTouchEnded(Touch *touch, Event *unusedEvent)
{
	Widget::onTouchEnded(touch, unusedEvent);
	m_bIsTouchBall = false;  //重置  //改
}

std::string Slider::getDescription() const
{
	return "lly Slider";   //改
}

void Slider::initRenderer()
{
	cocos2d::ui::Slider::initRenderer();

	_slidBallNormalRenderer = extension::Scale9Sprite::create(); //改变
	_slidBallPressedRenderer = extension::Scale9Sprite::create(); //改变
	_slidBallDisabledRenderer = extension::Scale9Sprite::create(); //改变
}

void Slider::barRendererScaleChangedWithSize()
{
	if (_ignoreSize)
	{

		_barRenderer->setScale(1.0f);
		if (_slidBallNormalRenderer)  //改，新添加，考虑滑块的体积，使其不超出范围
		{
			_barLength = _contentSize.width - _slidBallNormalRenderer->getContentSize().width; // _slidBallNormalRenderer->getScaleX();
			m_halfRendererWidth = _slidBallNormalRenderer->getContentSize().width / 2;
		}
	}
	else
	{
		if (_slidBallNormalRenderer)    //改，新添加，考虑滑块的体积，使其不超出范围
		{
			_barLength = _contentSize.width - _slidBallNormalRenderer->getContentSize().width; // _slidBallNormalRenderer->getScaleX();
			m_halfRendererWidth = _slidBallNormalRenderer->getContentSize().width / 2;
		}

		if (_scale9Enabled)
		{
			static_cast<extension::Scale9Sprite*>(_barRenderer)->setPreferredSize(_contentSize);
		}
		else
		{
			Size btextureSize = _barRenderer->getContentSize();
			if (btextureSize.width <= 0.0f || btextureSize.height <= 0.0f)
			{
				_barRenderer->setScale(1.0f);
				return;
			}
			float bscaleX = _contentSize.width / btextureSize.width;
			float bscaleY = _contentSize.height / btextureSize.height;
			_barRenderer->setScaleX(bscaleX);
			_barRenderer->setScaleY(bscaleY);
		}
	}
	_barRenderer->setPosition(_contentSize.width / 2.0f, _contentSize.height / 2.0f);
	setPercent(_percent);
}

ui::Widget* Slider::createCloneInstance()
{
	return Slider::create();
}

void Slider::copySpecialProperties(Widget *widget)
{
	Slider* slider = dynamic_cast<Slider*>(widget);
	if (slider)
	{
		_prevIgnoreSize = slider->_prevIgnoreSize;
		setScale9Enabled(slider->_scale9Enabled);
		loadBarTexture(slider->_textureFile, slider->_barTexType);
		loadProgressBarTexture(slider->_progressBarTextureFile, slider->_progressBarTexType);
		loadSlidBallTextureNormal(slider->_slidBallNormalTextureFile, slider->_ballNTexType);
		loadSlidBallTexturePressed(slider->_slidBallPressedTextureFile, slider->_ballPTexType);
		loadSlidBallTextureDisabled(slider->_slidBallDisabledTextureFile, slider->_ballDTexType);
		setPercent(slider->getPercent());
		_sliderEventListener = slider->_sliderEventListener;
		_sliderEventSelector = slider->_sliderEventSelector;
		_eventCallback = slider->_eventCallback;

		//自添加
		m_halfRendererWidth = slider->m_halfRendererWidth;
		m_fBallWidthPercent = slider->m_fBallWidthPercent;
		m_fNormalBallOriginalSize = slider->m_fNormalBallOriginalSize;
		m_fPressedBallOriginalSize = slider->m_fPressedBallOriginalSize;
		m_fDisabledBallOriginalSize = slider->m_fDisabledBallOriginalSize;
	}
}

void Slider::adaptRenderers()
{
	if (_barRendererAdaptDirty)
	{
		barRendererScaleChangedWithSize();
		setBallWidthPercent(m_fBallWidthPercent); //改了进度条长度同时该滑块尺寸
		_barRendererAdaptDirty = false;
	}
	if (_progressBarRendererDirty)
	{
		progressBarRendererScaleChangedWithSize();
		_progressBarRendererDirty = false;
	}
}

//========================================
void Slider::setBallWidthPercent( int persent )
{
	if (nullptr == _slidBallNormalRenderer && _slidBallNormalRenderer->getContentSize().width == 0) return; //没有滑动或者宽度为0块则不能设置

	float lengthTemp = persent * _contentSize.width / 100.0f;
	if (lengthTemp < m_fNormalBallOriginalSize.width || persent < 0) //小于零则还原，并且不能缩小滑块
	{
		m_fBallWidthPercent = -1;

		_slidBallNormalRenderer->setContentSize(m_fNormalBallOriginalSize);

		if (_slidBallPressedRenderer)
			_slidBallPressedRenderer->setContentSize(m_fPressedBallOriginalSize);

		if (_slidBallDisabledRenderer)
			_slidBallDisabledRenderer->setContentSize(m_fDisabledBallOriginalSize);

		return;
	}

	if (persent > 100) { persent = 100; } //最大值

	m_fBallWidthPercent = persent;

	_slidBallNormalRenderer->setContentSize(Size(
		lengthTemp, m_fNormalBallOriginalSize.height));

	if (_slidBallPressedRenderer)
		_slidBallPressedRenderer->setContentSize(Size(
		lengthTemp, m_fPressedBallOriginalSize.height));

	if (_slidBallDisabledRenderer)
		_slidBallDisabledRenderer->setContentSize(Size(
		lengthTemp, m_fDisabledBallOriginalSize.height));

	barRendererScaleChangedWithSize();
}

void Slider::setPercentThenTriggerEvent( int percent )
{
	setPercent(percent);
	percentChangedEvent();
}


