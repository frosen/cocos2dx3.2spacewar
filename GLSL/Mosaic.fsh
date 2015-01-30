#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//--------------------------

uniform vec2 resolution; //图片分辨率

const vec2 mosaicSize = vec2(18,18); //马赛克的大小（单位是像素）

//--------------------------

void main()            
{
    //获取此点的对应的像素位置
    vec2 intXY = vec2(v_texCoord.x * resolution.x, v_texCoord.y * resolution.y);
	
	//floor为向下取整，这里为取到此点对应马赛克的位置
    vec2 XYMosaic = 
	    vec2(floor(intXY.x / mosaicSize.x) * mosaicSize.x, 
	        floor(intXY.y / mosaicSize.y) * mosaicSize.y);
		
	//获取马赛克的像素位置对应的纹理位置
    vec2 UVMosaic = vec2(XYMosaic.x / resolution.x, XYMosaic.y / resolution.y);
	
	//获取颜色
    gl_FragColor = texture2D(CC_Texture0, UVMosaic) * v_fragmentColor;
	
} 
