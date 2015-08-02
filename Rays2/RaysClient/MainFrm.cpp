// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "RaysClient.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_SERVER_BAR, OnViewServerBar)
	ON_COMMAND(ID_CONNECT_SERVER,OnConnect)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CONNECT, OnUpdateConnect)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_CONNECT,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame():pApp(NULL),connected(false),dragImage(NULL),ObjID(DWORD(-1))
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	pApp = ((CRaysClientApp*)AfxGetApp());
	
	if (!m_wndToolBar.CreateEx(this,TBSTYLE_FLAT|TBSTYLE_TRANSPARENT) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndDlgBar) ||
		!m_wndReBar.AddBar(&m_wndToolBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}
	REBARBANDINFO rbbi;
	rbbi.cbSize = sizeof(REBARBANDINFO);
	rbbi.fMask = RBBIM_ID;
	m_wndReBar.GetReBarCtrl().GetBandInfo( 0, &rbbi );
	barIndex = rbbi.wID = 100;
	m_wndReBar.GetReBarCtrl().SetBandInfo( 0, &rbbi );

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY );
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

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

void CMainFrame::OnViewServerBar() 
{
	CMenu *pMenu = this->GetMenu()->GetSubMenu(2);
	UINT state = pMenu->GetMenuState(ID_VIEW_SERVER_BAR,MF_BYCOMMAND);
	UINT check;

	int index = m_wndReBar.GetReBarCtrl().IDToIndex(barIndex);
	if (state&MF_CHECKED)
	{
		check = pMenu->CheckMenuItem( ID_VIEW_SERVER_BAR, MF_UNCHECKED|MF_BYCOMMAND);
		m_wndReBar.GetReBarCtrl().ShowBand(index,FALSE);
	}
	else
	{
		check = pMenu->CheckMenuItem( ID_VIEW_SERVER_BAR, MF_CHECKED|MF_BYCOMMAND);
		m_wndReBar.GetReBarCtrl().ShowBand(index,TRUE);
	}
	Cout("Display updated");
}

void CMainFrame::OnUpdateConnect(CCmdUI *pCmdUI)
{
	CString text;
	pCmdUI->Enable(); 
	if (connected) text = "Online";
	else text = "      ";
    pCmdUI->SetText( text ); 
}

void CMainFrame::OnConnect() 
{
	int port = m_wndDlgBar.GetDlgItemInt(IDC_PORT);
	CString host;
	m_wndDlgBar.GetDlgItemText(IDC_SERVER,host);
	int index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_CONNECT);
	if (!pApp->ConnectToServer(host,port))
	{
		m_wndStatusBar.SetWindowText("Connection to server failed !");
		connected = false;
	}
	else
	{
		m_wndStatusBar.SetWindowText("Connection to server complete");
		connected = true;
	}
	this->PostMessage(ID_INDICATOR_CONNECT);
}


BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *nmhdr = (NMHDR*)lParam;

	switch(nmhdr->code)
	{
	case TVN_BEGINDRAG:
		{
			Cout("Select an object to add to document");
			CTreeCtrl *tree = (CTreeCtrl*)(CWnd::FromHandle(nmhdr->hwndFrom));
			CPoint point;

			GetCursorPos(&point);tree->ScreenToClient(&point);
			HTREEITEM hit = tree->HitTest(point,NULL);
			tree->SelectItem(hit);

			dragImage = tree->CreateDragImage(hit);
			ObjID = tree->GetItemData(hit);
			dragImage->SetDragCursorImage(1,CPoint(0,0));
			dragImage->BeginDrag(0,CPoint(0,0));
			GetCursorPos(&point);
			dragImage->DragEnter(NULL,point);
		}
		break;
	default:
		if (dragImage != NULL)
		{
			CPoint point;
			GetCursorPos(&point);
			dragImage->DragMove(point);
		}
		break;
	}
	
	return CMDIFrameWnd::OnNotify(wParam, lParam, pResult);
}
