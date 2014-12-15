/*
名称：llyRhythmGenerator.h
内容：节奏生成器，继承于node，并将SimpleAudioEngine组合进来，从音乐开始时每隔一个鼓点，运行一个回调函数

制作：卢乐颜
日期：2014.7.11

*/

#ifndef _LLY_RHYTHM_GENERATOR_H_
#define _LLY_RHYTHM_GENERATOR_H_

#include "cocos2d.h"
#include "audio\include\SimpleAudioEngine.h"

namespace lly {

class RhythmGenerator
{
public:
	RhythmGenerator();
	virtual ~RhythmGenerator();

	static RhythmGenerator* getInstance();
	static void end();

	void start();
	void stop();
	void pause();
	void resume();
	void rewind();

	//各种对背景音乐的操作
	virtual void playBackgroundMusic(const char* pszFilePath, bool bLoop = false);

	virtual void stopBackgroundMusic(bool bReleaseData = false);

	virtual void pauseBackgroundMusic();

	virtual void resumeBackgroundMusic();

    virtual void rewindBackgroundMusic();

	int64_t getCurrentTime(){ return m_nCurMSecond; };
	int64_t getBeginTime() { return m_nBeginMSecond; }

	//检测时间的循环
	void update(float dt);

	int64_t getCurrentMSecond();

public:
	CC_SYNTHESIZE(int64_t, m_nMSecondPerBeat, MSecondPerBeat); //重音之间的毫秒数

	CC_SYNTHESIZE(int64_t, m_nMarginMSecond, MarginMSecond); //前置空余毫秒数

	std::function <void(int)> eventPerBeat;

protected:
	static RhythmGenerator* s_RG; //单例

	bool m_bPlaying; //正在进行播放同时计算节奏
	bool m_bDelay; //是否有延迟开始计算节奏

	int64_t m_nCurMSecond; //当前时间
	int64_t m_nBeginMSecond; //开始时间
	int64_t m_nTotalWaitMSecond; //等待总时间
	int64_t m_nBeginWaitMSecond; //等待总时间

	int m_nBeatTimes; //节奏计次

	struct timeval tval; //获取系统时间结构
};

} //lly

#endif //_LLY_RHYTHM_GENERATOR_H_


