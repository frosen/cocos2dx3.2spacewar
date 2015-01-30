#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//-------------------------------------

uniform sampler2D u_normalMap;

float timeFactor = 0.2;
float offsetFactor = 0.5;
float refractionFactor = 0.7;

vec3 eyePos = vec3(0, 0, 1000);

//获得当前点的法线贴图的法线数据
vec3 waveNormal(vec2 p)
{
	vec3 normal = texture2D(u_normalMap, p).xyz;
	normal = -1.0 + normal * 2.0; //法线取值范围是从-1到1，而发现贴图中的rgb是从0到1，所以转换
	return normalize(normal);
}

void main()
{
	//获取当前点根据时间的偏移值，并且用fract（取小数点以下）保证该值在0到1间变化
	vec2 offset = fract(v_texCoord + vec2(-CC_Time.y * timeFactor, 0));
	
	//获取法线，法线会根据时间而偏移，模拟水流
	vec3 normal = waveNormal(offset);
	
	//以纹理中心为中心
	vec2 p = -1.0 + 2.0 * v_texCoord;
	
	//当前点指向眼睛位置的矢量
	vec3 inVec = normalize(vec3(p, 0) - eyePos);
	
	//折射后的偏移
	vec3 refractVec = refract(inVec, normal, refractionFactor);
	
	//把偏移量叠加到当前位置
	vec2 coord = v_texCoord + refractVec.xy * offsetFactor;

	gl_FragColor = texture2D(CC_Texture0, coord) * v_fragmentColor;
}


