/*
���ƣ�llyListView.h
���ݣ��µ�ListView
			llyO�е�listview��ԭ��һ�£�������Щ��������麯��Ϊ�˼̳У�����
			pushBackDefaultItem to removeAllItems
			updateInnerContainerSize
			remedyLayoutParameter
			��Ӷ���
			��Ӷ�Ŀ��ؼ��Ĳ���
			����ƶ��ؼ��ĺ���
			��Ӳ����ؼ���Ļص�

������¬����
���ڣ�2014.9.1
*/

#ifndef _LLY_LIST_VIEW_H__  
#define _LLY_LIST_VIEW_H__

#include "llyScrollView.h"  //�޸�
#include "cocos2d.h"  //�޸�

namespace llyO_for_lly_listview{

typedef enum
{
    LISTVIEW_ONSELECTEDITEM_START,
    LISTVIEW_ONSELECTEDITEM_END
}ListViewEventType;

typedef void (cocos2d::Ref::*SEL_ListViewEvent)(cocos2d::Ref*,ListViewEventType);
#define listvieweventselector(_SELECTOR) (SEL_ListViewEvent)(&_SELECTOR)

class ListView : public lly::ScrollView
{
 
    DECLARE_CLASS_GUI_INFO
    
public:
    enum class Gravity
    {
        LEFT,
        RIGHT,
        CENTER_HORIZONTAL,
        TOP,
        BOTTOM,
        CENTER_VERTICAL
    };
    
    enum class EventType
    {
        ON_SELECTED_ITEM_START,
        ON_SELECTED_ITEM_END
    };
    
    typedef std::function<void(cocos2d::Ref*, EventType)> ccListViewCallback;
    
    /**
     * Default constructor
     */
    ListView();
    
    /**
     * Default destructor
     */
    virtual ~ListView();
    
    /**
     * Allocates and initializes.
     */
    static ListView* create();
    
    /**
     * Sets a item model for listview
     *
     * A model will be cloned for adding default item.
     *
     * @param model  item model for listview
     */
    void setItemModel(cocos2d::ui::Widget* model);
    
    /**
     * Push back a default item(create by a cloned model) into listview.
     */
    virtual void pushBackDefaultItem();
    
    /**
     * Insert a default item(create by a cloned model) into listview.
     */
    virtual void insertDefaultItem(ssize_t index);
    
    /**
     * Push back custom item into listview.
     */
    virtual void pushBackCustomItem(cocos2d::ui::Widget* item);
    
    /**
     * Insert custom item into listview.
     */
    virtual void insertCustomItem(cocos2d::ui::Widget* item, ssize_t index);
    
    /**
     *  Removes the last item of listview.
     */
    virtual void removeLastItem();
    
    /**
     * Removes a item whose index is same as the parameter.
     *
     * @param index of item.
     */
    virtual void removeItem(ssize_t index);
    
    virtual void removeAllItems();
    
    /**
     * Returns a item whose index is same as the parameter.
     *
     * @param index of item.
     *
     * @return the item widget.
     */
    cocos2d::ui::Widget* getItem(ssize_t index)const;
    
    /**
     * Returns the item container.
     */
    cocos2d::Vector<cocos2d::ui::Widget*>& getItems();
    
    /**
     * Returns the index of item.
     *
     * @param item  the item which need to be checked.
     *
     * @return the index of item.
     */
    ssize_t getIndex(cocos2d::ui::Widget* item) const;
    
    /**
     * Changes the gravity of listview.
     * @see ListViewGravity
     */
    void setGravity(Gravity gravity);
    
    /**
     * Changes the margin between each item.
     *
     * @param margin
     */
    void setItemsMargin(float margin);
    
    float getItemsMargin()const;
    
    virtual void doLayout() override;
    
    virtual void addChild(cocos2d::Node* child)override;
    virtual void addChild(cocos2d::Node * child, int localZOrder)override;
    virtual void addChild(cocos2d::Node* child, int zOrder, int tag) override;
    virtual void addChild(cocos2d::Node* child, int zOrder, const std::string &name) override;
    virtual void removeAllChildren() override;
    virtual void removeAllChildrenWithCleanup(bool cleanup) override;
	virtual void removeChild(cocos2d::Node* child, bool cleaup = true) override;
    
    ssize_t getCurSelectedIndex() const;
    
    CC_DEPRECATED_ATTRIBUTE void addEventListenerListView(cocos2d::Ref* target, SEL_ListViewEvent selector);
    void addEventListener(const ccListViewCallback& callback);
    using lly::ScrollView::addEventListener;

    /**
     * Changes scroll direction of scrollview.
     *
     * @see Direction Direction::VERTICAL means vertical scroll, Direction::HORIZONTAL means horizontal scroll
     *
     * @param dir, set the list view's scroll direction
     */
    virtual void setDirection(Direction dir) override;
    
    virtual std::string getDescription() const override;
    
    void requestRefreshView();
    virtual void refreshView();

CC_CONSTRUCTOR_ACCESS:
    virtual bool init() override;
    
protected:    
    virtual void updateInnerContainerSize(); //�޸�
    virtual void remedyLayoutParameter(cocos2d::ui::Widget* item); //�޸�
    virtual void onSizeChanged() override;
    virtual cocos2d::ui::Widget* createCloneInstance() override;
    virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;
    virtual void copyClonedWidgetChildren(cocos2d::ui::Widget* model) override;
    void selectedItemEvent(TouchEventType event);
    virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch) override;
protected:
    cocos2d::ui::Widget* _model;
    
    cocos2d::Vector<cocos2d::ui::Widget*> _items;
    
    Gravity _gravity;
    
    float _itemsMargin;
    
    ssize_t _curSelectedIndex;
    bool _refreshViewDirty;
    
    cocos2d::Ref* _listViewEventListener;
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (push)
#pragma warning (disable: 4996)
#endif
    SEL_ListViewEvent    _listViewEventSelector;
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic warning "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (pop)
#endif
    ccListViewCallback _eventCallback;
};

} //llyO_for_lly_listview


//��listview

namespace lly{

class ListView : llyO_for_lly_listview::ListView
{
	DECLARE_CLASS_GUI_INFO

public:
	typedef enum //�ؼ��ĸı䷽ʽ
	{
		LISTVIEW_ADD_ITEM, //����
		LISTVIEW_REMOVE_ITEM, //ɾ��
		LISTVIEW_MOVE_ITEM, //�ƶ�
	}ListViewchangeItem;

	typedef enum //�ƶ��ؼ��ķ�ʽ
	{
		MOVE_BY, //�ƶ�ĳ������
		MOVE_TO, //�ƶ�������
	}WidgetMoveType;

	typedef enum //�ƶ��ؼ��ķ�ʽ
	{
		MOVE_FORWARD, //��ǰ�ƶ�
		MOVE_BACKWARD, //����ƶ�
	}WidgetMoveDirection;

	typedef std::function<void(Ref*, ListViewchangeItem t)> changeItemCallback;

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
	void moveItem(cocos2d::ui::Widget* item, WidgetMoveDirection dir, WidgetMoveType t = MOVE_BY, int num = 1);

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
			_direction == llyO::ScrollView::Direction::VERTICAL ? item->getContentSize().height : 
			_direction == llyO::ScrollView::Direction::VERTICAL ? item->getContentSize().width : 0); 
	}

	//��ɾ�ƶ��ӿؼ�ʱ����� Ĭ�Ͽ�
	void changeWidgetEvent(ListViewchangeItem type);

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


