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
	//��ʼ�������
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

	//ֹͣ
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
	m_nTotalWaitMSecond = getCurrentMSecond() - m_nBeginWaitMSecond; //������ͣ���ܵȴ�ʱ��
	m_nBeginMSecond += m_nTotalWaitMSecond; //��Ҫ����ʱ���ϼ�ȥ�ȴ���ʱ��

	Director::getInstance()->getScheduler()->resumeTarget(this);
}

void RhythmGenerator::rewind()
{
	//��ֹͣ
	m_bPlaying = false;
	m_bDelay = false;

	//���� ��ʼ�������
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
	//��������
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
		//�ӳٳ����涨ʱ����ʼ�������
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

	//ʱ���������˽���ʱ��ı���
	if (m_nCurMSecond - m_nBeginMSecond > m_nMSecondPerBeat * m_nBeatTimes)
	{
		//ִ��һ���ص�����
		if (eventPerBeat) eventPerBeat(m_nBeatTimes);

		//����ƴ�+1
		++m_nBeatTimes;
	}
}


