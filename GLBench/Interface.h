/***************************************************************************/
/*                                                                         */
/*  Interface.h                                                            */
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


#ifndef __INTERFACE_H__
#define __INTERFACE_H__

/////////////////////////////////////////////////////////////////////////////
//
// Function prototypes for module acces
//

#if !defined(GLBENCH_API)
	#if defined(_USRDLL)
		#define GLBENCH_API _declspec(dllexport)
	#else
		#define GLBENCH_API _declspec(dllimport)
	#endif
#endif
#if !defined(ENTRY)
	#define ENTRY _cdecl
#endif
#if !defined(DLLENTRY)
	#define DLLENTRY extern "C"	
#endif


typedef struct result_item_t
{
	const char*		description;
	unsigned int	score;
	float			fragment_rate;
	float			fps_rate;
	float			driver_overhead;
} result_item;
typedef result_item_t	*lp_result_item_t;

typedef struct result_bench_t
{
	const char*			name;
	const char*			author;
	const char*			description;
	unsigned int		nbItems;
	unsigned int		score;
	float				rate;
	lp_result_item_t	result_items;
} result_bench;
typedef result_bench_t *lp_result_bench_t;

typedef GLBENCH_API const char* (* GETMODULEINFO)(void);

typedef GLBENCH_API void (* INITMODULE)(void);

typedef GLBENCH_API void (* BENCH)(CWnd *parent);

typedef GLBENCH_API lp_result_bench_t (* RESULT)(void);

#include "Raptordll.h"
RAPTOR_NAMESPACE

//
//
/////////////////////////////////////////////////////////////////////////////

#endif