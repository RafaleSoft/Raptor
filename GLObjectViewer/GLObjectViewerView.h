// GLObjectViewerView.h : interface of the CGLObjectViewerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBJECTVIEWERVIEW_H__9F8EBFA5_EE4C_43B3_BEB2_5BF23D649B80__INCLUDED_)
#define AFX_GLOBJECTVIEWERVIEW_H__9F8EBFA5_EE4C_43B3_BEB2_5BF23D649B80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MFCExtension/CWnd/GLView.h"

RAPTOR_NAMESPACE

class CGLObjectViewerDoc;

class CGLObjectViewerView : public CGLView
{
public:
	CGLObjectViewerDoc* GetDocument();

    void setWireFrame(bool wireframe);
	void setBackFaces(bool backfaces);
    void setLighting(bool lighting);
    void setTexturing(bool texturing);
	void setBBoxes(bool bboxes);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLObjectViewerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint);
	//}}AFX_VIRTUAL


	virtual ~CGLObjectViewerView();

// Generated message map functions
protected:
    CGLObjectViewerView();
	DECLARE_DYNCREATE(CGLObjectViewerView)


	//{{AFX_MSG(CGLObjectViewerView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CPoint  lastPoint;
};


inline CGLObjectViewerDoc* CGLObjectViewerView::GetDocument()
{ 
    return (CGLObjectViewerDoc*)m_pDocument; 
}


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBJECTVIEWERVIEW_H__9F8EBFA5_EE4C_43B3_BEB2_5BF23D649B80__INCLUDED_)
