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
    if (pRet && cb) //新建无误，且第一个控件不为空
    {
		std::function <void(lly::CheckBox*, bool)> func = 
			CC_CALLBACK_2(CheckBoxManager::doCBClick_cb, pRet);

        //让第一个控件获知管理器
		
		cb->setCheckBoxManager(pRet, func);

        //如果是选择状态就加入列表
        if (cb->getSelectedState()) pRet->m_vBoxSelected.pushBack(cb);

        //把后面所有控件加入数组
        va_list params;
        va_start(params, cb);
        lly::CheckBox* p;

        while (true)
        {
            p = va_arg(params, lly::CheckBox*);

            if (p == nullptr) break;

			//同时让控件获知管理器
			p->setCheckBoxManager(pRet, func);

            //如果是选择状态就加入列表
            if (p->getSelectedState()) pRet->m_vBoxSelected.pushBack(p);
        }

        va_end(params);

        //加入自动释放
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
    n = n < 1 ? 1 : n; //n不能小于1
    m_nMaxSelected = n;
}

void CheckBoxManager::setSelectedBox( CheckBox* cb )
{
    //如果选择的控件数量等于最大数量，则从列表头部开始移除控件，并把移除的控件取消激活
    if ((int)m_vBoxSelected.size() == m_nMaxSelected) 
    { 
		m_vBoxSelected.at(0)->turnToUnselectedState();
    }
    else if ((int)m_vBoxSelected.size() > m_nMaxSelected)
    {
        for (int i = 0; i <= (int)m_vBoxSelected.size() - m_nMaxSelected; ++i )
        { //因为始终要删除1个，所以是从0到<=
			m_vBoxSelected.at(0)->turnToUnselectedState();
        }
    }

    m_vBoxSelected.pushBack(cb); //加入数组
}

void CheckBoxManager::removeBoxFromSelectedVector( CheckBox* cb )
{
    m_vBoxSelected.eraseObject(cb);
}


