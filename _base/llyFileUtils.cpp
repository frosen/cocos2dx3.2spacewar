#include "llyFileUtils.h"

#ifdef WIN32
#include <io.h>
#include <windows.h>
#else
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#endif

using namespace std;
using namespace lly;

bool lly::FileUtils::searchFolder( const char* DirPath, std::map<std::string, bool> &mapstrb, const char* extension/* = nullptr*/ )
{
#ifdef WIN32
	_finddata_t FileInfo;

	string newPath = setLastCharIsSlash(DirPath);
	
	string strfind = newPath + '*';
	long Handle = _findfirst(strfind.c_str(), &FileInfo);
	if (Handle == -1L) return false;

	string childPath = "";

	do
	{
		if (!extension) //�������Ҫƥ����չ��
		{
			childPath = newPath + FileInfo.name;

			//�ж��Ƿ�����Ŀ¼
			if (FileInfo.attrib & _A_SUBDIR)    
			{
				childPath += '/';
				mapstrb.insert(pair<string, bool>(childPath, true));
			}
			else  
			{
				mapstrb.insert(pair<string, bool>(childPath, false));
			}
		}
		else //����չ���Ĳ�����Ŀ¼
		{
			const char* curEx = getLastCharacters(FileInfo.name);

			if (!curEx || strcmp(curEx, extension) != 0) continue; //��չ����һ��ʱ���������б�
			
			childPath = newPath + FileInfo.name;
			mapstrb.insert(pair<string, bool>(childPath, false));
		}
	} while (_findnext(Handle, &FileInfo) == 0);

	_findclose(Handle);

#else
	DIR* dp;
	if((dp = opendir(DirPath)) == nullptr) return false;

	struct dirent* entry;
	struct stat statbuf;

	chdir(DirPath);

	string basePath = setLastCharIsSlash(DirPath);
	string newPath = "";

	while((entry = readdir(dp)) != nullptr) 
	{
		if (!extension) //�������Ҫƥ����չ��
		{
			newPath = basePath;
			newPath += entry->d_name;

			lstat(entry->d_name, &statbuf);
			if(S_ISDIR(statbuf.st_mode)) //�ж��Ƿ����ļ���
			{
				newPath += "/";
				mapstrb.insert(pair<string, bool>(newPath, true));
			} 
			else 
			{
				mapstrb.insert(pair<string, bool>(newPath, false));
			}
		}
		else //����չ���Ĳ�����Ŀ¼
		{
			const char* curEx = getLastCharacters(FileInfo.name);
			if (!curEx || strcmp(curEx, extension) != 0) continue; //��չ����һ��ʱ���������б�
			
			newPath = basePath;
			newPath += entry->d_name;
			mapstrb.insert(pair<string, bool>(newPath, false));
		}	
	}

	chdir("..");
	closedir(dp);

#endif
	return true;
}

bool lly::FileUtils::isFolderExist( const char* DirPath )
{
#ifdef WIN32
	DWORD d = GetFileAttributesA(DirPath);
	if (d == INVALID_FILE_ATTRIBUTES || (d & FILE_ATTRIBUTE_DIRECTORY) == 0)
		return false;
	else
		return true;
#else    
	if (opendir(DirPath))
		return true;
	else
		return false;
#endif
}

bool lly::FileUtils::createFolder( const char* DirPath )
{
#ifdef WIN32
	BOOL ret = CreateDirectoryA(DirPath, nullptr);
	if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
		return false;
	else
		return true;
#else
	mode_t processMask = umask(0); //��ȡlinux��Ȩ������
	int ret = mkdir(DirPath, S_IRWXU | S_IRWXG | S_IRWXO);
	umask(processMask);
	if (ret != 0 && (errno != EEXIST))
		return false;
	else
		return true;
#endif
}

bool lly::FileUtils::deleteFolder( const char* DirPath )
{
#ifdef WIN32
	string command = "rd /s /q "; // Path may include space.	������
	command += "\"";
	command += DirPath;
	command += "\"";
	return system(command.c_str()) == 0 ? true : false;
#else
	string command = "rm -r "; // Path may include space. linux������
	command += "\"";
	command += DirPath;
	command += "\"";
	return system(command.c_str()) == 0 ? true : false;
#endif
}

bool lly::FileUtils::isFileExist( const char* DirPath )
{
#ifdef WIN32
	DWORD d = GetFileAttributesA(DirPath);
	if (d == INVALID_FILE_ATTRIBUTES || (d & FILE_ATTRIBUTE_DIRECTORY) != 0) //�����ڻ������ļ���
		return false;
	else
		return true;
#else    
	bool bExist = false;
	FILE* fp = fopen(DirPath, "r");
	if(fp)
	{
		bExist = true;
		fclose(fp);
	}
	return bExist;
#endif
}

char * lly::FileUtils::readFile_needfree( const char* pszFile, uint32_t* pnLength /*= nullptr*/, uint32_t nReservedTop /*= 0*/, uint32_t nReservedTail /*= 0*/, uint32_t nStartPoint /*= 0*/, uint32_t nReadLength /*= 0*/ )
{
	FILE* fp = fopen(pszFile, "rb");
	if (fp == nullptr) return nullptr;

	do 
	{
		fseek( fp, 0L, SEEK_END);
		uint32_t len = ftell(fp);

		//�ļ����Ȳ���
		if (len <= nStartPoint) break;

		//ʵ����Ҫ��ȡ�ĳ���
		if (nReadLength == 0) 
			nReadLength=len-nStartPoint;
		else 
			nReadLength =  len - nStartPoint < nReadLength ? len - nStartPoint : nReadLength;

		if (nReadLength == 0) break;

		//ĩβ���ٱ���һ���ַ��Ŀռ䣬�Ÿ�\0
		nReservedTail = nReservedTail > 1 ? nReservedTail : 1; 

		char* pBuf = (char*)malloc(len + nReservedTop + nReservedTail);

		if (!pBuf) break;
		
		fseek(fp, (long)nStartPoint, SEEK_SET);
		fread(pBuf + nReservedTop, sizeof(char), nReadLength, fp);

		fclose(fp);

		if (pnLength != nullptr) *pnLength = nReadLength;

		//β�ͼӸ�0���������ʹ��
		pBuf[nReservedTop + nReadLength] = '\0';

		return pBuf;

	} while (0);
	
	fclose(fp);
	return nullptr;
}

std::string lly::FileUtils::readFile( const char* pszFile, uint32_t* pnLength /*= nullptr*/, uint32_t nReservedTop /*= 0*/, uint32_t nReservedTail /*= 0*/, uint32_t nStartPoint /*= 0*/, uint32_t nReadLength /*= 0*/ )
{
	char* c = readFile_needfree(pszFile, pnLength, nReservedTop, nReservedTail, nStartPoint, nReadLength);
	if (!c) return std::string(); 
	std::string str(c);
	free(c);
	return str;
}

bool lly::FileUtils::writeFile( const char* pszFile, const char* pData, int len )
{
	FILE* fp = fopen(pszFile, "wb");
	if (fp == nullptr || pData == nullptr) return false;
	int nRet = fwrite(pData, sizeof(char), len, fp);
	fclose(fp);

	if (nRet != len)
	{
		deleteFile(pszFile);
		return false;
	}
	return true;
}

int lly::FileUtils::getFileLength( const char* pszFile )
{
	FILE* fp = fopen(pszFile, "rb");
	if (fp == nullptr) return 0;

	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	fclose(fp);

	return len;
}

bool lly::FileUtils::deleteFile( const char* pszFile )
{
#ifdef WIN32
	return DeleteFileA(pszFile) ? true : false;
#else
	return (unlink(pszFile) == 0);
#endif
}

bool lly::FileUtils::move( const char* pszPath, const char* aimPathName )
{
#ifdef WIN32 
	return MoveFileA(pszPath, aimPathName) ? true : false;
#else
	string command = "mv "; // Path may include space. linux������ ��֪��Ϊʲô������ -f
	command += pszPath;
	command += " ";
	command += aimPathName;
	return system(command.c_str()) == 0 ? true : false; 
#endif
}

bool lly::FileUtils::copy( const char* pszPath, const char* aimPathName )
{
#ifdef WIN32 
	bool bRet;
	if (GetFileAttributesA(pszPath) & FILE_ATTRIBUTE_DIRECTORY)
	{
		//�����·�������ڣ���ֱ�Ӵ���
		if (!isFolderExist(aimPathName)) 
		{
			bool b = createFolder(aimPathName);
			if (!b) return false;
		}

		_finddata_t FileInfo;

		string strfind = setLastCharIsSlash(pszPath);

		string strfind2 = strfind + "*";
		long Handle = _findfirst(strfind2.c_str(), &FileInfo);

		if (Handle == -1L) return false;

		string childPath = "";
		string newPath = "";

		do
		{
			if((strcmp(FileInfo.name, ".") == 0 ) || (strcmp(FileInfo.name, "..") == 0)) 
				continue; //������Ŀ¼���ϼ�Ŀ¼

			childPath = strfind;
			childPath += FileInfo.name;

			newPath = setLastCharIsSlash(aimPathName);
			newPath += FileInfo.name;
				
			bRet = lly::FileUtils::copy(childPath.c_str(), newPath.c_str()); //ͨ���ݹ鸴��·�������е��ļ����ļ���

		} while (_findnext(Handle, &FileInfo) == 0);

		_findclose(Handle);
	}
	else //���ļ��Ļ�ֱ�Ӹ���
	{
		bRet = CopyFileA(pszPath, aimPathName, FALSE) ? true : false;
	}	
	return bRet;

#elif defined(ANDROID) //��׿��û��cp����
	bool bRet = false;
	DIR* dp;
	if ((dp = opendir(pszPath)) != nullptr) //�����һ���ļ���
	{
		//�����·�������ڣ���ֱ�Ӵ���
		if (!opendir(aimPathName))
		{
			bool b = createFolder(aimPathName);
			if (!b) return false;
		}

		struct dirent* entry;
		struct stat statbuf;

		chdir(pszPath);

		string basePath = setLastCharIsSlash(pszPath);
		string newPath = "";
		string newPathAim = "";

		while((entry = readdir(dp)) != nullptr) 
		{
			if((strcmp(entry->d_name, ".") == 0 ) || (strcmp(entry->d_name, "..") == 0)) 
				continue; //������Ŀ¼���ϼ�Ŀ¼

			newPath = basePath;
			newPath += entry->d_name;

			newPathAim = setLastCharIsSlash(aimPathName);
			newPathAim += entry->d_name;

			bRet = lly::FileUtils::copy(newPath.c_str(), newPathAim.c_str()); //�ݹ�
		}

		chdir("..");
		closedir(dp);
	}
	else if (isFileExist(pszPath)) //�ļ�ͨ����д���и���
	{
		uint32_t num;
		char* buf = readFile_needfree(pszPath, &num);
		if (buf)
		{
			bRet = writeFile(aimPathName, buf, num);
			free(buf);
		}
		else
		{
			bRet = false;
		}
	}	
	return bRet;

#else
	string command = "cp -r "; // Path may include space. linux������
	command += pszPath;
	command += " ";
	command += aimPathName;
	return system(command.c_str()) == 0 ? true : false; 
#endif
}

const char* lly::FileUtils::getLastCharacters( const char* s, char ch )
{
	const char* ss = s + strlen(s) - 1;
	while (ss >= s)
	{
		if (*ss == ch) return ss + 1;
		--ss;
	}
	return nullptr;
}

std::string lly::FileUtils::setLastCharIsSlash(const char* ch)
{
	const char* ss = ch + strlen(ch) - 1;
	string str(ch);
	if (*ss != '/' && *ss != '\\') str += '/'; //��֤ĩβ�� /
	return str;
}


