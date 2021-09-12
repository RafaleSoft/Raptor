/***************************************************************************/
/*                                                                         */
/*  MBFilter.cpp                                                           */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
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

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
    #include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_MBFILTER_H__53A619DD_DBAB_4709_9EAD_72C5D6C401E9__INCLUDED_)
    #include "MBFilter.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_OPENGLPROGRAMSTAGE_H__0BCE3B42_6E10_4F50_BB27_1993345ADBCF__INCLUDED_)
	#include "GLHierarchy/OpenGLProgramStage.h"
#endif


// Specific texture generator to flip between buffer display of previous frame
class CAccumulator
{
public:
    CAccumulator():
		pCurrentDisplay(NULL),pPreviousDisplay(NULL),
		m_pCurrentColorAccum(NULL),m_pPreviousColorAccum(NULL){};

    virtual ~CAccumulator() {};

    void switchDisplay(void)
    {
        CRaptorDisplay *tmpDisplay = pPreviousDisplay;
        pPreviousDisplay = pCurrentDisplay;
        pCurrentDisplay = tmpDisplay;

        ITextureObject  *tmpColorAccum = m_pPreviousColorAccum;
        m_pPreviousColorAccum = m_pCurrentColorAccum;
        m_pCurrentColorAccum = tmpColorAccum;
    }

    //! Flip-flop color output
    CRaptorDisplay *pCurrentDisplay;
    ITextureObject  *m_pCurrentColorAccum;

    CRaptorDisplay *pPreviousDisplay;
    ITextureObject  *m_pPreviousColorAccum;
};



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMBFilter::CMBFilter() :
	m_pMotionBlurShader(NULL), m_pFinalShader(NULL),
	m_pAccumulator(NULL), m_pRenderTextures2(NULL),
	mbParams(GL_COORD_VERTEX(0.75f, 0.75f, 0.75f, 1.0f))
{
	f_params.addParameter("percentage",mbParams.p);
	f_params.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
	f_params.addParameter("accum",CTextureUnitSetup::IMAGE_UNIT_1);

	f_params2.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
}

CMBFilter::~CMBFilter()
{
	glDestroyFilter();
}

void CMBFilter::setPercentage(float r,float g,float b,float a)
{
	mbParams.p = GL_COORD_VERTEX(r, g, b, a);
	f_params[0].copy(mbParams);
}

void CMBFilter::getPercentage(float &r,float &g,float &b,float &a)
{
    r = mbParams.p.x;
    g = mbParams.p.y;
    b = mbParams.p.z;
    a = mbParams.p.h;
}

void CMBFilter::glDestroyFilter(void)
{
    if (m_pAccumulator != NULL)
    {
        CAccumulator *pAccum = (CAccumulator*)m_pAccumulator;

        if (pAccum->m_pCurrentColorAccum != NULL)
            pAccum->m_pCurrentColorAccum->releaseReference();
        if (pAccum->m_pPreviousColorAccum != NULL)
            pAccum->m_pPreviousColorAccum->releaseReference();

        IRaptor::glDestroyDisplay(pAccum->pCurrentDisplay);
        IRaptor::glDestroyDisplay(pAccum->pPreviousDisplay);

        delete pAccum;
		m_pAccumulator = NULL;
    }

	if (m_pRenderTextures2 != NULL)
	{
		delete m_pRenderTextures2;
		m_pRenderTextures2 = NULL;
	}

    CRaptorDisplayFilter::glDestroyFilter();
}

void CMBFilter::glRenderFilter()
{
    //  Rendering accumulators
    CAccumulator *pAccum = (CAccumulator*)m_pAccumulator;

	const CRaptorGLExtensions *const pExtensions = IRaptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    // Accumulate previous render with colorSource
    RAPTOR_HANDLE noDevice;
	pAccum->pCurrentDisplay->glvkBindDisplay(noDevice);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
	pAccum->m_pPreviousColorAccum->glvkRender();
    glActiveTextureARB(GL_TEXTURE0_ARB);
	getColorInput()->glvkRender();

	m_pMotionBlurShader->glGetOpenGLShader()->setProgramParameters(f_params);
    m_pMotionBlurShader->glRender();
	glDrawFilter();
	m_pMotionBlurShader->glStop();

	glBindTexture(GL_TEXTURE_2D,0);
	pAccum->pCurrentDisplay->glvkUnBindDisplay();
}

void CMBFilter::glRenderFilterOutput()
{
    //  Render final buffer
    CAccumulator *pAccum = (CAccumulator*)m_pAccumulator;
	
	pAccum->m_pCurrentColorAccum->glvkRender();

	m_pFinalShader->glGetOpenGLShader()->setProgramParameters(f_params2);
	m_pFinalShader->glRender();
    glDrawFilter();
	m_pFinalShader->glStop();

	glBindTexture(GL_TEXTURE_2D,0);

    pAccum->switchDisplay();
}


bool CMBFilter::glInitFilter(void)
{
    if (!CRaptorDisplayFilter::glInitFilter())
        return false;

	CTextureFactory &filterFactory = CTextureFactory::glGetDefaultFactory();

	bool previousResize = filterFactory.getConfig().useTextureResize();
	filterFactory.getConfig().useTextureResize(false);

	if ((colorExternalSource != NULL) && (m_fModel == RENDER_TEXTURE))
	{
		colorInput = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															ITextureObject::CGL_BILINEAR,
															colorExternalSource);
	}

    CRaptorDisplayConfig state;
    state.width = colorInput->getWidth();
    state.height = colorInput->getHeight();
	state.acceleration = CRaptorDisplayConfig::HARDWARE;
	state.bind_to_texture = true;
	state.depth_buffer = false;
	state.double_buffer = false;
	state.stencil_buffer = false;
    state.display_mode = CGL_RGBA;
	state.renderer = CRaptorDisplayConfig::PIXEL_BUFFER;

	CAccumulator *accumulator = new CAccumulator;

	if (m_fModel == RENDER_BUFFER)
	{
		state.renderer = CRaptorDisplayConfig::RENDER_BUFFER;

		ITextureObject *T = NULL;
		T = filterFactory.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
											ITextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(T,state.width,state.height);
		T->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
		accumulator->m_pCurrentColorAccum = T;
		m_pRenderTextures->addTexture(T);

		m_pRenderTextures2 = new CTextureSet();
		T = filterFactory.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
											ITextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(T,state.width,state.height);	
		T->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
		accumulator->m_pPreviousColorAccum = T;
		m_pRenderTextures2->addTexture(T);
	}

    CRaptorDisplay* pDisplay1 = IRaptor::glCreateDisplay(state);
	IRenderingProperties &rp1 = pDisplay1->getRenderingProperties();
	rp1.setTexturing(IRenderingProperties::ENABLE);
	rp1.setLighting(IRenderingProperties::DISABLE);
	rp1.setCullFace(IRenderingProperties::DISABLE);
	rp1.setDepthTest(IRenderingProperties::DISABLE);
    rp1.clear(CGL_NULL);
    pDisplay1->setViewPoint(NULL);

	if (m_fModel == RENDER_BUFFER)
		pDisplay1->glvkBindDisplay(*m_pRenderTextures);

    CRaptorDisplay* pDisplay2 = IRaptor::glCreateDisplay(state);
	IRenderingProperties &rp2 = pDisplay2->getRenderingProperties();
    rp2 = rp1;
    pDisplay2->setViewPoint(NULL);

	if (m_fModel == RENDER_BUFFER)
		pDisplay2->glvkBindDisplay(*m_pRenderTextures2);


    accumulator->pCurrentDisplay = pDisplay1;
    accumulator->pPreviousDisplay = pDisplay2;
    m_pAccumulator = accumulator;

	if (m_fModel == RENDER_TEXTURE)
	{
		accumulator->m_pCurrentColorAccum = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
																					ITextureObject::CGL_BILINEAR,
																					accumulator->pCurrentDisplay);
		accumulator->m_pPreviousColorAccum = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
																					ITextureObject::CGL_BILINEAR,
																					accumulator->pPreviousDisplay);
	}

	m_pMotionBlurShader = new CShader("MB_SHADER");

#if defined(GL_ARB_geometry_shader4)
	m_pFinalShader = new CShader("MotionBlurShader");
	COpenGLShaderStage *stage = m_pMotionBlurShader->glGetOpenGLShader();
	stage->glGetVertexShader("EMPTY_PROGRAM");
	stage->glGetGeometryShader("FULL_SCREEN_GEO_PROGRAM");
	stage->glGetFragmentShader("MB_ACCUM_TEX_SHADER");
	bool res = stage->glCompileShader();

	stage = m_pFinalShader->glGetOpenGLShader();
	stage->glGetVertexShader("EMPTY_PROGRAM");
	stage->glGetGeometryShader("FULL_SCREEN_GEO_PROGRAM");
	stage->glGetFragmentShader("MB_ACCUM2_TEX_SHADER");
	res = res && stage->glCompileShader();
#elif defined(GL_ARB_vertex_shader)
	COpenGLShaderStage *stage = m_pMotionBlurShader->glGetOpenGLShader();
	stage->glGetFragmentShader("MB_ACCUM_OLD_TEX_SHADER");
	bool res = stage->glCompileShader();
#elif defined(GL_ARB_vertex_program)
	#error "Unsupported deprecated vertex program feature."
#endif

    filterFactory.getConfig().useTextureResize(previousResize);

    return res;
}




