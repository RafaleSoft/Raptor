// RaysClient.h : main header file for the RAYSCLIENT application
//

#if !defined(AFX_RAYSCLIENT_H__132CAF1B_B833_4B59_A474_2A768B3A699A__INCLUDED_)
#define AFX_RAYSCLIENT_H__132CAF1B_B833_4B59_A474_2A768B3A699A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "../Systemdll/Systemdll.h"
extern void Cout(CString msg);

/////////////////////////////////////////////////////////////////////////////
// CRaysClientApp:
// See RaysClient.cpp for the implementation of this class
//

class CRaysClientApp : public CWinApp
{
protected:
	CClient<CClientSocket>	Client;
public:
	CRaysClientApp();
	CDialogBar *Objects;
	CDialogBar *Output;
	CImageList *ObjectsData;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaysClientApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	bool ConnectToServer(CString name,int port);
	void BuildToolTree(void);
	//{{AFX_MSG(CRaysClientApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAYSCLIENT_H__132CAF1B_B833_4B59_A474_2A768B3A699A__INCLUDED_)
