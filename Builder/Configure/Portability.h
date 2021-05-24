/***************************************************************************/
/*                                                                         */
/*  Portability.h                                                          */
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

#pragma once


#ifndef __RAPTOR_PORTABILITY_H__
#define __RAPTOR_PORTABILITY_H__

//!
//!	Headers & types.
//!
#if defined(_WIN32)
	#if _MSC_VER > 1000
		#pragma once
	#endif // _MSC_VER > 1000

	#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
	#include <windows.h>
	#include <new>

	#if !defined(WINAPI)
        #define WINAPI	__stdcall
	#endif

	#ifdef EXPORT_RAPTOR_CORE
		#define RAPTOR_API __declspec(dllexport)
		#define EXPIMP_TEMPLATE
	#else
		#define RAPTOR_API _declspec(dllimport)
		#define EXPIMP_TEMPLATE extern
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
	#include <new>

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
#include <set>
#include <sstream>
#include <algorithm>
using namespace std;

//	Base definitions
//	TODO: development in progress, need to finalize pre-definitions exports.
#if defined(_WIN32)
	#pragma warning(disable: 4091)
	//#pragma warning(disable: 4100)	
	//#pragma warning(disable: 4244)
	//#pragma warning(disable: 4511)    // copy ctor not generated
	//#pragma warning(disable: 4512)    // assign operator not generated
	//#pragma warning(disable: 4663)    // C++ language changes
	#pragma warning(disable: 4251)    // DLL interface required for STL export
	//#pragma warning(disable: 4275)    // deriving exported class from non-exported
	//#pragma warning(disable: 4711)	// automatic inline expansion warning
	//#pragma warning(disable: 4786)    //  dbug info too large

	//EXPIMP_TEMPLATE template union RAPTOR_API std::_String_val<std::_Simple_types<char>>::_Bxty;
	//#if _MSC_VER > 1800
	//	EXPIMP_TEMPLATE struct RAPTOR_API std::_Container_base12;
	//#endif
	//EXPIMP_TEMPLATE template class RAPTOR_API std::_String_val<std::_Simple_types<char>>;
	//#if _MSC_VER > 1900
	//	EXPIMP_TEMPLATE template class RAPTOR_API std::_Compressed_pair<std::allocator<char>, std::_String_val<std::_Simple_types<char>>, true>;
	//#elif _MSC_VER > 1800
	//	EXPIMP_TEMPLATE template class RAPTOR_API std::_Compressed_pair<std::_Wrap_alloc<std::allocator<char>>, std::_String_val<std::_Simple_types<char>>, true>;
	//#endif
	//EXPIMP_TEMPLATE template class RAPTOR_API std::basic_string<char, std::char_traits<char>, std::allocator<char>>;

	//#define EXPORT_VECTOR(class) \
	//	EXPIMP_TEMPLATE template class RAPTOR_API std::vector<raptor::class *,std::allocator<raptor::class>>;
#endif
	   
//!
//!	Time and files
//!
#if defined(WIN32)
	#define LOCALTIME(_Tm,_Time) localtime_s(_Tm,_Time)
	#define ASCTIME(_Buf, _SizeInBytes, _Tm) asctime_s(_Buf, _SizeInBytes, _Tm)
	#define __FILENAME__(file) (strrchr(file,'\\') ? strrchr(file,'\\') + 1 : file)
#elif defined(LINUX)
	#define LOCALTIME(_Tm,_Time) localtime_r(_Time,_Tm)
	#define ASCTIME(_Buf, _SizeInBytes, _Tm) asctime_r(_Tm, _Buf)
	#define __FILENAME__(file) (strrchr(file,'/') ? strrchr(file,'/') + 1 : file)
#endif

//!
//!	Strings
//!
#ifdef WIN32
	#define STRDUP(s) _strdup(s)
#elif defined(LINUX)
	#define STRDUP(s) strdup(s)
#endif

typedef struct lessString
{
	bool operator()(const string& x, const string &y) const
	{
		return (x.compare(y) < 0);
	}
} lessString;
typedef map<string, void*, lessString> MapStringToPtr;

#endif
