#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//--------------------------

uniform vec2 resolution; //ͼƬ�ֱ���

const vec2 mosaicSize = vec2(18,18); //�����˵Ĵ�С����λ�����أ�

//--------------------------

void main()            
{
    //��ȡ�˵�Ķ�Ӧ������λ��
    vec2 intXY = vec2(v_texCoord.x * resolution.x, v_texCoord.y * resolution.y);
	
	//floorΪ����ȡ��������Ϊȡ���˵��Ӧ�����˵�λ��
    vec2 XYMosaic = 
	    vec2(floor(intXY.x / mosaicSize.x) * mosaicSize.x, 
	        floor(intXY.y / mosaicSize.y) * mosaicSize.y);
		
	//��ȡ�����˵�����λ�ö�Ӧ������λ��
    vec2 UVMosaic = vec2(XYMosaic.x / resolution.x, XYMosaic.y / resolution.y);
	
	//��ȡ��ɫ
    gl_FragColor = texture2D(CC_Texture0, UVMosaic) * v_fragmentColor;
	
} 
