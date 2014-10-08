#include "llyRhythmGenerator.h"
#include "audio\include\SimpleAudioEngine.h"

USING_NS_CC;
using namespace lly;
using namespace CocosDenshion;

RhythmGenerator* RhythmGenerator::m_RG = nullptr;

RhythmGenerator::RhythmGenerator() : m_bPlaying(false), m_bDelay(false), m_nMSecondPerBeat(1000), m_nMarginMSecond(0)
{

}

RhythmGenerator::~RhythmGenerator()
{

}

RhythmGenerator* RhythmGenerator::getInstance()
{
	if (!m_RG)
	{
		m_RG = new RhythmGenerator();
		if (!m_RG) return nullptr;
		else
			Director::getInstance()->getScheduler()->scheduleUpdate(m_RG, 0, true);
	}
	return m_RG;
}

void RhythmGenerator::end()
{
	Director::getInstance()->getScheduler()->unscheduleUpdate(m_RG);
	CC_SAFE_DELETE(m_RG);
}

void RhythmGenerator::start()
{
	//开始计算节奏
	m_nBeginMSecond = getCurrentMSecond();
	m_nBeatTimes = 1;
	m_nTotalWaitMSecond = 0;

	if (m_nMarginMSecond == 0)
	{
		m_bPlaying = true;
	}
	else
	{
		m_bDelay = true;
	}

	Director::getInstance()->getScheduler()->resumeTarget(this);
}

void RhythmGenerator::stop()
{
	Director::getInstance()->getScheduler()->pauseTarget(this);

	//停止
	m_bPlaying = false;
	m_bDelay = false;
}

void RhythmGenerator::pause()
{
	Director::getInstance()->getScheduler()->pauseTarget(this);

	m_nBeginWaitMSecond = getCurrentMSecond();
}

void RhythmGenerator::resume()
{
	m_nTotalWaitMSecond = getCurrentMSecond() - m_nBeginWaitMSecond; //本次暂停的总等待时间
	m_nBeginMSecond += m_nTotalWaitMSecond; //需要在总时间上减去等待的时间

	Director::getInstance()->getScheduler()->resumeTarget(this);
}

void RhythmGenerator::rewind()
{
	//先停止
	m_bPlaying = false;
	m_bDelay = false;

	//重新 开始计算节奏
	m_nBeginMSecond = getCurrentMSecond();
	m_nBeatTimes = 1;
	m_nTotalWaitMSecond = 0;

	if (m_nMarginMSecond == 0)
	{
		m_bPlaying = true;
	}
	else
	{
		m_bDelay = true;
	}
}


void RhythmGenerator::playBackgroundMusic( const char* pszFilePath, bool bLoop /*= false*/ )
{
	//播放音乐
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(pszFilePath, bLoop);

	this->start();
}

void RhythmGenerator::stopBackgroundMusic( bool bReleaseData /*= false*/ )
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic(bReleaseData);

	this->stop();
}

void RhythmGenerator::pauseBackgroundMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

	this->pause();
}

void RhythmGenerator::resumeBackgroundMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	
	this->resume();
}

void RhythmGenerator::rewindBackgroundMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->rewindBackgroundMusic();

	this->rewind();
}

int64_t RhythmGenerator::getCurrentMSecond()
{
	gettimeofday(&tval, NULL);
	return (int64_t)(tval.tv_sec * 1000LL + tval.tv_usec/1000);
}

void RhythmGenerator::update( float dt )
{
	if (m_bDelay)
	{
		//延迟超过规定时间则开始节奏计算
		if (getCurrentMSecond() - m_nBeginMSecond > m_nMarginMSecond)
		{
			m_bDelay = false;
			m_bPlaying = true;
			m_nBeginMSecond = m_nMarginMSecond;
			return;
		}
	}

	if (!m_bPlaying) return;
	
	m_nCurMSecond = getCurrentMSecond();

	//时间间隔大于了节奏时间的倍数
	if (m_nCurMSecond - m_nBeginMSecond > m_nMSecondPerBeat * m_nBeatTimes)
	{
		//执行一个回调方法
		if (eventPerBeat) eventPerBeat(m_nBeatTimes);

		//节奏计次+1
		++m_nBeatTimes;
	}
}


