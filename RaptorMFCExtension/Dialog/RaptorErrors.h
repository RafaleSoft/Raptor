#if !defined(AFX_RAPTORERRORS_H__4B829705_7FC0_44DD_83A2_CAB01172F485__INCLUDED_)
#define AFX_RAPTORERRORS_H__4B829705_7FC0_44DD_83A2_CAB01172F485__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RaptorErrors.h : header file
//


RAPTOR_NAMESPACE_BEGIN


/////////////////////////////////////////////////////////////////////////////
// CRaptorErrors dialog

class CRaptorErrors : public CDialog
{
// Construction
public:
	CRaptorErrors(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRaptorErrors)
	enum { IDD = IDD_DIALOG5 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaptorErrors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRaptorErrors)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

RAPTOR_NAMESPACE_END

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAPTORERRORS_H__4B829705_7FC0_44DD_83A2_CAB01172F485__INCLUDED_)

