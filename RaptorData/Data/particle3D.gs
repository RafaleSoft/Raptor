/***************************************************************************/
/*                                                                         */
/*  particle3D.gs                                                          */
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

in float angle[];
in float size[];
in vec4 v_color[];

layout(points) in;
layout(triangle_strip, max_vertices=4) out;
layout(location = 1) out vec4 g_TexCoord[1];
out vec4 g_color;

void main()
{
	float cs = cos(angle[0]);
	float ss = sin(angle[0]);
	float Hx = 0.5 * size[0] * (cs - ss);
	float Hy = 0.5 * size[0] * (cs + ss);

	g_color = v_color[0];
	float z = 1.0f - g_color.a;

	gl_Position = gl_in[0].gl_Position + vec4(-Hx, -Hy, 0.0, 0.0);
	g_TexCoord[0] = vec4(0.0,0.0,z,0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(Hy,-Hx,0.0,0.0);
	g_TexCoord[0] = vec4(1.0,0.0,z,0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-Hy,Hx,0.0,0.0);
	g_TexCoord[0] = vec4(0.0,1.0,z,0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(Hx,Hy,0.0,0.0);
	g_TexCoord[0] = vec4(1.0,1.0,z,0.0);
	EmitVertex();

	EndPrimitive();
}
