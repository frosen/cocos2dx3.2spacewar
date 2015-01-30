#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//----------------------------------------------

uniform vec2 resolution; //图片分辨率
//uniform vec2 motionDirection; //移动方向，按照纹理坐标，左上为0
//uniform int sampleNum; //取样数

//--------------------------------------------------

vec2 motionDirection = vec2(20.0, 0.0); //测试
int sampleNum = 20;

//----------------------------------------------

//动态模糊是有方向的模糊
void main(void)
{
	vec4 col = vec4(0);
    float count = 0.0;
	float weight = 0.0;
	
	//横坐标取样
	float sampleStepX = motionDirection.x / sampleNum;
	
	//纵坐标取样
	float sampleStepY = motionDirection.y / sampleNum;
	
	//模糊过程
	for (float x = 0.0, y = 0.0; x <= motionDirection.x && y <= motionDirection.y; x += sampleStepX, y += sampleStepY)
	{
	    //权重，离中心越近，数值越大，从0到xy
		if (motionDirection.x != 0)
		{
		    weight = (motionDirection.x - x) * (motionDirection.x - x); 
        }
		else if (motionDirection.y != 0)
		{
		    weight = (motionDirection.y - y) * (motionDirection.y - y); 
		}

        //获取偏移后的像素，加入像素和col中，越靠边权重越小
        col += texture2D(CC_Texture0, v_texCoord + vec2(x / resolution.x, y / resolution.y)).rgba * weight; 
        count += weight; //计算权重之和
	}
	
	gl_FragColor = col / count * v_fragmentColor;
    //gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor; 
}

