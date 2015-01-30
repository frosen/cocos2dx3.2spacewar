#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//uniform float blurRadius; //模糊半径 必须大于0
//uniform int sampleNum; //取样数 必须大于1

//uniform float ActionTime; //动作时间，0到1变化

//----------------------------------------------

float blurRadius = 10.0; // 测试
int sampleNum = 5;
float ActionTime = 0.5;

float t = 0.0; //根据actionTime从0变化到1，再从1变化到0

//-----------------------------------------------

vec3 blur(vec2 p, float r, int sampleNum);

void main(void)
{
	//时间变化系数
	if(ActionTime <= 0.5)
	{
		t = ActionTime * 2.0;
	}
	else
	{
		t = 2.0 - ActionTime * 2.0;
	}

	float r = blurRadius * t;

	vec3 col = blur(v_texCoord, r, sampleNum);
	gl_FragColor = vec4(col, 1.0) * v_fragmentColor;
	//gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;
}

vec3 blur(vec2 p, float r, int sampleNum)
{
	float sampleStep = r / float(sampleNum);

	vec3 col = vec3(0);
	float count = 0.0;

	for(float x = -r; x < r; x += sampleStep)
	{
		for(float y = -r; y < r; y += sampleStep)
		{
			float weight = (r - abs(x)) * (r - abs(y)); //权重，离中心越近，数值越大，从0到r的平方

			//获取偏移后的像素，加入像素和col中，越靠边权重越小
			//x，y是像素数，但texture2D中是0到1的变化，所以需要xy除以图片分辨率（比如1920 * 1080），此处为了简便全除以1000
			col += texture2D(CC_Texture0, p + vec2(x * 0.001, y * 0.001)).rgb * weight;
			count += weight; //计算权重之和
		}
	}

	return col / count; //得到平均rgb
}
