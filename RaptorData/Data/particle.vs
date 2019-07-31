/***************************************************************************/
/*                                                                         */
/*  particle.vs                                                            */
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

uniform float fPointSize;

layout(location = 0) in vec4 i_Position;
layout(location = 1) in float i_Size;
layout(location = 3) in vec4 i_Color;
layout(location = 5) in float i_Angle;

out float angle;
out float size;
out vec4 v_color;

void main (void)
{
	vec4 pos = vec4(fPointSize * vec3(i_Position.xyz),1.0);
	gl_Position =  gl_ModelViewProjectionMatrix * pos;
	angle = i_Angle;
	size = fPointSize * i_Size;
	v_color = i_Color;
}


