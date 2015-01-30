#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//----------------------------------------------

uniform vec2 resolution; //ͼƬ�ֱ���
//uniform vec2 motionDirection; //�ƶ����򣬰����������꣬����Ϊ0
//uniform int sampleNum; //ȡ����

//--------------------------------------------------

vec2 motionDirection = vec2(20.0, 0.0); //����
int sampleNum = 20;

//----------------------------------------------

//��̬ģ�����з����ģ��
void main(void)
{
	vec4 col = vec4(0);
    float count = 0.0;
	float weight = 0.0;
	
	//������ȡ��
	float sampleStepX = motionDirection.x / sampleNum;
	
	//������ȡ��
	float sampleStepY = motionDirection.y / sampleNum;
	
	//ģ������
	for (float x = 0.0, y = 0.0; x <= motionDirection.x && y <= motionDirection.y; x += sampleStepX, y += sampleStepY)
	{
	    //Ȩ�أ�������Խ������ֵԽ�󣬴�0��xy
		if (motionDirection.x != 0)
		{
		    weight = (motionDirection.x - x) * (motionDirection.x - x); 
        }
		else if (motionDirection.y != 0)
		{
		    weight = (motionDirection.y - y) * (motionDirection.y - y); 
		}

        //��ȡƫ�ƺ�����أ��������غ�col�У�Խ����Ȩ��ԽС
        col += texture2D(CC_Texture0, v_texCoord + vec2(x / resolution.x, y / resolution.y)).rgba * weight; 
        count += weight; //����Ȩ��֮��
	}
	
	gl_FragColor = col / count * v_fragmentColor;
    //gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor; 
}

