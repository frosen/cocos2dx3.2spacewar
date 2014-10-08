/*
���ƣ�llyTableView.h
���ݣ����ؼ����̳���listview��ֻ������ʽ��listview��ͬ

������¬����
���ڣ�2014.5.1
*/

#ifndef _LLY_TABLE_VIEW_H_
#define _LLY_TABLE_VIEW_H_

#include "llyListView.h"

namespace lly{

class TableView : llyO::ListView
{
public:
	typedef enum  //���ַ�ʽ�����Բ��ֱȽϺ�ƽ��ֱ����Բ����������ֵ�
	{
		ABSOLUTE,
		RELATIVE,
	}ELayoutType;

	typedef enum  //ͣ������
	{
		LEFT_TOP,
		RIGHT_TOP,
		LEFT_BOTTOM,
		RIGHT_BOTTOM,
	}ETableViewGravity;

	typedef enum //��д����
	{
		HORIZONTAL, //�Ⱥ����ţ���ͷ����
		VERTICAL,	//�������ţ���ͷ����
	}EArrangementDirection;

	TableView();
	virtual ~TableView();

	static TableView* create();

	//���ò��ַ�ʽ��Ĭ�Ͼ��Բ��֣�
	void setTableLayoutType(ELayoutType Ltype);

	//����ͣ������
	void setTableViewGravity(ETableViewGravity TVG);

	virtual void setGravity(Gravity gravity) override; //����

	//������д����Ĭ�ϴ����ң�
	void setWriteOrder(EWriteOrder wo);

	//�����Ű淽��
	void setArrangementDirection(EArrangementDirection dir);

	virtual void setDirection(Direction dir) override; //����

	//���ÿؼ���ࣨ���Բ��־��ǻ��㵽���㣬������ǿؼ�֮��ľ��룩��Ĭ��Ϊ20��
	void setWidgetSpacing(float fSpacing);

	//�����м�� ���㵽���㣨Ĭ��Ϊ20��
	void setLineSpacing(float fLineSpacing);

	//���õ����ұ߶��ľ��루Ĭ��Ϊ30��
	void setHorizontalMargin(float fMargin);

	//���õ��ϻ��±߶��ľ��루Ĭ��Ϊ30��
	void setVerticalMargin(float fMargin);

	//�����==================
	cocos2d::ui::Widget* getLastItem(); 
	
	//==========================

	virtual void refreshView() override;

CC_CONSTRUCTOR_ACCESS:
	virtual bool init() override;

protected:
	virtual void updateInnerContainerSize() override;
	virtual void remedyLayoutParameter(cocos2d::ui::Widget* item) override;

	virtual cocos2d::ui::Widget* createCloneInstance() override;
	virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;

protected:
	cocos2d::Vector<cocos2d::Node*> m_arHeadNode; //ÿ��ͷ��widget
	
	ELayoutType m_eLayoutType;
	ETableViewGravity m_eTVG;
	EArrangementDirection m_eArrangeDir;

	float m_fWidgetSpacing; //Ĭ��20
	float m_fLineSpacing; //Ĭ��20

	float m_fHorizontalMargin; //Ĭ��30
	float m_fVerticalMargin; //Ĭ��30

	cocos2d::ui::Widget* lastItem; //refresh�У�Ϊ���Ч����
	bool bneedcheckindex; //refresh�У�Ϊ���Ч����
};

} //lly

#endif // _LLY_TABLE_VIEW_H_




