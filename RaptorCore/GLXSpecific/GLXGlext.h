/***************************************************************************/
/*                                                                         */
/*  GLXGlext.h                                                             */
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

/*
 *	This header provides GLX specific OpenGL extensions.
 *	It is included where needed, depending on the content of 
 *	the file CodeGeneration.h. User should never include this file 
 *	directly because it is managed by Glext.h
 */

#ifndef __GLX_RAPTOR_GLEXT_H__
#define __GLX_RAPTOR_GLEXT_H__

#ifdef GLX_EXTENSIONS
	#include <malloc.h>
	
	typedef ptrdiff_t GLintptr;
	typedef ptrdiff_t GLsizeiptr;
	
	#include <GL/glx.h>
#endif



#endif

