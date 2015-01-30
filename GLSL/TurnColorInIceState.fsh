#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
	  gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;
	
	  //被冰冻（完全冻结）的颜色
	  float gray = (gl_FragColor.r + gl_FragColor.g + gl_FragColor.b) / 2.0; 
	  gl_FragColor = vec4(gray * 0.8, gray * 1.2, gray * 1.5, gl_FragColor.a);
}

