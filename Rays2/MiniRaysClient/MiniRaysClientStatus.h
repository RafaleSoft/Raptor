#if !defined(AFX_MINIRAYSCLIENTSTATUS_H__4DD4256B_D63A_4E7C_B454_29D0EB45004E__INCLUDED_)
#define AFX_MINIRAYSCLIENTSTATUS_H__4DD4256B_D63A_4E7C_B454_29D0EB45004E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MiniRaysClientStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MiniRaysClientStatus dialog

class MiniRaysClientStatus : public CDialog
{
// Construction
public:
	void SetAvailablePlugins(CStringArray &strs);
	void SetAvailableTextures(CStringArray &strs);
	void SetAvailableJobs(unsigned int *jobs);
	void SetCurrentJobInfo(CString &id,CString &st,CString &et,CString p);
	MiniRaysClientStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(MiniRaysClientStatus)
	enum { IDD = IDD_STATUS };
	CListBox	m_plugins;
	CListBox	m_textures;
	CComboBox	m_jobsAvailable;
	CString	m_jobID;
	CString	m_startTime;
	CString	m_endTime;
	CString	m_jobDone;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MiniRaysClientStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MiniRaysClientStatus)
	afx_msg void OnGetjob();
	afx_msg void OnDeljob();
	afx_msg void OnRefresh();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINIRAYSCLIENTSTATUS_H__4DD4256B_D63A_4E7C_B454_29D0EB45004E__INCLUDED_)
