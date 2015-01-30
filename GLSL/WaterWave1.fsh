#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

float timeFactor = 4.0; //时间影响度
float texFactor = 12.0; //控制水波粒度
float ampFactor = 0.01; //控制水波幅度

vec2 resolution = vec2(480, 320);
float scale = 240.0;

vec4 lightColor = vec4(1, 1, 1, 1); //白光
vec3 lightDir = normalize(vec3(1, 1, 1)); //光的方向 normalize得方向相同长度为1的矢量值

//用当前点到圆心的距离乘以系数（越大波长越大），
//加上时间的偏移量，获取其cos值（即随时间变化，值向一个方向增长时，cos值在-1,1之间变化）
//然后乘以幅度系数，得到当前点的高度
float waveHeight(vec2 p)
{
	return cos(-CC_Time.y * timeFactor + length(p) * texFactor) * ampFactor;
}

vec3 waveNormal(vec2 p)
{	
	//当前点往右2个像素的高度值乘以scale缩放系数，下同
	float waveHeightRight = waveHeight(p + vec2(2.0 / resolution.x, 0)) * scale;
	float waveHeightLeft = waveHeight(p - vec2(2.0 / resolution.x, 0)) * scale;
	//此处注意p的y轴是向下的，所以top用减的，Bottom用加
	float waveHeightTop = waveHeight(p - vec2(0, 2.0 / resolution.y)) * scale; 
	float waveHeightBottom = waveHeight(p + vec2(0, 2.0 / resolution.y)) * scale;
	
	//根据两边点的高度差，求出当前点在x或者y轴的切线
	vec3 t = vec3(1, 0, waveHeightRight - waveHeightLeft);
	vec3 b = vec3(0, 1, waveHeightTop - waveHeightBottom);
	
	//cross为叉积，求出一个垂直于t和b的矢量值，如果t到b视为逆时针旋转，则最后的值指向自己的方向
	//xy轴的切线的垂直方向就是当前点的法线方向
	vec3 n = cross(t, b);
	
	//返回方向相同长度为1的矢量值
	return normalize(n);
}

void main(void)
{
	//设定水波的中心位置，p为当前点到中心的纹理位置
	//vec2 center = vec2(0, 0);
	//p = (v_texCoord - center) * 2.0;
	
	//设水波中心为纹理中心，p为当前点到中心的纹理位置，p在xy两轴[-1,1]之间变化
	vec2 p = -1.0 + v_texCoord * 2.0;
	
	//normalize可以求出中心到p点的矢量变成距离为1后的矢量，
	//然后根据高度的变化，p点纹理在与中心点沿线方向正负变化
	vec2 coord = v_texCoord + normalize(p) * waveHeight(p);
	
	//求出当前点颜色
	gl_FragColor = texture2D(CC_Texture0, coord) * v_fragmentColor;
	
	//waveNormal求出当前点法线，dot求出光照和法线的夹角，越小光照越强，然后乘以光照颜色后叠乘到当前色上
	gl_FragColor = vec4(gl_FragColor.rgb * lightColor * max(0, dot(lightDir, waveNormal(p))), gl_FragColor.a);
}
