/***************************************************************************/
/*                                                                         */
/*  GLBenchResultDlg.h                                                     */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_GLBENCHRESULTDLG_H__27A0A4F5_5DF9_4DE7_9797_1135236AD1CE__INCLUDED_)
#define AFX_GLBENCHRESULTDLG_H__27A0A4F5_5DF9_4DE7_9797_1135236AD1CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GLBenchResultDlg.h : header file
//

#include "Interface.h"

/////////////////////////////////////////////////////////////////////////////
// CGLBenchResultDlg dialog

class CGLBenchResultDlg : public CDialog
{
// Construction
public:
	CGLBenchResultDlg(CWnd* pParent = NULL);   // standard constructor

	void SetData(lp_result_bench_t results);
// Dialog Data
	//{{AFX_DATA(CGLBenchResultDlg)
	enum { IDD = IDD_BENCH_RESULTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLBenchResultDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	lp_result_bench_t m_pResults;

	// Generated message map functions
	//{{AFX_MSG(CGLBenchResultDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLBENCHRESULTDLG_H__27A0A4F5_5DF9_4DE7_9797_1135236AD1CE__INCLUDED_)
