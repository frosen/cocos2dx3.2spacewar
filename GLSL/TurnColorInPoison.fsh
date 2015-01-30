#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
    gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;
    
    //让原有颜色变弱
    gl_FragColor.r *= 0.8;
  	gl_FragColor.g *= 0.8;
  	gl_FragColor.b *= 0.8;
  	
  	//加强绿色，表示中毒，略微加强红色，为了好看
  	gl_FragColor.r += 0.08 * gl_FragColor.a;
  	gl_FragColor.g += 0.2 * gl_FragColor.a;
}
