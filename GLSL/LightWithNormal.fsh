#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//-----------------------------------------------

uniform sampler2D u_normalMap;

uniform vec3 u_lightColor; //漫反射
uniform vec3 u_ambientLightColor; //环境光

uniform float u_diffuseRatio; //漫反射比例，取值0到1
uniform float u_ambientRatio; //环境光比例，取值0到1，漫反射的比例为（1-环境光比例）

uniform vec3 u_lightPos; //光位置
uniform float u_kBump; //凸起

vec2 resolution = vec2(480, 320); //content size

//-------------------------------------------------

void main(void)
{	
	gl_FragColor = texture2D(CC_Texture0, v_texCoord);
	
	//生成法线
    vec3 normal = texture2D(u_normalMap, v_texCoord).rgb;
	
	//make it to range [-1,1]. see: http://user.qzone.qq.com/350479720/blog/1399375155
    normal = normal * 2.0 - 1.0;
	
    //由于法线贴图生成于 'Nvidia Texture Tools for photoshop' Y轴向下, 所以需要转换
    //see: http://user.qzone.qq.com/350479720/blog/1403447890
    normal.y = -normal.y;
	
	//if the vertex.z mult kBump, then the normal.z should div kBump and re-normalize
    if(u_kBump != 1.0)
	{       
		normal.z /= u_kBump;
		normal = normalize(normal);
    }
	
	//当前点的像素位置
	vec3 curPixelPos = vec3(v_texCoord.x * resolution.x, (1.0 - v_texCoord.y) * resolution.y, 0.0);
	
	//光的方向，点指向光
	vec3 posToLightDir = normalize(u_lightPos - curPixelPos);
	
	//计算光照强度
	float LightIntensity = max(0.0, dot(normal, posToLightDir));
	
	//最终光
    vec3 light = u_lightColor * LightIntensity * u_diffuseRatio + u_ambientLightColor * u_ambientRatio;
	
	//乘以光获得最终颜色
	gl_FragColor = vec4(gl_FragColor.rgb * light, gl_FragColor.a);
	
}


