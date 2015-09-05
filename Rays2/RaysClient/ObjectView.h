#if !defined(AFX_OBJECTVIEW_H__33490D9A_65B2_47A1_8FDA_0A93A30E0073__INCLUDED_)
#define AFX_OBJECTVIEW_H__33490D9A_65B2_47A1_8FDA_0A93A30E0073__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include <afxcview.h>

class CObjectView : public CTreeView
{
protected:
	CObjectView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CObjectView)


// Attributes
public:
	CImageList	*ObjectsData;
	HTREEITEM	root;				// root of all items
	CString		buffer;				// buffer for text operations on items
	int			line;				// corresponding line in edit view ( temporary )

// Operations
public:
	CRaysClientDoc* GetDocument();	// returns shared document
	void UpdateTree(DWORD ID);		// adds all items relative to object ID

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
//	public:
	virtual ~CObjectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CObjectView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline CRaysClientDoc* CObjectView::GetDocument()
   { return (CRaysClientDoc*)m_pDocument; }
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTVIEW_H__33490D9A_65B2_47A1_8FDA_0A93A30E0073__INCLUDED_)
