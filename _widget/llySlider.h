/*
���ƣ�llySlider.h
���ݣ��µĻ������ؼ�����������º���Ҫ��virtual
				loadSlidBallTextureNormal
				loadSlidBallTexturePressed
				loadSlidBallTextureDisabled
				setPercent
				barRendererScaleChangedWithSize
			���ԣ�
            ���鲻���ܳ���������
			windowsʽ����ק������ʽ
			��������windowsʽ�Ŀɱ䳤���飬������ռ�ܻ������ı���
			����lambda���ʽ�Ļص�

������¬����
���ڣ�2014.5.1
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

	virtual void loadSlidBallTextureNormal(const std::string& normal, TextureResType texType = TextureResType::LOCAL);  //����˺���Ҫ���⻯

	virtual void loadSlidBallTexturePressed(const std::string& pressed, TextureResType texType = TextureResType::LOCAL);  //����˺���Ҫ���⻯

	virtual void loadSlidBallTextureDisabled(const std::string& disabled,TextureResType texType = TextureResType::LOCAL);  //����˺���Ҫ���⻯

	virtual void setPercent(int percent); //����˺���Ҫ���⻯

	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
	virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
	virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;

	//=======================================
	//���û�����ռ�������ı��� С�ڵ���0��ָ���ʼ
	virtual void setBallWidthPercent(int persent);

	//�ı�ٷ���ͬʱ�����¼�
	void setPercentThenTriggerEvent(int percent);

	//=======================================

	virtual std::string getDescription() const override;

protected:
	virtual void initRenderer() override;

	virtual void barRendererScaleChangedWithSize(); //����˺���Ҫ���⻯

	virtual cocos2d::ui::Widget* createCloneInstance() override;
	virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;

	virtual void adaptRenderers() override;

protected:
	cocos2d::extension::Scale9Sprite* _slidBallNormalRenderer; //�ı�
	cocos2d::extension::Scale9Sprite* _slidBallPressedRenderer; //�ı�
	cocos2d::extension::Scale9Sprite* _slidBallDisabledRenderer; //�ı�

	float m_halfRendererWidth;
	bool m_bIsTouchBall; //�Ƿ���ס�˻�����

	float m_ballBeginX;
	float m_touchPreX; //�������һ��Xλ��

	int m_fBallWidthPercent; //����ռ�ܳ��Ȱٷֱ� -1Ϊԭ�ߴ�

	//ԭʼ�ߴ�
	cocos2d::Size m_fNormalBallOriginalSize;
	cocos2d::Size m_fPressedBallOriginalSize;
	cocos2d::Size m_fDisabledBallOriginalSize;
};


} //lly

#endif // !_LLY_SLIDER_H_


