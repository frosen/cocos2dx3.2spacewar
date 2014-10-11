/*
���ƣ�llyCheckBox.h
���ݣ��¸�ѡ��ؼ����̳���ԭcheckbox��
            ������
            ����ѡ����Ӧ������ʱ���������ʼʱ��windows�򣩻��߽���ʱ�򣨴�������
            ���Լ��븴ѡ��������У�ʵ�ֵ�ѡ�������ѡ���
            �����ⲿ����״̬�ķ���
			����lambda���ʽ�Ļص�

������¬����
���ڣ�2014.5.1
*/

#ifndef _LLY_CHECK_BOX_H_
#define _LLY_CHECK_BOX_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

namespace lly{

enum CHECKBOX_ACT_TYPE
{
    WHEN_TOUCH_BEGAN,
    WHEN_TOUCH_ENDED,
};

class CheckBox : public cocos2d::ui::CheckBox
{
	DECLARE_CLASS_GUI_INFO

public:
	CheckBox();
	virtual ~CheckBox();

	static CheckBox* create();
    
	static CheckBox* create(const std::string& backGround,
                            const std::string& backGroundSeleted,
                            const std::string& cross,
                            const std::string& backGroundDisabled,
                            const std::string& frontCrossDisabled,
                           TextureResType texType = TextureResType::LOCAL);

    //��õ�ѡ������
    cocos2d::Ref* getManager() {return m_CKBmgr;}

	//������Ӧʱ��
    void setActType(CHECKBOX_ACT_TYPE t) { m_actType = t; }

    //��ʵ���ⲿ�ı临ѡ��״̬
    void turnToSelectedState(); //���ѡ��״̬
    void turnToUnselectedState(); //��ɷ�ѡ��״̬

    //���ش������ƣ�����ѡ����Ӧ������ʱ�������ʱ����Ӧ�����ߵ���ʱ����Ӧ
    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unusedEvent);
	virtual void releaseUpEvent();

	virtual std::string getDescription() { return "lly CheckBox";  }

	void setCheckBoxManager(cocos2d::Ref* mgr, std::function <void(lly::CheckBox*, bool)> func);
protected:
	virtual Widget* createCloneInstance() override;
	virtual void copySpecialProperties(Widget* model) override;

protected:
    cocos2d::Ref* m_CKBmgr;
    CHECKBOX_ACT_TYPE m_actType;

	//����й���������������ѵ���¼������������
	std::function <void(lly::CheckBox*, bool)> excuteManagerEvent;
};

} // namespace lly

#endif //_LLY_CHECK_BOX_H_


