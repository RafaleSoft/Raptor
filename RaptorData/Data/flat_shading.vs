/***************************************************************************/
/*                                                                         */
/*  flat_shading.vs                                                        */
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
layout(location = 6) in vec4 i_Tangent;
layout(location = 8) in vec4 i_TexCoord;

out vec4 d_Color;
out vec4 s_Color;
out vec4 o_texCoord;



void main (void)
{
	// Compute lighting in eye space
	vec3 normal = normalize(gl_NormalMatrix * vec3(i_Normal.xyz));
	vec3 T = normalize(gl_NormalMatrix * vec3(i_Tangent.xyz));
	vec3 ecPos = -vec3(gl_ModelViewMatrix * i_Position);

	vec3 binormal = cross(normal,T);
	vec3 eyedir;
	eyedir.x = dot(ecPos,T);
	eyedir.y = dot(ecPos,binormal);
	eyedir.z = dot(ecPos,normal);

	vec4 diffuse = R_LightProducts.scene_ambient;
	vec4 specular = vec4(0.0,0.0,0.0,0.0);

	for (int i=0 ; i<MAX_LIGHTS ; i++)
	{
		if (R_LightProducts.lights[i].enable)
		{
			// Compute lighting in eye space
			vec3 ldir = vec3(R_LightProducts.lights[i].position) + ecPos;

			float dist = length(ldir);
			ldir = normalize(ldir);
		
			vec4 lightDirs;
			lightDirs.x = dot(ldir,T);
			lightDirs.y = dot(ldir,binormal);
			lightDirs.z = dot(ldir,normal);
			
			vec4 attenuation = R_LightProducts.lights[i].attenuation;
			float att = 1.0 / (	attenuation.z + attenuation.y * dist + attenuation.x * dist * dist);

#ifdef AMBIENT_OCCLUSION
			// Compute ambient occlusion attenuation
			// gl_Color is provided from computeElements on self geometry
			att = att * gl_Color.r;
#endif
			diffuse += att * R_LightProducts.lights[i].ambient;

			float NdotL = max(0.0,dot(normal,normalize(lightDirs.xyz)));
			if (NdotL > 0.0)
			{
				diffuse += att * NdotL * R_LightProducts.lights[i].diffuse;
				
				vec3 r = reflect(-lightDirs.xyz,normal);
				float NdotHV = max(dot(normalize(r),normalize(eyedir)),0.0);

				specular += att * pow(NdotHV,R_LightProducts.shininess) * R_LightProducts.lights[i].specular;
			}
		}
	}

	d_Color = diffuse;
	s_Color = specular;
	o_texCoord = i_TexCoord;
	gl_Position =  gl_ModelViewProjectionMatrix * i_Position;
}
