#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//-------------------------------

//输入颜色变化量
uniform float redChange;
uniform float greenChange;
uniform float blueChange;

//-------------------------------

void main()            
{
    gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;
    
    gl_FragColor.r += redChange * gl_FragColor.a;
    gl_FragColor.g += greenChange * gl_FragColor.a;
    gl_FragColor.b += blueChange * gl_FragColor.a;
    
} 
