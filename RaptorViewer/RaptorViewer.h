// RaptorViewer.h : main header file for the RAPTORVIEWER application
//

#if !defined(AFX_RAPTORVIEWER_H__EB563B4B_5C3F_46D0_B03F_11C1E51C3858__INCLUDED_)
#define AFX_RAPTORVIEWER_H__EB563B4B_5C3F_46D0_B03F_11C1E51C3858__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRaptorViewerApp:
// See RaptorViewer.cpp for the implementation of this class
//

class CRaptorViewerApp : public CWinApp
{
public:
	CRaptorViewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaptorViewerApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CRaptorViewerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnRaptorStatus();
	afx_msg void OnObjectStatus();
	afx_msg void OnRaptorerrors();
	afx_msg void OnEditscript();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAPTORVIEWER_H__EB563B4B_5C3F_46D0_B03F_11C1E51C3858__INCLUDED_)
