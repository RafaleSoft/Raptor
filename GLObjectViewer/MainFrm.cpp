// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "GLObjectViewer.h"

#include "MainFrm.h"
#include "GLObjectViewerDoc.h"
#include "GLObjectViewerView.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDITION_SHADEREDITOR, OnEditionShadereditor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::updateTools(C3DSet *pScene)
{
	if (pScene != NULL)
		m_wndTreeDialogBar.setScene(pScene);
	m_wndPropertyDialogBar.clear();
	m_wndPropertyDialogBar.updateShaders();
}

void CMainFrame::redrawScene()
{
	CFrameWnd* pFrame = GetActiveFrame( );
	if (pFrame != NULL)
	{
		CGLObjectViewerView* pView = (CGLObjectViewerView*)pFrame->GetActiveView();
		pView->InvalidateRect(NULL);
	}
}

void CMainFrame::updateSelectedObject(CObject3D *pObj)
{
	if (pObj != NULL)
		m_wndPropertyDialogBar.setObject(pObj);
	else
		m_wndPropertyDialogBar.clear();

	CFrameWnd* pFrame = GetActiveFrame( );
	if (pFrame != NULL)
	{
		CGLObjectViewerDoc* pDoc = (CGLObjectViewerDoc*)pFrame->GetActiveDocument( );
		pDoc->selectObject(pObj);
	}

	redrawScene();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableDocking(CBRS_ALIGN_ANY);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	RecalcLayout();

	RECT rectDock;
	rectDock.top = 0;
	rectDock.left = 0;
    m_wndDialogBar.Create(this,IDD_DIALOGBAR,CBRS_LEFT|CBRS_GRIPPER|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC,ID_MAINDIALOGBAR);
    m_wndDialogBar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndDialogBar,AFX_IDW_DOCKBAR_LEFT,&rectDock);

	RecalcLayout();

	m_wndTreeDialogBar.Create(this,IDD_DIALOGTREE,CBRS_LEFT|CBRS_GRIPPER|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC,ID_TREEDIALOGBAR);
    m_wndTreeDialogBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndTreeDialogBar);

	RecalcLayout();

	m_wndDialogBar.GetWindowRect(&rectDock);
	rectDock.top += 2;
	rectDock.left = 0;
    DockControlBar(&m_wndTreeDialogBar,AFX_IDW_DOCKBAR_LEFT,&rectDock);

	m_wndPropertyDialogBar.Create(this,IDD_DIALOGPROPS,CBRS_LEFT|CBRS_GRIPPER|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC,ID_PROPDIALOGBAR);
    m_wndPropertyDialogBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndPropertyDialogBar);

	RecalcLayout();

	m_wndTreeDialogBar.GetWindowRect(&rectDock);
	rectDock.top += 1; //rectDock.bottom-5;
	rectDock.left = 0;
    DockControlBar(&m_wndPropertyDialogBar,AFX_IDW_DOCKBAR_LEFT,&rectDock);

	if (!m_wndShaderDialog.Create(IDD_DIALOGSHADER, this))
	{
		TRACE0("Failed to create shader dialog\n");
		return -1;
	}


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnEditionShadereditor() 
{
	m_wndShaderDialog.refreshShaders();
	m_wndShaderDialog.ShowWindow(SW_SHOW);
}
