// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "RaysClient.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "RaysClientDoc.h"
#include "RaysClientView.h"
#include "ObjectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//cs.style = cs.style|WS_HSCROLL|WS_VSCROLL;

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	CMDIChildWnd::OnMouseMove(nFlags, point);
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	BOOL res;
	CSize size(1,1);
	res = splitter.CreateStatic(this,1,2,WS_CHILD|WS_VISIBLE);
	splitter.SetColumnInfo( 0, 300, 1 );
	splitter.SetColumnInfo( 1, 100, 1 );
	size.cx = 300;
	size.cy = 50;
	res = res & splitter.CreateView( 0, 0, RUNTIME_CLASS(CRaysClientView), size, pContext );
	size.cx = 100;
	res = res & splitter.CreateView( 0, 1, RUNTIME_CLASS(CObjectView), size, pContext );
	splitter.RecalcLayout();
	
	//return CMDIChildWnd::OnCreateClient(lpcs, pContext);
	return res;
}
