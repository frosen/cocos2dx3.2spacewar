/*
���ƣ�llyValueListener.h
����������һ����ֵ�ı仯����һ���涨��ֵ�����һ���ص�
���ߣ�����
���ڣ�2014.7.2
*/

#ifndef _VALUE_LISTENER_H_
#define _VALUE_LISTENER_H_

#include "cocos2d.h"

namespace lly {

class ValueListener : public cocos2d::Ref
{
public:
	ValueListener();
	virtual ~ValueListener();

	//����һ��float�����ã�����ָ��ָ����ֵ�ı仯��Ȼ������һ���ص�����
	static ValueListener* create(
		float* var, float aim, 
		std::function<void(void)> doAfter, float maxWaitingTime = -1.0f); 

	//����һ���ص��������ص������ṩһ��float�ķ���ֵ�������������ֵ��Ȼ������һ���ص�����
	static ValueListener* create(
		std::function<float(void)> listen, float aim, 
		std::function<void(void)> doAfter, float maxWaitingTime = -1.0f); 

	//��ʼ�����ȴ�ʱ�䣬�����ֵ����0����û����涨��ֵ������ʱ��Ҳ��ִ�лص�
	void setMaxWaitingSecond(float fTime) { m_fMaxWaitingSecond = fTime; }

	void run(); //��ʼ����

	void update(float dt); //���仯�Ĵ�ѭ��

protected:
	virtual bool init(float* var, std::function<float(void)> listen, float aim, std::function<void(void)> doAfter, float maxWaitingTime);

	void onDone();

public:
	std::function<void(float)> excute_cb; //ִ���еĻص�

protected:
	float* m_pfvar; //����������ֵ
	std::function<float(void)> listen_cb; //�������ĺ���

	float m_fconstant; //����
	int m_nType; //������ = -1������С = 1��
	float fTemp; //��ʱ���������ܻص����ص�float
	
	std::function<void(void)> doAfte_cb; //�ص�����

	float m_fMaxWaitingSecond; //�����ʱ��
	float m_fTimeDelay; //�ܳ���ʱ��
};

} //lly

#endif //_VALUE_LISTENER_H_





