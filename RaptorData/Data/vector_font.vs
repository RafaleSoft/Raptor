/***************************************************************************/
/*                                                                         */
/*  vector_font.vs                                                         */
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

#version 440

layout(location = 0) in vec4 i_char;

out int car;
out int strip_start;
out int strip_end;
out vec2 offset;

void main (void)
{
	offset = vec2(i_char.x, i_char.y);

	int strip = int(i_char.w);
	
	strip_start = (strip >> 16);

	int size = strip & 0xff;
	strip_end = strip_start + size;

	car = int(i_char.z);
}


