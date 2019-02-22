// ViewPoint.cpp: implementation of the CViewPoint class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_OPENGLVIEWPOINT_H__94BDC36B_27AB_41FC_848E_DD28D1BDFC13__INCLUDED_)
	#include "OpenGLViewPoint.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COpenGLViewPoint::COpenGLViewPoint(const std::string& name) :
	IViewPoint(name)
{
}

COpenGLViewPoint::~COpenGLViewPoint()
{
}


void COpenGLViewPoint::glvkRender(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glScalef(Scale.X(),Scale.Y(),Scale.Z());

	glRotatef(m_lfGamma,0.0f,0.0f,1.0f);
	glRotatef(-m_lfBeta,1.0f,0.0f,0.0f);
	glRotatef(-m_lfAlpha,0.0f,1.0f,0.0f);

	glTranslatef(-Origin.X(),-Origin.Y(),-Origin.Z());

	C3DEngine::Get3DEngine()->glConfigureEngine(this);

	CATCH_GL_ERROR
}

void COpenGLViewPoint::glvkRenderViewPointModel(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	switch(model)
	{
		case ORTHOGRAPHIC:
			glOrtho(viewVolume[0],viewVolume[1],
					viewVolume[2],viewVolume[3],
					viewVolume[4],viewVolume[5]);
			break;
		case PERSPECTIVE:
		{
			//!	Deprecated since OpenGL 3.2
			// glFrustum(viewVolume[0], viewVolume[1], viewVolume[2], viewVolume[3], viewVolume[4], viewVolume[5]);
			
			CGenericMatrix<float, 4> frustum;
			getTransposeFrustum(frustum);
			glMultMatrixf(frustum.matrix());
		}
	}

	glMatrixMode(GL_MODELVIEW);

	//glConfigureEngine(this);

	CATCH_GL_ERROR
}



