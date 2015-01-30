#include "llyScrollView.h"
#include "llySlider.h"

USING_NS_CC;
using namespace lly;

IMPLEMENT_CLASS_GUI_INFO(lly::ScrollView)

lly::ScrollView::ScrollView() : cocos2d::ui::ScrollView(),
	//===============================
	m_innerContainerPositionXPercent(-1),
	m_innerContainerPositionYPercent(-1),
	m_heightSlider(nullptr),
	m_widthSlider(nullptr),
	_isInnerCanMove(true),
	m_targetItem(nullptr)
{

}

lly::ScrollView::~ScrollView()
{

}

lly::ScrollView* lly::ScrollView::create()
{
	auto widget = new (std::nothrow) lly::ScrollView();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

void lly::ScrollView::setInnerContainerSize( const cocos2d::Size &size )
{
	ui::ScrollView::setInnerContainerSize(size);
	changeInnerContainerSizeEvent(); //添加
}

std::string lly::ScrollView::getDescription() const 
{
	return "lly ScrollView";
}

void lly::ScrollView::onSizeChanged()
{
	ui::ScrollView::onSizeChanged();
	changeInnerContainerSizeEvent(); //添加
}

void lly::ScrollView::jumpToDestination(const cocos2d::Vec2& des)
{
	ui::ScrollView::jumpToDestination(des);

	//添加，记录百分比和进行回调			
	doAfterChangeInnerContainerPosition(false);
}

bool lly::ScrollView::scrollChildren(float touchOffsetX, float touchOffsetY)
{
	bool b = ui::ScrollView::scrollChildren(touchOffsetX, touchOffsetY);

	//添加，记录百分比和进行回调			
	doAfterChangeInnerContainerPosition();
	return b;
}

void lly::ScrollView::endRecordSlidAction()
{
	if (!_isInnerCanMove) return; //添加，设置了不可移动子层

	ui::ScrollView::endRecordSlidAction();
}

void lly::ScrollView::handleMoveLogic(cocos2d::Touch *touch)
{
	if (!_isInnerCanMove) return; //添加 设置了不能移动子层

	ui::ScrollView::handleMoveLogic(touch);
}

//=======================================================
void lly::ScrollView::setLinkedSlider( lly::Slider* heightSlider, lly::Slider* widthSlider /*= nullptr*/, bool canHideSlider /*= true*/ )
{
	if (heightSlider && !widthSlider) //只有纵向滑动条
	{
		this->addScrollViewEventListener([=](Ref* ob, int x, int y)
		{
			heightSlider->setPercent(y);
		});

		heightSlider->addEventListener([this](Ref* obj, lly::Slider::EventType t)
		{
			lly::Slider* slider = static_cast<lly::Slider*>(obj);
			this->jumpToPercentVertical(slider->getPercent());
		});

		if (canHideSlider) //如果需要不用时候隐藏滑动条
		{
			if (isInnerHigher()) //初始设置
			{
				heightSlider->setVisible(true);
			}
			else
			{
				heightSlider->setVisible(false);
			}

			this->addChangeInnerEventListener([=](Ref* ob, ChangeInnerSizeType wT, ChangeInnerSizeType hT, int w, int h)
			{
				switch (hT)
				{
				case ChangeInnerSizeType::INNER_TURN_LONGER_THAN_OUTER:
					heightSlider->setVisible(true);
					break;
				case ChangeInnerSizeType::INNER_TURN_NOT_LONGER_THAN_OUTER:
					heightSlider->setVisible(false);
					break;
				default:
					break;
				}
				heightSlider->setBallWidthPercent(h);
			});	
		}
	}

	if (widthSlider && !heightSlider) //只有横向滑动条
	{
		this->addScrollViewEventListener([=](Ref* ob, int x, int y)
		{
			widthSlider->setPercent(x);
		});

		widthSlider->addEventListener([this](Ref* obj, lly::Slider::EventType t)
		{
			lly::Slider* slider = static_cast<lly::Slider*>(obj);
			this->jumpToPercentHorizontal(slider->getPercent());
		});

		if (canHideSlider) //如果需要不用时候隐藏滑动条
		{
			if (isInnerWider()) //初始设置
			{
				widthSlider->setVisible(true);
			}
			else
			{
				widthSlider->setVisible(false);
			}

			this->addChangeInnerEventListener([=](Ref* ob, ChangeInnerSizeType wT, ChangeInnerSizeType hT, int w, int h)
			{
				switch (wT)
				{
				case ChangeInnerSizeType::INNER_TURN_LONGER_THAN_OUTER:
					widthSlider->setVisible(true);
					break;
				case ChangeInnerSizeType::INNER_TURN_NOT_LONGER_THAN_OUTER:
					widthSlider->setVisible(false);
					break;
				default:
					break;
				}
				widthSlider->setBallWidthPercent(w);
			});	
		}
	}

	if (widthSlider && heightSlider) //有横向滑动条，也有纵向
	{
		this->addScrollViewEventListener([=](Ref* ob, int x, int y)
		{
			widthSlider->setPercent(x);
			heightSlider->setPercent(y);
		});

		widthSlider->addEventListener([this](Ref* obj, lly::Slider::EventType t)
		{
			lly::Slider* slider = static_cast<lly::Slider*>(obj);
			this->jumpToPercentHorizontal(slider->getPercent());
		});

		heightSlider->addEventListener([this](Ref* obj, lly::Slider::EventType t)
		{
			lly::Slider* slider = static_cast<lly::Slider*>(obj);
			this->jumpToPercentVertical(slider->getPercent());
		});

		if (canHideSlider)
		{
			if (isInnerWider()) //初始设置
			{
				widthSlider->setVisible(true);
			}
			else
			{
				widthSlider->setVisible(false);
			}

			if (isInnerHigher()) //初始设置
			{
				heightSlider->setVisible(true);
			}
			else
			{
				heightSlider->setVisible(false);
			}

			this->addChangeInnerEventListener([=](Ref* ob, ChangeInnerSizeType wT, ChangeInnerSizeType hT, int w, int h)
			{
				switch (wT)
				{
				case ChangeInnerSizeType::INNER_TURN_LONGER_THAN_OUTER:
					widthSlider->setVisible(true);
					break;
				case ChangeInnerSizeType::INNER_TURN_NOT_LONGER_THAN_OUTER:
					widthSlider->setVisible(false);
					break;
				default:
					break;
				}
				widthSlider->setBallWidthPercent(w);

				switch (hT)
				{
				case ChangeInnerSizeType::INNER_TURN_LONGER_THAN_OUTER:
					heightSlider->setVisible(true);
					break;
				case ChangeInnerSizeType::INNER_TURN_NOT_LONGER_THAN_OUTER:
					heightSlider->setVisible(false);
					break;
				default:
					break;
				}
				heightSlider->setBallWidthPercent(h);
			});	
		}		
	}
}

void lly::ScrollView::doAfterChangeInnerContainerPosition(bool bRunFunction)
{
	if (_contentSize.height < _innerContainer->getContentSize().height) //记录
		m_innerContainerPositionYPercent = 100 - (int)(_innerContainer->getPositionY() * 100 / (_contentSize.height - _innerContainer->getContentSize().height));	

	if (_contentSize.width < _innerContainer->getContentSize().width) //记录
		m_innerContainerPositionXPercent =(int)(_innerContainer->getPositionX() * 100 / (_contentSize.width - _innerContainer->getContentSize().width));

	if (m_addScrollViewEventFunc && bRunFunction) //lambda回调
		m_addScrollViewEventFunc(this, m_innerContainerPositionXPercent, m_innerContainerPositionYPercent); 
}

void lly::ScrollView::changeInnerContainerSizeEvent()
{
	int widthRate = (int)(_contentSize.width * 100 / _innerContainer->getContentSize().width );
	int heightRate = (int)(_contentSize.height * 100 / _innerContainer->getContentSize().height);
	ChangeInnerSizeType _widthChangeType = ChangeInnerSizeType::NO_CHANGE;
	ChangeInnerSizeType _heightChangeType = ChangeInnerSizeType::NO_CHANGE;

	//原本没有外层宽，现在突然宽了
	if (!isInnerWidthThanOuter && _innerContainer->getContentSize().width > _contentSize.width)
	{
		isInnerWidthThanOuter = true;	
		_widthChangeType = ChangeInnerSizeType::INNER_TURN_LONGER_THAN_OUTER;
	}

	//原本比外层宽，现在突然窄了
	if (isInnerWidthThanOuter && _innerContainer->getContentSize().width <= _contentSize.width)
	{
		isInnerWidthThanOuter = false;
		_widthChangeType = ChangeInnerSizeType::INNER_TURN_NOT_LONGER_THAN_OUTER;

		//重新设定比例
		m_innerContainerPositionXPercent = -1;		
	}

	//原本没有外层低，现在突然高了
	if (!isInnerHeightThanOuter && _innerContainer->getContentSize().height > _contentSize.height)
	{
		isInnerHeightThanOuter = true;
		_heightChangeType = ChangeInnerSizeType::INNER_TURN_LONGER_THAN_OUTER;
	}

	//原本比外层高，现在突然低了
	if (isInnerHeightThanOuter && _innerContainer->getContentSize().height <= _contentSize.height)
	{
		isInnerHeightThanOuter = false;
		_heightChangeType = ChangeInnerSizeType::INNER_TURN_NOT_LONGER_THAN_OUTER;

		//重新设定比例
		m_innerContainerPositionYPercent = -1;
	}

	//变更宽度、高度比
	if (m_addChangeInnerEventFunc)
	{
		m_addChangeInnerEventFunc(this, _widthChangeType, _heightChangeType, widthRate, heightRate);
	}
}

cocos2d::ui::Widget* lly::ScrollView::createCloneInstance()
{
	return lly::ScrollView::create();
}

void ScrollView::copySpecialProperties(Widget *widget)
{
	lly::ScrollView* scrollView = dynamic_cast<lly::ScrollView*>(widget);
	if (scrollView)
	{
		ui::ScrollView::copySpecialProperties(widget);
		m_addScrollViewEventFunc = scrollView->m_addScrollViewEventFunc;
		m_addChangeInnerEventFunc = scrollView->m_addChangeInnerEventFunc;
		_isInnerCanMove = scrollView->_isInnerCanMove;
	}
}



