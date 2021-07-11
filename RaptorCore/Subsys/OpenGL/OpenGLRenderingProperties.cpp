// RenderingProperties.cpp: implementation of the CRenderingProperties class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_OPENGLRENDERINGPROPERTIES_H__1F0F1E67_FC84_4772_A6EE_923BD81F91D3__INCLUDED_)
	#include "OpenGLRenderingProperties.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COpenGLRenderingProperties::COpenGLRenderingProperties()
	:IRenderingProperties(),
	globalTexturing(IGNORE_PROPERTY), globalLighting(IGNORE_PROPERTY), 
	globalDepth(IGNORE_PROPERTY), globalCull(IGNORE_PROPERTY),
	globalBlend(IGNORE_PROPERTY), globalStencil(IGNORE_PROPERTY),
	globalMultisample(IGNORE_PROPERTY), globalClampFloat(IGNORE_PROPERTY),
	globalWireframe(IGNORE_PROPERTY)
{
}

COpenGLRenderingProperties::COpenGLRenderingProperties(const IRenderingProperties &properties)
	: IRenderingProperties(properties),
	globalTexturing(IGNORE_PROPERTY), globalLighting(IGNORE_PROPERTY),
	globalDepth(IGNORE_PROPERTY), globalCull(IGNORE_PROPERTY),
	globalBlend(IGNORE_PROPERTY), globalStencil(IGNORE_PROPERTY),
	globalMultisample(IGNORE_PROPERTY), globalClampFloat(IGNORE_PROPERTY),
	globalWireframe(IGNORE_PROPERTY)
{
}

COpenGLRenderingProperties::~COpenGLRenderingProperties()
{
}

void COpenGLRenderingProperties::glPushProperties(void)
{
	CRaptorInstance& instance = CRaptorInstance::GetInstance();
    if ((instance.getCurrentRenderingProperties() != this) && (m_pPrevious == NULL))
    {
        m_pPrevious = instance.getCurrentRenderingProperties();
		instance.setCurrentRenderingProperties(this);

#if defined(GL_ARB_multisample)
		glPushAttrib(GL_ENABLE_BIT|GL_MULTISAMPLE_BIT_ARB);
#else
        glPushAttrib(GL_ENABLE_BIT);
#endif
		IRenderingProperties* pGlobal = instance.getGlobalRenderingProperties();
		globalTexturing = pGlobal->getTexturing();
		globalLighting = pGlobal->getLighting();
		globalDepth = pGlobal->getDepthTest();
		globalCull = pGlobal->getCullFace();
		globalBlend = pGlobal->getBlending();
		globalStencil = pGlobal->getStencilTest();
		globalMultisample = pGlobal->getMultisampling();
		globalClampFloat = pGlobal->getFloatClamping();
		globalWireframe = pGlobal->getWireframe();

        glRender();

        CATCH_GL_ERROR
    }
	else
	{
		// TODO: DBUG INFO
	}
}

void COpenGLRenderingProperties::glGrabProperties(void)
{
	GLboolean value = GL_FALSE;

	glGetBooleanv(GL_TEXTURE_2D ,&value);
	if (value)
		enableTexturing;
	else
		disableTexturing;

	glGetBooleanv(GL_CULL_FACE ,&value);
	if (value)
		enableCullFace;
	else
		disableCullFace;

	GLint mode[2];
	glGetIntegerv(GL_POLYGON_MODE, mode);
	if ((mode[0] == GL_FILL) || (mode[1] == GL_FILL))
		disableWireframe;
	else
		enableWireframe;

	glGetBooleanv(GL_LIGHTING ,&value);
	if (value)
		enableLighting;
	else
		disableLighting;

	glGetBooleanv(GL_DEPTH_TEST ,&value);
	if (value)
		enableDepthTest;
	else
		disableDepthTest;

	glGetBooleanv(GL_BLEND ,&value);
	if (value)
		enableBlending;
	else
		disableBlending;

	glGetBooleanv(GL_STENCIL_TEST,&value);
	if (value)
		enableStencilTest;
	else
		disableStencilTest;

#if defined(GL_ARB_multisample)
	glGetBooleanv(GL_MULTISAMPLE_ARB, &value);
	if (value)
		enableMultisampling;
	else
		disableMultisampling;
#endif

#if defined(GL_ARB_color_buffer_float) || defined(WGL_ATI_pixel_format_float)
	glGetBooleanv(GL_CLAMP_READ_COLOR_ARB, &value);
	if (value)
		enableFloatClamping;
	else
		disableFloatClamping;
#endif
}

void COpenGLRenderingProperties::glPopProperties(void)
{
	CRaptorInstance& instance = CRaptorInstance::GetInstance();
    if (instance.getCurrentRenderingProperties() == this)
    {
        instance.setCurrentRenderingProperties(m_pPrevious);
        
        glPopAttrib();

		IRenderingProperties* pGlobalProperties = instance.getGlobalRenderingProperties();
		pGlobalProperties->setTexturing(globalTexturing);
		pGlobalProperties->setLighting(globalLighting);
		pGlobalProperties->setDepthTest(globalDepth);
		pGlobalProperties->setCullFace(globalCull);
		pGlobalProperties->setBlending(globalBlend);
		pGlobalProperties->setStencilTest(globalStencil);
		pGlobalProperties->setMultisampling(globalMultisample);
		pGlobalProperties->setFloatClamping(globalClampFloat);
		pGlobalProperties->setWireframe(globalWireframe);

		if ((m_bWireframe == ENABLE) && (globalWireframe != ENABLE) && (m_pPrevious->getWireframe() != ENABLE))
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else if ((m_bWireframe == DISABLE) && (globalWireframe != DISABLE) && (m_pPrevious->getWireframe() != DISABLE))
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// TODO: restore float clamping

		m_pPrevious = NULL;
		CATCH_GL_ERROR
    }
}

void COpenGLRenderingProperties::glRender(void)
{
	CRaptorInstance& instance = CRaptorInstance::GetInstance();

    if (m_clear != 0)
    {
        GLuint clearAttrib = 0;

        if ((m_clear & CGL_RGB) == CGL_RGB)
	        clearAttrib |= GL_COLOR_BUFFER_BIT;
        if ((m_clear & CGL_RGBA) == CGL_RGBA)
	        clearAttrib |= GL_COLOR_BUFFER_BIT;
        if ((m_clear & CGL_DEPTH_32) != CGL_NULL)
	        clearAttrib |= GL_DEPTH_BUFFER_BIT;
        if ((m_clear & CGL_STENCIL) == CGL_STENCIL)
	        clearAttrib |= GL_STENCIL_BUFFER_BIT;
        if ((m_clear & CGL_ACCUM) == CGL_ACCUM)
	        clearAttrib |= GL_ACCUM_BUFFER_BIT;

        if (clearAttrib != 0)
	        glClear(clearAttrib);
    }

	IRenderingProperties* pGlobalProperties = instance.getGlobalRenderingProperties();

	if (m_bTexturing == ENABLE)
    {
		glEnable(GL_TEXTURE_2D);
		pGlobalProperties->setTexturing(m_bTexturing);
    }
	else if (m_bTexturing == DISABLE)
    {
		glDisable(GL_TEXTURE_2D);
		pGlobalProperties->setTexturing(m_bTexturing);
    }

	if (m_bLighting == ENABLE)
    {
		glEnable(GL_LIGHTING);
		pGlobalProperties->setLighting(m_bLighting);
    }
	else if (m_bLighting == DISABLE)
    {
		glDisable(GL_LIGHTING);
		pGlobalProperties->setLighting(m_bLighting);
    }

	if (m_bWireframe == ENABLE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		pGlobalProperties->setWireframe(m_bWireframe);
	}
	else if (m_bWireframe == DISABLE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		pGlobalProperties->setWireframe(m_bWireframe);
	}

	if (m_bCullFace == ENABLE)
    {
		glEnable(GL_CULL_FACE);
		pGlobalProperties->setCullFace(m_bCullFace);
    }
	else if (m_bCullFace == DISABLE)
    {
		glDisable(GL_CULL_FACE);
		pGlobalProperties->setCullFace(m_bCullFace);
    }

	if (m_bDepthTest == ENABLE)
    {
		glEnable(GL_DEPTH_TEST);
		pGlobalProperties->setDepthTest(m_bDepthTest);
    }
	else if (m_bDepthTest == DISABLE)
    {
		glDisable(GL_DEPTH_TEST);
		pGlobalProperties->setDepthTest(m_bDepthTest);
    }

    if (m_bBlend == ENABLE)
    {
        glEnable(GL_BLEND);
		pGlobalProperties->setBlending(m_bBlend);
    }
    else if (m_bBlend == DISABLE)
    {
        glDisable(GL_BLEND);
		pGlobalProperties->setBlending(m_bBlend);
    }

    if (m_bStencilTest == ENABLE)
    {
        glEnable(GL_STENCIL_TEST);
		pGlobalProperties->setStencilTest(m_bStencilTest);
    }
    else if (m_bStencilTest == DISABLE)
    {
        glDisable(GL_STENCIL_TEST);
		pGlobalProperties->setStencilTest(m_bStencilTest);
    }

#if defined(GL_ARB_multisample)
	if (m_bMultisample == ENABLE)
    {
        glEnable(GL_MULTISAMPLE_ARB);
		pGlobalProperties->setMultisampling(m_bMultisample);
    }
    else if (m_bMultisample == DISABLE)
    {
        glDisable(GL_MULTISAMPLE_ARB);
		pGlobalProperties->setMultisampling(m_bMultisample);
    }
#endif
#if defined(GL_ARB_color_buffer_float) || defined(WGL_ATI_pixel_format_float)
	if (m_bClampFloats == ENABLE)
	{
		const CRaptorGLExtensions * const pExtensions = Raptor::glGetExtensions();
		if ((pExtensions != NULL) && (pExtensions->glClampColorARB != NULL))
		{
			pExtensions->glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB,GL_TRUE);
			pExtensions->glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB,GL_TRUE);
			pExtensions->glClampColorARB(GL_CLAMP_READ_COLOR_ARB,GL_TRUE);
		}
		pGlobalProperties->setFloatClamping(m_bClampFloats);
	}
	else if (m_bClampFloats == DISABLE)
	{
		const CRaptorGLExtensions * const pExtensions = Raptor::glGetExtensions();
		if ((pExtensions != NULL) && (pExtensions->glClampColorARB != NULL))
		{
			pExtensions->glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB,GL_FALSE);
			pExtensions->glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB,GL_FALSE);
			pExtensions->glClampColorARB(GL_CLAMP_READ_COLOR_ARB,GL_FALSE);
		}
		pGlobalProperties->setFloatClamping(m_bClampFloats);
	}
#endif
}


