// GLObjectViewer.h : main header file for the GLOBJECTVIEWER application
//

#if !defined(AFX_GLOBJECTVIEWER_H__E252E85A_1349_4F8F_B9F5_DA8A50660672__INCLUDED_)
#define AFX_GLOBJECTVIEWER_H__E252E85A_1349_4F8F_B9F5_DA8A50660672__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerApp:
// See GLObjectViewer.cpp for the implementation of this class
//

class CGLObjectViewerApp : public CWinApp
{
public:
	CGLObjectViewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLObjectViewerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CGLObjectViewerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBJECTVIEWER_H__E252E85A_1349_4F8F_B9F5_DA8A50660672__INCLUDED_)
