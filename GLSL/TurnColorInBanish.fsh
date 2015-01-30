#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
	gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;
	
	//被放逐的颜色
	float gray = (gl_FragColor.r + gl_FragColor.g + gl_FragColor.b) / 3.0;
	gl_FragColor = vec4(gray * 0.9, gray * 1.2, gray * 0.8, gl_FragColor.a * (gray + 0.1));
}

