#if !defined(AFX_GLMDICHILDWND_H__2AFD219F_0CC0_4C56_AE35_3F7B07095BAA__INCLUDED_)
#define AFX_GLMDICHILDWND_H__2AFD219F_0CC0_4C56_AE35_3F7B07095BAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GLMDIChildWnd.h : header file
//

RAPTOR_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CGLMDIChildWnd frame

class RAPTOR_API CGLMDIChildWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CGLMDIChildWnd)

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLMDIChildWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL


// Implementation
protected:
	CGLMDIChildWnd();           // protected constructor used by dynamic creation
	virtual ~CGLMDIChildWnd();

	void ShowCollision(CCmdUI *pCmdUI);
	void ShowStatus(CCmdUI *pCmdUI);
	void ShowWidth(CCmdUI *pCmdUI);
	void ShowHeight(CCmdUI *pCmdUI);
	void ShowFps(CCmdUI *pCmdUI);
	void ShowNbObj(CCmdUI *pCmdUI);
	void RefreshFps(void);
	// Generated message map functions
	//{{AFX_MSG(CGLMDIChildWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	int				m_width;
	int				m_height;
	int				m_menuHeight;

	CStatusBar		m_wndStatusBar;
};

RAPTOR_NAMESPACE_END

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLMDICHILDWND_H__2AFD219F_0CC0_4C56_AE35_3F7B07095BAA__INCLUDED_)
