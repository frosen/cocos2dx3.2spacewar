/*
���ƣ�MyLog.h
���ݣ�LLY_DEBUG == COCOS2D_DEBUG
		
������¬����
���ڣ�2014.8.21
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

//�Զ���log
#define LLY_LOG(format, ...) cocos2d::log(format, ##__VA_ARGS__)

//log��ǰλ�ò��������
#define LLY_LOG_CUR_LOC(_CONTENTS_) \
	LLY_LOG("%s AT %d, %s, %s", _CONTENT_, __LINE__, __FUNCTION__, __FILE__)

//=============================================
//��������򷵻ش���
#define LLY_RETURN_FALSE_IF_NOT(_CLASS_)\
	if(!_CLASS_) { \
	LLY_LOG("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
	return false; }
 
//��������򷵻�
#define LLY_RETURN_IF_NOT(_CLASS_)\
	if(!_CLASS_){ \
	LLY_LOG("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
	return; } 

//��������򷵻ش���
#define LLY_RETURN_FALSE_IF(_CLASS_)\
	if(_CLASS_) { \
	LLY_LOG("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
	return false; }

//��������򷵻�
#define LLY_RETURN_IF(_CLASS_)\
	if(_CLASS_){ \
	LLY_LOG("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
	return; } 

//=====================================================
//������ȷ����
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