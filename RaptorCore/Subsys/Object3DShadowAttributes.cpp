// Object3DShadowAttributes.cpp: implementation of the CObject3DShadowAttributes class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_OBJECT3DSHADOWATTRIBUTES_H__4FAFE331_A695_471C_AD48_0F541948AAAD__INCLUDED_)
    #include "Subsys/Object3DShadowAttributes.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObject3DShadowAttributes::CObject3DShadowAttributes()
{
    visibilityCount = 0;
    visibilityQuery = 0;
    queryIssued = false;

    m_pLight = NULL;
    m_pObject = NULL;

    const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#ifdef GL_ARB_imaging
	glBlendColorARB = pExtensions->glBlendColorARB;
#endif
#ifdef GL_EXT_stencil_two_side
	glActiveStencilFaceEXT = pExtensions->glActiveStencilFaceEXT;
#endif

#if defined(GL_ARB_occlusion_query)
	if (pExtensions->glGenQueriesARB != NULL)
		pExtensions->glGenQueriesARB(1,&visibilityQuery);

	CATCH_GL_ERROR
#endif
}

CObject3DShadowAttributes::~CObject3DShadowAttributes()
{

}


