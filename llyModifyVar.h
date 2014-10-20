/*
	���ƣ�llyModifyVar.h
	����������һ��������������һ��������һ��ʱ���ڲ��ϵر���ֵ����һ����ʼ��ֵ������ֵ�仯
	���ߣ�����
	���ڣ�2014.7.1
*/

#ifndef _MODIFY_FUNC_VAR_H_
#define _MODIFY_FUNC_VAR_H_

#include "cocos2d.h"

namespace lly {

class ModifyVar : public cocos2d::ActionInterval
{
public:
	ModifyVar();
	virtual ~ModifyVar();

	//ͨ��һ����ʼ�����仯ʱ�䣬������ֵ���޸�һ�����ոı����ֵ�Ļص�����
	static ModifyVar* create(std::function<void(float)> Modify,  float duration, float start,float destination);

	//override
	virtual ModifyVar* clone() const override;
	virtual ModifyVar* reverse(void) const override;	
	virtual void startWithTarget(cocos2d::Node *pTarget) override;
	virtual void update(float time) override;;

protected:
	bool initWithDuration(std::function<void(float)> Modify, float duration, float start, float destination);

	std::function<void(float)> Modify_cb; //�ص�

	float m_fDuration; //����ʱ��

	float m_fStart; //��ʼ��ֵ	
	float m_fDestination; //������ֵ

	float m_fDelta; //ÿ��ˢ��ʱ�ı仯��
	int nPlusOrMinus; //int 1Ϊ������-1Ϊ����

private:
	CC_DISALLOW_COPY_AND_ASSIGN(ModifyVar);
};

} //lly

#endif //_MODIFY_FUNC_VAR_H_




