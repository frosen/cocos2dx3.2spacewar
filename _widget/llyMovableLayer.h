/*
���ƣ�llyMovableLayer.h
���ݣ����������࣬�ֱ�̳�CCLayer��ui::layout�������������һ����㴥�صĹ���
			�����ڴ���ʱѡ��ʵ�֣�
			1. ��קƽ��
			2. ����Ŵ���С
			3. ������ת

������¬����
���ڣ�2014.5.1
*/

#ifndef _LLY_MOVABLE_LAYER_H_
#define _LLY_MOVABLE_LAYER_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

namespace lly{

class TouchFunction
{
public:
	typedef std::function<void(const cocos2d::Vec2*, const cocos2d::Vec2*, cocos2d::Event*, cocos2d::Node*)> TouchesCallback;

	enum //����ʹ�õĲ����ı�ʶ
	{
		move = 1,
		scale = 2,
		rotate = 4,
	};

	struct isHitted //��¼�����Ƿ����
	{
		char nNumHittedPoint;
		cocos2d::Vec2 anchorOld;
		cocos2d::Vec2 posNewAnchor;
		isHitted() {}
		isHitted(char NumHittedPoint, cocos2d::Vec2 oldAnchor, cocos2d::Vec2 newAnchorPoint) : 
			nNumHittedPoint(NumHittedPoint),
			anchorOld(oldAnchor), 
			posNewAnchor(newAnchorPoint) {}
	};
	
	//��node���ӹ���
	static void setTouchFunction(cocos2d::Node* node, uint32_t flag); 

	//ˢ��node nullΪˢ��ȫ��
	static void refresh(cocos2d::Node* node = nullptr);

private:
	static void touchBeginWithChangeAnchor(
		const std::vector<cocos2d::Touch*> &vTouch, 
		cocos2d::Event* event, 
		cocos2d::Node* node, 
		bool needChangeAnchor);

	static void touchMoveHasWidgetMove(
		const std::vector<cocos2d::Touch*> &vTouch, 
		cocos2d::Event* event, 
		cocos2d::Node* node, 
		TouchesCallback moveMoved, 
		TouchesCallback scaleMoved, 
		TouchesCallback rotateMoved);
												  
	static void touchMoveWithoutWidgetMove(
		const std::vector<cocos2d::Touch*> &vTouch, 
		cocos2d::Event* event,
		cocos2d::Node* node, 
		TouchesCallback moveMoved, 
		TouchesCallback scaleMoved, 
		TouchesCallback rotateMoved);

	static void touchEndWithChangeAnchor(
		const std::vector<cocos2d::Touch*> &vTouch, 
		cocos2d::Event* event, 
		cocos2d::Node* node, 
		bool needChangeAnchor);

	//�����Ƿ������node
	static bool containsPoint(cocos2d::Node* node, cocos2d::Vec2 point);

	//�ı�ê��λ�õ���һ���㣬���ź���ת�Դ�Ϊ���㣬��¼ԭ��ê��
	static void changeAnchorToTouchPoint(
		cocos2d::Node* node, 
		const cocos2d::Vec2 &pointInNode, 
		cocos2d::Vec2 &oldAnchor_out);

	static void changeAnchorBack(
		cocos2d::Node* node, 
		const cocos2d::Vec2 &pointInNode, 
		cocos2d::Vec2 &oldAnchor);

private:
	//���ִ�������
	static TouchesCallback m_moveMoved; //����ƶ�
	static TouchesCallback m_scaleMoved; //���㿪�ϸı��С
	static TouchesCallback m_rotateMoved; //����Ť����תͼ	

	//��¼��ǰnode���Ƿ�������bengin��move��end�䴫����Ϣ
	static std::map <cocos2d::Node*, isHitted> mNodeIsHitted; 
};

class MovableLayer : public cocos2d::Layer
{
public:
	MovableLayer();
	virtual ~MovableLayer();

	static MovableLayer* create(uint32_t flag);

protected:
	virtual bool init(uint32_t flag);
};

class MovableLayout : public cocos2d::ui::Layout
{
	DECLARE_CLASS_GUI_INFO

public:
	MovableLayout();
	virtual ~MovableLayout();

	CREATE_FUNC(MovableLayout);
	static MovableLayout* create(uint32_t flag);

	void setTouchFunction(uint32_t flag);

protected:
	virtual bool init();
	virtual bool init(uint32_t flag);

	virtual std::string getDescription() const override { return "lly Movable Layer"; };

};


} // namespace lly

#endif //_LLY_MOVABLE_LAYER_H_


