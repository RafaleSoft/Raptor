// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DD849FF6_9ADE_11D3_9843_2C5168000000__INCLUDED_)
#define AFX_STDAFX_H__DD849FF6_9ADE_11D3_9843_2C5168000000__INCLUDED_

#ifdef WIN32
	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000

	#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

	#include <afxwin.h>         // MFC core and standard components
	#include <afxext.h>         // MFC extensions
	#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
	#ifndef _AFX_NO_AFXCMN_SUPPORT
	#include <afxcmn.h>			// MFC support for Windows Common Controls
	#endif // _AFX_NO_AFXCMN_SUPPORT

	#ifndef CT2A
		#define CT2A(text) text
	#endif
	#ifndef CA2T
		#define CA2T(text) text
	#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif

#endif // !defined(AFX_STDAFX_H__DD849FF6_9ADE_11D3_9843_2C5168000000__INCLUDED_)
