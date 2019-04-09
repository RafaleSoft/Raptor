// RaptorDisplayFilter.cpp: implementation of the CRaptorDisplayFilter class.
//
//////////////////////////////////////////////////////////////////////

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
#if !defined(AFX_TEXTUREQUAD_H__1712AF34_6723_4E39_BC72_05ED6FA28418__INCLUDED_)
	#include "GLHierarchy/TextureQuad.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorDisplayFilter::CRaptorDisplayFilter():
	pFilter(NULL),m_fModel(RENDER_BUFFER),
    colorExternalSource(NULL),colorInternalSource(NULL),
    depthExternalSource(NULL),depthInternalSource(NULL),
	colorInput(NULL),depthInput(NULL),
	colorOutput(NULL),depthOutput(NULL),
	m_pRenderTextures(NULL),m_pOutputTextures(NULL),
    m_bEnabled(true),m_bBufferedOutputEnabled(true),
	m_fXfactor(1.0f),m_fYfactor(1.0f),
	m_pPreviousFilter(NULL), m_pDrawBuffer(NULL)
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
        glDrawBuffer();
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

void CRaptorDisplayFilter::glRenderFilter(void)
{
}

void CRaptorDisplayFilter::glRenderFilterOutput(void)
{
    if (pFilter)
    {
        if (pFilter->hasFragmentShader())
            pFilter->glGetFragmentShader()->glRender();
        if (pFilter->hasVertexShader())
            pFilter->glGetVertexShader()->glRender();
    }

    if (drawBuffer.handle > 0)
        glCallList(drawBuffer.handle);

    if (pFilter)
    {
        if (pFilter->hasFragmentShader())
            pFilter->glGetFragmentShader()->glStop();
        if (pFilter->hasVertexShader())
            pFilter->glGetVertexShader()->glStop();
    }
}

void CRaptorDisplayFilter::glDrawBuffer(void)
{
    if (drawBuffer.handle > 0)
        glCallList(drawBuffer.handle);
}

void CRaptorDisplayFilter::glDestroyFilter(void)
{
    if (drawBuffer.handle > 0)
    {
        glDeleteLists(drawBuffer.handle,1);
        drawBuffer.handle = 0;
    }

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

	drawBuffer.handle = glGenLists(1);
	glNewList(drawBuffer.handle,GL_COMPILE);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,0.0f);glVertex4f(-1.0,-1.0,-1.0f,1.0f);
			glTexCoord2f(1.0f,0.0f);glVertex4f(1.0,-1.0,-1.0f,1.0f);
			glTexCoord2f(1.0f,1.0f);glVertex4f(1.0,1.0,-1.0f,1.0f);
			glTexCoord2f(0.0f,1.0f);glVertex4f(-1.0,1.0,-1.0f,1.0f);
		glEnd();
	glEndList();
	/*
	m_pDrawBuffer = new CTextureQuad();
	m_pDrawBuffer->setQuadTexture(m_pImageSet->getTexture(0));
	m_pDrawBuffer->glSetQuadAttributes(GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 1.0f),
									   CColor::RGBA(1.0f, 1.0f, 1.0f, 1.0f),
									   GL_COORD_VERTEX(1.0f, 1.0f, 0.0f, 0.0f));
									   */
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

