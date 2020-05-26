/***************************************************************************/
/*                                                                         */
/*  vector_font.vs                                                         */
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

struct glyph
{
	vec2 strip0[14];
	vec2 strip1[14];
	vec2 strip2[14];
	vec2 strip3[14];
	float advance;
	uint strip_len;
};

struct strip
{
	vec2 point[14];
	uint len;
};

layout(binding = 0) uniform font_buffer
{
	glyph chars[95];
} font;


layout(location = 0) in vec4 i_char;

out strip g_strip;
out vec2 offset;

void main (void)
{
	offset = vec2(i_char.x, i_char.y);

	int num_strip = int(i_char.w);
	int num_char = int(i_char.z);

	glyph g = font.chars[num_char];
	
	if (num_strip == 0)
	{
		g_strip.point = g.strip0;
		g_strip.len = g.strip_len & 0xff;
	}
	else if (num_strip == 1)
	{
		g_strip.point = g.strip1;
		g_strip.len = (g.strip_len >> 8) & 0xff;
	}
	else if (num_strip == 2)
	{
		g_strip.point = g.strip2;
		g_strip.len = (g.strip_len >> 16) & 0xff;
	}
	else if (num_strip == 3)
	{
		g_strip.point = g.strip3;
		g_strip.len = (g.strip_len >> 24) & 0xff;
	}
}


