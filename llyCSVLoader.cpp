#include "llyCSVLoader.h"

USING_NS_CC;
using namespace std;
using namespace lly;

CSVLoader* CSVLoader::S_CsvLoader = nullptr;

CSVLoader::CSVLoader()
{

}

CSVLoader::~CSVLoader()
{

}

CSVLoader* CSVLoader::getInstance()
{
	if (!S_CsvLoader)
	{
		S_CsvLoader = new CSVLoader();
	}
	return S_CsvLoader;
}

void CSVLoader::destroyInstance()
{
	CC_SAFE_DELETE(S_CsvLoader);
}

bool CSVLoader::loadFile( const char* sPath )
{
	//第一步，读取文件为字符串
    ssize_t size;
	unsigned char* strData = FileUtils::getInstance()->getFileData(sPath, "rt", &size);
	if (size < 1)
    {
        CCLOG("loadfile size is 0");
        return false;
    }

	//第二步，建立容器
	//行的array，行为csv中的一行，每行以\n分隔
	vector<vector<unsigned char*> > vLine;

	//字符串的array，每个字符串都是csv中的一个元素，以逗号分隔
	vector<unsigned char*> vStr;

	//字符串，用于将单个字符连接成字符串后传给vector
	int pFrom = 0;

	/*
	第三步，以此检索每一个字符，
	如果是逗号就将之前的字符结合成一个字符串，放入vStr里面
	如果是\n 则将之前的vStr放入vLIne
	*/

	for (int i = 0; i < (int)size; ++i)
	{
		switch (strData[i])
		{
		case ',':
			{
                strData[i] = '\0';
                
				//将从from到i的子字符串加入数组
				vStr.push_back(strData + pFrom);

				//下一个字符
                pFrom = i + 1;
			}
			break;
		case '\n':
			{
                strData[i] = '\0';
                
				//把最后一组字符串放入vstr数组中
                vStr.push_back(strData + pFrom);
                
                //下一个字符
                pFrom = i + 1;

				//将整个数组放入arLine数组中
				vLine.push_back(vStr);

				//将arStr数组清空
				vStr.clear();
			}
			break;
		default:
        
			break;
		}
	}

	//第四步，将数据放入map
    m_mapCsvHold.insert(pair<const char*, unsigned char*>(sPath, strData));
	m_mapCsv.insert(pair<const char*, vector<vector<unsigned char*> > >(sPath, vLine));

	return true;
}

void CSVLoader::releaseFile(const char* sPath)
{
    unsigned char* str = m_mapCsvHold.at(sPath);
    if (str) free(str);
    
    m_mapCsvHold.erase(sPath);
    m_mapCsv.erase(sPath);
}

std::vector<std::vector<unsigned char*> > CSVLoader::getCSVFile(const char* csvFilePath)
{
     return m_mapCsv.at(csvFilePath);
}

const unsigned char* CSVLoader::get( int nRow, int nCol, const char* csvFilePath )
{
	do
	{
		vector<vector<unsigned char*> > vLine = m_mapCsv.at(csvFilePath);

		//如果配置文件的数据不存在，则加载配置文件
		if (vLine.empty())
		{
			CC_BREAK_IF(!loadFile(csvFilePath));
		}

		/* 下标越界 */
		CC_BREAK_IF(nRow >= (int)vLine.size());

		/* 获取第nRow行数据 */
		vector<unsigned char*> vStr = vLine.at(nRow);
		CC_BREAK_IF(nCol >=  (int)vStr.size());

		/* 获取第iCol列数据 */
		unsigned char* str = vStr.at(nCol);

		//返回
		return str;
	} while (0);

	//错误
	CCLOG("wrong in CsvLoader::get");
	return nullptr;
}

const int CSVLoader::getInt( int nRow, int nCol, const char* csvFilePath )
{
	const unsigned char* str = get(nRow, nCol, csvFilePath);
	int num;
	sscanf((const char*)str, "%d", &num);
	return num;

}

const float CSVLoader::getFloat( int nRow, int nCol, const char* csvFilePath )
{
	const unsigned char* str = get(nRow, nCol, csvFilePath);
	float num;
	sscanf((const char*)str, "%f", &num);
	return num;
}

const Size CSVLoader::getFileRowColNum( const char* csvFilePath )
{
	do
	{
		vector<vector<unsigned char*> > vLine = m_mapCsv.at(csvFilePath);

		//如果配置文件的数据不存在，则加载配置文件
		if (vLine.empty())
		{
			CC_BREAK_IF(!loadFile(csvFilePath));
		}

		//数据行数
		int nRowNum = (int)vLine.size();

		//获取第0行数据
		vector<unsigned char*> vStr = vLine.at(0);
		int nColNum = (int)vStr.size();

		return Size(nColNum, nRowNum);

	} while (0);

	//错误
	CCLOG("wrong in CsvLoader::getFileRowColNum");
	return Size::ZERO;
}

const int CSVLoader::findValueInWhichLine( const char* chValue, int nValueCol, const char* csvFilePath )
{
	do
	{
		vector<vector<unsigned char*> > vLine = m_mapCsv.at(csvFilePath);

		//如果配置文件的数据不存在，则加载配置文件
		CC_BREAK_IF(vLine.empty() && !loadFile(csvFilePath));

		/* 下标越界 */
		CC_BREAK_IF(vLine.size() <= 0);

		vector<unsigned char*> vStr;
		for(int i = 0; i <  (int)vLine.size(); ++i)
		{
			vStr = vLine.at(i);

			if ( (int)vStr.size() <= nValueCol) continue; //本行列数少的话则进入下一行

			const unsigned char* str = vStr.at(nValueCol);
			if(strcmp((const char*)str, chValue) == 0) return i; //对比合适后，返回行数
		}

	} while (0);

	//错误
	CCLOG("wrong in CsvLoader::findValueInWhichLine");
	return -1;
}


