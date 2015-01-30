#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
    gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;
    
    gl_FragColor.r = gl_FragColor.r * 0.5;
    gl_FragColor.g = gl_FragColor.g * 0.8;
    gl_FragColor.b = gl_FragColor.b + gl_FragColor.a * 0.2;
}

