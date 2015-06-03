/*
名称：I18N.h
描述：国际化字符串工具
作者：乐颜
日期：2013.12.24

//////////////需要定义PATH_I18N_PUBLIC：csv文件路径//////////////////

//////////////需要 include "CsvLoad.h"//////////////////
*/

#ifndef _I18N_H_
#define _I18N_H_

#include "cocos2d.h"

// 文件格式如下：
// id    english    chinese
// name1  Tom        汤姆
// name2  Jake       杰克
#ifndef PATH_I18N
#define PATH_I18N "i18n/i18n.csv"
#endif

#ifndef i18n
#define i18n(_str_) I18N::getInstance()->getString(_str_)
#endif

namespace lly {

class I18N : public cocos2d::Ref
{
public:
	enum class LangType
	{
		ENGLISH = 1, //1对应第二列，因为第一列是id
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
	virtual ~I18N();

	//单例
	static I18N* getInstance();
	static void destroyInstance();
    
    //变更语言 默认为1
    static void setLanguageType(LangType langType);
    static LangType getLanguageType() { return S_elanguage; }

	//根据key获取字符串，返回char对象
	std::string getString(const char* ch) { return m_mapString.at(ch); }

protected:
	//从配置文件中读取字符串，放置于字典里
	bool loadStringFromConf(const char* sFilePath);
    
    void setLanguageType_(LangType langType);

protected:
	static I18N* S_I18N;

	//字典： ID，字符串 无序map的效率更高
	std::unordered_map<std::string, std::string> m_mapString;

	//语言选择
	static LangType S_elanguage;
};

} //lly

#endif //_I18N_H_




