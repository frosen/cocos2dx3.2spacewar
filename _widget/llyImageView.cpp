#include "llyImageView.h"

USING_NS_CC;
using namespace lly;

lly::ImageView::ImageView() : ui::ImageView()
{

}

lly::ImageView::~ImageView()
{

}

ImageView* lly::ImageView::create()
{
	 lly::ImageView* widget = new  lly::ImageView();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

ImageView* lly::ImageView::create( const std::string& imageFileName, TextureResType texType /*= TextureResType::LOCAL*/ )
{
	 lly::ImageView *widget = new (std::nothrow) lly::ImageView;
	if (widget && widget->init(imageFileName, texType)) {
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

std::string lly::ImageView::getTextureFileName()
{
	return _textureFile;
}

