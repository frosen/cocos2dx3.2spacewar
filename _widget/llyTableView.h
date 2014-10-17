/*
���ƣ�llyTableView.h
���ݣ��������������listview���������з�ʽ���䲻ͬ
		llyO�е�listview��ԭ��һ�£�������Щ��������麯��Ϊ�˼̳У�����
			setGravity
			updateInnerContainerSize
			remedyLayoutParameter
		llyO�е�listview���Է��÷�widget�ؼ���������Ϊ�����ʹ�ã���ˣ�
			���е�widgetΪ�����ķ����غ���������Ϊnode
			���ɸ������ӿؼ�

		

������¬����
���ڣ�2014.5.1
*/

#ifndef _LLY_TABLE_VIEW_H_
#define _LLY_TABLE_VIEW_H_

#include "llyScrollView.h"  //�޸�
#include "cocos2d.h"  //�޸�

namespace llyO_for_lly_tableview{

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
    void setItemModel(cocos2d::ui::Widget* model); //wi still
    
    /**
     * Push back a default item(create by a cloned model) into listview.
     */
    void pushBackDefaultItem();
    
    /**
     * Insert a default item(create by a cloned model) into listview.
     */
    void insertDefaultItem(ssize_t index);
    
    /**
     * Push back custom item into listview.
     */
    void pushBackCustomItem(cocos2d::Node* item); //�޸�
    
    /**
     * Insert custom item into listview.
     */
    void insertCustomItem(cocos2d::Node* item, ssize_t index); //�޸�
    
    /**
     *  Removes the last item of listview.
     */
    void removeLastItem();
    
    /**
     * Removes a item whose index is same as the parameter.
     *
     * @param index of item.
     */
    void removeItem(ssize_t index); //�޸�
    
    void removeAllItems();
    
    /**
     * Returns a item whose index is same as the parameter.
     *
     * @param index of item.
     *
     * @return the item widget.
     */
    cocos2d::Node* getItem(ssize_t index)const; //�޸�
    
    /**
     * Returns the item container.
     */
    cocos2d::Vector<cocos2d::Node*>& getItems(); //�޸�
    
    /**
     * Returns the index of item.
     *
     * @param item  the item which need to be checked.
     *
     * @return the index of item.
     */
    ssize_t getIndex(cocos2d::Node* item) const; //�޸�
    
    /**
     * Changes the gravity of listview.
     * @see ListViewGravity
     */
    virtual void setGravity(Gravity gravity);
    
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
    virtual void addChild(cocos2d::Node* child, int zOrder, int tag) override; //�޸ģ����ж�widget
    virtual void addChild(cocos2d::Node* child, int zOrder, const std::string &name) override; //�޸ģ����ж�widget
    virtual void removeAllChildren() override;
    virtual void removeAllChildrenWithCleanup(bool cleanup) override;
	virtual void removeChild(cocos2d::Node* child, bool cleaup = true) override; //�޸ģ����ж�widget
    
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
    virtual void updateInnerContainerSize();
    virtual void remedyLayoutParameter(cocos2d::Node* item);
    virtual void onSizeChanged() override;
    virtual cocos2d::ui::Widget* createCloneInstance() override; //wi still
    virtual void copySpecialProperties(cocos2d::ui::Widget* model) override; //wi still
    virtual void copyClonedWidgetChildren(cocos2d::ui::Widget* model) override; //wi still  �޸�
    void selectedItemEvent(TouchEventType event);
    virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch) override; //wi still
protected:
    cocos2d::ui::Widget* _model;
    
    cocos2d::Vector<cocos2d::Node*> _items;
    
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

} //llyO_for_lly_tableview


//new widget tableview========================

namespace lly{

class TableView : public llyO_for_lly_tableview::ListView
{
public:
	DECLARE_CLASS_GUI_INFO

	enum class ELayoutType //���ַ�ʽ�����Բ��ֱȽϺ�ƽ��ֱ����Բ����������ֵ�
	{
		ABSOLUTE,
		RELATIVE,
	};

	enum class ETableViewGravity //ͣ������
	{
		LEFT_TOP,
		RIGHT_TOP,
		LEFT_BOTTOM,
		RIGHT_BOTTOM,
	};

	enum class EArrangementDirection//��д����
	{
		HORIZONTAL, //�Ⱥ����ţ���ͷ����
		VERTICAL,	//�������ţ���ͷ����
	};

	TableView();
	virtual ~TableView();

	static TableView* create();

	//���ò��ַ�ʽ��Ĭ�Ͼ��Բ��֣�
	void setTableLayoutType(ELayoutType Ltype);

	//����ͣ������
	void setTableViewGravity(ETableViewGravity TVG);

	virtual void setGravity(Gravity gravity) override; //����

	//�����Ű淽��
	void setArrangementDirection(EArrangementDirection dir);

	virtual void setDirection(Direction dir) override; //����

	//���ÿؼ���ࣨ���Բ��־��ǻ��㵽���㣬������ǿؼ�֮��ľ��룩��Ĭ��Ϊ20��
	void setWidgetSpacing(float fSpacing);

	//�����м�� ���㵽���㣨Ĭ��Ϊ20��
	void setLineSpacing(float fLineSpacing);

	//���õ����ұ߶��ľ��루Ĭ��Ϊ30��
	void setHorizontalMargin(float fMargin);

	//���õ��ϱ߶��ľ��루Ĭ��Ϊ40��
	void setTopMargin(float fMargin);

	//���õ��ϱ߶��ľ��루Ĭ��Ϊ20��
	void setBottomMargin(float fMargin);

	//�˿ؼ��Ƿ���һ�����з���
	bool isNewLineSymbol(cocos2d::Node* item); 

	//�����==================
	cocos2d::Node* getLastItem();
	
	//==========================

	virtual void refreshView() override;

CC_CONSTRUCTOR_ACCESS:
	virtual bool init() override;

protected:
	virtual void updateInnerContainerSize() override;
	virtual void remedyLayoutParameter(cocos2d::Node* item) override;

	virtual cocos2d::ui::Widget* createCloneInstance() override;
	virtual void copySpecialProperties(cocos2d::ui::Widget* model) override;

	virtual void interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget* sender, cocos2d::Touch* touch) override; //��Ӧ���к��ƶ�
	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, bool parentTransformUpdated); //Ŀ��ؼ�������

protected:
	cocos2d::Vector<cocos2d::Node*> m_arHeadNode; //ÿ��ͷ��widget
	
	ELayoutType m_eLayoutType;
	ETableViewGravity m_eTVG;
	EArrangementDirection m_eArrangeDir;

	float m_fWidgetSpacing; //Ĭ��20
	float m_fLineSpacing; //Ĭ��20

	float m_fHorizontalMargin; //Ĭ��30
	float m_fTopMargin; //Ĭ��40
	float m_fBottomMargin; //Ĭ��20

	cocos2d::Vec2 m_posMaxMove; //����ƶ����룬����������벻��Ӧ����

	//temp
	bool m_bChangeZOrder; //ˢ�º� Ŀ��ؼ��޸�z�ᵽ����
	bool _bMoved;
	cocos2d::Vec2 m_posBegintouch; 

	//temp
	cocos2d::Node* lastItem; //refresh�У�Ϊ���Ч����
	bool bneedcheckindex; //refresh�У�Ϊ���Ч����
};

} //lly

#endif // _LLY_TABLE_VIEW_H_




