#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform vec2 resolution; //图片分辨率

//---------------------------

const float PIx2 = 6.28318531; //2π = 2 * 3.1415926

//速度
const float speed = 0.2;
const float speed_x = 0.3;
const float speed_y = 0.3;

//几何参数
const float intensity = 3.0; //强度
const int steps = 8;
const float frequency = 4.0; //频率
const int angleNum = 7; //把一个圆弧切成多少个小弧，最好是一个素数

//反射和凸起的参数
const float delta = 20.0;
const float intence = 200.0;
const float emboss = 0.3;

//---------------------------------

//水晶效果
float col(vec2 coord)
{
    //angle是弧度，delta_theta是切分后的每个弧的弧长
    float delta_theta = PIx2 / float(angleNum);
    float col = 0.0;
    float theta = 0.0;
    for (int i = 0; i < steps; ++i)
    {
        vec2 adjc = coord;
        theta = delta_theta * float(i);

        //sin 和 cos 让值在0到1之间变化，两者结合不断变换速度就让此点取围绕这个点半径相同一圈的点
        //加上一个速度乘以时间就是让点再向右下偏移一定量
        //CC_Time是cocos提供的uniform参数，是一个vec4，其中y是每秒增加1的float，其他的都是乘以了不同的比例
        adjc.x += cos(theta) * CC_Time.y * speed + CC_Time.y * speed_x;
        adjc.y -= sin(theta) * CC_Time.y * speed - CC_Time.y * speed_y;

        //
        col = col + cos((adjc.x * cos(theta) - adjc.y * sin(theta)) * frequency) * intensity;
    }
    return cos(col);
}

void main(void)
{
    vec2 p = v_texCoord.xy / resolution.xy; //p为取每一个像素的点
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