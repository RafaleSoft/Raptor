/***************************************************************************/
/*                                                                         */
/*  phong.vs                                                               */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#version 440 compatibility

#include "Raptor.glsl"


LIGHT_PRODUCTS(0) R_LightProducts;

layout(location = 0) in vec4 i_Position;
layout(location = 2) in vec4 i_Normal;
layout(location = 8) in vec4 i_TexCoord;


out vec3 normal;
out	vec4 lightDirs[MAX_LIGHTS];
out	vec3 eyeDir;
out vec4 o_texCoord;

void main (void)
{
	normal = normalize(gl_NormalMatrix * vec3(i_Normal.xyz));

	eyeDir = -vec3(gl_ModelViewMatrix * i_Position);

	for (int i=0 ; i<MAX_LIGHTS ; i++)
	{
		if (R_LightProducts.lights[i].enable)
		{
			vec3 ldir = vec3(R_LightProducts.lights[i].position) + eyeDir;
			lightDirs[i].xyz = ldir;
			float dist = length(ldir);
			
			/*	In most situations, attenuation could be computed per vertex	*/
			vec4 attenuation = R_LightProducts.lights[i].attenuation;
			lightDirs[i].w = 1.0 / (	attenuation.z +
										attenuation.y * dist +
										attenuation.x * dist * dist);
		}
	} 

	gl_Position = gl_ModelViewProjectionMatrix * i_Position;
	o_texCoord = i_TexCoord;
}

