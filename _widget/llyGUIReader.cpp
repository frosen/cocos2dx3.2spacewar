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
		sharedReader = new lly::GUIReader();
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
			pReader = new lly::WidgetPropertiesReader0300(); //�˴��޸�
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
#if COCOS2D_DEBUG //�����ڼ��Ȳ鿴��Ҫ�������Ƿ����б����棬���ھ��������
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
	const char* classname = DICTOOL->getStringValue_json(data, "classname");
	const rapidjson::Value& uiOptions = DICTOOL->getSubDictionary_json(data, "options");

	//�Լ���ӵ����ݿ�ʼ====================================
	const char* name = cocostudio::DICTOOL->getStringValue_json(uiOptions, "name");
	CCLOG("%s is a %s, create", name, classname);

	//�ж�name��ͷ�Ƿ���PO������point������map�����¼�˿ؼ������ƺ�λ��
	if (name && name[0] == 'P' && name[1] == 'O')
	{
		//��json�л�ȡx��y�����
		float POSx = DICTOOL->getFloatValue_json(uiOptions, "x");
		float POSy = DICTOOL->getFloatValue_json(uiOptions, "y");
		CCLOG("get point name %s, x %f, y %f", name, POSx, POSy);

		//����map��
		GUIReader::getInstance()->recordPointInMap(name, Vec2(POSx, POSy));

		//���ؿ�ֵ����Ϊ����ؼ��Ͳ���ʾ�ڽ�������
		return nullptr;
	}

	//�ж�name��ͷ�ǲ���EX���ǵĻ������Զ���ؼ����������ԭ�пؼ�
	if (name && name[0] == 'E' && name[1] == 'X') 
	{
		std::string strTemp = "";
		strTemp += name[2];
		strTemp += classname;
		classname = strTemp.c_str();
	}
	//����======================

	Widget* widget = this->createGUI(classname); //�޸�
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
void lly::setWidgetFast( cocos2d::ui::Widget* widget, cocos2d::ui::Widget* father, char* childname, ... )
{
	//��ÿؼ�
	va_list args;
	va_start(args, childname);
	widget = getChildWidget(father, childname, args);
	va_end(args);
}

void lly::setWidgetFast( cocos2d::ui::Widget* widget, cocos2d::ui::Widget* father, Widget::ccWidgetTouchCallback callback, char* childname, ... )
{
	//��ÿؼ�
	va_list args;
	va_start(args, childname);
	widget = getChildWidget(father, childname, args);
	va_end(args);

	//���ؼ����败�ػص�
	widget->addTouchEventListener(callback);
}

void lly::setWidgetFast( cocos2d::ui::Widget* father, Widget::ccWidgetTouchCallback callback, char* childname, ... )
{
	//����һ��widget����
	cocos2d::ui::Widget* wiTemp = nullptr;

	//��ÿؼ�
	va_list args;
	va_start(args, childname);
	wiTemp = getChildWidget(father, childname, args);
	va_end(args);

	//���ؼ����败�ػص�
	wiTemp->addTouchEventListener(callback);
}

void lly::setCheckBoxFast( cocos2d::ui::Widget* widget, cocos2d::ui::Widget* father, CheckBox::ccCheckBoxCallback callback, char* childname, ... )
{
	//��ÿؼ�
	va_list args;
	va_start(args, childname);
	widget = getChildWidget(father, childname, args);
	va_end(args);

	//���ؼ����败�ػص�����ѡ��ģ�
	static_cast<cocos2d::ui::CheckBox*>(widget)->addEventListener(callback);
}

void lly::setCheckBoxFast( cocos2d::ui::Widget* father, CheckBox::ccCheckBoxCallback callback, char* childname, ... )
{
	//����һ����widget����һ�µ�ָ��
	cocos2d::ui::Widget* wiTemp = nullptr;

	//��ÿؼ�
	va_list args;
	va_start(args, childname);
	wiTemp = getChildWidget(father, childname, args);
	va_end(args);

	//���ؼ����败�ػص�����ѡ��ģ�
	static_cast<cocos2d::ui::CheckBox*>(wiTemp)->addEventListener(callback);
}

cocos2d::ui::Widget* lly::getChildWidget( cocos2d::ui::Widget* father, char* pch, ... )
{
	do 
	{
		//ȷ������Ϊ�գ����򷵻ؿ�ֵ
		if (!father) break;

		//����ĵ�һ���ӿؼ�
		cocos2d::ui::Widget* wiChild = static_cast<cocos2d::ui::Widget*>(father->getChildByName(pch));
		if (!wiChild) break;

		//����ɱ������ÿ���������¶�ȡһ����ӿؼ�
		va_list params;
		va_start(params, pch);
		char* childname;

		while (true)
		{
			childname = va_arg(params, char*);

			if (childname == nullptr || childname[0] == '\0') break;

			wiChild = static_cast<cocos2d::ui::Widget*>(wiChild->getChildByName(childname));
			if (wiChild == nullptr) break;
		}

		va_end(params);

		if (!wiChild) break;

		//�����ӿؼ�
		return wiChild;

	} while (0);
	
	//����ӿؼ�Ϊ������
	CCAssert(false, "@wrong widget child");
	return nullptr;
}



