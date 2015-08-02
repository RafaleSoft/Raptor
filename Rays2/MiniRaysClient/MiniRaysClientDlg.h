// MiniRaysClientDlg.h : header file
//

#if !defined(AFX_MINIRAYSCLIENTDLG_H__57386403_3628_4729_BF38_CE52B12B58B6__INCLUDED_)
#define AFX_MINIRAYSCLIENTDLG_H__57386403_3628_4729_BF38_CE52B12B58B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMiniRaysClientDlg dialog

class CMiniRaysClientDlg : public CDialog
{
// Construction
public:
	CMiniRaysClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMiniRaysClientDlg)
	enum { IDD = IDD_MINIRAYSCLIENT_DIALOG };
	CListBox	m_Output;
	CIPAddressCtrl	m_IPAddress;
	UINT	m_Port;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiniRaysClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	CString	m_IP;
	void	Output(CString msg);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMiniRaysClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnChooseScene();
	afx_msg void OnRender();
	afx_msg void OnStatus();
	afx_msg void OnInstall();
	afx_msg void OnOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINIRAYSCLIENTDLG_H__57386403_3628_4729_BF38_CE52B12B58B6__INCLUDED_)
