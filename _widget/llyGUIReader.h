/*
���ƣ�llyGUIReader.h
���ݣ��Զ����cocostudio�ļ���ȡ�����������ļ�����ǰ����EX���Ϳ���ʹ�ö�Ӧ���¿ؼ�
		�Զ���ؼ���Ҫ��ǰע�ᵽ��ȡ����
		����ؼ�����ǰ����PO����˿ؼ������븸���У����ǻ����xy��������б�

������¬����
���ڣ�2014.5.1
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
	static GUIReader* getInstance(); //����
	static void destroyInstance(); //����
	cocos2d::ui::Widget* widgetFromJsonFile(const char* fileName); //����

	//�Զ���Ŀؼ�ע�ᵽreader��
	//ָ���������ƣ����ʹ�������ָ�룬���ͽ�������ָ�룬���������ʶ�ַ�
	//����name��ͷΪEX��Ϊ�Զ����࣬EX������ŵ��ַ�Ϊ�����ʶ��Ĭ��Ϊ _ ��
	void registerTypeWithExName(const std::string& classType,
		cocos2d::ObjectFactory::Instance TypeIns,
		cocos2d::ObjectFactory::Instance ReaderIns,
		char ch = '_'); 

	//����json�ļ���������PO��ͷ�Ŀؼ�����¼���ǵ�position���Թ�������ʹ��
	void recordPointInMap(const char* name, cocos2d::Vec2 &point);

	//�������ƣ���ȡ�����
	cocos2d::Vec2 getPoint(const char* name);

protected:
	GUIReader();
	virtual ~GUIReader();

protected:
	std::map<std::string, cocos2d::Vec2> m_mapPoint; //����������map
};

class WidgetPropertiesReader0300 : public cocostudio::WidgetPropertiesReader0300
{
public:
	WidgetPropertiesReader0300(){};
	virtual ~WidgetPropertiesReader0300(){};

	virtual cocos2d::ui::Widget* widgetFromJsonDictionary(const rapidjson::Value& dic); //����
};

//=============================
//���ٻ��cocostudio�Ŀؼ�
void setWidgetFast(cocos2d::ui::Widget* widget, cocos2d::ui::Widget* father, char* childname, ...);

//���ذ汾�����ؼ����Ӵ����ص�
void setWidgetFast(cocos2d::ui::Widget* widget, cocos2d::ui::Widget* father, cocos2d::ui::Widget::ccWidgetTouchCallback callback, char* childname, ...);

//���ذ汾�����ؼ����Ӵ����ص�������Ҫ���ؼ�����һ��ָ�������
void setWidgetFast(cocos2d::ui::Widget* father, cocos2d::ui::Widget::ccWidgetTouchCallback callback, char* childname, ...);

//��ѡ��, �ص��Ǹ�ѡ��Ļص�
void setCheckBoxFast(cocos2d::ui::Widget* widget, cocos2d::ui::Widget* father, cocos2d::ui::CheckBox::ccCheckBoxCallback callback, char* childname, ...);

//����ѡ�����Ӵ����ص����ص��Ǹ�ѡ��Ļص�������Ҫ���ؼ�����һ��ָ�������
void setCheckBoxFast(cocos2d::ui::Widget* father, cocos2d::ui::CheckBox::ccCheckBoxCallback callback, char* childname, ...);

//��ȡ�ӿؼ������ݸ���͸�����ÿ���ӿؼ�������
//������������
cocos2d::ui::Widget* getChildWidget(cocos2d::ui::Widget* father, char* pch, ...);

} // namespace lly

#endif // _LLY_GUI_READER_H_

