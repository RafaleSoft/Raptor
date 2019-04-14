/***************************************************************************/
/*                                                                         */
/*  blinn.vs                                                               */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#version 120

const int MAX_LIGHTS = 3;
uniform int lightEnable[gl_MaxLights];

varying vec3 normal;
varying	vec4 lightDirs[MAX_LIGHTS];
varying	vec3 halfVector[MAX_LIGHTS];

void main (void)
{
	normal = gl_NormalMatrix * gl_Normal;
	vec3 ecPos = vec3(gl_ModelViewMatrix * gl_Vertex);

	/*	This is true only for directional lights	*/
	/*halfVector = gl_LightSource[0].halfVector.xyz;*/

	for (int i=0 ; i<MAX_LIGHTS ; i++)
	{
		int numl = lightEnable[i];
		if (numl >= 0)
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


