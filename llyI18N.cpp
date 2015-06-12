#include "llyI18N.h"
#include "llyCSVLoader.h"

USING_NS_CC;
using namespace lly;

I18N* I18N::S_I18N = nullptr;
I18N::LangType I18N::S_elanguage = I18N::LangType::ENGLISH;

I18N::I18N()
{

}

I18N::~I18N()
{

}

I18N* I18N::getInstance()
{
	if (nullptr == S_I18N)
	{
        S_I18N = new (std::nothrow) I18N();
		if (S_I18N && S_I18N->init())
		{
			return S_I18N;
		}
		else
		{
			CC_SAFE_DELETE(S_I18N);
			return nullptr;
		}
	}
	return S_I18N;
}

void I18N::destroyInstance()
{
	CC_SAFE_DELETE(S_I18N);
}

void I18N::setLanguageType( LangType langType )
{
    //更新语言
    S_elanguage = langType;
    
    //如果已经读取就重新读取
    if (S_I18N) S_I18N->setLanguageType_(S_elanguage);
}

//=================================
bool I18N::init()
{
    S_elanguage = (I18N::LangType)((int)cocos2d::Application::getInstance()->getCurrentLanguage() + 1);
    
    return loadStringFromConf(PATH_I18N);
}

bool I18N::loadStringFromConf( const char* sFilePath )
{
	//读取配置文件
	if (!CSVLoader::getInstance()->loadFile(sFilePath))
	{
		CCLOG("@wrong loadFile(sFilePath) in I18N::loadStringFromConf");
		return false;
	}

	//读取csv列表
	std::vector<std::vector<unsigned char*> > vec = CSVLoader::getInstance()->getCSVFile(sFilePath);

	//列数少于2，则配置文件有误
	int nSize = (int)vec.size();
	if (nSize < 2)
	{
		CCLOG("wrong nColNum in I18N::loadStringFromConf");
		return false;
	}

    std::vector<unsigned char*> vLine;
	unsigned char* StrKey;
	unsigned char* StrValue;

	//将配置文件的所有字符串放入字典中，忽略第一行
	for (int i = 1; i < nSize; ++i)
	{
		//csv的第一列是ID，第二列是字符串
        vLine = std::move(vec.at(i));
		StrKey = vLine.at(0);
		StrValue = vLine.at((int)S_elanguage);

		m_mapString.insert(std::pair<std::string, std::string>(
            std::string((char*)StrKey), std::string((char*)StrValue)));
	}

	//释放csv的内存
	CSVLoader::getInstance()->releaseFile(sFilePath);

	return true;
}

void I18N::setLanguageType_( LangType langType )
{
    //清除原来语言
    m_mapString.clear();
    
    //读取配置文件，用新的语言
    loadStringFromConf(PATH_I18N);
}


