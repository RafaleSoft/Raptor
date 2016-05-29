// GLView.cpp : implementation file
//
#include "StdAfx.h"

#include "Resource.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_GLVIEW_H__6E79FB08_9C61_11D3_9855_383E0D000000__INCLUDED_)
	#include "GLView.h"
#endif
#if !defined(AFX_GLDOCUMENT_H__248DDDD2_9C7C_11D3_9856_A83014000000__INCLUDED_)
	#include "GLDocument.h"
#endif
#if !defined(AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_)
	#include "GLWnd.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)
	#include "System/RaptorConsole.h"
#endif

RAPTOR_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// CGLView
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CGLView, CView)

CGLView::CGLView()
	:m_viewID(-1), displayFps(false), m_pDisplay(NULL)
{
}

CGLView::~CGLView()
{
//  Should be removed by purge at Raptor exit
//	if (m_pDisplay != NULL)
//		Raptor::glDestroyDisplay(m_pDisplay);
}


BEGIN_MESSAGE_MAP(CGLView, CView)
	//{{AFX_MSG_MAP(CGLView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
    ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLView drawing
//
//	TODO: GDI drawings could also be rendered here
//	
void CGLView::OnDraw(CDC* pDC)
{
	CGLDocument* pDoc = (CGLDocument* )GetDocument();

	//	Select current rendering context
	RAPTOR_HANDLE display;
    display.handle = (unsigned int)(pDC->m_hDC);
    display.hClass = DEVICE_CONTEXT_CLASS;
	m_pDisplay->glBindDisplay(display);

	pDoc->GLDisplayFunc(m_viewID);

	m_pDisplay->glRender();

	//	Errors management is done before
	//	the context is released
	CATCH_GL_ERROR

	//	Unselect rendering context for other windows
	m_pDisplay->glUnBindDisplay();

	//	Update associated GLFrameWnd Display
	if (displayFps)
	{
		CWnd *wnd = pDC->GetWindow();
		wnd->PostMessage(WM_COMMAND,ID_INDICATOR_FPS,0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGLView message handlers

int CGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	const CRaptorDisplayConfig& pcs = CGLWnd::GetDefaultDisplayConfig();
	if (pcs.status_bar)
		displayFps = true;

	CClientDC dc(this);

	m_pDisplay = Raptor::glCreateDisplay(pcs);

	if (m_pDisplay == NULL)
		return -1;

	RAPTOR_HANDLE display;
	display.handle = (unsigned int)(dc.m_hDC);
	display.hClass = DEVICE_CONTEXT_CLASS;
	if (m_pDisplay->glBindDisplay(display))
	{
		m_pDisplay->glUnBindDisplay();
		return 1;
	}
	else
	{
		// Try at least a software display
		Raptor::glDestroyDisplay(m_pDisplay);

		CRaptorDisplayConfig cs = pcs;
		cs.display_mode |= CGL_SOFTWARE;

		m_pDisplay = Raptor::glCreateDisplay(cs);

		if (m_pDisplay == NULL)
			return -1;

		if (m_pDisplay->glBindDisplay(display))
		{
			m_pDisplay->glUnBindDisplay();
			return 1;
		}
		else
			return -1;
	}
}


BOOL CGLView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;
    cs.dwExStyle |= WS_EX_TRANSPARENT ;

	return CView::PreCreateWindow(cs);
}


void CGLView::OnDestroy() 
{
	CView::OnDestroy();
}

void CGLView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if ( (cx <= 0) || (cy <= 0) ) return ;
	
	CClientDC dc(this) ;
	CGLDocument* pDoc = (CGLDocument* )GetDocument();
	
	RAPTOR_HANDLE display;
	display.handle = (unsigned int)(dc.m_hDC);
	display.hClass = DEVICE_CONTEXT_CLASS;
	m_pDisplay->glBindDisplay(display);
	m_pDisplay->glResize(cx,cy,0,0);

	CATCH_GL_ERROR

	m_pDisplay->glUnBindDisplay();
}


/////////////////////////////////////////////////////////////////////////////
// CGLView commands

void CGLView::glMakeCurrent(bool restoreContext)
{
	if (restoreContext == true)
	{
		CClientDC dc(this) ;
		RAPTOR_HANDLE display;
		display.handle = (unsigned int)(dc.m_hDC);
		display.hClass = DEVICE_CONTEXT_CLASS;
		m_pDisplay->glBindDisplay(display);
	}
	else
		m_pDisplay->glUnBindDisplay();
}


BOOL CGLView::OnEraseBkgnd(CDC* ) 
{
	return TRUE;
}

void CGLView::OnInitialUpdate() 
{
	CClientDC dc(this);
	CGLDocument* pDoc = (CGLDocument* )GetDocument();

	//	Select current rendering context
	RAPTOR_HANDLE display;
	display.handle = (unsigned int)(dc.m_hDC);
	display.hClass = DEVICE_CONTEXT_CLASS;
	m_pDisplay->glBindDisplay(display);

	//	Initialise linked document if necessary
	pDoc->glInitialize();

	//	get view ID
	m_viewID = pDoc->attachView(this);

	CATCH_GL_ERROR

	m_pDisplay->glUnBindDisplay();

	// Initialise MFC after GL init, because
	// a first 'draw' will occur before GL context initialisation
	CView::OnInitialUpdate();
}

void CGLView::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CView::OnChar(nChar,nRepCnt,nFlags);

	CRaptorConsole::CInputCollectorBase::broadcastCharacterInput(nChar);
}

void CGLView::OnKeyUp(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	CView::OnKeyUp(nChar,nRepCnt,nFlags);

	CRaptorConsole::CInputCollectorBase::broadcastSpecialKeyInput(nChar);
}

void CGLView::OnMouseMove(UINT nFlags,CPoint point)
{
	CView::OnMouseMove(nFlags,point);

	int button = 0;
	switch(nFlags)
	{
		case MK_LBUTTON:
			button = 1;
			break;
		case MK_MBUTTON:
			button = 2;
			break;
		case MK_RBUTTON:
			button = 3;
			break;
		case MK_XBUTTON1:
			button = 6;
			break;
	}

	CRaptorConsole::CInputCollectorBase::broadcastMouseInput(button,point.x,point.y);
}

BOOL CGLView::OnMouseWheel(UINT nFlags,short zDelta,CPoint pt)
{
	BOOL res = CView::OnMouseWheel(nFlags,zDelta,pt);

	int button = 4;
	CRaptorConsole::CInputCollectorBase::broadcastMouseInput(button,pt.x,pt.y);

	return res;
}

