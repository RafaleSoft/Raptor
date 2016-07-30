#version 120

#ifdef EMBM_RENDERING
	#define EYE_SPACE 1
//#define AMBIENT_OCCLUSION 1
#endif

const int MAX_LIGHTS = 5;

attribute vec4 tangent;

varying vec4 lightDirs[MAX_LIGHTS];
varying vec3 eyedir;

#ifdef EMBM_RENDERING
	varying vec3 rr;
#endif

uniform int lightEnable[gl_MaxLights];
uniform vec4 eyePos;


void main (void)
{
#ifdef EYE_SPACE
	// Compute lighting in eye space
	vec3 normal = normalize(gl_NormalMatrix * vec3(gl_Normal.xyz));
	vec3 T = normalize(gl_NormalMatrix * vec3(tangent.xyz));
	vec3 ecPos = -vec3(gl_ModelViewMatrix * gl_Vertex);
#else
	// Compute lighting in object space
	vec3 normal = vec3(gl_Normal.xyz);
	vec3 T = vec3(tangent.xyz);
	vec3 ecPos = vec3(eyePos.xyz) - vec3(gl_Vertex.xyz);
#endif

	vec3 binormal = cross(normal,T);
	eyedir.x = dot(ecPos,T);
	eyedir.y = dot(ecPos,binormal);
	eyedir.z = dot(ecPos,normal);

#ifdef EMBM_RENDERING
	rr = reflect(-ecPos,normal);
#endif

	for (int i=0 ; i<MAX_LIGHTS ; i++)
	{
		int numl = lightEnable[i];
		if (numl >= 0)
		{
#ifdef EYE_SPACE
			// Compute lighting in eye space
			vec3 ldir = vec3(gl_LightSource[numl].position) + ecPos;
#else
			// Compute lighting in object space
			vec4 lpos = gl_ModelViewMatrixInverse * gl_LightSource[numl].position - gl_Vertex;
			vec3 ldir = vec3(lpos.xyz);
#endif
			float dist = length(ldir);
			ldir = normalize(ldir);
		
			lightDirs[numl].x = dot(ldir,T);
			lightDirs[numl].y = dot(ldir,binormal);
			lightDirs[numl].z = dot(ldir,normal);
			
			lightDirs[numl].w = 1.0 / (	gl_LightSource[numl].constantAttenuation +
										gl_LightSource[numl].linearAttenuation * dist +
										gl_LightSource[numl].quadraticAttenuation * dist * dist);
#ifdef AMBIENT_OCCLUSION
			// Compute ambient occlusion attenuation
			// gl_Color is provided from computeElements on self geometry
			lightDirs[numl].w = lightDirs[numl].w * gl_Color.r;
#endif
		}
	}

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}
