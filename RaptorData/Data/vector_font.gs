/***************************************************************************/
/*                                                                         */
/*  vector_font.gs                                                         */
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

//	Expect the geometry shader extension to be available, warn if not.
#extension GL_ARB_geometry_shader4 : enable

uniform vec4 viewport;

struct strip
{
	vec2 point[14];
	uint len;
};

layout(points) in;
layout(line_strip, max_vertices=14) out;

in strip g_strip[];
in vec2 offset[];

void main()
{
	for (int i=0; i<g_strip[0].len;i++)
	{
		vec2 pos = g_strip[0].point[i] + offset[0];
		gl_Position = vec4(pos.x / viewport.z - 1.0, pos.y / viewport.w - 1.0, 0.0, 1.0);
	
		EmitVertex();
	}

	EndPrimitive();
}
