#include "llyCSVLoad.h"

USING_NS_CC;
using namespace std;
using namespace lly;

CSVLoad* CSVLoad::m_CsvLoad = nullptr;

CSVLoad::CSVLoad()
{

}

CSVLoad::~CSVLoad()
{

}

CSVLoad* CSVLoad::getInstance()
{
	if (!m_CsvLoad)
	{
		m_CsvLoad = new CSVLoad();
	}
	return m_CsvLoad;
}

void CSVLoad::destroyInstance()
{
	CC_SAFE_DELETE(m_CsvLoad);
}

bool CSVLoad::loadFile( const char* sPath )
{
	//第一步，读取文件为字符串
	string strData = FileUtils::getInstance()->getStringFromFile(sPath);
	if (strData.empty()) return false;

	//第二步，建立容器
	//行的array，行为csv中的一行，每行以\n分隔
	vector<vector<string> > vLine;

	//字符串的array，每个字符串都是csv中的一个元素，以逗号分隔
	vector<string> vStr;

	//字符串，用于将单个字符连接成字符串后传给vector
	char str[1024];
	int p = 0;

	string strTemp;

	/*
	第三步，以此检索每一个字符，
	如果是逗号就将之前的字符结合成一个字符串，放入vStr里面
	如果是\n 则将之前的vStr放入vLIne
	*/
	unsigned int strSize = (unsigned int)strData.size();
	for (unsigned int i = 0; i < strSize; ++i)
	{
		switch (strData[i])
		{
		case ',':
			{
				//转成string
				str[p] = '\0';
				strTemp = strData;
				
				//加入数组
				vStr.push_back(strTemp);

				//清空str中的字符
				p = 0;
			}
			break;
		case '\n':
			{
				//把最后一组字符串放入vstr数组中
				str[p] = '\0';
				strTemp = strData;
				vStr.push_back(strTemp);
				p = 0;

				//将整个数组放入arLine数组中
				vLine.push_back(vStr);

				//将arStr数组清空
				vStr.clear();
			}
			break;
		default:
			{
				//将字符依次放入str中
				str[p] = strData[i];
				++p;
			}
			break;
		}
	}

	//第四步，将数据放入map
	m_mapCsv.insert(pair<const char*, std::vector<std::vector<std::string> > >(sPath, vLine));

	return true;
}

const char* CSVLoad::get( int nRow, int nCol, const char* csvFilePath )
{
	do 
	{
		vector<vector<string> > vLine = m_mapCsv.at(csvFilePath);

		//如果配置文件的数据不存在，则加载配置文件
		if (vLine.empty())
		{
			CC_BREAK_IF(!loadFile(csvFilePath));			
		}

		/* 下标越界 */
		CC_BREAK_IF(nRow >= (int)vLine.size());
		
		/* 获取第nRow行数据 */
		vector<string> vStr = vLine.at(nRow);
		CC_BREAK_IF(nCol >=  (int)vStr.size());

		/* 获取第iCol列数据 */
		string str = vStr.at(nCol);
		
		//返回
		return str.c_str();
	} while (0);
	
	//错误
	CCLOG("wrong in CsvLoad::get");
	return "";
}

const int CSVLoad::getInt( int nRow, int nCol, const char* csvFilePath )
{
	const char* str = get(nRow, nCol, csvFilePath);
	int num;
	sscanf(str, "%d", &num);
	return num;

}

const float CSVLoad::getFloat( int nRow, int nCol, const char* csvFilePath )
{
	const char* str = get(nRow, nCol, csvFilePath);
	float num;
	sscanf(str, "%f", &num);
	return num;
}

const Size CSVLoad::getFileRowColNum( const char* csvFilePath )
{
	do 
	{
		vector<vector<string> > vLine = m_mapCsv.at(csvFilePath);

		//如果配置文件的数据不存在，则加载配置文件
		if (vLine.empty())
		{
			CC_BREAK_IF(!loadFile(csvFilePath));			
		}

		//数据行数
		int nRowNum = vLine.size();

		//获取第0行数据
		vector<string> vStr = vLine.at(0);
		int nColNum = vStr.size();

		return Size(nColNum, nRowNum);

	} while (0);

	//错误
	CCLOG("wrong in CsvLoad::getFileRowColNum");
	return Size::ZERO;
}

const int CSVLoad::findValueInWhichLine( const char* chValue, int nValueCol, const char* csvFilePath )
{
	do 
	{
		vector<vector<string> > vLine = m_mapCsv.at(csvFilePath);

		//如果配置文件的数据不存在，则加载配置文件
		CC_BREAK_IF(vLine.empty() && !loadFile(csvFilePath));

		/* 下标越界 */
		CC_BREAK_IF(vLine.size() <= 0);
		
		vector<string> vStr;
		for(int i = 0; i <  (int)vLine.size(); ++i) 
		{
			vStr = vLine.at(i);

			if ( (int)vStr.size() <= nValueCol) continue; //本行列数少的话则进入下一行

			const char* str = vStr.at(nValueCol).c_str(); 
			if(strcmp(str, chValue) == 0) return i; //对比合适后，返回行数
		}

	} while (0);
	
	//错误
	CCLOG("wrong in CsvLoad::findValueInWhichLine");
	return -1;
}


