/*
	���ƣ�llyFileUtils.h
	�������ļ����������ݲ�֧������·�����ļ������п�ƽ̨����
	���ߣ�����
	���ڣ�2014.7.30
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
	//===============�����ļ���=====================
	//��ȡĳ·�������е��ļ����ļ������ƣ���������ĳ���ļ�����չ����ͬ��
	//����һ��string�ļ��У�һ��map<string�ļ���, bool �Ƿ����ļ���>�����ã���һ����չ����������".", Ĭ��û�У�
	//�����Ƿ�ɹ�
	static bool searchFolder(
		const char* DirPath, 
		std::map<std::string, bool> &mapstrb,
		const char* extension = nullptr);

	//���һ���ļ����Ƿ����
	static bool isFolderExist(const char* DirPath);

	//�����ļ��� //�����ļ���·�������سɹ����	
	static bool createFolder(const char* DirPath);

	//ɾ���ļ��к����������ļ�
	static bool deleteFolder(const char* DirPath);

	//===============�����ļ�=======================
	//���һ���ļ��Ƿ����
	static bool isFileExist(const char* DirPath);

	//�����ڴ棬��ȡָ���ļ��ض�λ�õ�ָ������   ��Ҫfree
	//	pnLength			�����ļ����ȣ��ļ�ĩβ�����һ��0�ַ���������û�а���0��NULLʱ�����س���
	//	nReservedTop		���ػ�����ǰ���������ֽڣ�Ĭ��Ϊ0
	//	nReservedTail		���ػ��������������ֽڣ�Ĭ��Ϊ0
	//	nStartPoint			�ļ���ʼλ�ã�Ĭ��Ϊ0
	//	nReadLength			��ȡ���ȣ�Ĭ��Ϊ0����ʾ��ȡȫ������
	static char* readFile_needfree(const char* pszFile, uint32_t* pnLength = nullptr, 
		uint32_t nReservedTop = 0, uint32_t nReservedTail = 0, 
		uint32_t nStartPoint = 0, uint32_t nReadLength = 0);

	//����Ҫfree�Ķ�ȡ�ļ�
	static std::string readFile(const char* pszFile, uint32_t* pnLength = nullptr, 
		uint32_t nReservedTop = 0, uint32_t nReservedTail = 0, 
		uint32_t nStartPoint = 0, uint32_t nReadLength = 0);
	
	//д��һ���ļ����������ھ��Զ�����
	static bool writeFile(const char* pszFile, const char* pData, int len);

	//��ȡ�ļ�����
	static int getFileLength(const char* pszFile);

	//ɾ���ļ�
	static bool deleteFile(const char *pszFile);

	//===============��������=======================
	//�޸��ļ���Ŀ¼������ʵ���ƶ����߸��������ļ����ļ��ж���Ч
	//������Ƿ����������еĻ�windows��ʧ�ܣ�linux�¸���
	static bool move(const char* pszPath, const char* aimPathName);

	//�����ļ����µ��ļ�����
	static bool copy(const char* pszPath, const char* aimPathName);

	//��ȡ��չ������ȫ·���µ��ļ���
	static const char* getLastCharacters(const char* s, char ch = '.');

	//��·����ĩβΪ / 
	static std::string setLastCharIsSlash(const char* ch);
};

} //lly

#endif //_LLY_FILE_UTILS_H_



