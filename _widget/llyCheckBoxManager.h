/*
名称：llyCheckBoxManager.h
内容：单选管理器，往里面放入checkbox时，这些checkbox就只能同时选中一个
			可以设置最大复选框选择数量
			可以手动取消所有的复选框

制作：卢乐颜
日期：2014.4.11

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

    //获取所有选中状态的复选框
    cocos2d::Vector<CheckBox*> getSelectedBoxes() {return m_vBoxSelected;}

    //把所有的控件取消激活
    void setAllBoxesUnselected();

    //设置最大选择数量，不设定为单选，设定可以为有数量限制的多选
    void setMaxNumSelected(int n);

protected:
    //同时关闭其他的选择框
    void setSelectedBox(CheckBox* cb);

    //关闭
    void removeBoxFromSelectedVector(CheckBox* cb);

	void doCBClick_cb(CheckBox* cb, bool b)
	{
		if (b)
			setSelectedBox(cb);
		else
			removeBoxFromSelectedVector(cb);
	}

protected:
    cocos2d::Vector<CheckBox*> m_vBoxSelected; //已经被选择的选择框的数组

    int m_nMaxSelected; //最大选择数
};

} //namespace lly end

#endif //_LLY_CHECK_BOX_MANAGER_H_



