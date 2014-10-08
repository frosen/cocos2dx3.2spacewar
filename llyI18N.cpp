#include "llyI18N.h"
#include "llyCSVLoad.h"

USING_NS_CC;
using namespace lly;

I18N* I18N::s_I18N = nullptr;

I18N::I18N() : elanguage(ENGLISH)
{

}

I18N::~I18N()
{
 	
}

I18N* I18N::getInstance()
{
	if (nullptr == s_I18N)
	{
		s_I18N = new I18N();
		if (s_I18N && s_I18N->loadStringFromConf(PATH_I18N))
		{
			return s_I18N;
		}
		else
		{
			CC_SAFE_DELETE(s_I18N);
			return nullptr;
		}
	}
	return s_I18N;
}

void I18N::destroyInstance()
{
	CC_SAFE_DELETE(s_I18N);
}

void I18N::setLanguageType( LangType langType )
{
	//清除原来语言
	m_mapString.clear();

	//更新语言
	elanguage = langType;

	//读取配置文件，用新的语言
	loadStringFromConf(PATH_I18N);
}

//=================================
bool I18N::loadStringFromConf( const char* sFilePath )
{
	//读取配置文件
	if (!CSVLoad::getInstance()->loadFile(sFilePath))
	{
		CCLOG("@wrong loadFile(sFilePath) in I18N::loadStringFromConf");
		return false;
	}

	//读取csv列表
	std::vector<std::vector<std::string> > vec = CSVLoad::getInstance()->getCSVFile(sFilePath);

	//列数少于2，则配置文件有误
	int nSize = vec.size();
	if (nSize < 2)
	{
		CCLOG("wrong nColNum in I18N::loadStringFromConf");
		return false;
	}

	std::string StrKey = "";
	std::string StrValue = "";

	//将配置文件的所有字符串放入字典中，忽略第一行
	for (int i = 1; i < nSize; ++i)
	{
		//csv的第一列是ID，第二列是字符串
		StrKey = vec.at(i).at(0);
		StrValue = vec.at(i).at(elanguage);

		m_mapString.insert(std::pair<std::string, std::string>(StrKey, StrValue));
	}

	//释放csv的内存
	CSVLoad::getInstance()->releaseFile(sFilePath);

	return true;
}




