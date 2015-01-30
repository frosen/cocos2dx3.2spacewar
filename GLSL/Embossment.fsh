#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//--------------------------

uniform vec2 resolution; //图片分辨率

const vec2 comparisonPos = vec2(-3.0, -1.0); //比较点

const bool isConcave = false; //是否是凹面，设置为否定则为凸面

//--------------------------

void main()            
{
    //当前点颜色
    vec4 curColor = texture2D(CC_Texture0, v_texCoord); 
	
	//获取当前点左上角一点的位置 用比较点除以像素就得出一个像素点所需要偏移的距离
    vec2 texUpLeft = vec2(v_texCoord.x + comparisonPos.x / resolution.x, v_texCoord.y + comparisonPos.y / resolution.y);
	
	//获取比较点位置的颜色
    vec4 upLeftColor = texture2D(CC_Texture0, texUpLeft);
	
	//求当前点和比较点的颜色差值
	//如果当前点比比较点颜色浅则会为负值，转换成灰度后会发亮，显得像从左上方向打来了光，迎光面亮而背光面暗
	vec4 color_delta =  upLeftColor - curColor;
	
	//从凸面变成凹面
	if (isConcave) color_delta *= -1;
	
	//求差值的灰度，用RGB转YUV中获得Y（灰度）的公式
	float gray = dot(color_delta.rgb, vec3(0.299, 0.587, 0.114)); //dot就是r*0.299 + g*0.587 + b*0.114

    //增加一个差值后获得结果
    gl_FragColor = vec4(gray, gray, gray, 0.0) + vec4(0.5, 0.5, 0.5, 1.0);
} 
