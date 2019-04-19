/***************************************************************************/
/*                                                                         */
/*  BuilderNative.h                                                        */
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
#ifndef BUILDER_NATIVE_HH
#define BUILDER_NATIVE_HH

#ifdef BUILDERNATIVE_EXPORTS
	#define BUILDERNATIVE_API __declspec(dllexport)
#else
	#define BUILDERNATIVE_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <stdint.h>

typedef enum
{
	CPU,
	COREGL,
	GLPROFILE,
	COREVK,
	ARB,
	VK,
	EXT,
	NV,
	APPLE,
	ATI,
	HP,
	IBM,
	SGIS,
	SUN,
	WGL,
	GLX,
	KHR,
	MESA,
	OES,
	_3DFX
} EXTENSION_KIND;

typedef struct NATIVE_EXTENSION_t
{
	EXTENSION_KIND	kind;
	bool			active;
	const char*		extensionName;
	uint32_t		nb_dependencies;
	const char*		*dependencies;
} NATIVE_EXTENSION;

extern "C" BUILDERNATIVE_API 
bool glvkInitBuilder(HDC dc);

extern "C" BUILDERNATIVE_API
bool getExtensions(NATIVE_EXTENSION* ext, uint32_t* s);

#endif
