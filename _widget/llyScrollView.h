/*
���ƣ�llyScrollView.h
���ݣ��µĻ����б�ؼ���

			!!!!!< ԭʼ�����޸�һЩ������Ϊ�麯����setInnerContainerSize, jumpToDestination >!!!!!!

			���ԣ�
            ���ڲ㷶Χ�������ʱ�����˻ص������������ڴ�ʱ��ʾ��������
			���϶��ڲ�ʱ�����˻ص������������ڸ��Ļ�����
			��ֱ�ӹ�������ͺ���Ļ�����
			�ɲ����Ӳ�����ק�ƶ���windows��
			����������Ŀ��ؼ�
			���ڷ���̳�

������¬����
���ڣ�2014.5.1
*/

#ifndef _LLY_SCROLL_VIEW_H_
#define _LLY_SCROLL_VIEW_H_

#include "ui/UIScrollView.h"

namespace lly {

class Slider;

class ScrollView : public cocos2d::ui::ScrollView
{
	DECLARE_CLASS_GUI_INFO

public:
	enum class ChangeInnerSizeType//��ӣ����ڱ���ڲ��Сʱ�Ļص�
	{
		NO_CHANGE,
		INNER_TURN_LONGER_THAN_OUTER,
		INNER_TURN_NOT_LONGER_THAN_OUTER,
	};

	typedef std::function<void(cocos2d::Ref* ob, int x, int y)> ScrollViewCallback; 
	typedef std::function<void(cocos2d::Ref* ob, ChangeInnerSizeType wT, ChangeInnerSizeType hT, int w, int h)> ChangeInnerCallback; 

	ScrollView();
    virtual ~ScrollView();

    static ScrollView* create();

	virtual void setInnerContainerSize(const cocos2d::Size &size) override;

	//===========================================
	//�����¼������ƻ�����������Ϊ�ؼ����󣬵���X��%������y��%��-1Ϊ�ڲ㲻�������
	//x�������0��y���ϱ���0
	void addScrollViewEventListener(const ScrollViewCallback &cb) { m_addScrollViewEventFunc = cb; }

	//����ڲ�仯���¼���type��ʾ�Ƿ�ͻȻ�ڲ�����㣬��ͻȻ�����ˣ���ζ���Ƿ���ʾ��������w �� t ��СΪ0 ���100��Ϊ�����ڲ�ĳߴ����	
	void addChangeInnerEventListener(const ChangeInnerCallback &cb) { m_addChangeInnerEventFunc = cb; }

	//��ȡ�ڲ��Ƿ�������
	bool isInnerWider() { return isInnerWidthThanOuter; }
	bool isInnerHigher() { return isInnerHeightThanOuter; }

	//���ù����Ļ�������boolΪtrue��ʾ����Ҫ��������ʱ���������
	void setLinkedSlider(lly::Slider* heightSlider, lly::Slider* widthSlider = nullptr, bool canHideSlider = true);

	//���úͻ�ȡ�Ƿ�����ƶ��Ӳ㣨Ĭ�Ͽ��ԣ�
	void setInnerCanMove(bool b) { _isInnerCanMove = b; }
	bool isInnerCanMove() { return _isInnerCanMove; }

	//���ú͵õ�Ŀ��ؼ�
	void setTargetItem(cocos2d::Node* target) { m_targetItem = target; }
	cocos2d::Node* getTargetItem() { return m_targetItem; }

	//===========================================
    /**
     * Returns the "class name" of widget.
     */
    virtual std::string getDescription() const override;

protected:
	virtual void onSizeChanged() override;

	virtual void jumpToDestination(const cocos2d::Vec2& des); //����̳�

	virtual bool scrollChildren(float touchOffsetX, float touchOffsetY) override;

	virtual void endRecordSlidAction() override;

	virtual void handleMoveLogic(cocos2d::Touch *touch) override;

	//==============================
	//��¼�ٷֱȺͽ��лص�
	void doAfterChangeInnerContainerPosition(bool runFunction = true);

	//���ɾ���ؼ���ʱ�򣬿��ܻ�����ڲ�ؼ���С�ı䣬��ʱ���±��������ص�һ������
	void changeInnerContainerSizeEvent();

	//===============================
	virtual cocos2d::ui::Widget* createCloneInstance() override;
	virtual void copySpecialProperties(Widget* model) override;

protected:
	//==========================
	ScrollViewCallback m_addScrollViewEventFunc;
	ChangeInnerCallback m_addChangeInnerEventFunc;

	//�ڲ����������İٷֱ�λ��
	int m_innerContainerPositionXPercent; //�����0 Ĭ��-1
	int m_innerContainerPositionYPercent; //�ϱ���0 Ĭ��-1

	bool isInnerWidthThanOuter; //Ĭ��Ϊfalse�����sizeͬʱ���
	bool isInnerHeightThanOuter; //Ĭ��Ϊfalse�����sizeͬʱ���

	lly::Slider* m_heightSlider; //��ػ�����
	lly::Slider* m_widthSlider;

	bool _isInnerCanMove; //�Ƿ������ק�ƶ��Ӳ㣬Ĭ��Ϊtrue

	cocos2d::Node* m_targetItem;
};

}

#endif