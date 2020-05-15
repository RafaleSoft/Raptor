/***************************************************************************/
/*                                                                         */
/*  box.gs                                                                 */
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


//	Expect the geometry shader extension to be available, warn if not.
#extension GL_ARB_geometry_shader4 : enable

in vec4 v_color[];
in vec4 v_Min[];
in vec4 v_Max[];

layout(points) in;
layout(triangle_strip, max_vertices=18) out;

out vec4 g_color;

void main()
{
	g_color = v_color[0];

	// top: Back right
	vec4 pos = vec4(v_Max[0].x, v_Max[0].y, v_Min[0].z, 1.0);
	vec4 tbr = gl_ModelViewProjectionMatrix * pos;
	gl_Position = tbr;
	EmitVertex();
	// top: Back Left
	pos = vec4(v_Min[0].x, v_Max[0].y, v_Min[0].z, 1.0);
	vec4 tbl = gl_ModelViewProjectionMatrix * pos;
	gl_Position = tbl;
	EmitVertex();
	// top: Front Right
	pos = vec4(v_Max[0].x, v_Max[0].y, v_Max[0].z, 1.0);
	vec4 tfr = gl_ModelViewProjectionMatrix * pos;
	gl_Position = tfr;
	EmitVertex();
	// top: Front Left
	pos = vec4(v_Min[0].x, v_Max[0].y, v_Max[0].z, 1.0);
	vec4 tfl = gl_ModelViewProjectionMatrix * pos;
	gl_Position = tfl;
	EmitVertex();

	// front: Top Left
	gl_Position = tfl;
	EmitVertex();
	// front: Bottom Left
	pos = vec4(v_Min[0].x, v_Min[0].y, v_Max[0].z, 1.0);
	vec4 bfl = gl_ModelViewProjectionMatrix * pos;
	gl_Position = bfl;
	EmitVertex();
	// front: Top Right
	gl_Position = tfr;
	EmitVertex();
	// front: Bottom Right
	pos = vec4(v_Max[0].x, v_Min[0].y, v_Max[0].z, 1.0);
	vec4 bfr = gl_ModelViewProjectionMatrix * pos;
	gl_Position = bfr;
	EmitVertex();

	// right: Top back
	gl_Position = tbr;
	EmitVertex();
	// right: Bottom back
	pos = vec4(v_Max[0].x, v_Min[0].y, v_Min[0].z, 1.0);
	vec4 bbr = gl_ModelViewProjectionMatrix * pos;
	gl_Position = bbr;
	EmitVertex();

	// back: Top Left
	gl_Position = tbl;
	EmitVertex();
	// back: Bottom Left
	pos = vec4(v_Min[0].x, v_Min[0].y, v_Min[0].z, 1.0);
	vec4 bbl = gl_ModelViewProjectionMatrix * pos;
	gl_Position = bbl;
	EmitVertex();

	// left: Top front
	gl_Position = tfl;
	EmitVertex();
	// left: Bottom front
	gl_Position = bfl;
	EmitVertex();

	// bottom: Front Left
	gl_Position = bfl;
	EmitVertex();
	// bottom: Back Left
	gl_Position = bbl;
	EmitVertex();
	// bottom: Front Right
	gl_Position = bfr;
	EmitVertex();
	// bottom: Back Right
	gl_Position = bbr;
	EmitVertex();

	EndPrimitive();
}

