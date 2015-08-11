// GLDocument.cpp : implementation file
//
#include "StdAfx.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_GLDOCUMENT_H__248DDDD2_9C7C_11D3_9856_A83014000000__INCLUDED_)
	#include "GLDocument.h"
#endif
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

/////////////////////////////////////////////////////////////////////////////
// CGLDocument

IMPLEMENT_DYNCREATE(CGLDocument, CDocument)

CGLDocument::CGLDocument(CString name)
	:m_initialised(false),m_nbAttachedViews(0)
{
}

BOOL CGLDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	const string &caption = CGLWnd::GetDefaultDisplayConfig().caption;
	SetTitle(CA2T(caption.data()));

	m_initialised = false;

	return TRUE;
}

BOOL CGLDocument::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_initialised = false;

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	const string &caption = CGLWnd::GetDefaultDisplayConfig().caption;
	SetTitle(CA2T(caption.data()));

	return TRUE;
}


CGLDocument::~CGLDocument()
{
}


BEGIN_MESSAGE_MAP(CGLDocument, CDocument)
	//{{AFX_MSG_MAP(CGLDocument)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGLDocument serialization

void CGLDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGLDocument commands

//	Default virtual implementation
void CGLDocument::GLInitContext(void)
{
	glClearColor(0.0,0.0,0.0,0.0);

	CATCH_GL_ERROR
}


void CGLDocument::GLDisplayFunc(int )
{
	glClear(GL_COLOR_BUFFER_BIT);

	CATCH_GL_ERROR
}

void CGLDocument::glRender(void)
{
	UpdateAllViews(NULL);
}

bool CGLDocument::glInitialize(void)
{
	if (!m_initialised)
	{
		GLInitContext();
		m_initialised = true;
	}

	return !m_initialised;
}

int CGLDocument::attachView(const CGLView* view)
{
	return m_nbAttachedViews++;
}
