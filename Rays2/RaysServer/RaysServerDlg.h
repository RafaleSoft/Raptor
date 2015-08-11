// RaysServerDlg.h : header file
//

#if !defined(AFX_RAYSSERVERDLG_H__5097C132_23D2_4A53_88D4_B4112F31309C__INCLUDED_)
#define AFX_RAYSSERVERDLG_H__5097C132_23D2_4A53_88D4_B4112F31309C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRaysServerDlg dialog

class CRaysServerDlg : public CDialog
{
// Construction
public:
	CRaysServerDlg(CWnd* pParent = NULL);	// standard constructor

	void SetNbJobs(int n);

// Dialog Data
	//{{AFX_DATA(CRaysServerDlg)
	enum { IDD = IDD_RAYSSERVER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaysServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CServer<CSSocket>	*pServer;

	CPen	*black_pen;
	CPen	*blue_pen;
	CPen	*green_pen;
	CPen	*red_pen;
	CBrush	*blue_brush;
	CBrush	*red_brush;
	CBrush	*green_brush;

	CFont	font;
	bool	redraw;

	CWnd			*wJobID;
	CWnd			*wJobPercent;
	CProgressCtrl	*wProgress;
	CSpinButtonCtrl	*wSpin;

//	CCriticalSection cs;
	void JobDraw(CPaintDC *dc,bool Redraw = false);

	// Generated message map functions
	//{{AFX_MSG(CRaysServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnOptions();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAYSSERVERDLG_H__5097C132_23D2_4A53_88D4_B4112F31309C__INCLUDED_)
