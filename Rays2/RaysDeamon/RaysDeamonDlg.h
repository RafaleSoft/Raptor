// RaysDeamonDlg.h : header file
//

#if !defined(AFX_RAYSDEAMONDLG_H__644499DD_F261_4E14_A695_C7CAC99E087D__INCLUDED_)
#define AFX_RAYSDEAMONDLG_H__644499DD_F261_4E14_A695_C7CAC99E087D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRaysDeamonDlg dialog

class CRaysDeamonDlg : public CDialog
{
// Construction
public:
	void SetActive(bool active);
	CRaysDeamonDlg(CWnd* pParent = NULL);	// standard constructor

	void DisplayInfo(CString msg);

// Dialog Data
	//{{AFX_DATA(CRaysDeamonDlg)
	enum { IDD = IDD_RAYSDEAMON_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaysDeamonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRaysDeamonDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAYSDEAMONDLG_H__644499DD_F261_4E14_A695_C7CAC99E087D__INCLUDED_)
