#include "llySlider.h"

USING_NS_CC;
using namespace lly;
   
static const int BASEBAR_RENDERER_Z = (-2);
static const int PROGRESSBAR_RENDERER_Z = (-2);
static const int SLIDBALL_RENDERER_Z = (-1);
    
IMPLEMENT_CLASS_GUI_INFO(Slider)
    
Slider::Slider():
_barRenderer(nullptr),
_progressBarRenderer(nullptr),
_progressBarTextureSize(Size::ZERO),
_slidBallNormalRenderer(nullptr),
_slidBallPressedRenderer(nullptr),
_slidBallDisabledRenderer(nullptr),
_slidBallRenderer(nullptr),
_barLength(0.0),
_percent(0),
_scale9Enabled(false),
_prevIgnoreSize(true),
_textureFile(""),
_progressBarTextureFile(""),
_slidBallNormalTextureFile(""),
_slidBallPressedTextureFile(""),
_slidBallDisabledTextureFile(""),
_capInsetsBarRenderer(Rect::ZERO),
_capInsetsProgressBarRenderer(Rect::ZERO),
_sliderEventListener(nullptr),
_sliderEventSelector(nullptr),
_barTexType(TextureResType::LOCAL),
_progressBarTexType(TextureResType::LOCAL),
_ballNTexType(TextureResType::LOCAL),
_ballPTexType(TextureResType::LOCAL),
_ballDTexType(TextureResType::LOCAL),
_barRendererAdaptDirty(true),
_progressBarRendererDirty(true),
_eventCallback(nullptr),
//===========================
m_bIsTouchBall(false),
m_fBallWidthPercent(-1),
m_fNormalBallOriginalSize(Size::ZERO),
m_fPressedBallOriginalSize(Size::ZERO),
m_fDisabledBallOriginalSize(Size::ZERO)
{
    setTouchEnabled(true);
}

Slider::~Slider()
{
    _sliderEventListener = nullptr;
    _sliderEventSelector = nullptr;
}

Slider* Slider::create()
{
    Slider* widget = new Slider();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

bool Slider::init()
{
    if (Widget::init())
    {
        return true;
    }
    return false;
}

void Slider::initRenderer()
{
    _barRenderer = Sprite::create();
    _progressBarRenderer = Sprite::create();
    _progressBarRenderer->setAnchorPoint(Vec2(0.0f, 0.5f));
    addProtectedChild(_barRenderer, BASEBAR_RENDERER_Z, -1);
    addProtectedChild(_progressBarRenderer, PROGRESSBAR_RENDERER_Z, -1);
    _slidBallNormalRenderer = extension::Scale9Sprite::create(); //改变
    _slidBallPressedRenderer = extension::Scale9Sprite::create(); //改变
    _slidBallPressedRenderer->setVisible(false);
    _slidBallDisabledRenderer = extension::Scale9Sprite::create(); //改变
    _slidBallDisabledRenderer->setVisible(false);
    _slidBallRenderer = Node::create();
    _slidBallRenderer->addChild(_slidBallNormalRenderer);
    _slidBallRenderer->addChild(_slidBallPressedRenderer);
    _slidBallRenderer->addChild(_slidBallDisabledRenderer);
    addProtectedChild(_slidBallRenderer, SLIDBALL_RENDERER_Z, -1);
}

void Slider::loadBarTexture(const std::string& fileName, TextureResType texType)
{
    if (fileName.empty())
    {
        return;
    }
    _textureFile = fileName;
    _barTexType = texType;
    switch (_barTexType)
    {
        case TextureResType::LOCAL:
            if (_scale9Enabled)
            {
                static_cast<extension::Scale9Sprite*>(_barRenderer)->initWithFile(fileName);
            }
            else
            {
                static_cast<Sprite*>(_barRenderer)->setTexture(fileName);
            }
            break;
        case TextureResType::PLIST:
            if (_scale9Enabled)
            {
                static_cast<extension::Scale9Sprite*>(_barRenderer)->initWithSpriteFrameName(fileName);
            }
            else
            {
                static_cast<Sprite*>(_barRenderer)->setSpriteFrame(fileName);
            }
            break;
        default:
            break;
    }
    
    _barRendererAdaptDirty = true;
    _progressBarRendererDirty = true;
    updateContentSizeWithTextureSize(_barRenderer->getContentSize());
}

void Slider::loadProgressBarTexture(const std::string& fileName, TextureResType texType)
{
    if (fileName.empty())
    {
        return;
    }
    _progressBarTextureFile = fileName;
    _progressBarTexType = texType;
    switch (_progressBarTexType)
    {
        case TextureResType::LOCAL:
            if (_scale9Enabled)
            {
                static_cast<extension::Scale9Sprite*>(_progressBarRenderer)->initWithFile(fileName);
            }
            else
            {
                static_cast<Sprite*>(_progressBarRenderer)->setTexture(fileName);
            }
            break;
        case TextureResType::PLIST:
            if (_scale9Enabled)
            {
                static_cast<extension::Scale9Sprite*>(_progressBarRenderer)->initWithSpriteFrameName(fileName);
            }
            else
            {
                static_cast<Sprite*>(_progressBarRenderer)->setSpriteFrame(fileName);
            }
            break;
        default:
            break;
    }
    
    _progressBarRenderer->setAnchorPoint(Vec2(0.0f, 0.5f));
    _progressBarTextureSize = _progressBarRenderer->getContentSize();
    _progressBarRendererDirty = true;
}

void Slider::setScale9Enabled(bool able)
{
    if (_scale9Enabled == able)
    {
        return;
    }
    
    _scale9Enabled = able;
    removeProtectedChild(_barRenderer);
    removeProtectedChild(_progressBarRenderer);
    _barRenderer = nullptr;
    _progressBarRenderer = nullptr;
    if (_scale9Enabled)
    {
        _barRenderer = extension::Scale9Sprite::create();
        _progressBarRenderer = extension::Scale9Sprite::create();
    }
    else
    {
        _barRenderer = Sprite::create();
        _progressBarRenderer = Sprite::create();
    }
    loadBarTexture(_textureFile, _barTexType);
    loadProgressBarTexture(_progressBarTextureFile, _progressBarTexType);
    addProtectedChild(_barRenderer, BASEBAR_RENDERER_Z, -1);
    addProtectedChild(_progressBarRenderer, PROGRESSBAR_RENDERER_Z, -1);
    if (_scale9Enabled)
    {
        bool ignoreBefore = _ignoreSize;
        ignoreContentAdaptWithSize(false);
        _prevIgnoreSize = ignoreBefore;
    }
    else
    {
        ignoreContentAdaptWithSize(_prevIgnoreSize);
    }
    setCapInsetsBarRenderer(_capInsetsBarRenderer);
    setCapInsetProgressBarRebderer(_capInsetsProgressBarRenderer);
}
    
bool Slider::isScale9Enabled()const
{
    return _scale9Enabled;
}

void Slider::ignoreContentAdaptWithSize(bool ignore)
{
    if (!_scale9Enabled || (_scale9Enabled && !ignore))
    {
        Widget::ignoreContentAdaptWithSize(ignore);
        _prevIgnoreSize = ignore;
    }
}

void Slider::setCapInsets(const Rect &capInsets)
{
    setCapInsetsBarRenderer(capInsets);
    setCapInsetProgressBarRebderer(capInsets);
}

void Slider::setCapInsetsBarRenderer(const Rect &capInsets)
{
    _capInsetsBarRenderer = capInsets;
    if (!_scale9Enabled)
    {
        return;
    }
    static_cast<extension::Scale9Sprite*>(_barRenderer)->setCapInsets(capInsets);
}
    
const Rect& Slider::getCapInsetsBarRenderer()const
{
    return _capInsetsBarRenderer;
}

void Slider::setCapInsetProgressBarRebderer(const Rect &capInsets)
{
    _capInsetsProgressBarRenderer = capInsets;
    if (!_scale9Enabled)
    {
        return;
    }
    static_cast<extension::Scale9Sprite*>(_progressBarRenderer)->setCapInsets(capInsets);
}
    
const Rect& Slider::getCapInsetsProgressBarRebderer()const
{
    return _capInsetsProgressBarRenderer;
}

    void Slider::loadSlidBallTextures(const std::string& normal,const std::string& pressed,const std::string& disabled,TextureResType texType)
{
    loadSlidBallTextureNormal(normal, texType);
    loadSlidBallTexturePressed(pressed,texType);
    loadSlidBallTextureDisabled(disabled,texType);
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
    
// bool Slider::hitTest(const cocos2d::Vec2 &pt) //改，不重写hittest
// {
//     Vec2 nsp = this->_slidBallNormalRenderer->convertToNodeSpace(pt);
//     Size ballSize = this->_slidBallNormalRenderer->getContentSize();
//     Rect ballRect = Rect(0,0, ballSize.width, ballSize.height);
//     if (ballRect.containsPoint(nsp)) {
//         return true;
//     }
//     return false;
// }

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

void Slider::onTouchCancelled(Touch *touch, Event *unusedEvent)
{
    Widget::onTouchCancelled(touch, unusedEvent);
}

float Slider::getPercentWithBallPos(float px)const
{
    return ((px/_barLength)*100.0f);
}

void Slider::addEventListenerSlider(Ref *target, SEL_SlidPercentChangedEvent selector)
{
    _sliderEventListener = target;
    _sliderEventSelector = selector;
}
    
void Slider::addEventListener(const ccSliderCallback& callback)
{
    _eventCallback = callback;
}

void Slider::percentChangedEvent()
{
    if (_sliderEventListener && _sliderEventSelector)
    {
        (_sliderEventListener->*_sliderEventSelector)(this,SLIDER_PERCENTCHANGED);
    }
    if (_eventCallback) {
        _eventCallback(this, EventType::ON_PERCENTAGE_CHANGED);
    }
}

int Slider::getPercent()const
{
    return _percent;
}

void Slider::onSizeChanged()
{
    Widget::onSizeChanged();
    _barRendererAdaptDirty = true;
    _progressBarRendererDirty = true;
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

const Size& Slider::getVirtualRendererSize() const
{
    return _barRenderer->getContentSize();
}

Node* Slider::getVirtualRenderer()
{
    return _barRenderer;
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

void Slider::progressBarRendererScaleChangedWithSize()
{
    if (_ignoreSize)
    {
        if (!_scale9Enabled)
        {
            Size ptextureSize = _progressBarTextureSize;
            float pscaleX = _contentSize.width / ptextureSize.width;
            float pscaleY = _contentSize.height / ptextureSize.height;
            _progressBarRenderer->setScaleX(pscaleX);
            _progressBarRenderer->setScaleY(pscaleY);
        }
    }
    else
    {
        if (_scale9Enabled)
        {
            static_cast<extension::Scale9Sprite*>(_progressBarRenderer)->setPreferredSize(_contentSize);
            _progressBarTextureSize = _progressBarRenderer->getContentSize();
        }
        else
        {
            Size ptextureSize = _progressBarTextureSize;
            if (ptextureSize.width <= 0.0f || ptextureSize.height <= 0.0f)
            {
                _progressBarRenderer->setScale(1.0f);
                return;
            }
            float pscaleX = _contentSize.width / ptextureSize.width;
            float pscaleY = _contentSize.height / ptextureSize.height;
            _progressBarRenderer->setScaleX(pscaleX);
            _progressBarRenderer->setScaleY(pscaleY);
        }
    }
    _progressBarRenderer->setPosition(0.0f, _contentSize.height / 2.0f);
    setPercent(_percent);
}

void Slider::onPressStateChangedToNormal()
{
    _slidBallNormalRenderer->setVisible(true);
    _slidBallPressedRenderer->setVisible(false);
    _slidBallDisabledRenderer->setVisible(false);
}

void Slider::onPressStateChangedToPressed()
{
    _slidBallNormalRenderer->setVisible(false);
    _slidBallPressedRenderer->setVisible(true);
    _slidBallDisabledRenderer->setVisible(false);
}

void Slider::onPressStateChangedToDisabled()
{
    _slidBallNormalRenderer->setVisible(false);
    _slidBallPressedRenderer->setVisible(false);
    _slidBallDisabledRenderer->setVisible(true);
}

std::string Slider::getDescription() const
{
    return "lly Slider";   //改
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


