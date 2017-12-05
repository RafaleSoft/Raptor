#if !defined(AFX_INITDIALOG_H__57A7FAB0_3A1F_11D3_9142_B236E7C6A77C__INCLUDED_)
#define AFX_INITDIALOG_H__57A7FAB0_3A1F_11D3_9142_B236E7C6A77C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#if !defined(AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_)
	#include "CWnd/GLWnd.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CGLView;

/////////////////////////////////////////////////////////////////////////////
// CInitDialog dialog

class CInitDialog : public CDialog
{
// Construction
public:
	CInitDialog(CWnd* pParent = NULL);   // standard constructor

	CGLView *GLContext;
// Dialog Data
	//{{AFX_DATA(CInitDialog)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInitDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CGLWnd	Context;
	// Generated message map functions
	//{{AFX_MSG(CInitDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo1();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeList1();
};

RAPTOR_NAMESPACE_END

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INITDIALOG_H__57A7FAB0_3A1F_11D3_9142_B236E7C6A77C__INCLUDED_)

