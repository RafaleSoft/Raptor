#if !defined(AFX_GLFRAMEWND_H__F8E95403_4608_11D3_9142_9135643D7AEC__INCLUDED_)
#define AFX_GLFRAMEWND_H__F8E95403_4608_11D3_9142_9135643D7AEC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CStatusBar;

RAPTOR_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CGLFrameWnd frame

class RAPTOR_API CGLFrameWnd : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(CGLFrameWnd)
	CGLFrameWnd();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLFrameWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGLFrameWnd();

	void ShowCollision(CCmdUI *pCmdUI);
	void ShowStatus(CCmdUI *pCmdUI);
	void ShowWidth(CCmdUI *pCmdUI);
	void ShowHeight(CCmdUI *pCmdUI);
	void ShowFps(CCmdUI *pCmdUI);
	void RefreshFps(void);
	// Generated message map functions
	//{{AFX_MSG(CGLFrameWnd)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attributes
private:
	int				m_width;
	int				m_height;
	int				m_menuHeight;

	CStatusBar		*m_wndStatusBar;
};

RAPTOR_NAMESPACE_END

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLFRAMEWND_H__F8E95403_4608_11D3_9142_9135643D7AEC__INCLUDED_)
