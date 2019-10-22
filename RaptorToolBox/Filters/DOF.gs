/***************************************************************************/
/*                                                                         */
/*  DOF.gs                                                                 */
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

uniform vec4 offset;
uniform vec4 vector;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;
layout(location = 1) out vec4 g_TexCoord[7];

void main()
{
	gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
	g_TexCoord[0] = vec4(0.0,0.0,0.0,0.0);
	g_TexCoord[1] = vec4(0.0,0.0,0.0,0.0) + offset.x*vector;
	g_TexCoord[2] = vec4(0.0,0.0,0.0,0.0) - offset.x*vector;
	g_TexCoord[3] = vec4(0.0,0.0,0.0,0.0) + offset.y*vector;
	g_TexCoord[4] = vec4(0.0,0.0,0.0,0.0) - offset.y*vector;
	g_TexCoord[5] = vec4(0.0,0.0,0.0,0.0) + offset.z*vector;
	g_TexCoord[6] = vec4(0.0,0.0,0.0,0.0) - offset.z*vector;
	EmitVertex();

	gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
	g_TexCoord[0] = vec4(1.0,0.0,0.0,0.0);
	g_TexCoord[1] = vec4(1.0,0.0,0.0,0.0) + offset.x*vector;
	g_TexCoord[2] = vec4(1.0,0.0,0.0,0.0) - offset.x*vector;
	g_TexCoord[3] = vec4(1.0,0.0,0.0,0.0) + offset.y*vector;
	g_TexCoord[4] = vec4(1.0,0.0,0.0,0.0) - offset.y*vector;
	g_TexCoord[5] = vec4(1.0,0.0,0.0,0.0) + offset.z*vector;
	g_TexCoord[6] = vec4(1.0,0.0,0.0,0.0) - offset.z*vector;
	EmitVertex();

	gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
	g_TexCoord[0] = vec4(0.0, 1.0, 0.0, 0.0);
	g_TexCoord[1] = vec4(0.0, 1.0, 0.0, 0.0) + offset.x*vector;
	g_TexCoord[2] = vec4(0.0, 1.0, 0.0, 0.0) - offset.x*vector;
	g_TexCoord[3] = vec4(0.0, 1.0, 0.0, 0.0) + offset.y*vector;
	g_TexCoord[4] = vec4(0.0, 1.0, 0.0, 0.0) - offset.y*vector;
	g_TexCoord[5] = vec4(0.0, 1.0, 0.0, 0.0) + offset.z*vector;
	g_TexCoord[6] = vec4(0.0, 1.0, 0.0, 0.0) - offset.z*vector;
	EmitVertex();

	gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
	g_TexCoord[0] = vec4(1.0, 1.0, 0.0, 0.0);
	g_TexCoord[1] = vec4(1.0, 1.0, 0.0, 0.0) + offset.x*vector;
	g_TexCoord[2] = vec4(1.0, 1.0, 0.0, 0.0) - offset.x*vector;
	g_TexCoord[3] = vec4(1.0, 1.0, 0.0, 0.0) + offset.y*vector;
	g_TexCoord[4] = vec4(1.0, 1.0, 0.0, 0.0) - offset.y*vector;
	g_TexCoord[5] = vec4(1.0, 1.0, 0.0, 0.0) + offset.z*vector;
	g_TexCoord[6] = vec4(1.0, 1.0, 0.0, 0.0) - offset.z*vector;
	EmitVertex();

	EndPrimitive();
}

