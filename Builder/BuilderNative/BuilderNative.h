/***************************************************************************/
/*                                                                         */
/*  BuilderNative.h                                                        */
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


#ifndef BUILDER_NATIVE_HH
#define BUILDER_NATIVE_HH

#if defined(WIN32)
	#if defined(BUILDERNATIVE_EXPORTS)
		#define BUILDERNATIVE_API __declspec(dllexport)
	#else
		#define BUILDERNATIVE_API __declspec(dllimport)
	#endif
#else
	#define BUILDERNATIVE_API
#endif

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

typedef struct BUILD_SUPPLEMENT_t
{
	bool DEBUG;
	bool REDIST;
	bool COMPUTE;
	bool PHYSX;
} BUILD_SUPPLEMENT;

typedef struct NATIVE_EXTENSION_t
{
	EXTENSION_KIND	kind;
	bool			active;
	const char*		extensionName;
	uint32_t		nb_dependencies;
	const char*		*dependencies;
} NATIVE_EXTENSION;

#ifdef WIN32
	extern "C" BUILDERNATIVE_API
	bool glvkInitBuilder(HDC dc);
#elif defined(LINUX)
	extern "C" BUILDERNATIVE_API
	bool glvkInitBuilder(void*);
#endif

extern "C" BUILDERNATIVE_API
bool getExtensions(NATIVE_EXTENSION* ext, uint32_t* s);

extern "C" BUILDERNATIVE_API
bool freeExtensions(NATIVE_EXTENSION* extensions, uint32_t s);

extern "C" BUILDERNATIVE_API
bool checkConsistency(bool force);

extern "C" BUILDERNATIVE_API
bool writeHeader(const char* filename);

extern "C" BUILDERNATIVE_API
bool activateExtension(const char* extension, bool activate);

extern "C" BUILDERNATIVE_API
bool setBuildSupplement(BUILD_SUPPLEMENT *bld);

extern "C" BUILDERNATIVE_API
bool activateAllOrNone(bool all);

extern "C" BUILDERNATIVE_API
bool isExtensionActive(const char* extension);

#endif
