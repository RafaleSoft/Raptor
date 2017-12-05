#if !defined(AFX_MINIRAYSCLIENTUPLOAD_H__F7A338DF_4F6B_4588_90CB_E409F601FAC5__INCLUDED_)
#define AFX_MINIRAYSCLIENTUPLOAD_H__F7A338DF_4F6B_4588_90CB_E409F601FAC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MiniRaysClientUpload.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MiniRaysClientUpload dialog

class MiniRaysClientUpload : public CDialog
{
// Construction
public:
	MiniRaysClientUpload(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(MiniRaysClientUpload)
	enum { IDD = IDD_UPLOAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MiniRaysClientUpload)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MiniRaysClientUpload)
	afx_msg void OnUpload();
	afx_msg void OnChoosescene();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINIRAYSCLIENTUPLOAD_H__F7A338DF_4F6B_4588_90CB_E409F601FAC5__INCLUDED_)
