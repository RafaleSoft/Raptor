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

RAPTOR_NAMESPACE_BEGIN

COpenGLRenderingProperties    COpenGLRenderingProperties::m_globalProperties;

RAPTOR_NAMESPACE_END

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COpenGLRenderingProperties::COpenGLRenderingProperties()
	:CRenderingProperties()
{

}

COpenGLRenderingProperties::COpenGLRenderingProperties(const CRenderingProperties &properties)
	: CRenderingProperties(properties)
{
}

COpenGLRenderingProperties::~COpenGLRenderingProperties()
{
}

CRenderingProperties::PROPERTY_SETTING COpenGLRenderingProperties::getCurrentLighting(void) const
{
	return m_globalProperties.m_bLighting;
}
CRenderingProperties::PROPERTY_SETTING COpenGLRenderingProperties::getCurrentTexturing(void) const
{
	return m_globalProperties.m_bTexturing;
}

void COpenGLRenderingProperties::glPushProperties(void)
{
    if ((m_pCurrent != this) && (m_pPrevious == NULL))
    {
        m_pPrevious = m_pCurrent;

#if defined(GL_ARB_multisample)
		glPushAttrib(GL_ENABLE_BIT|GL_MULTISAMPLE_BIT_ARB);
#else
        glPushAttrib(GL_ENABLE_BIT);
#endif

        glRender();

        CATCH_GL_ERROR
    }
}

void COpenGLRenderingProperties::glPopProperties(void)
{
    if (m_pCurrent == this)
    {
        m_pCurrent = m_pPrevious;
        m_pPrevious = NULL;

        glPopAttrib();

        GLboolean value;

        if (m_pCurrent != NULL)
        {
			if (m_pCurrent->getTexturing() != IGNORE_PROPERTY)
				m_globalProperties.setTexturing(m_pCurrent->getTexturing());
            else
            {
                glGetBooleanv(GL_TEXTURE_2D,&value);
				m_globalProperties.setTexturing(value ? ENABLE : DISABLE);
            }

			if (m_pCurrent->getLighting() != IGNORE_PROPERTY)
				m_globalProperties.setLighting(m_pCurrent->getLighting());
            else
            {
                glGetBooleanv(GL_LIGHTING,&value);
				m_globalProperties.setLighting(value ? ENABLE : DISABLE);
            }

			if (m_pCurrent->getCullFace() != IGNORE_PROPERTY)
				m_globalProperties.setCullFace(m_pCurrent->getCullFace());
            else
            {
                glGetBooleanv(GL_CULL_FACE,&value);
				m_globalProperties.setCullFace(value ? ENABLE : DISABLE);
            }

			if (m_pCurrent->getDepthTest() != IGNORE_PROPERTY)
				m_globalProperties.setDepthTest(m_pCurrent->getDepthTest());
            else
            {
                glGetBooleanv(GL_DEPTH_TEST,&value);
				m_globalProperties.setDepthTest(value ? ENABLE : DISABLE);
            }

			if (m_pCurrent->getBlending() != IGNORE_PROPERTY)
				m_globalProperties.setBlending(m_pCurrent->getBlending());
            else
            {
                glGetBooleanv(GL_BLEND,&value);
				m_globalProperties.setBlending(value ? ENABLE : DISABLE);
            }

			if (m_pCurrent->getStencilTest() != IGNORE_PROPERTY)
				m_globalProperties.setStencilTest(m_pCurrent->getStencilTest());
            else
            {
                glGetBooleanv(GL_STENCIL_TEST,&value);
				m_globalProperties.setStencilTest(value ? ENABLE : DISABLE);
            }

#if defined(GL_ARB_multisample)
			if (m_pCurrent->getMultisampling() != IGNORE_PROPERTY)
				m_globalProperties.setMultisampling(m_pCurrent->getMultisampling());
            else
            {
				glGetBooleanv(GL_MULTISAMPLE_ARB,&value);
				m_globalProperties.setMultisampling(value ? ENABLE : DISABLE);
            }
#endif
#if defined(GL_ARB_color_buffer_float) || defined(WGL_ATI_pixel_format_float)
			if (m_pCurrent->getFloatClamping() != IGNORE_PROPERTY)
				m_globalProperties.setFloatClamping(m_pCurrent->getFloatClamping());
			else
			{
				glGetBooleanv(GL_CLAMP_READ_COLOR_ARB,&value);
				m_globalProperties.setFloatClamping(value ? ENABLE : DISABLE);
			}
#endif
        }
        
        CATCH_GL_ERROR
    }
}

void COpenGLRenderingProperties::glRender(void)
{
    m_pCurrent = this;

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

	/*
	 * For debugging purposes: grab all current status values
    GLboolean val,val2,val3,val4,val5,val6;
    glGetBooleanv(GL_TEXTURE_2D ,&val);
    glGetBooleanv(GL_CULL_FACE ,&val2);
    glGetBooleanv(GL_LIGHTING ,&val3);
    glGetBooleanv(GL_DEPTH_TEST ,&val4);
    glGetBooleanv(GL_BLEND ,&val5);
    glGetBooleanv(GL_STENCIL_TEST,&val5);
	*/

	if (m_bTexturing == ENABLE)
    {
		glEnable(GL_TEXTURE_2D);
		m_globalProperties.setTexturing(m_bTexturing);
    }
	else if (m_bTexturing == DISABLE)
    {
		glDisable(GL_TEXTURE_2D);
		m_globalProperties.setTexturing(m_bTexturing);
    }

	if (m_bLighting == ENABLE)
    {
		glEnable(GL_LIGHTING);
		m_globalProperties.setLighting(m_bLighting);
    }
	else if (m_bLighting == DISABLE)
    {
		glDisable(GL_LIGHTING);
		m_globalProperties.setLighting(m_bLighting);
    }

	if (m_bWireframe == ENABLE)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	else if (m_bWireframe == DISABLE)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	if (m_bCullFace == ENABLE)
    {
		glEnable(GL_CULL_FACE);
		m_globalProperties.setCullFace(m_bCullFace);
    }
	else if (m_bCullFace == DISABLE)
    {
		glDisable(GL_CULL_FACE);
		m_globalProperties.setCullFace(m_bCullFace);
    }

	if (m_bDepthTest == ENABLE)
    {
		glEnable(GL_DEPTH_TEST);
		m_globalProperties.setDepthTest(m_bDepthTest);
    }
	else if (m_bDepthTest == DISABLE)
    {
		glDisable(GL_DEPTH_TEST);
		m_globalProperties.setDepthTest(m_bDepthTest);
    }

    if (m_bBlend == ENABLE)
    {
        glEnable(GL_BLEND);
		m_globalProperties.setBlending(m_bBlend);
    }
    else if (m_bBlend == DISABLE)
    {
        glDisable(GL_BLEND);
		m_globalProperties.setBlending(m_bBlend);
    }

    if (m_bStencilTest == ENABLE)
    {
        glEnable(GL_STENCIL_TEST);
		m_globalProperties.setStencilTest(m_bStencilTest);
    }
    else if (m_bStencilTest == DISABLE)
    {
        glDisable(GL_STENCIL_TEST);
		m_globalProperties.setStencilTest(m_bStencilTest);
    }

#if defined(GL_ARB_multisample)
	if (m_bMultisample == ENABLE)
    {
        glEnable(GL_MULTISAMPLE_ARB);
		m_globalProperties.setMultisampling(m_bMultisample);
    }
    else if (m_bMultisample == DISABLE)
    {
        glDisable(GL_MULTISAMPLE_ARB);
		m_globalProperties.setMultisampling(m_bMultisample);
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
		m_globalProperties.setFloatClamping(m_bClampFloats);
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
		m_globalProperties.setFloatClamping(m_bClampFloats);
	}
#endif
}


