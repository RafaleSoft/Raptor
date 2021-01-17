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


#if !defined(AFX_STDAFX_H__B11AA2C1_D1B1_4A4D_B931_8AC832AA6498__INCLUDED_)
#define AFX_STDAFX_H__B11AA2C1_D1B1_4A4D_B931_8AC832AA6498__INCLUDED_

#ifdef WIN32
    #if _MSC_VER > 1000
    #pragma once
    #endif // _MSC_VER > 1000

    #define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
	#include <windows.h>
#else	// Linux environment
	#define MAX_PATH		260
#endif

#endif // !defined(AFX_STDAFX_H__B11AA2C1_D1B1_4A4D_B931_8AC832AA6498__INCLUDED_)

