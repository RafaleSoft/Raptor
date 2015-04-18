// CTextureFilter.cpp: implementation of the CTextureFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(AFX_TEXTUREFILTER_H__B090C282_EC70_4B33_9270_8D4C7654484E__INCLUDED_)
	#include "TextureFilter.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTextureFilter::CTextureFilter(CTextureObject* pSource, CRaptorDisplayFilter* pFilter)
 :m_pTextureSource(pSource),m_pFilter(pFilter)
{
	m_pFilter->setColorInput(m_pTextureSource);
	m_pFilterTexture = m_pFilter->glCreateColorOutput();
	m_pFilter->glInitFilter();
}

CTextureFilter::~CTextureFilter(void)
{
}

void CTextureFilter::glRender()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1,1,-1,1,1,100);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	m_pFilter->glRender();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}