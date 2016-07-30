// GLFrameWnd.cpp : implementation file
//
#include "StdAfx.h"

#include "Resource.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_GLFRAMEWND_H__F8E95403_4608_11D3_9142_9135643D7AEC__INCLUDED_)
	#include "GLFrameWnd.h"
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
#if !defined(AFX_RAPTORMESSAGES_H__55776166_2943_4D08_BFC8_65DFB74FD780__INCLUDED_)
    #include "System/RaptorMessages.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "System/RaptorConfig.h"
#endif


RAPTOR_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CGLFrameWnd

IMPLEMENT_DYNCREATE(CGLFrameWnd, CFrameWnd)

CGLFrameWnd::CGLFrameWnd()
	:m_width(0),m_height(0),m_menuHeight(0),m_wndStatusBar(NULL)
{
}

CGLFrameWnd::~CGLFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CGLFrameWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CGLFrameWnd)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CREATE()
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

extern AFX_EXTENSION_MODULE RaptorMFCExtensionDLL;

/////////////////////////////////////////////////////////////////////////////
// CGLFrameWnd message handlers

void CGLFrameWnd::OnDestroy() 
{
    if (m_wndStatusBar != NULL)
    {
        m_wndStatusBar->DestroyWindow();
        m_wndStatusBar = NULL;
    }

	CFrameWnd::OnDestroy();

    CWinApp* pApp = AfxGetApp();
    if ((pApp != NULL) && (pApp->m_pMainWnd == this))
	{
		if (Raptor::GetConfig().m_bAutoDestroy)
		    Raptor::glQuitRaptor();
        PostQuitMessage(0);
	}
}

void CGLFrameWnd::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

    CView *pView = GetActiveView();
    if (pView != NULL)
    {
        RECT rect;
        pView->GetClientRect(&rect);
        m_width = rect.right - rect.left; //cx - GetSystemMetrics(SM_CXEDGE);
	    m_height = rect.bottom - rect.top; //cy - m_menuHeight - 2;	
    }
    else
    {
        m_width = cx;
        m_height = cy;
    }
}

int CGLFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    CWinApp *pApp = AfxGetApp();
    if (pApp == NULL)
    {
		RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(),"Raptor GLFrameWnd cannot be created without an MFC Application");
		return -1;      // fail to create
    }

	bool status_bar = CGLWnd::GetDefaultDisplayConfig().status_bar;
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (status_bar)
	{
		HINSTANCE hresource = AfxGetResourceHandle( );
		AfxSetResourceHandle( RaptorMFCExtensionDLL.hResource ); 

        if (m_wndStatusBar == NULL)
            m_wndStatusBar = new CStatusBar();

		if (!m_wndStatusBar->Create(this,WS_CHILD | CBRS_BOTTOM | WS_VISIBLE))
		{
			RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(),"Raptor GLFrameWnd failed to create status bar");
			return -1;      // fail to create
		}
	
		if(!m_wndStatusBar->SetIndicators(indicators,sizeof(indicators)/sizeof(UINT)))
		{
			RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(),"Raptor GLFrameWnd failed to set status bar indicators");
			return -1;      // fail to create
		}

		UINT nID,nStyle;
		int cxWidth;
		m_wndStatusBar->GetPaneInfo(0,nID,nStyle,cxWidth );
		m_wndStatusBar->SetPaneInfo(0,nID,nStyle,100 );
		AfxSetResourceHandle( hresource ); 
	}

	return 0;
}


void CGLFrameWnd::ShowCollision(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString s;
	s.Format(TEXT("c:%d"), C3DEngine::GetNbCollisions());
	pCmdUI->SetText(s);
}

void CGLFrameWnd::ShowWidth(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString s;
	s.Format(TEXT("w:%d"), m_width);
	pCmdUI->SetText(s);
}

void CGLFrameWnd::ShowHeight(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString s;
	s.Format(TEXT("h:%d"), m_height);
	pCmdUI->SetText(s);
}

void CGLFrameWnd::ShowFps(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CGLView *pView = (CGLView *) GetActiveView();
    if (pView == NULL)
        return;

	CString s;

	const CRaptorDisplay* const pDisplay = pView->getDisplay();
	
	if (pDisplay != NULL) 
		s.Format(TEXT("Fps:%.1lf"), pDisplay->getRTFPS());
	else
		s = "fps: n/a";
	pCmdUI->SetText(s);
}

void CGLFrameWnd::RefreshFps()
{
	CGLView *pView = (CGLView *) GetActiveView();
    if (pView == NULL)
        return;

	CString s;
	const CRaptorDisplay* const pDisplay = pView->getDisplay();
	s.Format(TEXT("fps:%.1lf"), pDisplay->getRTFPS());
	m_wndStatusBar->SetPaneText(3,s,true );
}

BOOL CGLFrameWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	const CRaptorDisplayConfig& GLCS = CGLWnd::GetDefaultDisplayConfig();

	unsigned int ffx = GetSystemMetrics(SM_CXFIXEDFRAME);
	unsigned int fx = GetSystemMetrics(SM_CXFRAME);
	unsigned int ex = GetSystemMetrics(SM_CXEDGE);
	unsigned int ffy = GetSystemMetrics(SM_CYFIXEDFRAME);
	unsigned int fy = GetSystemMetrics(SM_CYFRAME);
	unsigned int ey = GetSystemMetrics(SM_CYEDGE);

	if ((cs.style & WS_THICKFRAME) == WS_THICKFRAME)
	{
		cs.cx = GLCS.width +2 * (fx + ex);
		cs.cy = GLCS.height + 2 * (fy + ey);
	}
	else
	{
		cs.cx = GLCS.width +2 * (ffx + ex);
		cs.cy = GLCS.height + 2 * (ffy + ey);
	}
	
	cs.cy += GetSystemMetrics(SM_CYCAPTION);

	if (cs.hMenu != 0)
	{
		m_menuHeight = GetSystemMetrics(SM_CYMENU);
		cs.cy += m_menuHeight;
	}
	if (GLCS.status_bar)
	{
		cs.cy += GetSystemMetrics(SM_CYMENU) - 2;
	}
					
	cs.x = GLCS.x;
	cs.y = GLCS.y;

	int padding = 2 * GetSystemMetrics(SM_CXPADDEDBORDER);
	if (padding > 0)
	{
		padding += 2;	// no justification: a bug in window size between VS2010/VS2013
		cs.cx += padding;
		cs.cy += padding;
	}

	return CFrameWnd::PreCreateWindow(cs);
}

