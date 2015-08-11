#if !defined(AFX_DIALOGWUPATH_H__0150C367_558D_411A_B44E_492155E2CFFB__INCLUDED_)
#define AFX_DIALOGWUPATH_H__0150C367_558D_411A_B44E_492155E2CFFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogWUPath.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogWUPath dialog

class CDialogWUPath : public CDialog
{
// Construction
public:
	CString m_cstrWUPath;
	CDialogWUPath(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogWUPath)
	enum { IDD = IDD_WORKUNITPATH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogWUPath)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogWUPath)
	afx_msg void OnBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGWUPATH_H__0150C367_558D_411A_B44E_492155E2CFFB__INCLUDED_)
