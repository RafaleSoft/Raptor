/***************************************************************************/
/*                                                                         */
/*  font2D.gs                                                              */
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

in vec4 v_texCoord[];
in vec2 v_size[];

layout(points) in;
layout(triangle_strip, max_vertices=4) out;
layout(location = 1) out vec4 g_TexCoord;

void main()
{
	gl_Position = gl_in[0].gl_Position;
	g_TexCoord = vec4(v_texCoord[0].x, v_texCoord[0].y, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(v_size[0].x, 0.0, 0.0, 0.0);
	g_TexCoord = vec4(v_texCoord[0].x + v_texCoord[0].z, v_texCoord[0].y, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.0, v_size[0].y, 0.0, 0.0);
	g_TexCoord = vec4(v_texCoord[0].x, v_texCoord[0].y + v_texCoord[0].w, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(v_size[0].x,v_size[0].y, 0.0, 0.0);
	g_TexCoord = vec4(v_texCoord[0].x + v_texCoord[0].z, v_texCoord[0].y + v_texCoord[0].w, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}
