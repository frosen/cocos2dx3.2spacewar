#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//-------------------------------

uniform vec2 lightPos; //光相对于纹理的位置 按纹理的比例
uniform int sampleNum; //采样数量
uniform float intensity; //强度
uniform float exposure; //曝光度 0.5左右
uniform float decay; //光强衰减量 0.95左右

//-------------------------------

void main(void)
{
    vec4 color = texture2D(CC_Texture0, v_texCoord); //原始纹理获取的当前像素rgba
    
    vec2 newTex = vec2(v_texCoord.x, v_texCoord.y); //新纹理位置
    
    vec2 deltaTexCoord = (newTex - lightPos) / float(sampleNum); //光照点指向纹理点的矢量，并按照采样数切成一小段矢量
    
      float illuminationdecay = 1.0; //光强衰减
      vec4 sampleColor = vec4(0.0); //采样点的色彩
      
      for(int i = 0; i < sampleNum; ++i)
      {
            //取纹理点向光照点偏移一个采样度的点
    	    newTex.x = newTex.x - deltaTexCoord.x;
    	    newTex.y = newTex.y - deltaTexCoord.y;
    
    	    //取此点的颜色
    	    sampleColor = texture2D(CC_Texture0, newTex);
    	    
    	    //颜色乘上系数（衰减度和强度）
    	    sampleColor *= (illuminationdecay * intensity);
    
    	    //累加到本点颜色上
    	    color += sampleColor;
    
    	    //衰减
    	    illuminationdecay *= illuminationdecay * decay; 
    }
    
    color *= exposure; //曝光度
    
    gl_FragColor = color * v_fragmentColor;
}