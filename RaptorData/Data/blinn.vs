#version 120

const int MAX_LIGHTS = 3;
uniform int lightEnable[gl_MaxLights];

varying vec3 normal;
varying	vec4 lightDirs[3];
varying	vec3 halfVector[3];

void main (void)
{
	normal = gl_NormalMatrix * gl_Normal;
	vec3 ecPos = vec3(gl_ModelViewMatrix * gl_Vertex);

	/*	This is true only for directional lights	*/
	/*halfVector = gl_LightSource[0].halfVector.xyz;*/

	for (int numl=0 ; numl<MAX_LIGHTS ; numl++)
	{
		if (lightEnable[numl] > 0)
		{
			vec3 ldir = vec3(gl_LightSource[numl].position) - ecPos;
			lightDirs[numl].xyz = ldir;
			float dist = length(ldir);
			
			/*	In most situations, attenuation could be computed per vertex	*/
			lightDirs[numl].w = 1.0 / (	gl_LightSource[numl].constantAttenuation +
										gl_LightSource[numl].linearAttenuation * dist +
										gl_LightSource[numl].quadraticAttenuation * dist * dist);

			halfVector[numl] = 0.5 * (normalize(-ecPos) + normalize(ldir));
		}
	}

	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
}


