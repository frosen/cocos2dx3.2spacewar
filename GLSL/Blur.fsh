#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//uniform float blurRadius; //ģ���뾶 �������0
//uniform int sampleNum; //ȡ���� �������1

//uniform float ActionTime; //����ʱ�䣬0��1�仯

//----------------------------------------------

float blurRadius = 10.0; // ����
int sampleNum = 5;
float ActionTime = 0.5;

float t = 0.0; //����actionTime��0�仯��1���ٴ�1�仯��0

//-----------------------------------------------

vec3 blur(vec2 p, float r, int sampleNum);

void main(void)
{
	//ʱ��仯ϵ��
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
			float weight = (r - abs(x)) * (r - abs(y)); //Ȩ�أ�������Խ������ֵԽ�󣬴�0��r��ƽ��

			//��ȡƫ�ƺ�����أ��������غ�col�У�Խ����Ȩ��ԽС
			//x��y������������texture2D����0��1�ı仯��������Ҫxy����ͼƬ�ֱ��ʣ�����1920 * 1080�����˴�Ϊ�˼��ȫ����1000
			col += texture2D(CC_Texture0, p + vec2(x * 0.001, y * 0.001)).rgb * weight;
			count += weight; //����Ȩ��֮��
		}
	}

	return col / count; //�õ�ƽ��rgb
}
