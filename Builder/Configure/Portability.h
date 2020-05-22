/***************************************************************************/
/*                                                                         */
/*  Portability.h                                                          */
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

#pragma once


#ifndef __RAPTOR_PORTABILITY_H__
#define __RAPTOR_PORTABILITY_H__


#if defined(_WIN32)
	#if !defined(WINAPI)
        #define WINAPI	__stdcall
	#endif

	#ifdef EXPORT_RAPTOR_CORE
		#define RAPTOR_API __declspec(dllexport)
	#else
		#define RAPTOR_API _declspec(dllimport)
	#endif
    #define RAPTOR_FASTCALL __fastcall
    #define RAPTOR_CCALL __cdecl
    #define RAPTOR_APICALL WINAPI

    #if _MSC_VER > 1200     // 1200 is Visual C++ 6.0
        #define RAPTOR_TYPENAME typename
    #else
        #define RAPTOR_TYPENAME
    #endif

	#define RAPTOR_INTERFACE __interface
#else // Linux environment
	#include <stdlib.h>
	#include <typeinfo>
	#define RAPTOR_API
    #define RAPTOR_FASTCALL
    #define RAPTOR_CCALL
    #define RAPTOR_APICALL
    #define RAPTOR_TYPENAME typename
	#define RAPTOR_INTERFACE class
#endif

//	Standard sized int types
#if defined(_MSC_VER) && (_MSC_VER < 1600)
	typedef signed   __int8  int8_t;
	typedef unsigned __int8  uint8_t;
	typedef signed   __int16 int16_t;
	typedef unsigned __int16 uint16_t;
	typedef signed   __int32 int32_t;
	typedef unsigned __int32 uint32_t;
	typedef signed   __int64 int64_t;
	typedef unsigned __int64 uint64_t;
#else
	#include <stdint.h>
#endif

//	Standard Template Library Headers used for Raptor
#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;


//	Time and files
#if defined(WIN32)
	#define LOCALTIME(_Tm,_Time) localtime_s(_Tm,_Time)
	#define ASCTIME(_Buf, _SizeInBytes, _Tm) asctime_s(_Buf, _SizeInBytes, _Tm)
	#define __FILENAME__(file) (strrchr(file,'\\') ? strrchr(file,'\\') + 1 : file)
#elif defined(LINUX)
	#define LOCALTIME(_Tm,_Time) localtime_r(_Time,_Tm)
	#define ASCTIME(_Buf, _SizeInBytes, _Tm) asctime_r(_Tm, _Buf)
	#define __FILENAME__(file) (strrchr(file,'/') ? strrchr(file,'/') + 1 : file)
#endif

#endif
