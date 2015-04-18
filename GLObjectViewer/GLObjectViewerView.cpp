// GLObjectViewerView.cpp : implementation of the CGLObjectViewerView class
//

#include "stdafx.h"
#include "GLObjectViewer.h"

#include "GLObjectViewerDoc.h"
#include "GLObjectViewerView.h"

#include "Engine/ViewPoint.h"
#include "System/RaptorDisplay.h"
#include "GLHierarchy/RenderingProperties.h"



/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerView

IMPLEMENT_DYNCREATE(CGLObjectViewerView, CGLView)

BEGIN_MESSAGE_MAP(CGLObjectViewerView, CGLView)
	//{{AFX_MSG_MAP(CGLObjectViewerView)
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerView construction/destruction

CGLObjectViewerView::CGLObjectViewerView()
{
	// TODO: add construction code here

}

CGLObjectViewerView::~CGLObjectViewerView()
{
}

BOOL CGLObjectViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CGLView::PreCreateWindow(cs);
}


void CGLObjectViewerView::OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint)
{
	CGLView::OnUpdate(pSender,lHint,pHint);

	InvalidateRect(NULL);

	// Why is this message needed ?
	SendMessage(WM_PAINT);
}

/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerView drawing

void CGLObjectViewerView::OnDraw(CDC* pDC)
{
	CGLView::OnDraw(pDC);
}


void CGLObjectViewerView::setBBoxes(bool bboxes)
{
	GetDocument()->setBBoxes(bboxes);

    InvalidateRect(NULL);
}


void CGLObjectViewerView::setWireFrame(bool wireframe)
{
    CRaptorDisplay *pDisplay = getDisplay();
    CRenderingProperties *props = pDisplay->getRenderingProperties();
    props->setWireframe(wireframe ? 
                        CRenderingProperties::ENABLE : 
                        CRenderingProperties::DISABLE);

    InvalidateRect(NULL);
}


void CGLObjectViewerView::setBackFaces(bool backfaces)
{
    CRaptorDisplay *pDisplay = getDisplay();
    CRenderingProperties *props = pDisplay->getRenderingProperties();
    props->setCullFace(	backfaces ? 
                        CRenderingProperties::DISABLE : 
                        CRenderingProperties::ENABLE);

    InvalidateRect(NULL);
}


void CGLObjectViewerView::setLighting(bool lighting)
{
	CRaptorDisplay *pDisplay = getDisplay();
    CRenderingProperties *props = pDisplay->getRenderingProperties();
    props->setLighting(	lighting ? 
                        CRenderingProperties::ENABLE : 
                        CRenderingProperties::DISABLE);

    InvalidateRect(NULL);
}

void CGLObjectViewerView::setTexturing(bool texturing)
{
	CRaptorDisplay *pDisplay = getDisplay();
    CRenderingProperties *props = pDisplay->getRenderingProperties();
    props->setTexturing(texturing ? 
                        CRenderingProperties::ENABLE : 
                        CRenderingProperties::DISABLE);

    InvalidateRect(NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerView printing

BOOL CGLObjectViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGLObjectViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGLObjectViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerView message handlers

void CGLObjectViewerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CGLView::OnKeyDown(nChar, nRepCnt, nFlags);

    CObject3DInstance *pRoot = GetDocument()->getRoot();

    switch (nChar)
    {
        case VK_LEFT:
            pRoot->translate(1.0f,0.0f,0.0f);
            break;
        case VK_RIGHT:
            pRoot->translate(-1.0f,0.0f,0.0f);
            break;
        case VK_UP:
            pRoot->translate(0.0f,-1.0f,0.0f);
            break;
        case VK_DOWN:
            pRoot->translate(0.0f,1.0f,0.0f);
            break;
    }

    InvalidateRect(NULL);
}



void CGLObjectViewerView::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (MK_LBUTTON == (nFlags & MK_LBUTTON))
    {
        CPoint delta = point - lastPoint;

		CObject3DInstance *pRoot = GetDocument()->getRoot();
		pRoot->translate(0.05f * delta.x, -0.05f * delta.y, 0.0f );

        InvalidateRect(NULL);

        lastPoint = point;
    }
	else if (MK_RBUTTON == (nFlags & MK_RBUTTON))
    {
		CPoint delta = point - lastPoint;

		CObject3DInstance *pRoot = GetDocument()->getRoot();
		pRoot->rotationX(0.1f * delta.y);
		pRoot->rotationY(0.1f * delta.x);

        InvalidateRect(NULL);

        lastPoint = point;
	}

	CGLView::OnMouseMove(nFlags, point);
}

BOOL CGLObjectViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	CRaptorDisplay *pDisplay = getDisplay();
	CViewPoint * const vp = pDisplay->getViewPoint();
	if (zDelta < 0)
        vp->translate(0.0f,0.0f,-1.0f);
    else
        vp->translate(0.0f,0.0f,1.0f);

    InvalidateRect(NULL);

	return CGLView::OnMouseWheel(nFlags, zDelta, pt);
}

void CGLObjectViewerView::OnLButtonDown(UINT nFlags, CPoint point) 
{
    lastPoint = point;
	CGLView::OnLButtonDown(nFlags, point);
}

void CGLObjectViewerView::OnRButtonDown(UINT nFlags, CPoint point) 
{
    lastPoint = point;
	CGLView::OnLButtonDown(nFlags, point);
}
