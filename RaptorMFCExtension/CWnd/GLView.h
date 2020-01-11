#if !defined(AFX_GLVIEW_H__6E79FB08_9C61_11D3_9855_383E0D000000__INCLUDED_)
#define AFX_GLVIEW_H__6E79FB08_9C61_11D3_9855_383E0D000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GLView.h : header file
//

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CRaptorDisplay;

/////////////////////////////////////////////////////////////////////////////
// CGLView view

class RAPTOR_API CGLView : public CView
{
protected:
	DECLARE_DYNCREATE(CGLView)

// Operations
public:
	//!	Returns the unique ID of this view, respective
	//! to the underlying CDocument.
	int getViewID(void) const { return m_viewID; };

	//! Returns the display associated with this view
	//! The display is only valid if the view has properly
	//! been created.
	CRaptorDisplay* const getDisplay(void) const { return m_pDisplay; };

	//!	Activates the OpenGL context associated with this view
	void glMakeCurrent(bool restoreContext = true);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CGLView();           // protected constructor used by dynamic creation
	virtual ~CGLView();

	// Generated message map functions
	//{{AFX_MSG(CGLView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnChar( UINT, UINT, UINT );
	afx_msg void OnKeyUp(UINT, UINT, UINT );
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg BOOL OnMouseWheel(UINT,short,CPoint);


	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int				m_viewID;
	bool			displayFps;

	CRaptorDisplay	*m_pDisplay;
};

RAPTOR_NAMESPACE_END

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLVIEW_H__6E79FB08_9C61_11D3_9855_383E0D000000__INCLUDED_)
