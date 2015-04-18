// RootInstance.cpp: implementation of the CRootInstance class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GLObjectViewer.h"
#include "RootInstance.h"

#include "GLHierarchy/3DSet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRootInstance::CRootInstance()
	:m_bRenderBoxes(false),m_pSelection(NULL)
{

}

CRootInstance::~CRootInstance()
{

}

void CRootInstance::glClipRender()
{
	if (m_pSelection != NULL)
	{
		const GL_MATRIX &M = getTransform();
		glPushMatrix();
		glMultMatrixf(M);
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_LIGHTING);
		glColor4f(1.0f,0.0f,0.0f,1.0f);
		m_pSelection->glRenderBBox();
		glPopAttrib();
		glPopMatrix();
	}

	CObject3DInstance::glClipRender();

	if (m_bRenderBoxes)
	{
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		CObject3DInstance::glRenderBBox();

		const GL_MATRIX &M = getTransform();
		glPushMatrix();
		glMultMatrixf(M);
		((C3DSet*)getObject())->glRenderAllBBox();
		glPopMatrix();
	}
}