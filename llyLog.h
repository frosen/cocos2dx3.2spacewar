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

#define llyLog(format, ...) do {} while (0);
#define llyLog_curLoc(_CONTENTS_) do {} while (0);
#define llyReturn_false_ifNot(_CLASS_) if (!_CLASS_) return false;
#define llyReturn_ifNot(_CLASS_) if (!_CLASS_) return;
#define llyReturn_false_if(_CLASS_) if (_CLASS_) return false;
#define llyReturn_if(_CLASS_) if (_CLASS_) return;

#define llyReturn_true_for_success(...) return true;
#define llyReturn_false_for_fail(...) return false;
#define llyReturn_for_fail(...) return;

#else

#define llyLog(format, ...) cocos2d::log(format, ##__VA_ARGS__)

//log��ǰλ��
#define llyLog_curLoc(_CONTENTS_) \
	cocos2d::log("%s AT %d, %s, %s", _CONTENT_, __LINE__, __FUNCTION__, __FILE__);

//=============================================
//�������ش���
#define llyReturn_false_ifNot(_CLASS_)\
	if(!_CLASS_) { \
<<<<<<< HEAD
<<<<<<< HEAD
	LLY_LOG_CUR_LOC("@wrong"); \
=======
	cocos2d::log("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
>>>>>>> parent of b3b9793... 根据代码大全上的内容，优化代码
=======
	cocos2d::log("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
>>>>>>> parent of b3b9793... 根据代码大全上的内容，优化代码
	return false; }
 
//��������
#define llyReturn_ifNot(_CLASS_)\
	if(!_CLASS_){ \
<<<<<<< HEAD
<<<<<<< HEAD
	LLY_LOG_CUR_LOC("@wrong"); \
=======
	cocos2d::log("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
>>>>>>> parent of b3b9793... 根据代码大全上的内容，优化代码
=======
	cocos2d::log("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
>>>>>>> parent of b3b9793... 根据代码大全上的内容，优化代码
	return; } 

//�������ش���
#define llyReturn_false_if(_CLASS_)\
	if(_CLASS_) { \
<<<<<<< HEAD
<<<<<<< HEAD
	LLY_LOG_CUR_LOC("@wrong"); \
=======
	cocos2d::log("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
>>>>>>> parent of b3b9793... 根据代码大全上的内容，优化代码
=======
	cocos2d::log("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
>>>>>>> parent of b3b9793... 根据代码大全上的内容，优化代码
	return false; }

//��������
#define llyReturn_if(_CLASS_)\
	if(_CLASS_){ \
<<<<<<< HEAD
<<<<<<< HEAD
	LLY_LOG_CUR_LOC("@wrong"); \
=======
	cocos2d::log("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
>>>>>>> parent of b3b9793... 根据代码大全上的内容，优化代码
=======
	cocos2d::log("@wrong %s AT %d, %s, %s", #_CLASS_, __LINE__, __FUNCTION__, __FILE__); \
>>>>>>> parent of b3b9793... 根据代码大全上的内容，优化代码
	return; } 

//=====================================================
//������ȷ����
<<<<<<< HEAD
<<<<<<< HEAD
#define LLY_RETURN_TRUE_FOR_SUCCESS \
	LLY_LOG_CUR_LOC("right excute"); \
	return true
				
#define LLY_RETURN_FALSE_FOR_FAIL \
	LLY_LOG_CUR_LOC("@wrong excute"); \
	return false

#define LLY_RETURN_FOR_FAIL \
	LLY_LOG_CUR_LOC("@wrong excute"); \
	return
=======
#define llyReturn_true_for_success \
	cocos2d::log("right excute AT %d, %s, %s", __LINE__, __FUNCTION__, __FILE__); \
	return true; 
				
#define llyReturn_false_for_fail \
	cocos2d::log("@wrong excute AT %d, %s, %s", __LINE__, __FUNCTION__, __FILE__); \
	return false;

#define llyReturn_for_fail \
	cocos2d::log("@wrong excute AT %d, %s, %s", __LINE__, __FUNCTION__, __FILE__); \
	return;
>>>>>>> parent of b3b9793... 根据代码大全上的内容，优化代码
=======
#define llyReturn_true_for_success \
	cocos2d::log("right excute AT %d, %s, %s", __LINE__, __FUNCTION__, __FILE__); \
	return true; 
				
#define llyReturn_false_for_fail \
	cocos2d::log("@wrong excute AT %d, %s, %s", __LINE__, __FUNCTION__, __FILE__); \
	return false;

#define llyReturn_for_fail \
	cocos2d::log("@wrong excute AT %d, %s, %s", __LINE__, __FUNCTION__, __FILE__); \
	return;
>>>>>>> parent of b3b9793... 根据代码大全上的内容，优化代码

#endif //

#endif //_MY_LOG_H_