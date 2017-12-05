#version 120

const int MAX_LIGHTS = 3;
uniform int lightEnable[gl_MaxLights];

varying vec3 normal;
varying	vec4 lightDirs[MAX_LIGHTS];
varying	vec3 eyeDir;

void main (void)
{
	normal = gl_NormalMatrix * gl_Normal;

	eyeDir = -vec3(gl_ModelViewMatrix * gl_Vertex);
	for (int i=0 ; i<MAX_LIGHTS ; i++)
	{
		int numl = lightEnable[i];
		if (numl >= 0)
		{
			vec3 ldir = vec3(gl_LightSource[numl].position) + eyeDir;
			lightDirs[numl].xyz = ldir;
			float dist = length(ldir);
			lightDirs[numl].w = 1.0 / (	gl_LightSource[numl].constantAttenuation +
										gl_LightSource[numl].linearAttenuation * dist +
										gl_LightSource[numl].quadraticAttenuation * dist * dist);
		}
	} 

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}

