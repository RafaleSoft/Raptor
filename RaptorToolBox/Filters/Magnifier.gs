/***************************************************************************/
/*                                                                         */
/*  Magnifier.gs                                                           */
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

uniform vec4 center;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;
layout(location = 1) out vec4 g_TexCoord;

void main()
{
	gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
	g_TexCoord = center + vec4(0.0,0.0,0.0,0.0);
	EmitVertex();

	gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
	g_TexCoord = center + vec4(1.0,0.0,0.0,0.0);
	EmitVertex();

	gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
	g_TexCoord = center + vec4(0.0, 1.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
	g_TexCoord = center + vec4(1.0, 1.0, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}

