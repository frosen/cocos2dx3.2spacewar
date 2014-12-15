/*
���ƣ�llyRhythmGenerator.h
���ݣ��������������̳���node������SimpleAudioEngine��Ͻ����������ֿ�ʼʱÿ��һ���ĵ㣬����һ���ص�����

������¬����
���ڣ�2014.7.11

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

	//���ֶԱ������ֵĲ���
	virtual void playBackgroundMusic(const char* pszFilePath, bool bLoop = false);

	virtual void stopBackgroundMusic(bool bReleaseData = false);

	virtual void pauseBackgroundMusic();

	virtual void resumeBackgroundMusic();

    virtual void rewindBackgroundMusic();

	int64_t getCurrentTime(){ return m_nCurMSecond; };
	int64_t getBeginTime() { return m_nBeginMSecond; }

	//���ʱ���ѭ��
	void update(float dt);

	int64_t getCurrentMSecond();

public:
	CC_SYNTHESIZE(int64_t, m_nMSecondPerBeat, MSecondPerBeat); //����֮��ĺ�����

	CC_SYNTHESIZE(int64_t, m_nMarginMSecond, MarginMSecond); //ǰ�ÿ��������

	std::function <void(int)> eventPerBeat;

protected:
	static RhythmGenerator* s_RG; //����

	bool m_bPlaying; //���ڽ��в���ͬʱ�������
	bool m_bDelay; //�Ƿ����ӳٿ�ʼ�������

	int64_t m_nCurMSecond; //��ǰʱ��
	int64_t m_nBeginMSecond; //��ʼʱ��
	int64_t m_nTotalWaitMSecond; //�ȴ���ʱ��
	int64_t m_nBeginWaitMSecond; //�ȴ���ʱ��

	int m_nBeatTimes; //����ƴ�

	struct timeval tval; //��ȡϵͳʱ��ṹ
};

} //lly

#endif //_LLY_RHYTHM_GENERATOR_H_


