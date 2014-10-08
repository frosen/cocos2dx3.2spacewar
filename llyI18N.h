/*
���ƣ�I18N.h
���������ʻ��ַ�������
���ߣ�����
���ڣ�2013.12.24

//////////////��Ҫ����PATH_I18N_PUBLIC��csv�ļ�·��//////////////////

//////////////��Ҫ include "ToolCsvLoad.h"//////////////////
*/

#ifndef _I18N_H_
#define _I18N_H_

#include "cocos2d.h"

#ifndef PATH_I18N
#define PATH_I18N "i18n.csv"
#endif

#ifndef i18n
#define i18n(_str_) I18N::getInstance()->getString(_str_)
#endif

namespace lly {

class I18N : public cocos2d::Ref
{
public:
	enum LangType
	{
		ENGLISH = 1,
		CHINESE,
		FRENCH,
		ITALIAN,
		GERMAN,
		SPANISH,
		DUTCH,
		RUSSIAN,
		KOREAN,
		JAPANESE,
		HUNGARIAN,
		PORTUGUESE,
		ARABIC,
		NORWEGIAN,
		POLISH
	};

	I18N();
	~I18N();

	//����
	static I18N* getInstance();
	static void destroyInstance();

	//����key��ȡ�ַ���������char����
	std::string getString(const char* ch) { return m_mapString.at(ch); }

	//������� Ĭ��Ϊ1
	void setLanguageType(LangType langType);
	LangType getLanguageType() { return elanguage; }

protected:
	//�������ļ��ж�ȡ�ַ������������ֵ���
	bool loadStringFromConf(const char* sFilePath);

protected:
	static I18N* s_I18N;

	//�ֵ䣺 ID���ַ���
	std::map<std::string, std::string> m_mapString;

	//����ѡ��
	LangType elanguage;
};

} //lly

#endif //_I18N_H_




