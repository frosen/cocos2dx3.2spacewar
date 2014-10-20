#include "llyShaderEffectBase.h"

USING_NS_CC;
using namespace lly;

lly::ShaderEffectBase::ShaderEffectBase() : 
	Ref(), 
	_glprogramstate(nullptr),
	m_eTargetType(ETargetType::NONE_TARGET)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	_backgroundListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*)
	{
		auto glProgram = _glprogramstate->getGLProgram();
		glProgram->reset();
		glProgram->initWithByteArrays(_vertSource.c_str(), _fragSource.c_str());
		glProgram->link();
		glProgram->updateUniforms();
	});

	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backgroundListener, -1);
#endif
}

lly::ShaderEffectBase::~ShaderEffectBase()
{
	CC_SAFE_RELEASE_NULL(_glprogramstate);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	Director::getInstance()->getEventDispatcher()->removeEventListener(_backgroundListener);
#endif
}

ShaderEffectBase* lly::ShaderEffectBase::createWithShaderFileName( const std::string &vert, const std::string &frag )
{
	auto pRet = new ShaderEffectBase();
	if (pRet && pRet->init(vert, frag))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

void lly::ShaderEffectBase::setTarget( cocos2d::Sprite* sprite )
{
	if (m_eTargetType == ETargetType::NONE_TARGET)
	{
		sprite->setGLProgramState(_glprogramstate);

		m_target = sprite;
		m_eTargetType = ETargetType::SPRITE;

		retain();
	}
}

void lly::ShaderEffectBase::setTargetWithChild( cocos2d::Node* node )
{
	if (m_eTargetType == ETargetType::NONE_TARGET)
	{
		setShaderWithChild_IfIsSprite(node);

		m_target = node;
		m_eTargetType = ETargetType::NODE_WITH_CHILD;

		retain();
	}
	
}

void lly::ShaderEffectBase::clearTarget()
{
	if (m_eTargetType == ETargetType::NONE_TARGET) return;

	auto glprogram = GLProgram::createWithByteArrays(LLY_SHADER_VERT_DEFAULT, LLY_SHADER_FRAG_DEFAULT);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	_vertSource = vertSource;
	_fragSource = fragSource;
#endif

	auto glprogramstateDefault = GLProgramState::getOrCreateWithGLProgram(glprogram);

	if (m_eTargetType == ETargetType::SPRITE)
	{
		m_target->setGLProgramState(glprogramstateDefault);
	}
	else if (m_eTargetType == ETargetType::NODE_WITH_CHILD)
	{
		setShaderDefaultWithChild_IfIsSprite(m_target, glprogramstateDefault);
	}

	m_target = nullptr;
	m_eTargetType = ETargetType::NONE_TARGET;

	release();
}

bool lly::ShaderEffectBase::init( const std::string &vert, const std::string &frag )
{

	auto fileUtiles = FileUtils::getInstance();

	// vert
	std::string vertSource;
	if (vert.empty()) //vert输入""则使用默认的vert shader
	{
		vertSource = LLY_SHADER_VERT_DEFAULT;
	} 
	else 
	{
		std::string vertexFilePath = fileUtiles->fullPathForFilename(vert);
		vertSource = fileUtiles->getStringFromFile(vertexFilePath);
	}

	// frag
	auto fragmentFilePath = fileUtiles->fullPathForFilename(frag);
	auto fragSource = fileUtiles->getStringFromFile(fragmentFilePath);

	auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	_vertSource = vertSource;
	_fragSource = fragSource;
#endif

	if(!glprogram) return false;

	_glprogramstate = GLProgramState::getOrCreateWithGLProgram(glprogram);
	_glprogramstate->retain();

	return true;
}

void lly::ShaderEffectBase::setShaderWithChild_IfIsSprite( cocos2d::Node* node )
{
	//如果自己是精灵，则修改渲染程序
	auto sp = dynamic_cast<Sprite*>(node);
	if (sp) sp->setGLProgramState(_glprogramstate);

	//遍历所有子节点，如果是精灵，则修改渲染程序
	Vector<Node*> vchlid = node->getChildren();

	for (auto chlid : vchlid)
	{
		setShaderWithChild_IfIsSprite(chlid);
	}
}

void lly::ShaderEffectBase::setShaderDefaultWithChild_IfIsSprite( cocos2d::Node* node, GLProgramState* GLState )
{
	//如果自己是精灵，则修改渲染程序
	auto sp = dynamic_cast<Sprite*>(node);
	if (sp) sp->setGLProgramState(GLState);

	//遍历所有子节点，如果是精灵，则修改渲染程序
	Vector<Node*> vchlid = node->getChildren();

	for (auto chlid : vchlid)
	{
		setShaderDefaultWithChild_IfIsSprite(chlid, GLState);
	}
}

