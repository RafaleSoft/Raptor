// MainDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "GLObjectViewerView.h"
#include "resource.h"
#include "MainDialogBar.h"

/////////////////////////////////////////////////////////////////////////////
// CMainDialogBar dialog


CMainDialogBar::CMainDialogBar(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CMainDialogBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMainDialogBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainDialogBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMainDialogBar, CDialogBar)
	//{{AFX_MSG_MAP(CMainDialogBar)
	ON_BN_CLICKED(IDC_BACKFACES, OnBackFaces)
	ON_BN_CLICKED(IDC_WIREFRAME, OnWireframe)
	ON_BN_CLICKED(IDC_LIGHTING, OnLighting)
	ON_BN_CLICKED(IDC_TEXTURING, OnTexturing)
	ON_BN_CLICKED(IDC_BBOXES,OnBBoxes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDialogBar message handlers

void CMainDialogBar::OnWireframe() 
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
    CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
    CGLObjectViewerView *pView = (CGLObjectViewerView *) pChild->GetActiveView();

	if (pView != NULL)
	{
		bool wireframe = (0 != IsDlgButtonChecked(IDC_WIREFRAME));
		pView->setWireFrame(wireframe);
	}
}

void CMainDialogBar::OnBackFaces() 
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
    CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
    CGLObjectViewerView *pView = (CGLObjectViewerView *) pChild->GetActiveView();

	if (pView != NULL)
	{
		bool backfaces = (0 != IsDlgButtonChecked(IDC_BACKFACES));
		pView->setBackFaces(backfaces);
	}
}

void CMainDialogBar::OnLighting() 
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
    CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
    CGLObjectViewerView *pView = (CGLObjectViewerView *) pChild->GetActiveView();

	if (pView != NULL)
	{
		bool lighting = (0 != IsDlgButtonChecked(IDC_LIGHTING));
		pView->setLighting(lighting);
	}
}

void CMainDialogBar::OnTexturing() 
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
    CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
    CGLObjectViewerView *pView = (CGLObjectViewerView *) pChild->GetActiveView();

	if (pView != NULL)
	{
		bool texturing = (0 != IsDlgButtonChecked(IDC_TEXTURING));
		pView->setTexturing(texturing);
	}
}

void CMainDialogBar::OnBBoxes() 
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
    CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
    CGLObjectViewerView *pView = (CGLObjectViewerView *) pChild->GetActiveView();

	if (pView != NULL)
	{
		bool bboxes = (0 != IsDlgButtonChecked(IDC_BBOXES));
		pView->setBBoxes(bboxes);
	}
}

