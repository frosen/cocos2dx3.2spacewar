/*
名称：llySlider.h
内容：新的滑动条控件，复制于原slider，不用继承是因为里面有非虚函数要修改，
			特性：
            滑块不会跑出滑动区，
			windows式的拖拽滑动方式
			可以设置windows式的可变长滑块，能设置占总滑动区的比例
			增加lambda表达式的回调

制作：卢乐颜
日期：2014.5.1
*/

#ifndef _LLY_SLIDER_H_
#define _LLY_SLIDER_H_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/UIWidget.h"

namespace lly {

typedef enum
{
    SLIDER_PERCENTCHANGED
}SliderEventType;

typedef void (cocos2d::Ref::*SEL_SlidPercentChangedEvent)(cocos2d::Ref*, SliderEventType);
#define sliderpercentchangedselector(_SELECTOR) (SEL_SlidPercentChangedEvent)(&_SELECTOR)

/**
*   @js NA
*   @lua NA
*/
class Slider : public cocos2d::ui::Widget
{
    
    DECLARE_CLASS_GUI_INFO
    
public:
    enum class EventType
    {
        ON_PERCENTAGE_CHANGED
    };
    typedef std::function<void(Ref*,EventType)> ccSliderCallback;
    /**
     * Default constructor
     */
    Slider();
    
    /**
     * Default destructor
     */
    virtual ~Slider();
    
    /**
     * Allocates and initializes.
     */
    static Slider* create();
    
    /**
     * Load texture for slider bar.
     *
     * @param fileName   file name of texture.
     *
     * @param texType    @see TextureResType
     */
    void loadBarTexture(const std::string& fileName,TextureResType texType = TextureResType::LOCAL);
    
    /**
     * Sets if slider is using scale9 renderer.
     *
     * @param true that using scale9 renderer, false otherwise.
     */
    void setScale9Enabled(bool able);
    
    bool isScale9Enabled()const;
    
    /**
     * Sets capinsets for slider, if slider is using scale9 renderer.
     *
     * @param capInsets    capinsets for slider
     */
    void setCapInsets(const cocos2d::Rect &capInsets);
    
    /**
     * Sets capinsets for slider, if slider is using scale9 renderer.
     *
     * @param capInsets    capinsets for slider
     */
    void setCapInsetsBarRenderer(const cocos2d::Rect &capInsets);
    
    const cocos2d::Rect& getCapInsetsBarRenderer()const;
    
    /**
     * Sets capinsets for slider, if slider is using scale9 renderer.
     *
     * @param capInsets    capinsets for slider
     */
    void setCapInsetProgressBarRebderer(const cocos2d::Rect &capInsets);
    
    const cocos2d::Rect& getCapInsetsProgressBarRebderer()const;
    
    /**
     * Load textures for slider ball.
     *
     * @param slider ball normal    normal state texture.
     *
     * @param slider ball selected    selected state texture.
     *
     * @param slider ball disabled    dark state texture.
     *
     * @param texType    @see TextureResType
     */
    void loadSlidBallTextures(const std::string& normal,
                              const std::string& pressed,
                              const std::string& disabled,
                              TextureResType texType = TextureResType::LOCAL);
    
    /**
     * Load normal state texture for slider ball.
     *
     * @param normal    normal state texture.
     *
     * @param texType    @see TextureResType
     */
    void loadSlidBallTextureNormal(const std::string& normal, TextureResType texType = TextureResType::LOCAL);
    
    /**
     * Load selected state texture for slider ball.
     *
     * @param selected    selected state texture.
     *
     * @param texType    @see TextureResType
     */
    void loadSlidBallTexturePressed(const std::string& pressed, TextureResType texType = TextureResType::LOCAL);
    
    /**
     * Load dark state texture for slider ball.
     *
     * @param disabled    dark state texture.
     *
     * @param texType    @see TextureResType
     */
    void loadSlidBallTextureDisabled(const std::string& disabled,TextureResType texType = TextureResType::LOCAL);
    
    /**
     * Load dark state texture for slider progress bar.
     *
     * @param fileName    file path of texture.
     *
     * @param texType    @see TextureResType
     */
    void loadProgressBarTexture(const std::string& fileName, TextureResType texType = TextureResType::LOCAL);
    
    /**
     * Changes the progress direction of slider.
     *
     * @param percent    percent value from 1 to 100.
     */
    void setPercent(int percent);
    
    /**
     * Gets the progress direction of slider.
     *
     * @return percent    percent value from 1 to 100.
     */
    int getPercent()const;
    
    /**
     * Add call back function called when slider's percent has changed to slider.
     */
    CC_DEPRECATED_ATTRIBUTE void addEventListenerSlider(cocos2d::Ref* target,SEL_SlidPercentChangedEvent selector);
    void addEventListener(const ccSliderCallback& callback);
    
    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
    virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
    virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
    
    //override "getVirtualRendererSize" method of widget.
    virtual const cocos2d::Size& getVirtualRendererSize() const override;
    
    //override "getVirtualRenderer" method of widget.
    virtual cocos2d::Node* getVirtualRenderer() override;
    
    //override "ignoreContentAdaptWithSize" method of widget.
    virtual void ignoreContentAdaptWithSize(bool ignore) override;
    
    //override the widget's hitTest function to perfom its own
    //virtual bool hitTest(const cocos2d::Vec2 &pt) override;

	//=======================================
	//设置滑动块占滑动条的比例 小于等于0则恢复初始
	virtual void setBallWidthPercent(int persent);

	//改变百分数同时触发事件
	void setPercentThenTriggerEvent(int percent);

	//=======================================

    /**
     * Returns the "class name" of widget.
     */
    virtual std::string getDescription() const override;
    
CC_CONSTRUCTOR_ACCESS:
    virtual bool init() override;

protected:
    virtual void initRenderer() override;
    float getPercentWithBallPos(float location)const;
    void percentChangedEvent();
    virtual void onPressStateChangedToNormal() override;
    virtual void onPressStateChangedToPressed() override;
    virtual void onPressStateChangedToDisabled() override;
    virtual void onSizeChanged() override;
   
    void barRendererScaleChangedWithSize();
    void progressBarRendererScaleChangedWithSize();
    virtual cocos2d::ui::Widget* createCloneInstance() override;
    virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;
    virtual void adaptRenderers() override;
protected:
    cocos2d::Node*  _barRenderer;
    cocos2d::Node* _progressBarRenderer;
    cocos2d::Size _progressBarTextureSize;
    
	cocos2d::extension::Scale9Sprite* _slidBallNormalRenderer; //改变
	cocos2d::extension::Scale9Sprite* _slidBallPressedRenderer; //改变
	cocos2d::extension::Scale9Sprite* _slidBallDisabledRenderer; //改变
    cocos2d::Node* _slidBallRenderer;
    
    float _barLength;
    int _percent;
    
    bool _scale9Enabled;
    bool _prevIgnoreSize;
    std::string _textureFile;
    std::string _progressBarTextureFile;
    std::string _slidBallNormalTextureFile;
    std::string _slidBallPressedTextureFile;
    std::string _slidBallDisabledTextureFile;

    cocos2d::Rect _capInsetsBarRenderer;
    cocos2d::Rect _capInsetsProgressBarRenderer;

    cocos2d::Ref*  _sliderEventListener;
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (push)
#pragma warning (disable: 4996)
#endif
    SEL_SlidPercentChangedEvent    _sliderEventSelector;
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic warning "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (pop)
#endif
    
    ccSliderCallback  _eventCallback;
    
    TextureResType _barTexType;
    TextureResType _progressBarTexType;
    TextureResType _ballNTexType;
    TextureResType _ballPTexType;
    TextureResType _ballDTexType;
    bool _barRendererAdaptDirty;
    bool _progressBarRendererDirty;

	//====================
	float m_halfRendererWidth;
	bool m_bIsTouchBall; //是否点击住了滑动块

	float m_ballBeginX;
	float m_touchPreX; //点击的上一个X位置

	int m_fBallWidthPercent; //滑块占总长度百分比 -1为原尺寸

	//原始尺寸
	cocos2d::Size m_fNormalBallOriginalSize;
	cocos2d::Size m_fPressedBallOriginalSize;
	cocos2d::Size m_fDisabledBallOriginalSize;
};


} // namespace lly end

#endif // !_LLY_SLIDER_H_


