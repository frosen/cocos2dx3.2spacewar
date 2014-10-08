/*
名称：MyLog.h
内容：LLY_DEBUG == COCOS2D_DEBUG
		
制作：卢乐颜
日期：2014.8.21
*/
#ifndef _LLY_LOG_H_
#define _LLY_LOG_H_

#include "cocos2d.h"

#if !COCOS2D_DEBUG

#define LLY_LOG(format, ...) do {} while (0)
#define LLY_LOG_CUR_LOC(_CONTENTS_) do {} while (0)
#define LLY_RETURN_FALSE_IF_NOT(_CLASS_) if (!_CLASS_) return false
#define LLY_RETURN_IF_NOT(_CLASS_) if (!_CLASS_) return
#define LLY_RETURN_FALSE_IF(_CLASS_) if (_CLASS_) return false
#define LLY_RETURN_IF(_CLASS_) if (_CLASS_) return

#define LLY_RETURN_TRUE_FOR_SUCCESS return true
#define LLY_RETURN_FALSE_FOR_FAIL return false
#define LLY_RETURN_FOR_FAIL return

#else

//自定义log
#define LLY_LOG(format, ...) cocos2d::log(format, ##__VA_ARGS__)

//log当前位置并输出内容
#define LLY_LOG_CUR_LOC(_CONTENTS_) \
	LLY_LOG("%s AT %d, %s, %s", _CONTENT_, __LINE__, __FUNCTION__, __FILE__)

//=============================================
//如果出错则返回错误
#define LLY_RETURN_FALSE_IF_NOT(_CLASS_)\
	if(!_CLASS_) { \
	LLY_LOG("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
	return false; }
 
//如果出错则返回
#define LLY_RETURN_IF_NOT(_CLASS_)\
	if(!_CLASS_){ \
	LLY_LOG("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
	return; } 

//如果怎样则返回错误
#define LLY_RETURN_FALSE_IF(_CLASS_)\
	if(_CLASS_) { \
	LLY_LOG("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
	return false; }

//如果怎样则返回
#define LLY_RETURN_IF(_CLASS_)\
	if(_CLASS_){ \
	LLY_LOG("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
	return; } 

//=====================================================
//函数正确返回
#define LLY_RETURN_TRUE_FOR_SUCCESS \
	LLY_LOG("right excute AT %d, %s, %s", __LINE__, __FUNCTION__, __FILE__); \
	return true
				
#define LLY_RETURN_FALSE_FOR_FAIL \
	LLY_LOG("@wrong excute AT %d, %s, %s", __LINE__, __FUNCTION__, __FILE__); \
	return false

#define LLY_RETURN_FOR_FAIL \
	LLY_LOG("@wrong excute AT %d, %s, %s", __LINE__, __FUNCTION__, __FILE__); \
	return

#endif //

#endif //_MY_LOG_H_