// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B11AA2C1_D1B1_4A4D_B931_8AC832AA6498__INCLUDED_)
#define AFX_STDAFX_H__B11AA2C1_D1B1_4A4D_B931_8AC832AA6498__INCLUDED_

#ifdef WIN32
    #if _MSC_VER > 1000
    #pragma once
    #endif // _MSC_VER > 1000

    #define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

	#pragma warning(disable: 4275)  // deriving exported class from non-exported
	#pragma warning(disable: 4251)  // using non-exported as public in exported
	#include <windows.h>
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif

#endif // !defined(AFX_STDAFX_H__B11AA2C1_D1B1_4A4D_B931_8AC832AA6498__INCLUDED_)

