/***************************************************************************/
/*                                                                         */
/*  AO.vs                                                                  */
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

#version 120

varying vec4 vertex;
varying vec4 normal;

uniform mat4 vertexMat;
uniform mat4 normalMat;

void main (void)
{
	/* Use texture unit 2, to share configuration */
	/*	with final rendering: lightmap is unit2	  */
	gl_Position = gl_MultiTexCoord2;
	/*gl_TexCoord[0] = gl_MultiTexCoord0;*/
	
	vertex = vertexMat * gl_Vertex;
	normal = normalMat * vec4(gl_Normal,1);
}
