#include "llyAcceDirCtrler.h"

USING_NS_CC;
using namespace lly;

//结构体的构造函数
DirectionRatio::DirectionRatio( float xx, float yy ) : x(xx), y(yy) {}

//单例
AcceDirCtrler* AcceDirCtrler::s_ADCer = nullptr;

AcceDirCtrler::AcceDirCtrler() : 
	_fXorignal(0.0f), 
	_fYorignal(0.0f), 
	_fXbuffer(0.0f),
	_fYbuffer(0.0f),
	controlDirection_cb(nullptr)
{

}

AcceDirCtrler::~AcceDirCtrler()
{
	
}

AcceDirCtrler* AcceDirCtrler::getInstance()
{
	if (s_ADCer == nullptr)
	{
		s_ADCer = new (std::nothrow) AcceDirCtrler();
		if (!s_ADCer || !s_ADCer->init())
		{
			CC_SAFE_DELETE(s_ADCer);
			return nullptr;
		}
	}
	return s_ADCer;
}

void AcceDirCtrler::destroyInstance()
{
	CC_SAFE_DELETE(s_ADCer);
}

void AcceDirCtrler::start()
{
	Device::setAccelerometerEnabled(true);

	auto dispatcher = Director::getInstance()->getEventDispatcher();
	listener = EventListenerAcceleration::create([&, this](Acceleration* acc, Event* event){
		switch (m_enState)
		{
		case AcceDirCtrler::BEGIN: //开始时记录当下重力值作为原值
			_fXorignal = (float)acc->x;
			_fYorignal = (float)acc->y;
			m_enState = MOVE;
			break;
		case AcceDirCtrler::MOVE: //根据xy轴变化的比例得出当前方向
			{
				xD = (float)acc->x - _fXorignal + _fXbuffer; //buffer是为了防止因不断变化的方向导致移动目标晃动而增加的缓冲距离
				yD = (float)acc->y - _fYorignal + _fYbuffer;

				if (yD == 0) //yd为0则不能进行除法
				{
					if (xD > 0)
						m_CurDirction = D90;
					else if (xD < 0)
						m_CurDirction = D270;
					else
						m_CurDirction = Dnone;
				}
				else
				{
					rate = xD / yD; //通过x，y轴的比例以及x轴的正负 判断此向量的方向，然后看落在哪个区域

					//两分法在v_vD中查找
					if (rate <= v_vD[3].rateMax)
						if (rate <= v_vD[1].rateMax)
							if (rate <= v_vD[0].rateMax) m_CurDirction = xD > 0 ? v_vD[0].e1 : v_vD[0].e2;
							else m_CurDirction = xD > 0 ? v_vD[1].e1 : v_vD[1].e2;
						else
							if (rate <= v_vD[2].rateMax) m_CurDirction = xD > 0 ? v_vD[2].e1 : v_vD[2].e2;
							else m_CurDirction = xD > 0 ? v_vD[3].e1 : v_vD[3].e2;
					else
						if (rate <= v_vD[6].rateMax)
							if (rate <= v_vD[4].rateMax) m_CurDirction = yD > 0 ? v_vD[4].e1 : v_vD[4].e2; //yd
							else	if (rate <= v_vD[5].rateMax) m_CurDirction = xD > 0 ? v_vD[5].e1 : v_vD[5].e2;
							else m_CurDirction = xD > 0 ? v_vD[6].e1 : v_vD[6].e2;
						else
							if (rate <= v_vD[7].rateMax) m_CurDirction = xD > 0 ? v_vD[7].e1 : v_vD[7].e2;
							else m_CurDirction = xD > 0 ? v_vD[8].e1 : v_vD[8].e2;				
				}

				//设置缓冲值
				switch (m_CurDirction)
				{
				case D0:
					_fXbuffer = 0;
					_fYbuffer = m_fPrecision90;
					break;
				case D22p5:
					_fXbuffer = m_fPrecision22p5;
					_fYbuffer = m_fPrecision67p5;
					break;
				case D45:
					_fXbuffer = m_fPrecision45;
					_fYbuffer = m_fPrecision45;
					break;
				case D67p5:
					_fXbuffer = m_fPrecision67p5;
					_fYbuffer = m_fPrecision22p5;
					break;
				case D90:
					_fXbuffer = m_fPrecision90;
					_fYbuffer = 0;
					break;
				case D112p5:
					_fXbuffer = m_fPrecision67p5;
					_fYbuffer = -m_fPrecision22p5;
					break;
				case D135:
					_fXbuffer = m_fPrecision45;
					_fYbuffer = -m_fPrecision45;
					break;
				case D157p5:
					_fXbuffer = m_fPrecision22p5;
					_fYbuffer = -m_fPrecision67p5;
					break;
				case D180:
					_fXbuffer = 0;
					_fYbuffer = -m_fPrecision90;
					break;
				case D202p5:
					_fXbuffer = -m_fPrecision22p5;
					_fYbuffer = -m_fPrecision67p5;
					break;
				case D225:
					_fXbuffer = -m_fPrecision45;
					_fYbuffer = -m_fPrecision45;
					break;
				case D247p5:
					_fXbuffer = -m_fPrecision67p5;
					_fYbuffer = -m_fPrecision22p5;
					break;
				case D270:
					_fXbuffer = -m_fPrecision90;
					_fYbuffer = 0;
					break;
				case D292p5:
					_fXbuffer = -m_fPrecision67p5;
					_fYbuffer = m_fPrecision22p5;
					break;
				case D315:
					_fXbuffer = -m_fPrecision45;
					_fYbuffer = m_fPrecision45;
					break;
				case D337p5:
					_fXbuffer = -m_fPrecision22p5;
					_fYbuffer = m_fPrecision67p5;
					break;
				case Dnone:
					_fXbuffer = 0;
					_fYbuffer = 0;
					break;
				default:
					break;
				}

				//回调*/
				if (controlDirection_cb) controlDirection_cb(this);				

				break;
			}
		case AcceDirCtrler::PAUSE: 
			break;
		default:
			break;
		}		
	});

	dispatcher->addEventListenerWithFixedPriority(listener, -1); //注册

	m_enState = BEGIN; //开始
}

void AcceDirCtrler::stop()
{
	Device::setAccelerometerEnabled(false); //关闭加速器

	//注销
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	dispatcher->removeEventListener(listener);

	//方向清零
	m_CurDirction = Dnone;
}

void AcceDirCtrler::pause()
{
	m_enState = PAUSE;
	m_CurDirction = Dnone;
}

void AcceDirCtrler::resume()
{
	m_enState = MOVE;
}

void AcceDirCtrler::restrat()
{
	m_enState = BEGIN;
}

void AcceDirCtrler::setPrecision( int n )
{
	if (n < 0) n = 0;

	m_nPrecison = n;
	m_fPrecision22p5 = cut22p5 * n;
	m_fPrecision45 = cut45 * n;
	m_fPrecision67p5 = cut67p5 * n;
	m_fPrecision90 = cut90 * n;
}

bool AcceDirCtrler::init()
{
	//生成比率列表
	m_vDirRatio.push_back(DirectionRatio(0, 1)); //0
	m_vDirRatio.push_back(DirectionRatio(0.3827f, 0.9239f));
	m_vDirRatio.push_back(DirectionRatio(0.7071f, 0.7071f));
	m_vDirRatio.push_back(DirectionRatio(0.9239f, 0.3827f));

	m_vDirRatio.push_back(DirectionRatio(1, 0)); //90
	m_vDirRatio.push_back(DirectionRatio(0.9239f, -0.3827f));
	m_vDirRatio.push_back(DirectionRatio(0.7071f, -0.7071f));
	m_vDirRatio.push_back(DirectionRatio(0.3827f, -0.9239f));

	m_vDirRatio.push_back(DirectionRatio(0, -1)); //180
	m_vDirRatio.push_back(DirectionRatio(-0.3827f, -0.9239f));
	m_vDirRatio.push_back(DirectionRatio(-0.7071f, -0.7071f));
	m_vDirRatio.push_back(DirectionRatio(-0.9239f, -0.3827f));

	m_vDirRatio.push_back(DirectionRatio(-1, 0)); //270
	m_vDirRatio.push_back(DirectionRatio(-0.9239f, 0.3827f));
	m_vDirRatio.push_back(DirectionRatio(-0.7071f, 0.7071f));
	m_vDirRatio.push_back(DirectionRatio(-0.3827f, 0.9239f));

	m_vDirRatio.push_back(DirectionRatio(0, 0));

	//查找获得xy轴比例对应方向的列表
	v_vD[0] = value_Dir(-5.0273f, D90, D270);
	v_vD[1] = value_Dir(-1.4966f, D112p5, D292p5);
	v_vD[2] = value_Dir(-0.6682f, D135, D315);
	v_vD[3] = value_Dir(-0.1989f, D157p5, D337p5);
	v_vD[4] = value_Dir(0.1989f, D0, D180); //这个特殊是判断y轴是不是大于0
	v_vD[5] = value_Dir(0.6682f, D22p5, D202p5);
	v_vD[6] = value_Dir(1.4966f, D45, D225);
	v_vD[7] = value_Dir(5.0273f, D67p5, D247p5);
	v_vD[8] = value_Dir(5.0273f, D67p5, D247p5);
	v_vD[9] = value_Dir(FLT_MAX, D67p5, D247p5);

	//设置灵敏度
	setPrecision(5);

	return true;
}



