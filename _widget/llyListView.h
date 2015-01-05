/*
���ƣ�llyListView.h
���ݣ��µ�ListView

			!!!!< �丸������Щ����Ҫ����麯����Ϊ�˼̳У�����
			pushBackDefaultItem to removeAllItems
			updateInnerContainerSize
			remedyLayoutParameter 

			�丸��Ҫ�̳���lly::Scrollview
			>!!!!

			��Ӷ���
			��Ӷ�Ŀ��ؼ��Ĳ���
			����ƶ��ؼ��ĺ���
			��Ӳ����ؼ���Ļص�

������¬����
���ڣ�2014.9.1
*/

#ifndef _LLY_LIST_VIEW_H__  
#define _LLY_LIST_VIEW_H__

#include "ui/UIListView.h"

//��listview

namespace lly{

class ListView : cocos2d::ui::ListView
{
	DECLARE_CLASS_GUI_INFO

public:
	enum class changeItemType//�ؼ��ĸı䷽ʽ
	{
		ADD_ITEM, //����
		REMOVE_ITEM, //ɾ��
		MOVE_ITEM, //�ƶ�
	};

	enum class WidgetMoveType//�ƶ��ؼ��ķ�ʽ
	{
		MOVE_BY, //�ƶ�ĳ������
		MOVE_TO, //�ƶ�������
	};

	enum class WidgetMoveDirection//�ƶ��ؼ��ķ�ʽ
	{
		MOVE_FORWARD, //��ǰ�ƶ�
		MOVE_BACKWARD, //����ƶ�
	};

	typedef std::function<void(cocos2d::Ref*, changeItemType t)> changeItemCallback;

	ListView();
	virtual ~ListView();

	static ListView* create();

	virtual void pushBackDefaultItem(); //�����virtual
    virtual void insertDefaultItem(ssize_t index); //�����virtual
    virtual void pushBackCustomItem(Widget* item); //�����virtual
    virtual void insertCustomItem(Widget* item, ssize_t index); //�����virtual

	virtual void removeItem(ssize_t index); //�����virtual
	virtual void removeLastItem(); //�����virtual
    virtual void removeAllItems(); //�����virtual

	virtual void doLayout() override;

	virtual std::string getDescription() const override { return "lly ListView"; }

	//�����public=================================================

	//ֱ��ͨ������ɾ���ؼ����Ż�ֻ��ͨ��������ɾ���ķ�ʽ������Ҳ������
	void removeItem(cocos2d::ui::Widget* item);

	//������pushbackdefault���ʹ�ã���ȡ���һ���ؼ�
	cocos2d::ui::Widget* getLastItem() { return static_cast<cocos2d::ui::Widget*>(_items.back()); }

	//�Ƿ���������Ĭ�Ϲرգ�=========================================
	void setAnimationEnabled(bool b) { m_bAnimation = b; }
	bool isAnimationEnabled() { return m_bAnimation; }

	//���ö������ٶȣ�Ĭ��ÿ֡1�����أ�
	void setWidgetSpeedWhenMove(float len) { m_fWidgetMoveSpeed = len; }

	//�ı�ؼ�ʱ�Ļص�
	void addChangeItemEventListener(changeItemCallback call) { addChangeItemEventFunc = call; }

	//���ÿؼ�������������
	void setMarginToTop(float fMargin) { m_fMarginToTop = fMargin; }

	//�ƶ��ؼ�λ�ã������ֱ�ΪҪ�ƶ��Ŀؼ����ƶ���ʽ�����ƶ����ٸ��������ƶ���ĳλ�ã��ƶ���λ��λ�ã��������������޶����ƶ������λ��
	void moveItem(
		cocos2d::ui::Widget* item, 
		WidgetMoveDirection dir, 
		WidgetMoveType t = WidgetMoveType::MOVE_BY, 
		int num = 1);

	//��ȡ��ǰ�Ƿ��ڱ��
	bool isRefreshing() { return _refreshViewDirty; }	

	//��������ƶ����룬�������벻��Ӧ���أ�touch end��
	void setListMaxMovePoint(const cocos2d::Vec2 &pos) { m_posMaxMove = pos; }
	bool isMoveList() { return _bMoved; } //�ƶ����б����ڴ����ص��н�ֹ����
	//========================================

protected:
	virtual void updateInnerContainerSize(); //�����virtual
	virtual void remedyLayoutParameter(cocos2d::ui::Widget* item); //�����virtual

	virtual cocos2d::ui::Widget* createCloneInstance() override;
	virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;
	virtual void copyClonedWidgetChildren(cocos2d::ui::Widget* model) override;
	virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch) override; //��Ӧ���к��ƶ�

	//����ʱ��ȡ��ʼ����=======================
	float getDistance(cocos2d::ui::Widget* item) 
	{ 
		return _itemsMargin + (
			_direction == cocos2d::ui::ScrollView::Direction::VERTICAL ? item->getContentSize().height : 
			_direction == cocos2d::ui::ScrollView::Direction::VERTICAL ? item->getContentSize().width : 0); 
	}

	//��ɾ�ƶ��ӿؼ�ʱ����� Ĭ�Ͽ�
	void changeWidgetEvent(changeItemType type);

	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, bool parentTransformUpdated);

	//================================

protected:
	//==========================
	bool m_bAnimation; //�Ƿ���������Ĭ�ϲ�������
	float m_fWidgetMoveSpeed; //�ؼ������ƶ����ٶȣ�Ĭ��ÿ֡1���أ�
	float m_fMarginToTop; //��һ���ؼ������������ľ���
	changeItemCallback addChangeItemEventFunc;
	cocos2d::Vec2 m_posMaxMove; //����ƶ����룬����������벻��Ӧ����

	//temp
	cocos2d::ui::Widget* m_itemMove; //��������Ҫ�ƶ����Ǹ��ؼ�
	float m_fDistance; //�����пؼ��ƶ��ľ���
	bool m_bChangeZOrder; //���½���ʱ��visit�������Ŀ��ؼ���z�ᵽ���ϲ�
	bool _bMoved; //�Ƿ��ƶ����б�
	cocos2d::Vec2 m_posBegintouch; 
};

} //lly

#endif /* defined(__ListView__) */


