/***************************************************************************/
/*                                                                         */
/*  RaptorDisplayFilter.cpp                                                */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif
#if !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)
    #include "RaptorDisplayFilter.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
	#include "RaptorDisplayConfig.h"
#endif
#if !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)
	#include "RaptorDisplay.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "Raptor.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "RaptorErrorManager.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
    #include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
	#include "GLHierarchy/FragmentProgram.h"
#endif


RAPTOR_NAMESPACE_BEGIN

GL_COORD_VERTEX	*CRaptorDisplayFilter::s_attributes = NULL;
CShader	* CRaptorDisplayFilter::s_pIdentity = NULL;
#if defined(GL_COMPATIBILITY_profile)
	RAPTOR_HANDLE	CRaptorDisplayFilter::s_drawBuffer = RAPTOR_HANDLE(0,(void*)0);
#endif

RAPTOR_NAMESPACE_END

RAPTOR_NAMESPACE

CShader	*CRaptorDisplayFilter::getIdentityShader(void) const
{ return s_pIdentity; }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorDisplayFilter::CRaptorDisplayFilter():
	m_fModel(RENDER_BUFFER),
    colorExternalSource(NULL),colorInternalSource(NULL),
    depthExternalSource(NULL),depthInternalSource(NULL),
	colorInput(NULL),depthInput(NULL),
	colorOutput(NULL),depthOutput(NULL),
	m_pRenderTextures(NULL),m_pOutputTextures(NULL),
    m_bEnabled(true),m_bBufferedOutputEnabled(true),
	m_fXfactor(1.0f),m_fYfactor(1.0f),
	m_pPreviousFilter(NULL)
{
}

CRaptorDisplayFilter::operator RAPTOR_HANDLE() const
{
	RAPTOR_HANDLE filterHandle(CShader::CShaderClassID::GetClassId().ID(),(void*)this);
    return filterHandle;
}

CRaptorDisplayFilter::~CRaptorDisplayFilter()
{
    glDestroyFilter();   
}

bool CRaptorDisplayFilter::setSizeFactors(float xFactor, float yFactor)
{
    if ((xFactor < 0.0f) || (yFactor < 0.0f))
        return false;

    m_fXfactor = xFactor;
    m_fYfactor = yFactor;

    return true;
}

void CRaptorDisplayFilter::setPreviousFilter(CRaptorDisplayFilter *pFilter)
{
	m_pPreviousFilter = pFilter;
	if (pFilter != NULL)
	{
		if (m_fModel == RENDER_BUFFER)
		{
			setColorInput(pFilter->glCreateColorOutput());
			setDepthInput(pFilter->glCreateDepthOutput());
		}
		else if (m_fModel == RENDER_TEXTURE)
		{
			setColorSource(pFilter->glCreateColorSource());
			setDepthSource(pFilter->glCreateDepthSource());
		}
	}
}

void CRaptorDisplayFilter::glRender(void)
{
	if (NULL == getColorInput())
		return;

	if (m_bEnabled)
		glRenderFilter();

	if ((colorInternalSource != NULL) && m_bBufferedOutputEnabled)
    {
        CRaptorDisplay *output = ((CRaptorDisplay*)colorInternalSource);
        RAPTOR_HANDLE noDevice;
		output->glvkBindDisplay(noDevice);
    }

	if (m_bEnabled)
		glRenderFilterOutput();
	else
    {
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

		pExtensions->glActiveTextureARB(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);
        pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
		getColorInput()->glvkRender();
#if defined(GL_COMPATIBILITY_profile)
		glDrawFilter();
#else
		s_pIdentity->glRender();
		glDrawFilter();
		s_pIdentity->glStop();
#endif
    }

	if ((colorInternalSource != NULL) && m_bBufferedOutputEnabled)
    {
        CRaptorDisplay *output = ((CRaptorDisplay*)colorInternalSource);
		output->glvkUnBindDisplay();
	}

	if (colorExternalSource != NULL)
	{
        CRaptorDisplay *input = ((CRaptorDisplay*)colorExternalSource);
		input->glvkUnBindDisplay();
    }

	CATCH_GL_ERROR
}

#if defined(GL_COMPATIBILITY_profile)
	void CRaptorDisplayFilter::glDrawFilter(void)
	{
		if (s_drawBuffer.handle() > 0)
			glCallList(s_drawBuffer.handle());
	}
#else
	void CRaptorDisplayFilter::glDrawFilter(void) const
	{
		if (NULL != s_attributes)
		{
			const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

			pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::POSITION);
			pExtensions->glVertexAttribPointerARB(CProgramParameters::POSITION, 4, GL_FLOAT, false, 0, s_attributes);

			glDrawArrays(GL_POINTS, 0, 1);

			pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::POSITION);
		}
	}
#endif


void CRaptorDisplayFilter::glDestroyFilter(void)
{
#if defined(GL_COMPATIBILITY_profile)
    if (s_drawBuffer.handle() > 0)
    {
        glDeleteLists(s_drawBuffer.handle(),1);
        drawBuffer.handle(0);
    }
#endif

    if (colorInternalSource != NULL)
    {
        Raptor::glDestroyDisplay((CRaptorDisplay*)colorInternalSource);
        colorInternalSource = NULL;
    }

	if (depthInternalSource != NULL)
	{
		Raptor::glDestroyDisplay((CRaptorDisplay*)depthInternalSource);
		depthInternalSource = NULL;
	}

	if (colorInput != NULL)
	{
		colorInput->releaseReference();
		colorInput = NULL;
	}

	if (depthInput != NULL)
	{
		depthInput->releaseReference();
		depthInput = NULL;
	}

	if (m_pRenderTextures != NULL)
	{
		delete m_pRenderTextures;
		m_pRenderTextures = NULL;
	}
	
	if (m_pOutputTextures != NULL)
	{
		delete m_pOutputTextures;
		m_pOutputTextures = NULL;
	}

    CATCH_GL_ERROR
}

bool CRaptorDisplayFilter::glInitFilter(void)
{
	if (colorExternalSource != NULL)
	{
		if (colorInput == NULL)
		{
			if (colorExternalSource->getKind() != ITextureGenerator::BUFFERED)
				return false;
		}
		else if (m_fModel == RENDER_TEXTURE)
		{
			colorInput->releaseReference();
			colorInput = NULL;
		}
	}
	else
	{
		if (getColorInput() == NULL)
			return false;
	}

#if defined(GL_COMPATIBILITY_profile)
	if (0 == s_drawBuffer.handle())
	{
		s_drawBuffer.handle(glGenLists(1));
		glNewList(s_drawBuffer.handle(), GL_COMPILE);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex4f(-1.0, -1.0, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex4f(1.0, -1.0, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex4f(1.0, 1.0, -1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex4f(-1.0, 1.0, -1.0f, 1.0f);
		glEnd();
		glEndList();
	}
#else
	if (NULL == s_pIdentity)
	{
		s_pIdentity = new CShader("HDR_IDENTITY");
		CVertexProgram *vp = s_pIdentity->glGetVertexProgram("EMPTY_PROGRAM");
		CGeometryProgram *gp = s_pIdentity->glGetGeometryProgram("FULL_SCREEN_GEO_PROGRAM");
		CFragmentProgram *fp = s_pIdentity->glGetFragmentProgram("DIFFUSE_PROGRAM");

		CProgramParameters identityParams;
		identityParams.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
		fp->setProgramParameters(identityParams);
		if (!s_pIdentity->glCompileShader())
			return false;
	}
#endif

	{
		if (NULL == s_attributes)
		{
			CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
			bool lock = pAllocator->isMemoryLocked();
			if (lock)
				pAllocator->glvkLockMemory(false);

			size_t s = sizeof(GL_COORD_VERTEX) / sizeof(float);
			s_attributes = (GL_COORD_VERTEX*)(pAllocator->allocateVertices(s));

			s_attributes = (GL_COORD_VERTEX*)pAllocator->glvkMapPointer((float*)s_attributes);
			*s_attributes = GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 0.0f);
			s_attributes = (GL_COORD_VERTEX*)pAllocator->glvkUnMapPointer((float*)s_attributes);

			if (lock)
				pAllocator->glvkLockMemory(true);
		}
	}
	
	if (m_fModel == RENDER_BUFFER)
	{
		if (m_pRenderTextures != NULL)
			delete m_pRenderTextures;
		m_pRenderTextures = new CTextureSet("Filter Render Textures");
	}

    CATCH_GL_ERROR

    return true;
}

CTextureObject* CRaptorDisplayFilter::getColorInput(void)
{
	if ((m_pPreviousFilter != NULL) && (m_fModel == RENDER_BUFFER))
		return m_pPreviousFilter->getColorOutput();
	else
		return colorInput;
}

CTextureObject* CRaptorDisplayFilter::getColorOutput(void)
{
	if (m_bEnabled)
		return colorOutput;
	else if (m_pPreviousFilter != NULL)
		return m_pPreviousFilter->getColorOutput();
	else
		return colorInput;
}

void CRaptorDisplayFilter::setColorInput(CTextureObject* pInput)
{
	if (colorInput != NULL)
		colorInput->releaseReference();
    colorInput = pInput;
	if (colorInput != NULL)
		colorInput->addReference();
}

void CRaptorDisplayFilter::setDepthInput(CTextureObject* pInput)
{
	if (depthInput != NULL)
		depthInput->releaseReference();
    depthInput = pInput;
	if (depthInput != NULL)
		depthInput->addReference();
}



void CRaptorDisplayFilter::setColorSource(ITextureGenerator* pSource)
{
    colorExternalSource = pSource;
}

void CRaptorDisplayFilter::setDepthSource(ITextureGenerator* pSource)
{
    depthExternalSource = pSource;
}

ITextureGenerator*  CRaptorDisplayFilter::glCreateColorSource(void)
{
	if (colorInternalSource != NULL)
        return colorInternalSource;
    if (colorInput == NULL)
        return NULL;

    CRaptorDisplayConfig state;
    state.width = m_fXfactor * colorInput->getWidth();
    state.height = m_fYfactor * colorInput->getHeight();
    state.display_mode = CGL_RGBA;
	state.bind_to_texture = true;
	state.acceleration = CRaptorDisplayConfig::HARDWARE;
	if (m_fModel == RENDER_BUFFER)
		state.renderer = CRaptorDisplayConfig::RENDER_BUFFER;
	else if (m_fModel == CRaptorDisplayFilter::RENDER_TEXTURE)
		state.renderer = CRaptorDisplayConfig::PIXEL_BUFFER;

    CRaptorDisplay* pDisplay = Raptor::glCreateDisplay(state);
	IRenderingProperties &rp = pDisplay->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setCullFace(IRenderingProperties::DISABLE);
	rp.setDepthTest(IRenderingProperties::DISABLE);
    rp.clear(CGL_NULL);
    pDisplay->setViewPoint(NULL);

	CATCH_GL_ERROR

    return (colorInternalSource = pDisplay);
}

ITextureGenerator*  CRaptorDisplayFilter::glCreateDepthSource(void)
{
    return depthExternalSource;

    CATCH_GL_ERROR
}

CTextureObject*  CRaptorDisplayFilter::glCreateColorOutput(void)
{
	if (colorOutput != NULL)
		return colorOutput;
	if (getColorInput() == NULL)
        return NULL;

	CRaptorDisplayConfig state;
	state.width = m_fXfactor * getColorInput()->getWidth();
	state.height = m_fYfactor * getColorInput()->getHeight();
	state.acceleration = CRaptorDisplayConfig::HARDWARE;
	state.display_mode = CGL_RGBA;
	state.bind_to_texture = true;
	if (m_fModel == RENDER_BUFFER)
		state.renderer = CRaptorDisplayConfig::RENDER_BUFFER;
	else if (m_fModel == CRaptorDisplayFilter::RENDER_TEXTURE)
		state.renderer = CRaptorDisplayConfig::PIXEL_BUFFER;

	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();
	colorOutput = filterFactory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
		                                        CTextureObject::CGL_OPAQUE,
			                                    ITextureObject::CGL_UNFILTERED);
	filterFactory.glResizeTexture(colorOutput,state.width,state.height);
	colorOutput->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);

	if (m_pOutputTextures != NULL)
		delete m_pOutputTextures;
	m_pOutputTextures = new CTextureSet("Filter Output Render Textures");
	m_pOutputTextures->addTexture(colorOutput);
	colorOutput->releaseReference();

	CRaptorDisplay *renderBuffer = NULL;
    renderBuffer = Raptor::glCreateDisplay(state);
	IRenderingProperties &rp = renderBuffer->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setCullFace(IRenderingProperties::DISABLE);
	rp.setDepthTest(IRenderingProperties::DISABLE);
	rp.setLighting(IRenderingProperties::DISABLE);
    rp.clear(CGL_NULL);
    renderBuffer->setViewPoint(NULL);

	if (m_fModel == RENDER_BUFFER)
		renderBuffer->glvkBindDisplay(*m_pOutputTextures);

    RAPTOR_HANDLE noDevice;
	renderBuffer->glvkBindDisplay(noDevice);
	renderBuffer->glvkUnBindDisplay();

	colorInternalSource = renderBuffer;
	CATCH_GL_ERROR

	return colorOutput;
}

CTextureObject*  CRaptorDisplayFilter::glCreateDepthOutput(void)
{
	CATCH_GL_ERROR

	//return depthOutput;
	return depthInput;
}

