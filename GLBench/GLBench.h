// GLBench.h : main header file for the GLBENCH application
//

#if !defined(AFX_GLBENCH_H__A42A3DDD_0F11_4B54_AD69_2FCFEF127AEB__INCLUDED_)
#define AFX_GLBENCH_H__A42A3DDD_0F11_4B54_AD69_2FCFEF127AEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGLBenchApp:
// See GLBench.cpp for the implementation of this class
//

class CGLBenchApp : public CWinApp
{
public:
	CGLBenchApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLBenchApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGLBenchApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLBENCH_H__A42A3DDD_0F11_4B54_AD69_2FCFEF127AEB__INCLUDED_)
