#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

vec2 dd;

void main(void)
{
	dd.x = sin(CC_Time.y * 4 + v_texCoord.x * 10) * 0.01 + v_texCoord.x;
	dd.y = cos(CC_Time.y * 3 + v_texCoord.y * 20) * 0.01 + v_texCoord.y;
	
	gl_FragColor = texture2D(CC_Texture0, dd) * v_fragmentColor;
}
