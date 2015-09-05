// RaysClientView.h : interface of the CRaysClientView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAYSCLIENTVIEW_H__51956FDE_CCAD_404E_8B61_115A872982A9__INCLUDED_)
#define AFX_RAYSCLIENTVIEW_H__51956FDE_CCAD_404E_8B61_115A872982A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Key mapping
#define KEY_BKSPC	8
#define KEY_TAB		9
#define KEY_ENTER	13
#define KEY_SHIFT	16
#define KEY_CTRL	17
#define KEY_CAPS	20
#define KEY_ESC		27
#define KEY_PGUP	33
#define KEY_PGDOWN	34
#define KEY_END		35
#define KEY_HOME	36
#define KEY_LEFT	37
#define KEY_UP		38
#define KEY_RIGHT	39
#define KEY_DOWN	40
#define KEY_INS		45
#define KEY_DEL		46
#define KEY_F1		112
#define KEY_F2		113
#define KEY_F3		114
#define KEY_F4		115
#define KEY_F5		116
#define KEY_F6		117
#define KEY_F7		118
#define KEY_F8		119
#define KEY_F9		120
#define KEY_F10		121
#define KEY_F11		122
#define KEY_F12		123

#define CURSOR_WIDTH	2
#define TAB_SIZE		8

class CRaysClientView : public CScrollView
{
protected: // create from serialization only
	CRaysClientView();
	DECLARE_DYNCREATE(CRaysClientView)

	// Lignes a retracer
	int		DrawLine;
	// Origine texte pour Scrolling
	int		orgx,orgy;
	// Position et taille curseur
	int		Cursorx,Cursory,CursorOffset,CursorHeight;
	// La fonte du texte
	LOGFONT	fontSpecs;
	CFont	*Font;

// Attributes
public:
	CRaysClientDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaysClientView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRaysClientView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRaysClientView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnBuild();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RaysClientView.cpp
inline CRaysClientDoc* CRaysClientView::GetDocument()
   { return (CRaysClientDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAYSCLIENTVIEW_H__51956FDE_CCAD_404E_8B61_115A872982A9__INCLUDED_)
