/***************************************************************************/
/*                                                                         */
/*  vector_font.gs                                                         */
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

//	Expect the geometry shader extension to be available, warn if not.
#extension GL_ARB_geometry_shader4 : enable

uniform vec4 viewport;

struct glyph
{
	vec2 strip[56];
	float advance;
	float reserved[7];
};

layout(binding = 0) uniform font_buffer
{
	glyph chars[95];
} font;

layout(points) in;
layout(line_strip, max_vertices=56) out;

in int car[];
in int strip_start[];
in int strip_end[];
in vec2 offset[];

void main()
{
	glyph g_strip = font.chars[car[0]];

	int start = strip_start[0];
	int end = strip_end[0];

	for (int i=start; i<end; i++)
	{
		vec2 pos = g_strip.strip[i] + offset[0];
		gl_Position = vec4(pos.x / viewport.z - 1.0, pos.y / viewport.w - 1.0, 0.0, 1.0);
	
		EmitVertex();
	}

	EndPrimitive();
}
