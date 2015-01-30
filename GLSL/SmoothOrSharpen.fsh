#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//------------------------------------

uniform vec2 resolution; //ͼƬ�ֱ���

//�����˲�������Ҫ������λ��Ϊ��ǰ����Χ��8���㣬���·ֱ��Ӧ8�����x��yλ��
const mat3 filPosDelX = mat3(vec3(-1.0, 0.0, 1.0), vec3(0.0, 0.0 ,0.0), vec3(1.0,1.0,1.0));
const mat3 filPosDelY = mat3(vec3(-1.0,-1.0,-1.0), vec3(-1.0,0.0,1.0), vec3(-1.0,0.0,1.0));

//BOX ģ��Ч�� ϵ�� ��ǰ���Լ���Χ8���ƽ��ֵ 1/9 = 0.1111111111111
//const mat3 filterCoefficient = mat3(0.11111111, 0.11111111, 0.11111111,
//                                    0.11111111, 0.11111111, 0.11111111,
//                                    0.11111111, 0.11111111, 0.11111111);

//��˹ ģ��Ч�� ϵ�� ��ǰ���Լ���Χ8��ļ�Ȩƽ��ֵ 1/16 = 0.0625, 1/8 = 0.125, 1/4 = 0.25
//const mat3 filterCoefficient = mat3(0.0625, 0.125, 0.0625,
//                                    0.125, 0.25, 0.125,
//                                    0.0625, 0.125, 0.0625);

//������˹ ��Ч�� ϵ�� 
//�Ƚ���������Χ��8�������������ʾ��������Χ���صĲ���ٽ����������������Ϊ������
const mat3 filterCoefficient = mat3(9, -1, -1,
                                    -1, -1, -1,
                                    -1, -1, -1);

//-------------------------------------


void main()
{
    //��ǰ�������λ��
    vec2 intXY = vec2(v_texCoord.x * resolution.x, v_texCoord.y * resolution.y);
	
	//�����ۼӵ���ɫ
	vec4 final_color = vec4(0.0, 0.0, 0.0, 0.0);
	
	//�����˲���������ֵ���̣�ȡ��ǰ����Χ8���㣬��ȡ���ǵ���ɫ����ϵ��������һ��
	//ϵ��Ҫ�������������1
    for(int i = 0; i < 3; ++i)  
    {  
        for(int j = 0; j < 3; ++j)  
        {
		    //ȡ��Χ8���������λ�ã���ת��Ϊ����λ��
            vec2 sampleUV = vec2((intXY.x + filPosDelX[i][j]) / resolution.x, 
			                     (intXY.y + filPosDelY[i][j]) / resolution.y);
			
			//ȥ����λ�õ���ɫ������ϵ������ӵ�������ɫ��
            final_color += texture2D(CC_Texture0, sampleUV) * filterCoefficient[i][j];
        }  
    }
	
    gl_FragColor = final_color * v_fragmentColor; 
} 

 
 
