/*
	名称：llyFileUtils.h
	描述：文件管理函数，暂不支持中文路径和文件名，有跨平台处理
	作者：乐颜
	日期：2014.7.30
*/

#ifndef _LLY_FILE_UTILS_H_
#define _LLY_FILE_UTILS_H_

#include <string>
#include <stdint.h>
#include <map>

namespace lly {

class FileUtils
{
public:
	//===============操作文件夹=====================
	//获取某路径下所有的文件和文件夹名称，或者所有某类文件（扩展名相同）
	//输入一个string文件夹，一个map<string文件名, bool 是否是文件夹>的引用，和一个扩展名（不包括".", 默认没有）
	//返回是否成功
	static bool searchFolder(
		const char* DirPath, 
		std::map<std::string, bool> &mapstrb,
		const char* extension = nullptr);

	//检查一个文件夹是否存在
	static bool isFolderExist(const char* DirPath);

	//生成文件夹 //输入文件夹路径，返回成功与否	
	static bool createFolder(const char* DirPath);

	//删除文件夹和里面所有文件
	static bool deleteFolder(const char* DirPath);

	//===============操作文件=======================
	//检查一个文件是否存在
	static bool isFileExist(const char* DirPath);

	//申请内存，读取指定文件特定位置的指定长度   需要free
	//	pnLength			返回文件长度，文件末尾添加了一个0字符，但长度没有包含0，NULL时不返回长度
	//	nReservedTop		返回缓冲区前保留多少字节，默认为0
	//	nReservedTail		返回缓冲区后保留多少字节，默认为0
	//	nStartPoint			文件起始位置，默认为0
	//	nReadLength			读取长度，默认为0，表示读取全部内容
	static char* readFile_needfree(const char* pszFile, uint32_t* pnLength = nullptr, 
		uint32_t nReservedTop = 0, uint32_t nReservedTail = 0, 
		uint32_t nStartPoint = 0, uint32_t nReadLength = 0);

	//不需要free的读取文件
	static std::string readFile(const char* pszFile, uint32_t* pnLength = nullptr, 
		uint32_t nReservedTop = 0, uint32_t nReservedTail = 0, 
		uint32_t nStartPoint = 0, uint32_t nReadLength = 0);
	
	//写入一个文件，若不存在就自动生成
	static bool writeFile(const char* pszFile, const char* pData, int len);

	//获取文件长度
	static int getFileLength(const char* pszFile);

	//删除文件
	static bool deleteFile(const char *pszFile);

	//===============操作所有=======================
	//修改文件的目录，可以实现移动或者更名，对文件，文件夹都有效
	//不检测是否有重名，有的话windows下失败，linux下覆盖
	static bool move(const char* pszPath, const char* aimPathName);

	//复制文件到新的文件夹内
	static bool copy(const char* pszPath, const char* aimPathName);

	//获取扩展名或者全路径下的文件名
	static const char* getLastCharacters(const char* s, char ch = '.');

	//让路径的末尾为 / 
	static std::string setLastCharIsSlash(const char* ch);
};

} //lly

#endif //_LLY_FILE_UTILS_H_



