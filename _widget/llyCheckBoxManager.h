/*
���ƣ�llyCheckBoxManager.h
���ݣ���ѡ�����������������checkboxʱ����Щcheckbox��ֻ��ͬʱѡ��һ��
			�����������ѡ��ѡ������
			�����ֶ�ȡ�����еĸ�ѡ��

������¬����
���ڣ�2014.4.11

*/

#ifndef _LLY_CHECK_BOX_MANAGER_H_
#define _LLY_CHECK_BOX_MANAGER_H_

#include "cocos2d.h"

namespace lly {

class CheckBox;

class CheckBoxManager : public cocos2d::Ref
{
public:
    CheckBoxManager();
    virtual ~CheckBoxManager();

    static CheckBoxManager* createWithCheckBox(CheckBox* cb, ...);

    //��ȡ����ѡ��״̬�ĸ�ѡ��
    cocos2d::Vector<CheckBox*> getSelectedBoxes() {return m_vBoxSelected;}

    //�����еĿؼ�ȡ������
    void setAllBoxesUnselected();

    //�������ѡ�����������趨Ϊ��ѡ���趨����Ϊ���������ƵĶ�ѡ
    void setMaxNumSelected(int n);

protected:
    //ͬʱ�ر�������ѡ���
    void setSelectedBox(CheckBox* cb);

    //�ر�
    void removeBoxFromSelectedVector(CheckBox* cb);

	void doCBClick_cb(CheckBox* cb, bool b)
	{
		if (b)
			setSelectedBox(cb);
		else
			removeBoxFromSelectedVector(cb);
	}

protected:
    cocos2d::Vector<CheckBox*> m_vBoxSelected; //�Ѿ���ѡ���ѡ��������

    int m_nMaxSelected; //���ѡ����
};

} //namespace lly end

#endif //_LLY_CHECK_BOX_MANAGER_H_



