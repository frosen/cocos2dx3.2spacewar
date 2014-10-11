#include "llyCheckBoxManager.h"
#include "llyCheckBox.h"

USING_NS_CC;
using namespace lly;

lly::CheckBoxManager::CheckBoxManager() : Ref(), m_nMaxSelected(1)
{

}

lly::CheckBoxManager::~CheckBoxManager()
{

}

CheckBoxManager* lly::CheckBoxManager::createWithCheckBox( CheckBox* cb, ... )
{
    CheckBoxManager* pRet = new CheckBoxManager();
    if (pRet && cb) //�½������ҵ�һ���ؼ���Ϊ��
    {
		std::function <void(lly::CheckBox*, bool)> func = 
			CC_CALLBACK_2(CheckBoxManager::doCBClick_cb, pRet);

        //�õ�һ���ؼ���֪������
		
		cb->setCheckBoxManager(pRet, func);

        //�����ѡ��״̬�ͼ����б�
        if (cb->getSelectedState()) pRet->m_vBoxSelected.pushBack(cb);

        //�Ѻ������пؼ���������
        va_list params;
        va_start(params, cb);
        lly::CheckBox* p;

        while (true)
        {
            p = va_arg(params, lly::CheckBox*);

            if (p == nullptr) break;

			//ͬʱ�ÿؼ���֪������
			p->setCheckBoxManager(pRet, func);

            //�����ѡ��״̬�ͼ����б�
            if (p->getSelectedState()) pRet->m_vBoxSelected.pushBack(p);
        }

        va_end(params);

        //�����Զ��ͷ�
        pRet->autorelease();

        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

void lly::CheckBoxManager::setAllBoxesUnselected()
{
    for (auto cb : m_vBoxSelected)
    {
        cb->turnToUnselectedState();
    }
}

void lly::CheckBoxManager::setMaxNumSelected( int n )
{
    n = n < 1 ? 1 : n; //n����С��1
    m_nMaxSelected = n;
}

void CheckBoxManager::setSelectedBox( CheckBox* cb )
{
    //���ѡ��Ŀؼ����������������������б�ͷ����ʼ�Ƴ��ؼ��������Ƴ��Ŀؼ�ȡ������
    if ((int)m_vBoxSelected.size() == m_nMaxSelected) 
    { 
		m_vBoxSelected.at(0)->turnToUnselectedState();
    }
    else if ((int)m_vBoxSelected.size() > m_nMaxSelected)
    {
        for (int i = 0; i <= (int)m_vBoxSelected.size() - m_nMaxSelected; ++i )
        { //��Ϊʼ��Ҫɾ��1���������Ǵ�0��<=
			m_vBoxSelected.at(0)->turnToUnselectedState();
        }
    }

    m_vBoxSelected.pushBack(cb); //��������
}

void CheckBoxManager::removeBoxFromSelectedVector( CheckBox* cb )
{
    m_vBoxSelected.eraseObject(cb);
}


