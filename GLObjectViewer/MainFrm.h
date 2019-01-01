// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__9D49C47A_AFA7_4919_85FF_E149DB28CA10__INCLUDED_)
#define AFX_MAINFRM_H__9D49C47A_AFA7_4919_85FF_E149DB28CA10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MainDialogBar.h"
#include "TreeDialogBar.h"
#include "PropertyDialog.h"
#include "ShaderDialog.h"

#include "GLHierarchy/3DSet.h"

RAPTOR_NAMESPACE



class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:
	void updateTools(C3DSet *pScene);

	void updateSelectedObject(CObject3D *pObj);

	void redrawScene();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar      m_wndStatusBar;
	CToolBar        m_wndToolBar;
    CMainDialogBar  m_wndDialogBar;
	CTreeDialogBar  m_wndTreeDialogBar;
	CPropertyDialog	m_wndPropertyDialogBar;
	CShaderDialog	m_wndShaderDialog;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditionShadereditor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__9D49C47A_AFA7_4919_85FF_E149DB28CA10__INCLUDED_)
