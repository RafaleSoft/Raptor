/***************************************************************************/
/*                                                                         */
/*  tquad.vs                                                               */
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

//uniform Transform {
//	mat4 ModelViewMatrix;
//	mat4 ModelViewProjectionMatrix;
//};

layout(location = 0) in vec4 i_Position;
layout(location = 3) in vec4 i_Color;
layout(location = 6) in vec4 i_Size;

out vec4 size;
out vec4 v_color;

void main (void)
{
	vec4 pos = vec4(vec3(i_Position.xyz),1.0);
	gl_Position =  gl_ModelViewProjectionMatrix * pos;
	//gl_Position =  ModelViewMatrix * pos;

	//	The size has to be projected because it is added to the posision
	//	in the geometry stage.
	size = gl_ModelViewProjectionMatrix * i_Size;
	v_color =  i_Color;
}

