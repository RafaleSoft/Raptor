/***************************************************************************/
/*                                                                         */
/*  StdAfx.h                                                               */
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


#if !defined(AFX_STDAFX_H__555140C7_E8B3_49E6_8C50_57967A2C968C__INCLUDED_)
#define AFX_STDAFX_H__555140C7_E8B3_49E6_8C50_57967A2C968C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
    #define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
	#define _WIN32_WINNT _WIN32_WINNT_WIN7	// Minimum target is Win7

    #include <afxwin.h>         // MFC core and standard components

	#ifndef CT2A
		#define CT2A(text) text
	#endif
	#ifndef CA2T
		#define CA2T(text) text
	#endif
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__555140C7_E8B3_49E6_8C50_57967A2C968C__INCLUDED_)
