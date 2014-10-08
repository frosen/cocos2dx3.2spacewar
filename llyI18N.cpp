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
	//���ԭ������
	m_mapString.clear();

	//��������
	elanguage = langType;

	//��ȡ�����ļ������µ�����
	loadStringFromConf(PATH_I18N);
}

//=================================
bool I18N::loadStringFromConf( const char* sFilePath )
{
	//��ȡ�����ļ�
	if (!CSVLoad::getInstance()->loadFile(sFilePath))
	{
		CCLOG("@wrong loadFile(sFilePath) in I18N::loadStringFromConf");
		return false;
	}

	//��ȡcsv�б�
	std::vector<std::vector<std::string> > vec = CSVLoad::getInstance()->getCSVFile(sFilePath);

	//��������2���������ļ�����
	int nSize = vec.size();
	if (nSize < 2)
	{
		CCLOG("wrong nColNum in I18N::loadStringFromConf");
		return false;
	}

	std::string StrKey = "";
	std::string StrValue = "";

	//�������ļ��������ַ��������ֵ��У����Ե�һ��
	for (int i = 1; i < nSize; ++i)
	{
		//csv�ĵ�һ����ID���ڶ������ַ���
		StrKey = vec.at(i).at(0);
		StrValue = vec.at(i).at(elanguage);

		m_mapString.insert(std::pair<std::string, std::string>(StrKey, StrValue));
	}

	//�ͷ�csv���ڴ�
	CSVLoad::getInstance()->releaseFile(sFilePath);

	return true;
}




