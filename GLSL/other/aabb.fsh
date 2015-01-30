#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

float timeFactor = 4.0;
float texFactor = 12.0;
float ampFactor = 0.01;
vec4 lightColor = vec4(1, 1, 1, 1);
vec3 lightDir = vec3(1, 1, 1);

vec2 p;
vec2 coord;

float waveHeight(vec2 p)
{
	return cos(-CC_Time.y * timeFactor + length(p) * texFactor) * ampFactor;
}

vec3 waveNormal(vec2 p)
{
	vec2 resolution = vec2(480, 320);

	float scale = 240;

	float waveHeightRight = waveHeight(p + vec2(2.0 / resolution.x, 0)) * scale;

	float waveHeightLeft = waveHeight(p - vec2(2.0 / resolution.x, 0)) * scale;

	float waveHeightTop = waveHeight(p + vec2(0, 2.0 / resolution.y)) * scale;

	float waveHeightBottom = waveHeight(p - vec2(0, 2.0 / resolution.y)) * scale;

	vec3 t = vec3(1, 0, waveHeightRight - waveHeightLeft);

	vec3 b = vec3(0, 1, waveHeightTop - waveHeightBottom);

	vec3 n = cross(t, b);

	return normalize(n);
}

void main(void)
{	
	//vec2 center = vec2(0, 0);
	//p = (v_texCoord - center) * 2.0;
	
	p = -1.0 + v_texCoord * 2.0s;
	
	coord = v_texCoord + normalize(p) * waveHeight(p);
	
	gl_FragColor = texture2D(CC_Texture0, coord) * v_fragmentColor * lightColor * max(0, dot(lightDir, waveNormal(p)));;
}
