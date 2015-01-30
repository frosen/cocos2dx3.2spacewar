#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()            
{
    vec4 v_orColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
    
    //以下为为RGB转YUV中获得Y（灰度）的公式
    float gray = dot(v_orColor.rgb, vec3(0.299, 0.587, 0.114)); //dot就是r*0.299 + g*0.587 + b*0.114
    gl_FragColor = vec4(gray, gray, gray, v_orColor.a);
} 
