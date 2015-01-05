/*
名称：ImageView.h
内容：自定义imageView，添加获取其纹理路径名的功能

制作：卢乐颜
日期：2015.1.1
*/

#ifndef _LLY_IMAGE_VIEW_H_
#define _LLY_IMAGE_VIEW_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

namespace lly{

class ImageView : public cocos2d::ui::ImageView
{
	DECLARE_CLASS_GUI_INFO

public:
	ImageView();
	virtual ~ImageView();

	static ImageView* create();
    
	static ImageView* create(const std::string& imageFileName, TextureResType texType = TextureResType::LOCAL);

	std::string getTextureFileName();

 
};

} // namespace lly

#endif //_LLY_IMAGE_VIEW_H_


