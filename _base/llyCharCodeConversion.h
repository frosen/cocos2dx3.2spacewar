/*
	���ƣ�llyCharCodeConversion.h
	�������ַ���ת���࣬�п�ƽ̨����
	���ߣ�����
	���ڣ�2014.7.30
*/

#ifndef _LLY_CHAR_CODE_CONVERSION_H_
#define _LLY_CHAR_CODE_CONVERSION_H_

#include <string>

namespace lly{

//ȫ��static��ֻ�ܱ��ļ�ʹ��
static bool iconvert( 
	const char* from_charset, 
	const char* to_charset, 
	const char* inbuf, 
	int inlen, 
	char* outbuf, 
	int outlen);

std::string convertGBKToUTF8(const std::string& str);

std::string convertUTF8ToGBK(const std::string& str);

} //lly

#endif //_LLY_CHAR_CODE_CONVERSION_H_



