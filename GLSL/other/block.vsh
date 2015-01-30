#ifdef GL_ES
precision mediump float;
#endif

uniform vec3 LightPosition;

const float SpecularContribution = 0.3;
const float DiffuseContuibution = 1.0 - SpecularContribution;

varying float LightIntensity;
varying vec2 MCposition;

void main(void)
{
	vec3 ecPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	vec3 tnorm = normalize(gl_NormalMatrix * gl_Normal);
	vec3 lightVec = normalize(LightPosition - ecPosition);
	vec3 reflectVec = reflect(-lightVec, tnorm);
	vec3 viewVec = normalize(-ecPosition);
	
	float diffuse = max(dot(lightVec, tnorm), 0.0);
	float spec = 0.0;
	
	if(diffsue > 0.0)
	{
		spec = max(dot(reflectVec, viewVec), 0.0);
		spec = pow(spec, 16.0);
	}
	
	LightIntensity = DiffuseContuibution * diffuse + SpecularContribution * spec;
	MCposition = gl_Vertex.xy;
	gl_Position = ftransform();
}