/***************************************************************************/
/*                                                                         */
/*  Raptor.glsl                                                            */
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

// Version shall be specified in including file.
//#version 440 compatibility

//	Maximum number of lights due to number of interpolators available
const int MAX_LIGHTS = 5;

// Maximum number of lights in OpenGL fixed pipeline.
const int GL_MAX_LIGHTS = 8;


//
//	Raptor Shader interfaces
//

//
//	Raptor Uniform Transform blocs
//
//layout (binding = 0) uniform Transform {
//	mat4 ModelViewMatrix;
//	mat4 ModelViewMatrixInverse;
//	mat4 ModelViewProjectionMatrix;
//	mat4 NormalMatrix;
//} R_Transform;

//
//	Raptor Uniform Light products
//
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

//
//	Helper macro to instantiate lights products
//	Usage: to declara a uniform buffer bound to binding location e.g. 3,
//		LIGHT_PRODUCTS(3) MyLightProducts;
#ifndef LIGHT_PRODUCTS
#define LIGHT_PRODUCTS(bind_point) \
	layout (binding = 0) uniform LightProducts \
	{ \
		LightProduct lights[MAX_LIGHTS]; \
		vec4		 scene_ambient; \
	}
#endif



