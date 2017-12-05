#if !defined(AFX_TESTVIEW_H__8CBA01DA_223B_49C8_B68E_671B2260E35C__INCLUDED_)
#define AFX_TESTVIEW_H__8CBA01DA_223B_49C8_B68E_671B2260E35C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestView.h : header file
//

#include "Raptordll.h"

#include "MFCExtension/CWnd/GLView.h"

RAPTOR_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CTestView view

class CTestView : public CGLView
{
protected:
	CTestView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTestView)

	bool	shift;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestView)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTestView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTVIEW_H__8CBA01DA_223B_49C8_B68E_671B2260E35C__INCLUDED_)
