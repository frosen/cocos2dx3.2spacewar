/*
	名称：llyCharCodeConversion.h
	描述：字符串转换类，有跨平台处理
	作者：乐颜
	日期：2014.7.30
*/

#ifndef _LLY_CHAR_CODE_CONVERSION_H_
#define _LLY_CHAR_CODE_CONVERSION_H_

#include <string>

namespace lly{

//全局static：只能本文件使用
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



