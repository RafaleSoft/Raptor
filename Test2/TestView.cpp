// TestView.cpp : implementation file
//

#include "stdafx.h"
#include "TestView.h"
#include "Teapot.h"

#include "GLHierarchy/Geometry.h"

/////////////////////////////////////////////////////////////////////////////
// CTestView

IMPLEMENT_DYNCREATE(CTestView, CGLView)

CTestView::CTestView()
{
	shift = false;
}

CTestView::~CTestView()
{
}


BEGIN_MESSAGE_MAP(CTestView, CGLView)
	//{{AFX_MSG_MAP(CTestView)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTestView diagnostics

#ifdef _DEBUG
void CTestView::AssertValid() const
{
	CView::AssertValid();
}

void CTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestView message handlers

void CTestView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CTeapot	*pDoc = (CTeapot *)GetDocument();

	switch(nChar)
	{
		case VK_UP:
		{
			break;
		}
		case VK_DOWN:
		{
			break;
		}
		case VK_LEFT:
		{
			break;
		}
		case VK_RIGHT:
		{
			break;
		}
		case VK_RETURN:
		{
			break;
		}
		case VK_BACK:
		{
			break;
		}
		case VK_SHIFT:
		{
			shift = true;
			break;
		}
		case 'R':
		{
			break;
		}
		case 'G':
		{
			break;
		}
		case 'B':
		{
			break;
		}	
		case 'A':
		{
			break;
		}
		case 'C':
		{
			pDoc->projectionDisplay.switchCaustics();
			pDoc->vertexShadersDisplay.switchBump();
			break;
		}
		default:
			break;
	}

	CGLView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTestView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	switch(nChar)
	{
		case VK_SHIFT:
			{
				shift = false;
			}
			break;
		default:
			break;
	}

	CGLView::OnKeyUp(nChar, nRepCnt, nFlags);
}
