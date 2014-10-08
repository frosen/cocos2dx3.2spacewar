/*
���ƣ�llyCSVLoad.h
������Csv�ļ���ȡ
���ߣ�����
���ڣ�2013.12.23
*/

#ifndef _CSV_LOAD_H_
#define _CSV_LOAD_H_

#include "cocos2d.h"

namespace lly {

class CSVLoad : public cocos2d::Ref
{
public:
	CSVLoad();
	virtual ~CSVLoad();

	//����
	static CSVLoad* getInstance();
	static void destroyInstance();

	//���������ļ� 
	bool loadFile(const char* sPath);

	//�ͷ������ļ�
	void releaseFile(const char* sPath) { m_mapCsv.erase(sPath); }

	//��ȡĳ��csv�б�
	std::vector<std::vector<std::string> > getCSVFile(const char* csvFilePath)
		 { return m_mapCsv.at(csvFilePath); }

	//��ȡĳ��ĳ�е�ֵ 
	const char* get(int nRow, int nCol, const char* csvFilePath);

	//��ȡĳ��ĳ�е�ֵ������ת��Ϊ����
	const int getInt(int nRow, int nCol, const char* csvFilePath);

	//��ȡĳ��ĳ�е�ֵ������ת��Ϊ������
	const float getFloat(int nRow, int nCol, const char* csvFilePath);

	//��ȡ�����ļ����ݵ��к�������
	const cocos2d::Size getFileRowColNum(const char* csvFilePath);

	/* ����ĳ���е�ֵ�����Ҹ�ֵ���ڵ��� */
	const int findValueInWhichLine(const char* chValue, int nValueCol, const char* csvFilePath);

protected:
	static CSVLoad* m_CsvLoad;

	std::map<const char*, std::vector<std::vector<std::string> > > m_mapCsv;
};

} //lly

#endif //_CSV_LOAD_H_




