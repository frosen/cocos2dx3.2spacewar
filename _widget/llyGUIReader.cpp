#include "llyGUIReader.h"

//================

USING_NS_CC;
using namespace ui;
using namespace cocostudio;

static lly::GUIReader* sharedReader = nullptr;

lly::GUIReader* lly::GUIReader::getInstance()
{
	if (!sharedReader)
	{
		sharedReader = new (std::nothrow) lly::GUIReader();
	}
	return sharedReader;
}

void lly::GUIReader::destroyInstance()
{
	CC_SAFE_DELETE(sharedReader);
}

cocos2d::ui::Widget* lly::GUIReader::widgetFromJsonFile( const char* fileName )
{
	std::string jsonpath;
	rapidjson::Document jsonDict;
	jsonpath = fileName;
	//    jsonpath = CCFileUtils::getInstance()->fullPathForFilename(fileName);
	size_t pos = jsonpath.find_last_of('/');
	m_strFilePath = jsonpath.substr(0,pos+1);
	std::string contentStr = FileUtils::getInstance()->getStringFromFile(jsonpath);
	jsonDict.Parse<0>(contentStr.c_str());
	if (jsonDict.HasParseError())
	{
		CCLOG("GetParseError %s\n",jsonDict.GetParseError());
	}
	Widget* widget = nullptr;
	const char* fileVersion = DICTOOL->getStringValue_json(jsonDict, "version");
	WidgetPropertiesReader * pReader = nullptr;
	if (fileVersion)
	{
		int versionInteger = getVersionInteger(fileVersion);
		if (versionInteger < 250)
		{
			pReader = new WidgetPropertiesReader0250();
			widget = pReader->createWidget(jsonDict, m_strFilePath.c_str(), fileName);
		}
		else
		{
			pReader = new lly::WidgetPropertiesReader0300(); //此处修改
			widget = pReader->createWidget(jsonDict, m_strFilePath.c_str(), fileName);
		}
	}
	else
	{
		pReader = new WidgetPropertiesReader0250();
		widget = pReader->createWidget(jsonDict, m_strFilePath.c_str(), fileName);
	}

	CC_SAFE_DELETE(pReader);
	return widget;
}

void lly::GUIReader::registerTypeWithExName( const std::string& classType, cocos2d::ObjectFactory::Instance TypeIns, cocos2d::ObjectFactory::Instance ReaderIns, char ch )
{
	ObjectFactory* factoryCreate = ObjectFactory::getInstance();

	std::string strCunstomType = "";
	strCunstomType += ch;
	strCunstomType += classType;	

	ObjectFactory::TInfo t(strCunstomType, TypeIns);
	factoryCreate->registerType(t);

	strCunstomType.append("Reader");

	ObjectFactory::TInfo t2(strCunstomType, ReaderIns);
	factoryCreate->registerType(t2);
}

void lly::GUIReader::recordPointInMap( const char* name, Vec2 &point )
{
	m_mapPoint.insert(std::pair<std::string, Vec2>(name, point));
}

cocos2d::Vec2 lly::GUIReader::getPoint( const char* name )
{
#if COCOS2D_DEBUG //调试期间先查看需要点名称是否在列表里面，不在就输出错误
	if (m_mapPoint.find(name) == m_mapPoint.end()) 
	{
		std::string msg = "@wrong point at ";
		msg += name;
		CCAssert(false, msg.c_str());
	}
#endif //COCOS2D_DEBUG
	return m_mapPoint.at(name);
}

//================================================
lly::GUIReader::GUIReader() : cocostudio::GUIReader()
{

}

lly::GUIReader::~GUIReader()
{

}

//========================================

cocos2d::ui::Widget* lly::WidgetPropertiesReader0300::widgetFromJsonDictionary( const rapidjson::Value& data )
{
	//去掉const
	std::string strclassname = DICTOOL->getStringValue_json(data, "classname");

	const rapidjson::Value& uiOptions = DICTOOL->getSubDictionary_json(data, "options");

	//自己添加的内容开始====================================
	const char* name = cocostudio::DICTOOL->getStringValue_json(uiOptions, "name");
	CCLOG("%s is a %s, create", name, strclassname.c_str());

	//判断name开头是否是PO，代表point，则在map里面记录此控件的名称和位置
	if (name && name[0] == 'P' && name[1] == 'O')
	{
		//在json中获取x，y坐标点
		float POSx = DICTOOL->getFloatValue_json(uiOptions, "x");
		float POSy = DICTOOL->getFloatValue_json(uiOptions, "y");
		CCLOG("get point name %s, x %f, y %f", name, POSx, POSy);

		//放入map中
		Vec2 pointRecord(POSx, POSy);
		GUIReader::getInstance()->recordPointInMap(name, pointRecord);

		//返回空值，因为这个控件就不显示于界面上了
		return nullptr;
	}

	//判断name开头是不是EX，是的话就是自定义控件，否则就是原有控件
	if (name && name[0] == 'E' && name[1] == 'X') 
	{
		std::string strTemp = "";
		strTemp += name[2];
		strTemp += strclassname;
		strclassname = strTemp;
	}

	const char* classname = strclassname.c_str();
	//结束======================

	Widget* widget = this->createGUI(classname); //修改
	//    CCLOG("classname = %s", classname);
	std::string readerName = this->getWidgetReaderClassName(classname);

	WidgetReaderProtocol* reader = this->createWidgetReaderProtocol(readerName);

	if (reader)
	{
		// widget parse with widget reader
		setPropsForAllWidgetFromJsonDictionary(reader, widget, uiOptions);
	}
	else
	{
		readerName = this->getWidgetReaderClassName(widget);

		reader = dynamic_cast<WidgetReaderProtocol*>(ObjectFactory::getInstance()->createObject(readerName));

		if (reader && widget) {
			setPropsForAllWidgetFromJsonDictionary(reader, widget, uiOptions);

			// 2nd., custom widget parse with custom reader
			const char* customProperty = DICTOOL->getStringValue_json(uiOptions, "customProperty");
			rapidjson::Document customJsonDict;
			customJsonDict.Parse<0>(customProperty);
			if (customJsonDict.HasParseError())
			{
				CCLOG("GetParseError %s\n", customJsonDict.GetParseError());
			}
			setPropsForAllCustomWidgetFromJsonDictionary(classname, widget, customJsonDict);
		}else{
			CCLOG("Widget or WidgetReader doesn't exists!!!  Please check your json file.");
		}

	}

	int childrenCount = DICTOOL->getArrayCount_json(data, "children");
	for (int i = 0; i < childrenCount; i++)
	{
		const rapidjson::Value& subData = DICTOOL->getDictionaryFromArray_json(data, "children", i);
		cocos2d::ui::Widget* child = widgetFromJsonDictionary(subData);
		if (child)
		{
			PageView* pageView = dynamic_cast<PageView*>(widget);
			if (pageView)
			{
				pageView->addPage(static_cast<Layout*>(child));
			}
			else
			{
				ListView* listView = dynamic_cast<ListView*>(widget);
				if (listView)
				{
					listView->pushBackCustomItem(child);
				}
				else
				{
					if (!dynamic_cast<Layout*>(widget))
					{
						if (child->getPositionType() == ui::Widget::PositionType::PERCENT)
						{
							child->setPositionPercent(Vec2(child->getPositionPercent().x + widget->getAnchorPoint().x, child->getPositionPercent().y + widget->getAnchorPoint().y));
						}
						child->setPosition(Vec2(child->getPositionX() + widget->getAnchorPointInPoints().x, child->getPositionY() + widget->getAnchorPointInPoints().y));
					}
					widget->addChild(child);
				}
			}
		}
	}
	return widget;
}

//===========================================
void lly::setWidgetFast( cocos2d::ui::Widget** widget, cocos2d::ui::Widget* father, char* childname )
{
	//获得控件
	*widget = getChildWidget(father, childname);
}

void lly::setButtonFast( cocos2d::ui::Widget** widget, cocos2d::ui::Widget* father, Widget::ccWidgetTouchCallback callback, char* childname )
{
	//获得控件
	*widget = getChildWidget(father, childname);

	//给控件赋予触控回调
	(*widget)->addTouchEventListener(callback);
}

void lly::setButtonFast( cocos2d::ui::Widget* father, cocos2d::ui::Widget::ccWidgetTouchCallback callback, char* childname )
{
	//声明一个和widget类型一致的指针
	cocos2d::ui::Widget* widget = nullptr;

	//获得控件
	widget = getChildWidget(father, childname);

	//给控件赋予触控回调
	widget->addTouchEventListener(callback);
}

void lly::setCheckBoxFast( cocos2d::ui::Widget** widget, cocos2d::ui::Widget* father, CheckBox::ccCheckBoxCallback callback, char* childname )
{
	//获得控件
	*widget = getChildWidget(father, childname);

	//给控件赋予触控回调（复选框的）
	static_cast<cocos2d::ui::CheckBox*>(*widget)->addEventListener(callback);
}

void lly::setCheckBoxFast( cocos2d::ui::Widget* father, CheckBox::ccCheckBoxCallback callback, char* childname )
{
	//声明一个和widget类型一致的指针
	cocos2d::ui::Widget* widget = nullptr;

	//获得控件
	widget = getChildWidget(father, childname);

	//给控件赋予触控回调（复选框的）
	static_cast<cocos2d::ui::CheckBox*>(widget)->addEventListener(callback);
}

cocos2d::ui::Widget* lly::getChildWidget( cocos2d::ui::Widget* father, char* childname )
{
	std::string strError = "@wrong widget child: ";
	do 
	{
		if (!father) break;

		cocos2d::ui::Widget* wiChild = father;
		std::string strName(childname);
		std::string::size_type i = 0;
		std::string::size_type n = 0;
		std::string::size_type max = strName.size();
		while (true)
		{
			n = strName.find('/', i);

			if (n != std::string::npos)
			{
				wiChild = static_cast<cocos2d::ui::Widget*>(wiChild->getChildByName(strName.substr(i, n - i)));
				if (wiChild == nullptr) 
				{
					strError += strName.substr(i, n - i);
					break;
				}
				i = n + 1;
			}
			else
			{
				//std::string a = strName.substr(i);
				wiChild = static_cast<cocos2d::ui::Widget*>(wiChild->getChildByName(strName.substr(i)));
				if (wiChild == nullptr) strError += strName.substr(i);
				break;
			}
		}

		if (!wiChild) break;

		//返回子控件
		return wiChild;
	} while (0);

	//如果子控件为空则打断
	CCAssert(false, strError.c_str());
	return nullptr;
}



