// RaysDeamon.h : main header file for the RAYSDEAMON application
//

#if !defined(AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_)
#define AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"				// main symbols
#include "../Messages.h"			// io messages IDs and structs
#include "../Systemdll/Systemdll.h"	// client/server
#include "DSocket.h"

class CRaysDeamonApp;
extern CRaysDeamonApp Deamon;

/////////////////////////////////////////////////////////////////////////////
// CRaysDeamonApp:
// See RaysDeamon.cpp for the implementation of this class
//

class CRaysDeamonApp : public CWinApp
{
public:
	CRaysDeamonApp();


public:
	CServer<CDSocket>	*m_Server;
	iosock_t<CDSocket>	*m_RaysServer;

	void ManageMsg(MSGSTRUCT& msg,unsigned char raw_data[]);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaysDeamonApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRaysDeamonApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_)
