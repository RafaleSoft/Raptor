/***************************************************************************/
/*                                                                         */
/*  Version.h                                                              */
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



#ifndef __RAPTOR_VERSION_H__
#define __RAPTOR_VERSION_H__

#define RAPTOR_VERSION_MAJOR	2
#define RAPTOR_VERSION_MINOR	17
#define RAPTOR_VERSION_PATCH	3
#define RAPTOR_VERSION_BUILD	17

#define RAPTOR_VERSION				(RAPTOR_VERSION_MAJOR << 24) + (RAPTOR_VERSION_MINOR << 16) + (RAPTOR_VERSION_PATCH << 8)
#define	RAPTOR_VERSION_DOT(a,b,c)	#a"."#b"."#c
#define	RAPTOR_VERSION_INVK(a,b,c)	RAPTOR_VERSION_DOT(a,b,c)
#define	RAPTOR_VERSION_STR			RAPTOR_VERSION_INVK(RAPTOR_VERSION_MAJOR,RAPTOR_VERSION_MINOR,RAPTOR_VERSION_PATCH)

#endif
