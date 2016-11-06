// GLBenchDlg.h : header file
//

#if !defined(AFX_GLBENCHDLG_H__639DD0D3_378D_4B9B_902A_D558B45FF325__INCLUDED_)
#define AFX_GLBENCHDLG_H__639DD0D3_378D_4B9B_902A_D558B45FF325__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGLBenchDlg dialog

class CGLBenchDlg : public CDialog
{
// Construction
public:
	CGLBenchDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGLBenchDlg)
	enum { IDD = IDD_GLBENCH_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLBenchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	CFrameWnd *benchWindow;

	void LoadModules(const char* path);

	// Generated message map functions
	//{{AFX_MSG(CGLBenchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeList();
	afx_msg void OnBench();
	afx_msg void OnSysinfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLBENCHDLG_H__639DD0D3_378D_4B9B_902A_D558B45FF325__INCLUDED_)
