//为图片描边，沿着图片最外层透明的区域
//可以设置描边宽度（宽度需要除以图片分辨率），描边颜色

#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//---------------------------

uniform float radius; //描边的宽度
uniform vec3 strokeColor; //描边的颜色

//---------------------------

void main()            
{
    //原图颜色
    vec4 normal = texture2D(CC_Texture0, v_texCoord);
    
    //一个用于累积的值
    vec4 accum = vec4(0.0);
    
    //取原点左上，左下，右上，右下距离为radius的点的颜色值进行加和
    accum += texture2D(CC_Texture0, vec2(v_texCoord.x - radius, v_texCoord.y - radius));
    accum += texture2D(CC_Texture0, vec2(v_texCoord.x + radius, v_texCoord.y - radius));
    accum += texture2D(CC_Texture0, vec2(v_texCoord.x + radius, v_texCoord.y + radius));
    accum += texture2D(CC_Texture0, vec2(v_texCoord.x - radius, v_texCoord.y + radius));
    
    if (normal.a > 0.0) //如果此点的原图透明度不为0，说明此点有颜色，则用原来的颜色
    {
        accum = normal;
    }
    else if (accum.a > 0.0) 
    {
        //如果此点的原图透明度为0，说明没有颜色，但是accum的透明度不为0，而accum是其周边4点的累积值，
        //说明他周边有带颜色的点，也就说明此点在原图周围，于是赋值为描边颜色
        accum.r = strokeColor.r;
        accum.g = strokeColor.g;
        accum.b = strokeColor.b;
    }
    
    gl_FragColor = v_fragmentColor * accum;
} 