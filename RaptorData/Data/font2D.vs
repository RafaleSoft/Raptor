/***************************************************************************/
/*                                                                         */
/*  font2D.vs                                                              */
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

#version 440

uniform vec4 viewport;

layout(location = 0) in vec4 i_Position;
layout(location = 8) in vec4 i_TexCoord;

out vec4 v_texCoord;
out vec2 v_size;

void main (void)
{
	vec4 pos = vec4(i_Position.x / viewport.z - 1.0, 1.0 - i_Position.y / viewport.w, 0.0, 1.0);
	gl_Position = pos;
	v_texCoord = i_TexCoord;
	v_size = vec2(i_Position.z / viewport.z, i_Position.w / viewport.w);
}


