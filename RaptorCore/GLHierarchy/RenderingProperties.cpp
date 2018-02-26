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

#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "RenderingProperties.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
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

CRenderingProperties*   CRenderingProperties::m_pCurrent = NULL;
CRenderingProperties    CRenderingProperties::m_globalProperties;

RAPTOR_NAMESPACE_END

RAPTOR_NAMESPACE

CRenderingProperties::PROPERTY_SETTING CRenderingProperties::getCurrentLighting(void) const
{
    return m_globalProperties.m_bLighting;
}
CRenderingProperties::PROPERTY_SETTING CRenderingProperties::getCurrentTexturing(void) const
{
    return m_globalProperties.m_bTexturing;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRenderingProperties::CRenderingProperties()
	:m_bTexturing(IGNORE_PROPERTY/*DISABLE*/),m_bLighting(IGNORE_PROPERTY/*DISABLE*/),
	m_bWireframe(IGNORE_PROPERTY/*DISABLE*/),m_bCullFace(ENABLE),
	m_bDepthTest(ENABLE),m_bBlend(IGNORE_PROPERTY),m_bStencilTest(IGNORE_PROPERTY),
	m_bMultisample(IGNORE_PROPERTY/*DISABLE*/),m_bClampFloats(IGNORE_PROPERTY),
	m_clear(0),m_pPrevious(NULL)
{

}

CRenderingProperties::CRenderingProperties(const CRenderingProperties &properties)
{
    if (&properties != this)
    {
        m_clear = properties.m_clear;
        m_bWireframe = properties.m_bWireframe;
        m_bTexturing = properties.m_bTexturing;
        m_bLighting = properties.m_bLighting;
        m_bDepthTest = properties.m_bDepthTest;
        m_bCullFace = properties.m_bCullFace;
        m_bBlend = properties.m_bBlend;
        m_bStencilTest = properties.m_bStencilTest;
		m_bMultisample = properties.m_bMultisample;
		m_bClampFloats = properties.m_bClampFloats;
        m_pPrevious = NULL;
    }
}

CRenderingProperties& CRenderingProperties::operator=(const CRenderingProperties &properties)
{
    if (&properties != this)
    {
        m_clear = properties.m_clear;
        m_bWireframe = properties.m_bWireframe;
        m_bTexturing = properties.m_bTexturing;
        m_bLighting = properties.m_bLighting;
        m_bDepthTest = properties.m_bDepthTest;
        m_bCullFace = properties.m_bCullFace;
        m_bBlend = properties.m_bBlend;
        m_bStencilTest = properties.m_bStencilTest;
		m_bMultisample = properties.m_bMultisample;
		m_bClampFloats = properties.m_bClampFloats;
    }

    return *this;
}

CRenderingProperties::~CRenderingProperties()
{
}

CRenderingProperties *CRenderingProperties::GetCurrentProperties(void)
{ 
    return m_pCurrent; 
}

void CRenderingProperties::glPushProperties(void)
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

void CRenderingProperties::glPopProperties(void)
{
    if (m_pCurrent == this)
    {
        m_pCurrent = m_pPrevious;
        m_pPrevious = NULL;

        glPopAttrib();

        GLboolean value;

        if (m_pCurrent != NULL)
        {
            if (m_pCurrent->m_bTexturing != IGNORE_PROPERTY)
                m_globalProperties.m_bTexturing = m_pCurrent->m_bTexturing;
            else
            {
                glGetBooleanv(GL_TEXTURE_2D,&value);
                m_globalProperties.m_bTexturing = ( value ? ENABLE : DISABLE);
            }

            if (m_pCurrent->m_bLighting != IGNORE_PROPERTY)
                m_globalProperties.m_bLighting = m_pCurrent->m_bLighting;
            else
            {
                glGetBooleanv(GL_LIGHTING,&value);
                m_globalProperties.m_bLighting = ( value ? ENABLE : DISABLE);
            }

            if (m_pCurrent->m_bCullFace != IGNORE_PROPERTY)
                m_globalProperties.m_bCullFace = m_pCurrent->m_bCullFace;
            else
            {
                glGetBooleanv(GL_CULL_FACE,&value);
                m_globalProperties.m_bCullFace = ( value ? ENABLE : DISABLE);
            }

            if (m_pCurrent->m_bDepthTest != IGNORE_PROPERTY)
                m_globalProperties.m_bDepthTest = m_pCurrent->m_bDepthTest;
            else
            {
                glGetBooleanv(GL_DEPTH_TEST,&value);
                m_globalProperties.m_bDepthTest = ( value ? ENABLE : DISABLE);
            }

            if (m_pCurrent->m_bBlend != IGNORE_PROPERTY)
                m_globalProperties.m_bBlend = m_pCurrent->m_bBlend;
            else
            {
                glGetBooleanv(GL_BLEND,&value);
                m_globalProperties.m_bBlend = ( value ? ENABLE : DISABLE);
            }

            if (m_pCurrent->m_bStencilTest != IGNORE_PROPERTY)
                m_globalProperties.m_bStencilTest = m_pCurrent->m_bStencilTest;
            else
            {
                glGetBooleanv(GL_STENCIL_TEST,&value);
                m_globalProperties.m_bStencilTest = ( value ? ENABLE : DISABLE);
            }

#if defined(GL_ARB_multisample)
			if (m_pCurrent->m_bMultisample != IGNORE_PROPERTY)
                m_globalProperties.m_bMultisample = m_pCurrent->m_bMultisample;
            else
            {
				glGetBooleanv(GL_MULTISAMPLE_ARB,&value);
                m_globalProperties.m_bMultisample = ( value ? ENABLE : DISABLE);
            }
#endif
#if defined(GL_ARB_color_buffer_float) || defined(WGL_ATI_pixel_format_float)
			if (m_pCurrent->m_bClampFloats != IGNORE_PROPERTY)
				m_globalProperties.m_bClampFloats = m_pCurrent->m_bClampFloats;
			else
			{
				glGetBooleanv(GL_CLAMP_READ_COLOR_ARB,&value);
				m_globalProperties.m_bClampFloats = ( value ? ENABLE : DISABLE);
			}
#endif
        }
        
        CATCH_GL_ERROR
    }
}

void CRenderingProperties::glRender(void)
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
        m_globalProperties.m_bTexturing = m_bTexturing;
    }
	else if (m_bTexturing == DISABLE)
    {
		glDisable(GL_TEXTURE_2D);
        m_globalProperties.m_bTexturing = m_bTexturing;
    }

	if (m_bLighting == ENABLE)
    {
		glEnable(GL_LIGHTING);
        m_globalProperties.m_bLighting = m_bLighting;
    }
	else if (m_bLighting == DISABLE)
    {
		glDisable(GL_LIGHTING);
        m_globalProperties.m_bLighting = m_bLighting;
    }

	if (m_bWireframe == ENABLE)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	else if (m_bWireframe == DISABLE)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	if (m_bCullFace == ENABLE)
    {
		glEnable(GL_CULL_FACE);
        m_globalProperties.m_bCullFace = m_bCullFace;
    }
	else if (m_bCullFace == DISABLE)
    {
		glDisable(GL_CULL_FACE);
        m_globalProperties.m_bCullFace = m_bCullFace;
    }

	if (m_bDepthTest == ENABLE)
    {
		glEnable(GL_DEPTH_TEST);
        m_globalProperties.m_bDepthTest = m_bDepthTest;
    }
	else if (m_bDepthTest == DISABLE)
    {
		glDisable(GL_DEPTH_TEST);
        m_globalProperties.m_bDepthTest = m_bDepthTest;
    }

    if (m_bBlend == ENABLE)
    {
        glEnable(GL_BLEND);
        m_globalProperties.m_bBlend = m_bBlend;
    }
    else if (m_bBlend == DISABLE)
    {
        glDisable(GL_BLEND);
        m_globalProperties.m_bBlend = m_bBlend;
    }

    if (m_bStencilTest == ENABLE)
    {
        glEnable(GL_STENCIL_TEST);
        m_globalProperties.m_bStencilTest = m_bStencilTest;
    }
    else if (m_bStencilTest == DISABLE)
    {
        glDisable(GL_STENCIL_TEST);
        m_globalProperties.m_bStencilTest = m_bStencilTest;
    }

#if defined(GL_ARB_multisample)
	if (m_bMultisample == ENABLE)
    {
        glEnable(GL_MULTISAMPLE_ARB);
        m_globalProperties.m_bMultisample = m_bMultisample;
    }
    else if (m_bMultisample == DISABLE)
    {
        glDisable(GL_MULTISAMPLE_ARB);
        m_globalProperties.m_bMultisample = m_bMultisample;
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
		m_globalProperties.m_bClampFloats = m_bClampFloats;
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
		m_globalProperties.m_bClampFloats = m_bClampFloats;
	}
#endif
}

bool CRenderingProperties::exportObject(CRaptorIO& o)
{
    return false;
}

bool CRenderingProperties::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

    string data = io.getValueName();
    while (!data.empty())
    {
        //  TODO : converting an enum to int& is dangerous.
        //  Keep this only while testing IO
        if (data == "texturing")
            io >> (int&)m_bTexturing;
        else if (data == "lighting")
            io >> (int&)m_bLighting;
        else if (data == "wireframe")
            io >> (int&)m_bWireframe;
        else if (data == "cullface")
            io >> (int&)m_bCullFace;
        else if (data == "depthtest")
            io >> (int&)m_bCullFace;
        else if (data == "blending")
            io >> (int&)m_bBlend;
        else if (data == "stenciltest")
            io >> (int&)m_bStencilTest;
		else if (data == "multisampling")
            io >> (int&)m_bMultisample;
		else if (data == "clamping")
            io >> (int&)m_bClampFloats;

        data = io.getValueName();
    }
    io >> name;

    return true;
}

