// RaysINetBridge.h : main header file for the RAYSINETBRIDGE application
//

#if !defined(AFX_RAYSINETBRIDGE_H__84100A2A_2C25_4A68_9E94_6BA0A76BEA6A__INCLUDED_)
#define AFX_RAYSINETBRIDGE_H__84100A2A_2C25_4A68_9E94_6BA0A76BEA6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRaysINetBridgeApp:
// See RaysINetBridge.cpp for the implementation of this class
//

class CRaysINetBridgeApp : public CWinApp
{
public:
	CRaysINetBridgeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaysINetBridgeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRaysINetBridgeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAYSINETBRIDGE_H__84100A2A_2C25_4A68_9E94_6BA0A76BEA6A__INCLUDED_)
