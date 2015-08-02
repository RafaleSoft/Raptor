// RaysINetBridgeDlg.h : header file
//

#if !defined(AFX_RAYSINETBRIDGEDLG_H__94B51B27_6F2F_4963_A32A_D05487351023__INCLUDED_)
#define AFX_RAYSINETBRIDGEDLG_H__94B51B27_6F2F_4963_A32A_D05487351023__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "InetSocket.h"

/////////////////////////////////////////////////////////////////////////////
// CRaysINetBridgeDlg dialog

class CRaysINetBridgeDlg : public CDialog
{
// Construction
public:
	CRaysINetBridgeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRaysINetBridgeDlg)
	enum { IDD = IDD_RAYSINETBRIDGE_DIALOG };
	CIPAddressCtrl	m_ipAddress2Ctrl;
	CIPAddressCtrl	m_ipAddress1Ctrl;
	long	m_lPort1;
	long	m_lPort2;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaysINetBridgeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void Output(CString &msg);

// Implementation
protected:
	HICON m_hIcon;

	CInetSocket *m_pInetSocket;

	// Generated message map functions
	//{{AFX_MSG(CRaysINetBridgeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConnect();
	afx_msg void OnSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAYSINETBRIDGEDLG_H__94B51B27_6F2F_4963_A32A_D05487351023__INCLUDED_)
