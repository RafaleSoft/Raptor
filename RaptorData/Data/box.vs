/***************************************************************************/
/*                                                                         */
/*  box.vs                                                                 */
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


layout(location = 0) in vec4 i_Min;
layout(location = 3) in vec4 i_Color;
layout(location = 6) in vec4 i_Max;

out vec4 v_color;
out vec4 v_Min;
out vec4 v_Max;

void main (void)
{
	v_color = i_Color;

	v_Min = i_Min;
	v_Max = i_Max;
}
