// ViewPoint.cpp: implementation of the CViewPoint class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_VULKANVIEWPOINT_H__08D29395_9883_45F8_AE51_5174BD6BC19B__INCLUDED_)
	#include "VulkanViewPoint.h"
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
CVulkanViewPoint::CVulkanViewPoint(const std::string& name)
	:IViewPoint(name)
{
	modelview.Ident();
	IDENT_MATRIX(transform.modelview);
	IDENT_MATRIX(transform.projection);
}

CVulkanViewPoint::~CVulkanViewPoint()
{
}


void CVulkanViewPoint::glvkRender(void)
{
	modelview.Ident();
	modelview.Scale(Scale.X(), Scale.Y(), Scale.Z());

	modelview.Rotate(m_lfGamma, 0.0f, 0.0f, 1.0f);
	modelview.Rotate(-m_lfBeta, 1.0f, 0.0f, 0.0f);
	modelview.Rotate(-m_lfAlpha, 0.0f, 1.0f, 0.0f);

	modelview.Translate(-Origin.X(), -Origin.Y(), -Origin.Z());

	C3DEngine::Generic_to_MATRIX(transform.modelview, modelview.Transpose());

	CATCH_GL_ERROR
}

void CVulkanViewPoint::glvkRenderViewPointModel(void)
{
	CGenericMatrix<float, 4> frustum;
	getFrustum(frustum);

	//	Need this additional transform to map depth [-1.0 .. 1.0] to [0.0 .. 1.0] range.
	CGenericMatrix<float, 4> view;
	view.Ident();
	view[10] = 0.5f;
	view[11] = 0.5f;
	view *= frustum;

	C3DEngine::Generic_to_MATRIX(transform.projection, view.Transpose());

	CATCH_GL_ERROR
}



