#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//------------------------------------

uniform vec2 resolution; //图片分辨率

//数字滤波器所需要采样的位置为当前点周围的8个点，以下分别对应8个点的x，y位置
const mat3 filPosDelX = mat3(vec3(-1.0, 0.0, 1.0), vec3(0.0, 0.0 ,0.0), vec3(1.0,1.0,1.0));
const mat3 filPosDelY = mat3(vec3(-1.0,-1.0,-1.0), vec3(-1.0,0.0,1.0), vec3(-1.0,0.0,1.0));

//BOX 模糊效果 系数 求当前点以及周围8点的平均值 1/9 = 0.1111111111111
//const mat3 filterCoefficient = mat3(0.11111111, 0.11111111, 0.11111111,
//                                    0.11111111, 0.11111111, 0.11111111,
//                                    0.11111111, 0.11111111, 0.11111111);

//高斯 模糊效果 系数 求当前点以及周围8点的加权平均值 1/16 = 0.0625, 1/8 = 0.125, 1/4 = 0.25
//const mat3 filterCoefficient = mat3(0.0625, 0.125, 0.0625,
//                                    0.125, 0.25, 0.125,
//                                    0.0625, 0.125, 0.0625);

//拉普拉斯 锐化效果 系数 
//先将自身与周围的8个象素相减，表示自身与周围象素的差别；再将这个差别加上自身作为新象素
const mat3 filterCoefficient = mat3(9, -1, -1,
                                    -1, -1, -1,
                                    -1, -1, -1);

//-------------------------------------


void main()
{
    //当前点的像素位置
    vec2 intXY = vec2(v_texCoord.x * resolution.x, v_texCoord.y * resolution.y);
	
	//最终累加的颜色
	vec4 final_color = vec4(0.0, 0.0, 0.0, 0.0);
	
	//数字滤波器采样赋值过程，取当前点周围8个点，获取他们的颜色乘以系数后混合在一起
	//系数要求叠加起来等于1
    for(int i = 0; i < 3; ++i)  
    {  
        for(int j = 0; j < 3; ++j)  
        {
		    //取周围8个点的像素位置，并转换为纹理位置
            vec2 sampleUV = vec2((intXY.x + filPosDelX[i][j]) / resolution.x, 
			                     (intXY.y + filPosDelY[i][j]) / resolution.y);
			
			//去采样位置的颜色，乘以系数后叠加到最终颜色里
            final_color += texture2D(CC_Texture0, sampleUV) * filterCoefficient[i][j];
        }  
    }
	
    gl_FragColor = final_color * v_fragmentColor; 
} 

 
 
