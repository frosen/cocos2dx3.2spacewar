/*
名称：llyGUIReader.h
内容：自定义的cocostudio文件读取器，可以在文件名称前加上EX，就可以使用对应的新控件
		自定义控件需要提前注册到读取器内
		如果控件名称前增加PO，则此控件不加入父层中，而是获得其xy坐标放入列表，

制作：卢乐颜
日期：2014.5.1
*/

#ifndef _LLY_GUI_READER_H_
#define _LLY_GUI_READER_H_

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

namespace lly{

class GUIReader : public cocostudio::GUIReader
{
public:
	static GUIReader* getInstance(); //覆盖
	static void destroyInstance(); //覆盖
	cocos2d::ui::Widget* widgetFromJsonFile(const char* fileName); //覆盖

	//自定义的控件注册到reader里
	//指定类型名称，类型创建函数指针，类型解析函数指针，还有特殊标识字符
	//所有name开头为EX的为自定义类，EX后紧跟着的字符为特殊标识（默认为 _ ）
	void registerTypeWithExName(const std::string& classType,
		cocos2d::ObjectFactory::Instance TypeIns,
		cocos2d::ObjectFactory::Instance ReaderIns,
		char ch = '_'); 

	//根据json文件里面所有PO打头的控件，记录他们的position，以供程序里使用
	void recordPointInMap(const char* name, cocos2d::Vec2 &point);

	//根据名称，获取坐标点
	cocos2d::Vec2 getPoint(const char* name);

protected:
	GUIReader();
	virtual ~GUIReader();

protected:
	std::map<std::string, cocos2d::Vec2> m_mapPoint; //放置坐标点的map
};

class WidgetPropertiesReader0300 : public cocostudio::WidgetPropertiesReader0300
{
public:
	WidgetPropertiesReader0300(){};
	virtual ~WidgetPropertiesReader0300(){};

	virtual cocos2d::ui::Widget* widgetFromJsonDictionary(const rapidjson::Value& dic); //覆盖
};

//=============================
//快速获得cocostudio的控件
void setWidgetFast(cocos2d::ui::Widget* widget, cocos2d::ui::Widget* father, char* childname, ...);

//重载版本，给控件增加触摸回调
void setWidgetFast(cocos2d::ui::Widget* widget, cocos2d::ui::Widget* father, cocos2d::ui::Widget::ccWidgetTouchCallback callback, char* childname, ...);

//重载版本，给控件增加触摸回调，不需要给控件声明一个指针控制它
void setWidgetFast(cocos2d::ui::Widget* father, cocos2d::ui::Widget::ccWidgetTouchCallback callback, char* childname, ...);

//复选框, 回调是复选框的回调
void setCheckBoxFast(cocos2d::ui::Widget* widget, cocos2d::ui::Widget* father, cocos2d::ui::CheckBox::ccCheckBoxCallback callback, char* childname, ...);

//给复选框增加触摸回调，回调是复选框的回调，不需要给控件声明一个指针控制它
void setCheckBoxFast(cocos2d::ui::Widget* father, cocos2d::ui::CheckBox::ccCheckBoxCallback callback, char* childname, ...);

//获取子控件，根据父层和父层下每层子控件的名称
//如果出错则会打断
cocos2d::ui::Widget* getChildWidget(cocos2d::ui::Widget* father, char* pch, ...);

} // namespace lly

#endif // _LLY_GUI_READER_H_

