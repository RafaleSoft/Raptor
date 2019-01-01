// Test2.h : main header file for the TEST2 application
//

#if !defined(AFX_TEST2_H__DD849FF4_9ADE_11D3_9843_2C5168000000__INCLUDED_)
#define AFX_TEST2_H__DD849FF4_9ADE_11D3_9843_2C5168000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols
#include "Raptordll.h"
RAPTOR_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// CTest2App:
// See Test2.cpp for the implementation of this class
//

class CTestEngine;


class CTest2App : public CWinApp
{
public:
	typedef enum
	{
		BUMPDEMO,
		SKINNINGDEMO,
		LODDEMO,
		SHADOWDEMO,
		SPLINEDEMO,
		PARTICLEDEMO,
		EDITDEMO,
		PROJECTIONDEMO,
		WARPINGDEMO,
		COLLISIONDEMO,
		VRTXSHADERSDEMO,
		SHADOWMAPDEMO,
		AMBIENTOCCLUSIONDEMO,
		MAX_DEMO
	} DEMOTYPE;

	CTest2App();
	CAnimator	*Animator;
	CTestEngine	*Engine;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTest2App)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTest2App)
	afx_msg void OnAppAbout();
	afx_msg void OnRaptorStatus();
	afx_msg void OnObjectStatus();
	afx_msg void OnVideoSettings();
	afx_msg void OnAboutRaptor();
	afx_msg void OnSelectDemo(UINT nID);
	afx_msg void OnRaptorerrors();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEST2_H__DD849FF4_9ADE_11D3_9843_2C5168000000__INCLUDED_)
