#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
    gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;
    
    //��ԭ����ɫ����
    gl_FragColor.r *= 0.8;
  	gl_FragColor.g *= 0.8;
  	gl_FragColor.b *= 0.8;
  	
  	//��ǿ��ɫ����ʾ�ж�����΢��ǿ��ɫ��Ϊ�˺ÿ�
  	gl_FragColor.r += 0.08 * gl_FragColor.a;
  	gl_FragColor.g += 0.2 * gl_FragColor.a;
}
