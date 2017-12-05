// GLMDIChildWnd.cpp : implementation file
//
#include "StdAfx.h"

#include "Resource.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_GLMDICHILDWND_H__2AFD219F_0CC0_4C56_AE35_3F7B07095BAA__INCLUDED_)
	#include "GLMDIChildWnd.h"
#endif
#if !defined(AFX_GLVIEW_H__6E79FB08_9C61_11D3_9855_383E0D000000__INCLUDED_)
	#include "GLView.h"
#endif
#if !defined(AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_)
	#include "GLWnd.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif


extern AFX_EXTENSION_MODULE RaptorMFCExtensionDLL;

RAPTOR_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CGLMDIChildWnd

IMPLEMENT_DYNCREATE(CGLMDIChildWnd, CMDIChildWnd)

CGLMDIChildWnd::CGLMDIChildWnd()
	:m_width(0),m_height(0)
{
}

CGLMDIChildWnd::~CGLMDIChildWnd()
{
}


BEGIN_MESSAGE_MAP(CGLMDIChildWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(CGLMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_INDICATOR_FPS,RefreshFps)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COLLISION,ShowCollision)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_WIDTH,ShowWidth)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_HEIGHT,ShowHeight)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FPS,ShowFps)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_COLLISION,
	ID_INDICATOR_WIDTH,
	ID_INDICATOR_HEIGHT,
	ID_INDICATOR_FPS
};
/////////////////////////////////////////////////////////////////////////////
// CGLMDIChildWnd message handlers

BOOL CGLMDIChildWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	m_menuHeight = GetSystemMetrics(SM_CYMENU);

	const CRaptorDisplayConfig& GLCS = CGLWnd::GetDefaultDisplayConfig();

	cs.cx = GLCS.width+2*GetSystemMetrics(SM_CXFRAME);
	cs.cy = GLCS.height+2*GetSystemMetrics(SM_CYFRAME)
					+GetSystemMetrics(SM_CYCAPTION)
					+m_menuHeight;
					
	cs.x = GLCS.x;
	cs.y = GLCS.y;

	//cs.style = cs.dwExStyle = 0;//cs.style ^ WS_OVERLAPPEDWINDOW;
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

int CGLMDIChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_width = lpCreateStruct->cx;
	m_height = lpCreateStruct->cy;

	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	HINSTANCE hresource = AfxGetResourceHandle( );
	AfxSetResourceHandle( RaptorMFCExtensionDLL.hResource ); 

	if (!m_wndStatusBar.Create(this,WS_CHILD | CBRS_BOTTOM | WS_VISIBLE))
	{
		::MessageBox(NULL, TEXT("failed to create status bar"), TEXT("Raptor GLFrameWnd Create"), MB_OK | MB_ICONERROR);
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	if(!m_wndStatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT)))
	{
		::MessageBox(NULL, TEXT("failed to set status bar indicators"), TEXT("Raptor GLFrameWnd Create"), MB_OK | MB_ICONERROR);
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	UINT nID,nStyle;
	int cxWidth;
	m_wndStatusBar.GetPaneInfo(0,nID,nStyle,cxWidth );
	m_wndStatusBar.SetPaneInfo(0,nID,nStyle,100 );
	AfxSetResourceHandle( hresource );
	
	return 0;
}

void CGLMDIChildWnd::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	m_width = cx;
	m_height = cy;
}

void CGLMDIChildWnd::OnDestroy() 
{
	CMDIChildWnd::OnDestroy();
}

void CGLMDIChildWnd::ShowCollision(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString s;
	s.Format(TEXT("c:%d"),C3DEngine::GetNbCollisions());
	pCmdUI->SetText(s);
}

void CGLMDIChildWnd::ShowWidth(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString s;
	s.Format(TEXT("w:%d"), m_width);
	pCmdUI->SetText(s);
}

void CGLMDIChildWnd::ShowHeight(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString s;
	s.Format(TEXT("h:%d"), m_height - m_menuHeight + 1);
	pCmdUI->SetText(s);
}

void CGLMDIChildWnd::ShowFps(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CGLView *pView = (CGLView *) GetActiveView();
	CString s;
	const CRaptorDisplay* const pDisplay = pView->getDisplay();
	s.Format(TEXT("fps:%.1lf"), pDisplay->getRTFPS());
	pCmdUI->SetText(s);
}

void CGLMDIChildWnd::RefreshFps()
{
	CGLView *pView = (CGLView *) GetActiveView();
	CString s;
	const CRaptorDisplay* const pDisplay = pView->getDisplay();
	s.Format(TEXT("fps:%.1lf"), pDisplay->getRTFPS());
	m_wndStatusBar.SetPaneText(3,s,true );
}



