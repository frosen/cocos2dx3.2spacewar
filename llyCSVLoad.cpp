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
	//��һ������ȡ�ļ�Ϊ�ַ���
	string strData = FileUtils::getInstance()->getStringFromFile(sPath);
	if (strData.empty()) return false;

	//�ڶ�������������
	//�е�array����Ϊcsv�е�һ�У�ÿ����\n�ָ�
	vector<vector<string> > vLine;

	//�ַ�����array��ÿ���ַ�������csv�е�һ��Ԫ�أ��Զ��ŷָ�
	vector<string> vStr;

	//�ַ��������ڽ������ַ����ӳ��ַ����󴫸�vector
	char str[1024];
	int p = 0;

	string strTemp;

	/*
	���������Դ˼���ÿһ���ַ���
	����Ƕ��žͽ�֮ǰ���ַ���ϳ�һ���ַ���������vStr����
	�����\n ��֮ǰ��vStr����vLIne
	*/
	unsigned int strSize = (unsigned int)strData.size();
	for (unsigned int i = 0; i < strSize; ++i)
	{
		switch (strData[i])
		{
		case ',':
			{
				//ת��string
				str[p] = '\0';
				strTemp = strData;
				
				//��������
				vStr.push_back(strTemp);

				//���str�е��ַ�
				p = 0;
			}
			break;
		case '\n':
			{
				//�����һ���ַ�������vstr������
				str[p] = '\0';
				strTemp = strData;
				vStr.push_back(strTemp);
				p = 0;

				//�������������arLine������
				vLine.push_back(vStr);

				//��arStr�������
				vStr.clear();
			}
			break;
		default:
			{
				//���ַ����η���str��
				str[p] = strData[i];
				++p;
			}
			break;
		}
	}

	//���Ĳ��������ݷ���map
	m_mapCsv.insert(pair<const char*, std::vector<std::vector<std::string> > >(sPath, vLine));

	return true;
}

const char* CSVLoad::get( int nRow, int nCol, const char* csvFilePath )
{
	do 
	{
		vector<vector<string> > vLine = m_mapCsv.at(csvFilePath);

		//��������ļ������ݲ����ڣ�����������ļ�
		if (vLine.empty())
		{
			CC_BREAK_IF(!loadFile(csvFilePath));			
		}

		/* �±�Խ�� */
		CC_BREAK_IF(nRow >= (int)vLine.size());
		
		/* ��ȡ��nRow������ */
		vector<string> vStr = vLine.at(nRow);
		CC_BREAK_IF(nCol >=  (int)vStr.size());

		/* ��ȡ��iCol������ */
		string str = vStr.at(nCol);
		
		//����
		return str.c_str();
	} while (0);
	
	//����
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

		//��������ļ������ݲ����ڣ�����������ļ�
		if (vLine.empty())
		{
			CC_BREAK_IF(!loadFile(csvFilePath));			
		}

		//��������
		int nRowNum = vLine.size();

		//��ȡ��0������
		vector<string> vStr = vLine.at(0);
		int nColNum = vStr.size();

		return Size(nColNum, nRowNum);

	} while (0);

	//����
	CCLOG("wrong in CsvLoad::getFileRowColNum");
	return Size::ZERO;
}

const int CSVLoad::findValueInWhichLine( const char* chValue, int nValueCol, const char* csvFilePath )
{
	do 
	{
		vector<vector<string> > vLine = m_mapCsv.at(csvFilePath);

		//��������ļ������ݲ����ڣ�����������ļ�
		CC_BREAK_IF(vLine.empty() && !loadFile(csvFilePath));

		/* �±�Խ�� */
		CC_BREAK_IF(vLine.size() <= 0);
		
		vector<string> vStr;
		for(int i = 0; i <  (int)vLine.size(); ++i) 
		{
			vStr = vLine.at(i);

			if ( (int)vStr.size() <= nValueCol) continue; //���������ٵĻ��������һ��

			const char* str = vStr.at(nValueCol).c_str(); 
			if(strcmp(str, chValue) == 0) return i; //�ԱȺ��ʺ󣬷�������
		}

	} while (0);
	
	//����
	CCLOG("wrong in CsvLoad::findValueInWhichLine");
	return -1;
}


