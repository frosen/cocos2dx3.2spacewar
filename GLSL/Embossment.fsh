#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//--------------------------

uniform vec2 resolution; //ͼƬ�ֱ���

const vec2 comparisonPos = vec2(-3.0, -1.0); //�Ƚϵ�

const bool isConcave = false; //�Ƿ��ǰ��棬����Ϊ����Ϊ͹��

//--------------------------

void main()            
{
    //��ǰ����ɫ
    vec4 curColor = texture2D(CC_Texture0, v_texCoord); 
	
	//��ȡ��ǰ�����Ͻ�һ���λ�� �ñȽϵ�������ؾ͵ó�һ�����ص�����Ҫƫ�Ƶľ���
    vec2 texUpLeft = vec2(v_texCoord.x + comparisonPos.x / resolution.x, v_texCoord.y + comparisonPos.y / resolution.y);
	
	//��ȡ�Ƚϵ�λ�õ���ɫ
    vec4 upLeftColor = texture2D(CC_Texture0, texUpLeft);
	
	//��ǰ��ͱȽϵ����ɫ��ֵ
	//�����ǰ��ȱȽϵ���ɫǳ���Ϊ��ֵ��ת���ɻҶȺ�ᷢ�����Ե�������Ϸ�������˹⣬ӭ�������������氵
	vec4 color_delta =  upLeftColor - curColor;
	
	//��͹���ɰ���
	if (isConcave) color_delta *= -1;
	
	//���ֵ�ĻҶȣ���RGBתYUV�л��Y���Ҷȣ��Ĺ�ʽ
	float gray = dot(color_delta.rgb, vec3(0.299, 0.587, 0.114)); //dot����r*0.299 + g*0.587 + b*0.114

    //����һ����ֵ���ý��
    gl_FragColor = vec4(gray, gray, gray, 0.0) + vec4(0.5, 0.5, 0.5, 1.0);
} 
