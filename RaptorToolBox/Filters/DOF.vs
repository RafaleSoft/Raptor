/***************************************************************************/
/*                                                                         */
/*  DOF.vs                                                                 */
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

uniform vec4 offset;
uniform vec4 vector;

void main (void)
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord0 + offset.x*vector;
	gl_TexCoord[2] = gl_MultiTexCoord0 - offset.x*vector;
	gl_TexCoord[3] = gl_MultiTexCoord0 + offset.y*vector;
	gl_TexCoord[4] = gl_MultiTexCoord0 - offset.y*vector;
	gl_TexCoord[5] = gl_MultiTexCoord0 + offset.z*vector;
	gl_TexCoord[6] = gl_MultiTexCoord0 - offset.z*vector;
	gl_Position = ftransform();
}

