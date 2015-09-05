#if !defined(AFX_DEAMONIP_H__642C92B5_A924_4751_94E8_C3B15163FB62__INCLUDED_)
#define AFX_DEAMONIP_H__642C92B5_A924_4751_94E8_C3B15163FB62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeamonIP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeamonIP dialog

class CDeamonIP : public CDialog
{
// Construction
public:
	CDeamonIP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeamonIP)
	enum { IDD = IDD_DEAMONIP_DIALOG };
	//}}AFX_DATA

	DWORD	m_address;

	BYTE	m_b0;
	BYTE	m_b1;
	BYTE	m_b2;
	BYTE	m_b3;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeamonIP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeamonIP)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEAMONIP_H__642C92B5_A924_4751_94E8_C3B15163FB62__INCLUDED_)
