/*
名称：llyCSVLoader.h
描述：Csv文件读取
作者：乐颜
日期：2013.12.23
*/

#ifndef _CSV_LOADER_H_
#define _CSV_LOADER_H_

#include "cocos2d.h"

namespace lly {

class CSVLoader : public cocos2d::Ref
{
public:
	CSVLoader();
	virtual ~CSVLoader();

	//单例
	static CSVLoader* getInstance();
	static void destroyInstance();

	//加载配置文件
	bool loadFile(const char* sPath);

	//释放配置文件
	void releaseFile(const char* sPath);
    
	//获取某个csv列表
	std::vector<std::vector<unsigned char*> > getCSVFile(const char* csvFilePath);

	//获取某行某列的值
	const unsigned char* get(int nRow, int nCol, const char* csvFilePath);

	//获取某行某列的值，并且转换为整型
	const int getInt(int nRow, int nCol, const char* csvFilePath);

	//获取某行某列的值，并且转换为浮点型
	const float getFloat(int nRow, int nCol, const char* csvFilePath);

	//获取配置文件数据的行和列数量
	const cocos2d::Size getFileRowColNum(const char* csvFilePath);

	/* 根据某个列的值，查找该值所在的行 */
	const int findValueInWhichLine(const char* chValue, int nValueCol, const char* csvFilePath);

protected:
	static CSVLoader* S_CsvLoader;

    //csv读取后作为一个字符串储存在此，而vec里是对字符串各个位置的引用
    std::map<const char*, unsigned char*> m_mapCsvHold;
    std::map<const char*, std::vector<std::vector<unsigned char*> > > m_mapCsv;
};

} //lly

#endif //_CSV_LOAD_H_




