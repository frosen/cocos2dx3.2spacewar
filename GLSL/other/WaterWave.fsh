#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform vec2 resolution; //ͼƬ�ֱ���

//---------------------------

const float PIx2 = 6.28318531; //2�� = 2 * 3.1415926

//�ٶ�
const float speed = 0.2;
const float speed_x = 0.3;
const float speed_y = 0.3;

//���β���
const float intensity = 3.0; //ǿ��
const int steps = 8;
const float frequency = 4.0; //Ƶ��
const int angleNum = 7; //��һ��Բ���гɶ��ٸ�С���������һ������

//�����͹��Ĳ���
const float delta = 20.0;
const float intence = 200.0;
const float emboss = 0.3;

//---------------------------------

//ˮ��Ч��
float col(vec2 coord)
{
    //angle�ǻ��ȣ�delta_theta���зֺ��ÿ�����Ļ���
    float delta_theta = PIx2 / float(angleNum);
    float col = 0.0;
    float theta = 0.0;
    for (int i = 0; i < steps; ++i)
    {
        vec2 adjc = coord;
        theta = delta_theta * float(i);

        //sin �� cos ��ֵ��0��1֮��仯�����߽�ϲ��ϱ任�ٶȾ��ô˵�ȡΧ�������뾶��ͬһȦ�ĵ�
        //����һ���ٶȳ���ʱ������õ���������ƫ��һ����
        //CC_Time��cocos�ṩ��uniform��������һ��vec4������y��ÿ������1��float�������Ķ��ǳ����˲�ͬ�ı���
        adjc.x += cos(theta) * CC_Time.y * speed + CC_Time.y * speed_x;
        adjc.y -= sin(theta) * CC_Time.y * speed - CC_Time.y * speed_y;

        //
        col = col + cos((adjc.x * cos(theta) - adjc.y * sin(theta)) * frequency) * intensity;
    }
    return cos(col);
}

void main(void)
{
    vec2 p = v_texCoord.xy / resolution.xy; //pΪȡÿһ�����صĵ�
    vec2 c1 = p;
    vec2 c2 = p;
    float cc1 = col(c1);

    c2.x += resolution.x / delta;
    float dx = emboss * (cc1 - col(c2)) / delta;

    c2.x = p.x;
    c2.y += resolution.y/delta;
    float dy = emboss * (cc1 - col(c2)) / delta;

    c1.x += dx;
    c1.y += dy;
    float alpha = 1. + dot(dx, dy) * intence;
    gl_FragColor = texture2D(CC_Texture0, c1) * alpha * v_fragmentColor * alpha;
}