// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AF60EFA8_AEA7_405B_8B20_767A0E4B6D93__INCLUDED_)
#define AFX_STDAFX_H__AF60EFA8_AEA7_405B_8B20_767A0E4B6D93__INCLUDED_

#ifdef WIN32
    #if _MSC_VER > 1000
    #pragma once
    #endif // _MSC_VER > 1000

    #define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
	#define _WIN32_WINNT _WIN32_WINNT_WIN7	// Minimum target is Win7

    #include <afxwin.h>         // MFC core and standard components
    #include <afxext.h>         // MFC extensions

	#ifndef CT2A
		#define CT2A(text) text
	#endif
	#ifndef CA2T
		#define CA2T(text) text
	#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif

#endif // !defined(AFX_STDAFX_H__AF60EFA8_AEA7_405B_8B20_767A0E4B6D93__INCLUDED_)
