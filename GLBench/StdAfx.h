// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__555140C7_E8B3_49E6_8C50_57967A2C968C__INCLUDED_)
#define AFX_STDAFX_H__555140C7_E8B3_49E6_8C50_57967A2C968C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
    #define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

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
