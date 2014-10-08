#include "llyTableView.h"

using namespace lly;
USING_NS_CC;
using namespace ui;

lly::TableView::TableView() : 
	llyO::ListView(),
	m_eLayoutType(ABSOLUTE),
	m_eTVG(LEFT_TOP),
	m_eArrangeDir(HORIZONTAL),
	m_fWidgetSpacing(20),
	m_fLineSpacing(20),
	m_fHorizontalMargin(30),
	m_fVerticalMargin(30),
	lastItem(nullptr),
	bneedcheckindex(true)
{

}

lly::TableView::~TableView()
{

}

TableView* lly::TableView::create()
{
	TableView* widget = new TableView();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

void lly::TableView::setTableLayoutType( ELayoutType Ltype )
{
	if (m_eLayoutType == Ltype) return;
	m_eLayoutType = Ltype;
	_refreshViewDirty = true;
}

void lly::TableView::setTableViewGravity( ETableViewGravity TVG )
{
	if (m_eTVG == TVG) return;
	m_eTVG = TVG;
	_refreshViewDirty = true;
}

void lly::TableView::setGravity( Gravity gravity )
{
	if (_gravity == gravity) return;
	_gravity = gravity;

	switch (gravity)
	{
	case llyO::ListView::Gravity::LEFT:
		setTableViewGravity(LEFT_TOP);
		break;
	case llyO::ListView::Gravity::RIGHT:
		setTableViewGravity(RIGHT_TOP);
		break;
	case llyO::ListView::Gravity::CENTER_HORIZONTAL:
		setTableViewGravity(LEFT_TOP);
		break;
	case llyO::ListView::Gravity::TOP:
		setTableViewGravity(LEFT_TOP);
		break;
	case llyO::ListView::Gravity::BOTTOM:
		setTableViewGravity(LEFT_BOTTOM);
		break;
	case llyO::ListView::Gravity::CENTER_VERTICAL:
		setTableViewGravity(LEFT_TOP);
		break;
	default:
		break;
	}
}

void lly::TableView::setArrangementDirection( EArrangementDirection dir )
{
	if (m_eArrangeDir == dir) return;
	m_eArrangeDir = dir;	

	switch (m_eArrangeDir) //滚动方向设置
	{
	case lly::TableView::EArrangementDirection::HORIZONTAL:
		ScrollView::setDirection(ScrollView::Direction::VERTICAL);
		break;
	case lly::TableView::EArrangementDirection::VERTICAL:
		ScrollView::setDirection(ScrollView::Direction::HORIZONTAL);
		break;
	default:
		break;
	}

	_refreshViewDirty = true; //刷新开启
}

void lly::TableView::setDirection( Direction dir )
{
	switch (dir)
	{
	case ScrollView::Direction::VERTICAL:
		setArrangementDirection(HORIZONTAL);
		break;
	case ScrollView::Direction::HORIZONTAL:
		setArrangementDirection(VERTICAL);
		break;
	default:
		break;
	}
}

void lly::TableView::setWidgetSpacing(float fSpacing)
{
	if (m_fWidgetSpacing == fSpacing) return;
	m_fWidgetSpacing = fSpacing;
	_refreshViewDirty = true;
}

void lly::TableView::setLineSpacing(float fLineSpacing)
{
	if (m_fLineSpacing == fLineSpacing) return;
	m_fLineSpacing = fLineSpacing;
	_refreshViewDirty = true;
}

void lly::TableView::setHorizontalMargin(float fMargin)
{
	if (m_fHorizontalMargin == fMargin) return;
	m_fHorizontalMargin = fMargin;
	_refreshViewDirty = true;
}

void lly::TableView::setVerticalMargin(float fMargin)
{
	if (m_fVerticalMargin == fMargin) return;
	m_fVerticalMargin = fMargin;
	_refreshViewDirty = true;
}

Widget* lly::TableView::getLastItem()
{
	return static_cast<cocos2d::ui::Widget*>(_items.back());
}

void lly::TableView::refreshView()
{
	bneedcheckindex = false; //为了提高效率，不检测
	lastWidget = nullptr;
	llyO::ListView::refreshView();
	bneedcheckindex = true;
}

//=====================================
bool lly::TableView::init()
{
	if (ScrollView::init())
	{
		setLayoutType(Type::ABSOLUTE);
		return true;
	}
	else return false;	
}

void lly::TableView::updateInnerContainerSize()
{
	switch (m_eArrangeDir) //水平排列
	{
	case EArrangementDirection::HORIZONTAL: 
		{
			if (!getItem(0) || !getLastItem()) return;

			//第一个控件到最后一个控件的垂直长度加上两边留白的总距离
			float f = abs(getItem(0)->getPositionY() - getLastItem()->getPositionY()) + 2 * m_fVerticalMargin;
			if (f > _contentSize.height && f != _innerContainer->getContentSize().height )
			{
				setInnerContainerSize(CCSize( _contentSize.width, f ));

				if (m_eTVG == LEFT_TOP || m_eTVG == RIGHT_TOP)
				{
					float fDistance = 0.0f;
					bool first = true;
					for (auto item : _items)
					{
						if (!item) break;

						if (first)
						{
							float fORI = item->getPositionY();
							item->setPositionY(_innerContainer->getContentSize().height - m_fVerticalMargin);
							fDistance = item->getPositionY() - fORI;
						}
						else
						{
							item->setPositionY(item->getPositionY() + fDistance);
						}
					}
				}				
			}
		}
		break;
	case EArrangementDirection::VERTICAL:
		{
			if (!getItem(0) || !getLastItem()) return;

			//第一个控件到最后一个控件的水平长度加上两边留白的总距离
			float f = abs(getItem(0)->getPositionX() - getLastItem()->getPositionX()) + 2 * m_fVerticalMargin;

			if (f >= _contentSize.width && f != _innerContainer->getContentSize().width )
			{
				setInnerContainerSize(CCSize( f, _contentSize.height ));

				if (m_eTVG == RIGHT_TOP || m_eTVG == RIGHT_BOTTOM)
				{
					float fDistance = 0.0f;
					bool first = true;
					for (auto item : _items)
					{
						if (!item) break;

						if (first)
						{
							float fORI = item->getPositionX();
							item->setPositionX(_innerContainer->getSize().width - m_fVerticalMargin);
							fDistance = item->getPositionX() - fORI;
						}
						else
						{
							item->setPositionX(item->getPositionX() + fDistance);
						}
					}
				}			
			}			
		}	
		break;
	default:
		break;
	}
}

void lly::TableView::remedyLayoutParameter( cocos2d::ui::Widget* item )
{
	if (!item) return;

	if (bneedcheckindex)
	{
		int nIndex = getIndex(item);
		lastItem = getItem(nIndex - 1); //上一个控件
	}

	switch (m_eArrangeDir) //排版方向
	{
	case EArrangementDirection::HORIZONTAL:

		switch (m_eTVG) //根据停靠方向不同
		{
		case ETableViewGravity::LEFT_TOP: //左上
			if (lastItem == nullptr)
			{
				item->setPosition(ccp(m_fHorizontalMargin, _innerContainer->getContentSize().height - m_fVerticalMargin));
				m_arHeadNode.pushBack(item); //行头
			}
			else
			{
				//把位置放到上一个控件往右增加一定距离的地方
				if (m_Ltype == TABLEVIEW_LAYOUT_TYPE_ABSOLUTE)
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(m_fWidgetSpacing, 0)));
				}
				else
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(lastitem->getContentSize().width, 0)));
				}

				if (item->getPositionX() + m_fHorizontalMargin > _innerContainer->getContentSize().width) //如果超出了一行的宽度，就换行
				{
					item->setPosition(ccp(m_fHorizontalMargin, lastitem->getPositionY() - m_fLineSpacing));
					m_arHeadNode.push_back(item); //行头
				}
			}
			break;
		case lly::TABLEVIEW_GRAVITY_RIGHT_TOP:
			if (nIndex == 0)
			{
				item->setPosition(ccp(_innerContainer->getContentSize().width - m_fHorizontalMargin, _innerContainer->getContentSize().height - m_fVerticalMargin));
				m_arHeadNode.push_back(item); //行头
			}
			else
			{
				//上一个控件
				if (lastitem == NULL) lastitem = getItem(nIndex - 1);

				//把位置放到上一个控件往左边增加一定距离的地方
				if (m_Ltype == TABLEVIEW_LAYOUT_TYPE_ABSOLUTE)
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(-m_fWidgetSpacing, 0)));
				}
				else
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(-lastitem->getContentSize().width, 0)));
				}

				if (item->getPositionX() - m_fHorizontalMargin < 0) //如果超出了一行的宽度，就换行
				{
					item->setPosition(ccp(_innerContainer->getContentSize().width - m_fHorizontalMargin, lastitem->getPositionY() - m_fLineSpacing));
					m_arHeadNode.push_back(item); //行头
				}
			}
			break;
		case lly::TABLEVIEW_GRAVITY_LEFT_BOTTOM:
			if (nIndex == 0)
			{
				item->setPosition(ccp(m_fHorizontalMargin, m_fVerticalMargin));
				m_arHeadNode.push_back(item); //行头
			}
			else
			{
				//上一个控件
				if (lastitem == NULL) lastitem = getItem(nIndex - 1);

				//把位置放到上一个控件往右增加一定距离的地方
				if (m_Ltype == TABLEVIEW_LAYOUT_TYPE_ABSOLUTE)
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(m_fWidgetSpacing, 0)));
				}
				else
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(lastitem->getContentSize().width, 0)));
				}

				if (item->getPositionX() + m_fHorizontalMargin > _innerContainer->getContentSize().width) //如果超出了一行的宽度，就换行
				{
					item->setPosition(ccp(m_fHorizontalMargin, lastitem->getPositionY() + m_fLineSpacing));
					m_arHeadNode.push_back(item); //行头
				}
			}
			break;
		case lly::TABLEVIEW_GRAVITY_RIGHT_BOTTOM:
			if (nIndex == 0)
			{
				item->setPosition(ccp(_innerContainer->getContentSize().width - m_fHorizontalMargin, m_fVerticalMargin));
				m_arHeadNode.push_back(item); //行头
			}
			else
			{
				//上一个控件
				if (lastitem == NULL) lastitem = getItem(nIndex - 1);

				//把位置放到上一个控件往左边增加一定距离的地方
				if (m_Ltype == TABLEVIEW_LAYOUT_TYPE_ABSOLUTE)
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(-m_fWidgetSpacing, 0)));
				}
				else
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(-lastitem->getContentSize().width, 0)));
				}

				if (item->getPositionX() - m_fHorizontalMargin < 0) //如果超出了一行的宽度，就换行
				{
					item->setPosition(ccp(_innerContainer->getContentSize().width - m_fHorizontalMargin, lastitem->getPositionY() + m_fLineSpacing));
					m_arHeadNode.push_back(item); //行头
				}
			}
			break;
		default:
			break;
		}
		break;
	case lly::TABLEVIEW_DIR_VERTICAL_FIRST:

		switch (m_gravity) //根据停靠方向不同
		{
		case lly::TABLEVIEW_GRAVITY_LEFT_TOP: //左上
			if (nIndex == 0)
			{
				item->setPosition(ccp(m_fHorizontalMargin, _innerContainer->getContentSize().height - m_fVerticalMargin));
				m_arHeadNode.push_back(item); //行头
			}
			else
			{
				//上一个控件
				if (lastitem == NULL) lastitem = getItem(nIndex - 1);

				//把位置放到上一个控件往右增加一定距离的地方
				if (m_Ltype == TABLEVIEW_LAYOUT_TYPE_ABSOLUTE)
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(0, -m_fWidgetSpacing)));
				}
				else
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(0, -lastitem->getContentSize().height)));
				}

				if (item->getPositionY() - m_fHorizontalMargin < 0) //如果超出了一行的宽度，就换行
				{
					item->setPosition(ccp(lastitem->getPositionX() + m_fLineSpacing, _innerContainer->getContentSize().height - m_fVerticalMargin));
					m_arHeadNode.push_back(item); //行头
				}
			}
			break;
		case lly::TABLEVIEW_GRAVITY_RIGHT_TOP:
			if (nIndex == 0)
			{
				item->setPosition(ccp(_innerContainer->getContentSize().width - m_fHorizontalMargin, _innerContainer->getContentSize().height - m_fVerticalMargin));
				m_arHeadNode.push_back(item); //行头
			}
			else
			{
				//上一个控件
				if (lastitem == NULL) lastitem = getItem(nIndex - 1);

				//把位置放到上一个控件往下边增加一定距离的地方
				if (m_Ltype == TABLEVIEW_LAYOUT_TYPE_ABSOLUTE)
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(0, -m_fWidgetSpacing)));
				}
				else
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(0, -lastitem->getContentSize().height)));
				}

				if (item->getPositionY() - m_fHorizontalMargin < 0) //如果超出了一行的宽度，就换行
				{
					item->setPosition(ccp(lastitem->getPositionX() - m_fLineSpacing, _innerContainer->getContentSize().height - m_fVerticalMargin));
					m_arHeadNode.push_back(item); //行头
				}
			}
			break;
		case lly::TABLEVIEW_GRAVITY_LEFT_BOTTOM:
			if (nIndex == 0)
			{
				item->setPosition(ccp(m_fHorizontalMargin, m_fVerticalMargin));
				m_arHeadNode.push_back(item); //行头
			}
			else
			{
				//上一个控件
				if (lastitem == NULL) lastitem = getItem(nIndex - 1);

				//把位置放到上一个控件往上增加一定距离的地方
				if (m_Ltype == TABLEVIEW_LAYOUT_TYPE_ABSOLUTE)
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(0, m_fWidgetSpacing)));
				}
				else
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(0, lastitem->getContentSize().height)));
				}

				if (item->getPositionY() + m_fHorizontalMargin > _innerContainer->getContentSize().height) //如果超出了一行的宽度，就换行
				{
					item->setPosition(ccp(lastitem->getPositionX() + m_fLineSpacing, m_fHorizontalMargin));
					m_arHeadNode.push_back(item); //行头
				}
			}
			break;
		case lly::TABLEVIEW_GRAVITY_RIGHT_BOTTOM:
			if (nIndex == 0)
			{
				item->setPosition(ccp(_innerContainer->getContentSize().width - m_fHorizontalMargin, m_fVerticalMargin));
				m_arHeadNode.push_back(item); //行头
			}
			else
			{
				if (lastitem == NULL) lastitem = getItem(nIndex - 1);

				//把位置放到上一个控件往左边增加一定距离的地方
				if (m_Ltype == TABLEVIEW_LAYOUT_TYPE_ABSOLUTE)
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(0, m_fWidgetSpacing)));
				}
				else
				{
					item->setPosition(ccpAdd(lastitem->getPosition(), ccp(0, lastitem->getContentSize().height)));
				}

				if (item->getPositionY() + m_fHorizontalMargin > _innerContainer->getContentSize().height) //如果超出了一行的宽度，就换行
				{
					item->setPosition(ccp(lastitem->getPositionX() - m_fLineSpacing, m_fHorizontalMargin));
					m_arHeadNode.push_back(item); //行头
				}
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	//把自己作为下个控件的上一个控件
	lastWidget = item;
}

cocos2d::ui::Widget* lly::TableView::createCloneInstance()
{

}

void lly::TableView::copySpecialProperties( cocos2d::ui::Widget* model )
{

}


