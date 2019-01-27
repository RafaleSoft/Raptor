/***************************************************************************/
/*                                                                         */
/*  StdAfx.h                                                               */
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


#if !defined(AFX_STDAFX_H__909C2875_45DA_42C6_9E54_30FBE03C74F9__INCLUDED_)
#define AFX_STDAFX_H__909C2875_45DA_42C6_9E54_30FBE03C74F9__INCLUDED_

#ifdef WIN32
    #if _MSC_VER > 1000
        #pragma once
    #endif // _MSC_VER > 1000

    #define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

	#pragma warning(disable:4786)	//	> 255 chars, no debug
	#pragma warning(disable:4100)	//	unreferenced formal parameter
	#pragma warning(disable:4511)	//	copy constructor could not be generated
	#pragma warning(disable:4512)	//	assignment operator could not be generated
	#pragma warning(disable:4146)	//	unary minus operator applied to unsigned type, result still unsigned

	#define SPRINTF sprintf_s
#else
	#define SPRINTF sprintf
#endif


#include <iostream>
#include <string>
using namespace std;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__909C2875_45DA_42C6_9E54_30FBE03C74F9__INCLUDED_)
