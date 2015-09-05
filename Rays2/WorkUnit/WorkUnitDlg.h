// WorkUnitDlg.h : header file
//

#if !defined(AFX_WORKUNITDLG_H__3BDD2E04_B183_4A39_9CDE_72B025788B5A__INCLUDED_)
#define AFX_WORKUNITDLG_H__3BDD2E04_B183_4A39_9CDE_72B025788B5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWorkUnitDlg dialog

typedef struct _raytracer_data * LPRAYTRACERDATA;

class CWorkUnitDlg : public CDialog
{
	friend class CWorkUnitApp;

// Construction
public:
	CWorkUnitDlg(CString address,unsigned int port,int ID,CWnd* pParent = NULL);	// standard constructor

	void SetProgress(int progress);

	LPRAYTRACERDATA	data;

// Dialog Data
	//{{AFX_DATA(CWorkUnitDlg)
	enum { IDD = IDD_WORKUNIT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON			m_hIcon;

	CString			sname;
	unsigned int	sport;
	int				sID;

	CListBox		*lb;
	CProgressCtrl	*pCtrl; 
	// Generated message map functions
	//{{AFX_MSG(CWorkUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKUNITDLG_H__3BDD2E04_B183_4A39_9CDE_72B025788B5A__INCLUDED_)
