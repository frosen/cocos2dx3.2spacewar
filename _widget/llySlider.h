/*
名称：llySlider.h
内容：新的滑动条控件，父类的如下函数要加virtual
				loadSlidBallTextureNormal
				loadSlidBallTexturePressed
				loadSlidBallTextureDisabled
				setPercent
				barRendererScaleChangedWithSize
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
#include "ui/UISlider.h"
#include "extensions/GUI/CCControlExtension/CCScale9Sprite.h"

namespace lly {

class Slider : public cocos2d::ui::Slider
{
	DECLARE_CLASS_GUI_INFO

public:
	Slider();

	virtual ~Slider();

	static Slider* create();

	virtual void loadSlidBallTextureNormal(const std::string& normal, TextureResType texType = TextureResType::LOCAL);  //父类此函数要虚拟化

	virtual void loadSlidBallTexturePressed(const std::string& pressed, TextureResType texType = TextureResType::LOCAL);  //父类此函数要虚拟化

	virtual void loadSlidBallTextureDisabled(const std::string& disabled,TextureResType texType = TextureResType::LOCAL);  //父类此函数要虚拟化

	virtual void setPercent(int percent); //父类此函数要虚拟化

	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
	virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
	virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;

	//=======================================
	//设置滑动块占滑动条的比例 小于等于0则恢复初始
	virtual void setBallWidthPercent(int persent);

	//改变百分数同时触发事件
	void setPercentThenTriggerEvent(int percent);

	//=======================================

	virtual std::string getDescription() const override;

protected:
	virtual void initRenderer() override;

	virtual void barRendererScaleChangedWithSize(); //父类此函数要虚拟化

	virtual cocos2d::ui::Widget* createCloneInstance() override;
	virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;

	virtual void adaptRenderers() override;

protected:
	cocos2d::extension::Scale9Sprite* _slidBallNormalRenderer; //改变
	cocos2d::extension::Scale9Sprite* _slidBallPressedRenderer; //改变
	cocos2d::extension::Scale9Sprite* _slidBallDisabledRenderer; //改变

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


} //lly

#endif // !_LLY_SLIDER_H_


