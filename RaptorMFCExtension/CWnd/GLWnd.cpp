/***************************************************************************/
/*                                                                         */
/*  GLWnd.cpp                                                              */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

#include "StdAfx.h"

#if !defined(AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_)
	#include "GLWnd.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE

static CRaptorDisplayConfig defaultConfig;

void CGLWnd::SetDefaultDisplayConfig(const CRaptorDisplayConfig& pcs)
{
	defaultConfig = pcs;

	if (defaultConfig.x < 0)
		defaultConfig.x = 0;
	if (defaultConfig.y < 0)
		defaultConfig.y = 0;
	if (defaultConfig.height < 1)
		defaultConfig.height = 1;
	if (defaultConfig.width < 1)
		defaultConfig.width = 1;

	//	validates refresh rate
	if (defaultConfig.refresh_rate.fps<CGL_MINREFRESHRATE)
		defaultConfig.refresh_rate.fps = CGL_MINREFRESHRATE;
	else if (defaultConfig.refresh_rate.fps>CGL_MAXREFRESHRATE)
		defaultConfig.refresh_rate.fps = CGL_MAXREFRESHRATE;
	
	int xmax = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int ymax = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	if (defaultConfig.height + defaultConfig.y > ymax)
		defaultConfig.height = ymax - defaultConfig.y;
	if (defaultConfig.width + defaultConfig.x > xmax)
		defaultConfig.width = xmax - defaultConfig.x;
}

const CRaptorDisplayConfig &CGLWnd::GetDefaultDisplayConfig(void)
{
	return defaultConfig;
}

/////////////////////////////////////////////////////////////////////////////
// CGLWnd
/////////////////////////////////////////////////////////////////////////////
CGLWnd::CGLWnd(CString name)
{
	m_pDisplay = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CGLView commands
/////////////////////////////////////////////////////////////////////////////
void CGLWnd::glMakeCurrent(bool restoreContext)
{
	if (restoreContext == true)
	{
		CClientDC dc(this) ;
		RAPTOR_HANDLE display(DEVICE_CONTEXT_CLASS, (void*)(dc.m_hDC));
		m_pDisplay->glvkBindDisplay(display);
	}
	else
		m_pDisplay->glvkUnBindDisplay();
}

BOOL CGLWnd::OnEraseBkgnd(CDC* ) 
{
	return TRUE;
}


bool CGLWnd::GLCreateWindow (CString name,CWnd *parent, const CRaptorDisplayConfig& glCreateStruct)
{
	RECT GLRect;
	
	GLRect.top = glCreateStruct.y;
	GLRect.left = glCreateStruct.x;
	GLRect.right = glCreateStruct.x + glCreateStruct.width;
	GLRect.bottom = glCreateStruct.y + glCreateStruct.height;

	SetDefaultDisplayConfig(glCreateStruct);
	
	CString ClassName = AfxRegisterWndClass(CS_PARENTDC|CS_DBLCLKS);

	if (parent == NULL)
	{
		if (0 != CreateEx(	0,
							ClassName,
							name,
							WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
							glCreateStruct.x,glCreateStruct.y,
							glCreateStruct.width,glCreateStruct.height,
							NULL,
							NULL,
							NULL) )
		{
			CClientDC dc(this) ;

			RAPTOR_HANDLE display(DEVICE_CONTEXT_CLASS, (void*)(dc.m_hDC));
			m_pDisplay->glvkBindDisplay(display);

			GLInitContext();
		
			m_pDisplay->glResize(glCreateStruct.width,glCreateStruct.height,0,0);

			m_pDisplay->glRender();

			CATCH_GL_ERROR

			m_pDisplay->glvkUnBindDisplay();

			return true;
		}
		else
			return false;
	}
	else if (0!=Create(ClassName,name,WS_CHILD | WS_VISIBLE,GLRect,parent,0))
	{
		CClientDC dc(this) ;

		RAPTOR_HANDLE display(DEVICE_CONTEXT_CLASS, (void*)(dc.m_hDC));
		m_pDisplay->glvkBindDisplay(display);

		GLInitContext();

		m_pDisplay->glResize(glCreateStruct.width,glCreateStruct.height,0,0);

		m_pDisplay->glRender();

		CATCH_GL_ERROR

		m_pDisplay->glvkUnBindDisplay();

		return true;
	}
	else
		return false;
}

BEGIN_MESSAGE_MAP(CGLWnd, CWnd)
	//{{AFX_MSG_MAP(CGLWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGLWnd message handlers
/////////////////////////////////////////////////////////////////////////////
int CGLWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	COLORREF c = GetSysColor(COLOR_3DFACE);
	SystemBlue=((float)((c>>16)&0xFF))/255.0f;
	SystemGreen=((float)((c>>8)&0xFF))/255.0f;
	SystemRed=((float)(c&0xFF))/255.0f;

	CClientDC	dc(this);

	const CRaptorDisplayConfig& pcs = GetDefaultDisplayConfig();
	m_pDisplay = Raptor::glCreateDisplay(pcs);

	if (m_pDisplay == NULL)
		return -1;

	RAPTOR_HANDLE display(DEVICE_CONTEXT_CLASS, (void*)(dc.m_hDC));
	if (m_pDisplay->glvkBindDisplay(display))
	{
		m_pDisplay->glvkUnBindDisplay();
		return 1;
	}
	else
	{
		// Try at least a software display
		Raptor::glDestroyDisplay(m_pDisplay);

		CRaptorDisplayConfig cs = pcs;
		cs.acceleration = CRaptorDisplayConfig::SOFTWARE;

		m_pDisplay = Raptor::glCreateDisplay(cs);

		if (m_pDisplay == NULL)
			return -1;

		if (m_pDisplay->glvkBindDisplay(display))
		{
			m_pDisplay->glvkUnBindDisplay();
			return 1;
		}
		else
			return -1;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGLWnd Implementation
/////////////////////////////////////////////////////////////////////////////
void CGLWnd::glRender(void)
{
	this->Invalidate();
}


void CGLWnd::OnDestroy()
{
	CWnd::OnDestroy();

	if (m_pDisplay != NULL)
		Raptor::glDestroyDisplay(m_pDisplay);
}


void CGLWnd::GLInitContext(void)
{
	glClearColor(SystemRed,SystemGreen,SystemBlue,0.0);

	CATCH_GL_ERROR
}


void CGLWnd::GLDisplayFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	CATCH_GL_ERROR
}


void CGLWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if ( (cx <= 0) || (cy <= 0) ) return ;
	
	CClientDC dc(this) ;

	RAPTOR_HANDLE display(DEVICE_CONTEXT_CLASS, (void*)(dc.m_hDC));
	m_pDisplay->glvkBindDisplay(display);
	m_pDisplay->glResize(cx,cy,0,0);
	
	CATCH_GL_ERROR

	m_pDisplay->glvkUnBindDisplay();
}



void CGLWnd::OnPaint() 
{
	CPaintDC dc(this); 
	
	RAPTOR_HANDLE display(DEVICE_CONTEXT_CLASS, (void*)(dc.m_hDC));
	m_pDisplay->glvkBindDisplay(display);

	GLDisplayFunc();

	m_pDisplay->glRender();

	CATCH_GL_ERROR

	m_pDisplay->glvkUnBindDisplay();
	// Do not call CWnd::OnPaint() for painting messages
}
