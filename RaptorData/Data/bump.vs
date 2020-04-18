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

layout (binding = 0) uniform Transform {
	mat4 ModelViewMatrix;
	mat4 ModelViewProjectionMatrix;
} gl_Transform;


uniform int lightEnable[GL_MAX_LIGHTS];
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
		int numl = lightEnable[i];
		if (numl >= 0)
		{
#ifdef EYE_SPACE
			// Compute lighting in eye space
			vec3 ldir = vec3(gl_LightSource[numl].position) + ecPos;
#else
			// Compute lighting in object space
			vec4 lpos = gl_ModelViewMatrixInverse * gl_LightSource[numl].position - i_Position;
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


	o_texCoord = i_TexCoord;
	gl_Position =  gl_ModelViewProjectionMatrix * i_Position;
}
