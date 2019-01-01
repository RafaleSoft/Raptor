// MultiGLWnd.cpp : implementation file
//
#include "StdAfx.h"

#if !defined(AFX_MULTIGLWND_H__8025342E_91E0_11D3_9800_8282AC000000__INCLUDED_)
	#include "MultiGLWnd.h"
#endif

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN

static const int CGL_MAX_DISPLAYS=16;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CMultiGLWnd
/////////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4355)	// use of this pointer in constructor
CMultiGLWnd::CMultiGLWnd()
	:nb_displays(0),m_width(0),m_height(0)
{
	Displays = new display_t[CGL_MAX_DISPLAYS];
}

CMultiGLWnd::~CMultiGLWnd()
{
	delete [] Displays;
}

bool CMultiGLWnd::GLCreateFrame(CString name,int x,int y,int width,int height)
{
	RECT GLRect;

	m_width=width;
	m_height=height;
		
	GLRect.top=y;
	GLRect.left=x;
	GLRect.right=x+width+1;
	GLRect.bottom=y+height+1;

	GLRect.right+=2*GetSystemMetrics(SM_CXFRAME);
	GLRect.bottom+=2*GetSystemMetrics(SM_CYFRAME);
	//GLRect.right+=2*GetSystemMetrics(SM_CXBORDER);
	//GLRect.bottom+=2*GetSystemMetrics(SM_CYBORDER);
	GLRect.bottom+=GetSystemMetrics(SM_CYCAPTION);

	//	Registering window class
	WndClass = AfxRegisterWndClass(	CS_DBLCLKS|CS_OWNDC,
									LoadCursor(NULL,IDC_ARROW),
									(HBRUSH)COLOR_WINDOW,
									LoadIcon(NULL,IDI_WINLOGO));
	
	return (0 != CreateEx(	0,
							(LPCTSTR)WndClass,
							(LPCTSTR)name,
							WS_OVERLAPPEDWINDOW,
							GLRect.left,GLRect.top,
							GLRect.right-GLRect.left,GLRect.bottom-GLRect.top,
							NULL,
							NULL,
							NULL));
}

bool CMultiGLWnd::AddDisplay(CGLWnd *d)
{
	ASSERT(d!=NULL);

	if (nb_displays<CGL_MAX_DISPLAYS)
	{
		Displays[nb_displays].display=d;
		CRect rect;
		d->GetClientRect(&rect);
		
		Displays[nb_displays].percentx=(float)(rect.left)/m_width;
		Displays[nb_displays].percenty=(float)(rect.top)/m_height;
		Displays[nb_displays].percentw=(float)(rect.right-rect.left)/m_width;
		Displays[nb_displays++].percenth=(float)(rect.bottom-rect.top)/m_height;

		//registerDestruction(d);

		return true;
	}
	else
		return false;
}

void CMultiGLWnd::glRender()
{
	for (int i=0;i<nb_displays;i++)
	if (IsWindow(Displays[i].display->m_hWnd))
		Displays[i].display->glRender();
}


BEGIN_MESSAGE_MAP(CMultiGLWnd, CWnd)
	//{{AFX_MSG_MAP(CMultiGLWnd)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMultiGLWnd message handlers
void CMultiGLWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
	while (nb_displays>0)
	{
		//unregisterDestruction(Displays[nb_displays-1].display);

		Displays[nb_displays-1].display->DestroyWindow();
		delete Displays[--nb_displays].display;
	}
}

void CMultiGLWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	m_width=cx;m_height=cy;

	for (int i=0;i<nb_displays;i++)
	if (IsWindow(Displays[i].display->m_hWnd))
	{
		int x,y,w,h;
		x=(int)(cx*Displays[i].percentx);
		y=(int)(cy*Displays[i].percenty);
		w=(int)(cx*Displays[i].percentw);
		h=(int)(cy*Displays[i].percenth);

		Displays[i].display->MoveWindow(x,y,w,h);	
		Displays[i].display->glRender();
	}
}

int CMultiGLWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}
