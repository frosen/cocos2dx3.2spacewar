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
		_slidBallNormalRenderer->initWithFile(normal); //��
		break;
	case TextureResType::PLIST:
		_slidBallNormalRenderer->initWithSpriteFrameName(normal); //��
		break;
	default:
		break;
	}

	//��¼ԭʼ�ߴ� //��
	m_fNormalBallOriginalSize = _slidBallNormalRenderer->getContentSize();

	barRendererScaleChangedWithSize(); //����ӣ�//�ı�
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
		_slidBallPressedRenderer->initWithFile(pressed);  //��
		break;
	case TextureResType::PLIST:
		_slidBallPressedRenderer->initWithSpriteFrameName(pressed); //��
		break;
	default:
		break;
	}

	//��¼ԭʼ�ߴ� //��
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
		_slidBallDisabledRenderer->initWithFile(disabled); //��
		break;
	case TextureResType::PLIST:
		_slidBallDisabledRenderer->initWithSpriteFrameName(disabled); //��
		break;
	default:
		break;
	}

	//��¼ԭʼ�ߴ� //��
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
		dis + m_halfRendererWidth, //�����˰������ľ��룬Ϊ�˲�������Χ //��
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
		//��
		m_ballBeginX = _slidBallRenderer->getPositionX() - m_halfRendererWidth;

		//������ס�˻����飬���¼������move��קʱ�ƶ�������ֱ���ƶ������λ��
		if (nsp.x > m_ballBeginX	&& nsp.x < m_ballBeginX + m_halfRendererWidth * 2) //���Ҷ��а���������
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
	if (m_bIsTouchBall == false) return; //��

	_touchMovePosition = touch->getLocation();
	Vec2 nsp = convertToNodeSpace(_touchMovePosition);

	float f = m_ballBeginX + nsp.x - m_touchPreX; //��

	_slidBallRenderer->setPosition( Point(f, _contentSize.height * 0.5f)); //��
	setPercent(getPercentWithBallPos(f));

	percentChangedEvent();
}

void Slider::onTouchEnded(Touch *touch, Event *unusedEvent)
{
	Widget::onTouchEnded(touch, unusedEvent);
	m_bIsTouchBall = false;  //����  //��
}

std::string Slider::getDescription() const
{
	return "lly Slider";   //��
}

void Slider::initRenderer()
{
	cocos2d::ui::Slider::initRenderer();

	_slidBallNormalRenderer = extension::Scale9Sprite::create(); //�ı�
	_slidBallPressedRenderer = extension::Scale9Sprite::create(); //�ı�
	_slidBallDisabledRenderer = extension::Scale9Sprite::create(); //�ı�
}

void Slider::barRendererScaleChangedWithSize()
{
	if (_ignoreSize)
	{

		_barRenderer->setScale(1.0f);
		if (_slidBallNormalRenderer)  //�ģ�����ӣ����ǻ���������ʹ�䲻������Χ
		{
			_barLength = _contentSize.width - _slidBallNormalRenderer->getContentSize().width; // _slidBallNormalRenderer->getScaleX();
			m_halfRendererWidth = _slidBallNormalRenderer->getContentSize().width / 2;
		}
	}
	else
	{
		if (_slidBallNormalRenderer)    //�ģ�����ӣ����ǻ���������ʹ�䲻������Χ
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

		//�����
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
		setBallWidthPercent(m_fBallWidthPercent); //���˽���������ͬʱ�û���ߴ�
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
	if (nullptr == _slidBallNormalRenderer && _slidBallNormalRenderer->getContentSize().width == 0) return; //û�л������߿��Ϊ0����������

	float lengthTemp = persent * _contentSize.width / 100.0f;
	if (lengthTemp < m_fNormalBallOriginalSize.width || persent < 0) //С������ԭ�����Ҳ�����С����
	{
		m_fBallWidthPercent = -1;

		_slidBallNormalRenderer->setContentSize(m_fNormalBallOriginalSize);

		if (_slidBallPressedRenderer)
			_slidBallPressedRenderer->setContentSize(m_fPressedBallOriginalSize);

		if (_slidBallDisabledRenderer)
			_slidBallDisabledRenderer->setContentSize(m_fDisabledBallOriginalSize);

		return;
	}

	if (persent > 100) { persent = 100; } //���ֵ

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


