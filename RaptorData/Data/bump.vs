/***************************************************************************/
/*                                                                         */
/*  bump.vs                                                                */
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

#version 440 compatibility


#ifdef EMBM_RENDERING
	#define EYE_SPACE 1
//#define AMBIENT_OCCLUSION 1
#endif

//	Maximum number of lights due to number of interpolators available
const int MAX_LIGHTS = 5;
const int GL_MAX_LIGHTS = 8;

//
//	Raptor Uniform blocs
//
//layout (binding = 0) uniform Transform {
//	mat4 ModelViewMatrix;
//	mat4 ModelViewMatrixInverse;
//	mat4 ModelViewProjectionMatrix;
//	mat4 NormalMatrix;
//} R_Transform;

struct LightProduct
{
	vec4 position;
	vec4 attenuation;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	float reserved[3];
	bool enable;
	bool reserved2[3];
};

layout (binding = 0) uniform LightProducts
{
	LightProduct lights[MAX_LIGHTS];
	vec4		 scene_ambient;
} R_LightProducts;


uniform vec4 eyePos;


layout(location = 0) in vec4 i_Position;
layout(location = 2) in vec4 i_Normal;
layout(location = 6) in vec4 i_Tangent;
layout(location = 8) in vec4 i_TexCoord;

out vec4 lightDirs[MAX_LIGHTS];
out vec3 eyedir;
out vec4 o_texCoord;


#ifdef EMBM_RENDERING
	varying vec3 rr;
#endif



void main (void)
{
#ifdef EYE_SPACE
	// Compute lighting in eye space
	vec3 normal = normalize(gl_NormalMatrix * vec3(i_Normal.xyz));
	vec3 T = normalize(gl_NormalMatrix * vec3(i_Tangent.xyz));
	vec3 ecPos = -vec3(gl_ModelViewMatrix * i_Position);
#else
	// Compute lighting in object space
	vec3 normal = vec3(i_Normal.xyz);
	vec3 T = vec3(i_Tangent.xyz);
	vec3 ecPos = vec3(eyePos.xyz) - vec3(i_Position.xyz);
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
		if (R_LightProducts.lights[i].enable)
		{
#ifdef EYE_SPACE
			// Compute lighting in eye space
			vec3 ldir = vec3(R_LightProducts.lights[i].position) + ecPos;
#else
			// Compute lighting in object space
			vec4 lpos = gl_ModelViewMatrixInverse * R_LightProducts.lights[i].position - i_Position;
			vec3 ldir = vec3(lpos.xyz);
#endif
			float dist = length(ldir);
			ldir = normalize(ldir);
		
			lightDirs[i].x = dot(ldir,T);
			lightDirs[i].y = dot(ldir,binormal);
			lightDirs[i].z = dot(ldir,normal);
			
			vec4 attenuation = R_LightProducts.lights[i].attenuation;
			lightDirs[i].w = 1.0 / (	attenuation.z +
										attenuation.y * dist +
										attenuation.x * dist * dist);

#ifdef AMBIENT_OCCLUSION
			// Compute ambient occlusion attenuation
			// gl_Color is provided from computeElements on self geometry
			lightDirs[i].w = lightDirs[i].w * gl_Color.r;
#endif
		}
	}


	o_texCoord = i_TexCoord;
	gl_Position =  gl_ModelViewProjectionMatrix * i_Position;
}
