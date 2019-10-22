/***************************************************************************/
/*                                                                         */
/*  HDRFilter.cpp                                                          */
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


#if !defined(AFX_HDRFILTER_H__6E9B4FC8_154A_46DD_A218_6BC438E45C6A__INCLUDED_)
	#include "HDRFilter.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
  #include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_OPENGLPROGRAMSTAGE_H__0BCE3B42_6E10_4F50_BB27_1993345ADBCF__INCLUDED_)
	#include "GLHierarchy/OpenGLProgramStage.h"
#endif


static const float BLUR_BUFFER_SIZE_FACTOR = 0.125f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHDRFilter::CHDRFilter(const CRaptorDisplayConfig &da)
	:luminanceParams(GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 1.0f)),
	thresholdParam(GL_COORD_VERTEX(1.0f, 0.5, 1.0f, 1.0f))
{
    rda = da;
    rda.x = 0;
    rda.y = 0;
    rda.refresh_rate.fps = CGL_MAXREFRESHRATE;
	rda.acceleration = CRaptorDisplayConfig::HARDWARE;
	rda.antialias = CRaptorDisplayConfig::ANTIALIAS_NONE;
	rda.depth_buffer = false;
	rda.double_buffer = false;
	rda.stencil_buffer = false;
	rda.bind_to_texture = true;
    rda.display_mode =	(da.display_mode & (CGL_RGB|CGL_RGBA|CGL_FLOAT_32|CGL_FLOAT_16));
	rda.renderer = CRaptorDisplayConfig::PIXEL_BUFFER;

#if defined(GL_EXT_framebuffer_object)
	if ((Raptor::glIsExtensionSupported(GL_EXT_FRAMEBUFFER_OBJECT_EXTENSION_NAME)) &&
		(da.renderer == CRaptorDisplayConfig::RENDER_BUFFER))
		rda.renderer = CRaptorDisplayConfig::RENDER_BUFFER;
#endif

	nLevels = 0;
	nBlurPass = 1;
	m_pDownSizedBuffer = NULL;
	m_pDownSizedDisplay = NULL;
	m_pDownSizedAttachments = NULL;
	m_pDownBlurXDisplay = NULL;
	m_pDownBlurYDisplay = NULL;
	m_pDownBlurXBuffer = NULL;
	m_pDownBlurYBuffer = NULL;
	m_pDownHighFreqs = NULL;
	m_pDownHFBuffer = NULL;
	m_pComposite = NULL;
	m_pTreshholdFreqs = NULL;
	m_maxLuminance = NULL;
	m_lastMaxLuminance = NULL;

	m_pBlenderX = NULL;
	m_pBlenderY = NULL;
}

CHDRFilter::~CHDRFilter()
{
	glDestroyFilter();
}

void CHDRFilter::setBlurNbPass(unsigned int nb)
{
	nBlurPass = MAX(MIN(nb,4),1);
}

void CHDRFilter::setHFTreshold(float t)
{
	thresholdParam.p.y = MAX(0, t);
}


void CHDRFilter::glRenderFilter()
{
    if (getColorInput() == NULL)
        return;

    //  Downsize buffers to compute global intensity
    RAPTOR_HANDLE nodevice;
    CTextureObject  *currentBuffer = getColorInput();
    for (unsigned int i=0;i<nLevels;i++)
    {
		m_pDownSizedDisplay[i]->glvkBindDisplay(nodevice);
		currentBuffer->glvkRender();

		luminanceParams.p = GL_COORD_VERTEX(0.5f / currentBuffer->getWidth(),
											0.5f / currentBuffer->getHeight(),
											-0.5f / currentBuffer->getWidth(),
											0.0f);
		maxLuminanceParams[0].copy(luminanceParams);

		if (i < 2)
		{
			getIdentityShader()->glRender();
			glDrawFilter();
			getIdentityShader()->glStop();
		}
        else if (i<nLevels-1)
        {
#if defined(GL_ARB_vertex_shader) || defined(GL_ARB_geometry_shader4)
			m_maxLuminance->glGetOpenGLShader()->setProgramParameters(maxLuminanceParams);
#elif defined(GL_ARB_vertex_program)
			m_maxLuminance->glGetOpenGLProgram()->setProgramParameters(maxLuminanceParams);
#endif
            m_maxLuminance->glRender();
			glDrawFilter();
            m_maxLuminance->glStop();
        }
        else
        {
#if defined(GL_ARB_vertex_shader) || defined(GL_ARB_geometry_shader4)
			m_lastMaxLuminance->glGetOpenGLShader()->setProgramParameters(maxLuminanceParams);
#elif defined(GL_ARB_vertex_program)
			m_lastMaxLuminance->glGetOpenGLProgram()->setProgramParameters(maxLuminanceParams);
#endif
            m_lastMaxLuminance->glRender();
			glDrawFilter();
            m_lastMaxLuminance->glStop();
        }

		m_pDownSizedDisplay[i]->glvkUnBindDisplay();
        currentBuffer = m_pDownSizedBuffer[i];
    }

    //
    //  Extract high frequencies only
    //
	m_pDownHighFreqs->glvkBindDisplay(nodevice);
	m_pDownSizedBuffer[1]->glvkRender();
	thresholdParams[1].copy(thresholdParam);
#if defined(GL_ARB_vertex_shader) || defined(GL_ARB_geometry_shader4)
	m_pTreshholdFreqs->glGetOpenGLShader()->setProgramParameters(thresholdParams);
#elif defined(GL_ARB_vertex_program)
    m_pTreshholdFreqs->glGetOpenGLProgram()->setProgramParameters(threshholdParams);
#endif
    m_pTreshholdFreqs->glRender();
	glDrawFilter();
    m_pTreshholdFreqs->glStop();
	m_pDownHighFreqs->glvkUnBindDisplay();
  
    //
    //  Blur high frequencies  
    //
	m_pDownBlurXDisplay->glvkBindDisplay(nodevice);
	m_pDownHFBuffer->glvkRender();

#if defined(GL_ARB_geometry_shader4)
	m_pBlenderX->glGetOpenGLShader()->setProgramParameters(blurOffsets);
#else
	m_pBlenderX->glGetOpenGLProgram()->setProgramParameters(blurOffsets);
#endif

	m_pBlenderX->glRender();
	glDrawFilter();
	m_pBlenderX->glStop();

	m_pDownBlurXDisplay->glvkUnBindDisplay();
	m_pDownBlurYDisplay->glvkBindDisplay(nodevice);
	m_pDownBlurXBuffer->glvkRender();

#if defined(GL_ARB_geometry_shader4)
	m_pBlenderY->glGetOpenGLShader()->setProgramParameters(blurOffsets);
#else
	m_pBlenderY->glGetOpenGLProgram()->setProgramParameters(blurOffsets);
#endif
	m_pBlenderY->glRender();
	glDrawFilter();
	m_pBlenderY->glStop();
	
	m_pDownBlurYDisplay->glvkUnBindDisplay();

	for (unsigned int i=2;i<=nBlurPass;i++)
	{
		//
		//  Second pass Blur high frequencies  
		//
		m_pDownBlurXDisplay->glvkBindDisplay(nodevice);
		m_pDownBlurYBuffer->glvkRender();

		m_pBlenderX->glRender();
		glDrawFilter();
		m_pBlenderX->glStop();
	
		m_pDownBlurXDisplay->glvkUnBindDisplay();
		m_pDownBlurYDisplay->glvkBindDisplay(nodevice);
		m_pDownBlurXBuffer->glvkRender();

		m_pBlenderY->glRender();
		glDrawFilter();
		m_pBlenderY->glStop();
	
		m_pDownBlurYDisplay->glvkUnBindDisplay();
	}

}

void CHDRFilter::glRenderFilterOutput()
{
	//
    // Final rendering : composite global illumination with gloom and original HDR scene.
    //
	const CRaptorGLExtensions*   const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
	m_pDownSizedBuffer[nLevels - 1]->glvkRender();    // texture is a single texel with luminance max.
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
	m_pDownBlurYBuffer->glvkRender();
	glActiveTextureARB(GL_TEXTURE0_ARB);
	
	getColorInput()->glvkRender();

    // compose blur + hdr
    m_pComposite->glRender();
	glDrawFilter();
    m_pComposite->glStop();

    glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);

	glBindTexture(GL_TEXTURE_2D,0);
}


bool CHDRFilter::glInitFilter(void)
{
    if (!CRaptorDisplayFilter::glInitFilter())
        return false;

	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();

	bool previousResize = filterFactory.getConfig().useTextureResize();
	filterFactory.getConfig().useTextureResize(false);

    bool res = true;

    unsigned int filter_cs_width = rda.width;
    unsigned int filter_cs_height = rda.height;

    unsigned int width = rda.width;
    unsigned int height = rda.height;

    nLevels = (unsigned int)(MAX(log((float)width),log((float)height)) / log(2.0f));

    //! height & width of sub displays will be reduced to a square dimension because
    //! the computation of the luminance max computes the max of 4 texels into one,
    //! reducing the buffer by 2 in both dimensions at each iteration.
    width = height = 1 << nLevels ;
    m_pDownSizedBuffer = new CTextureObject*[nLevels];
    m_pDownSizedDisplay = new CRaptorDisplay*[nLevels];

	if ((colorExternalSource != NULL) && (m_fModel == RENDER_TEXTURE))
	{
		colorInput = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															ITextureObject::CGL_BILINEAR,
															colorExternalSource);
	}
	else if (m_fModel == RENDER_BUFFER)
	{
		m_pDownSizedAttachments = new CTextureSet*[nLevels];
	}

	IRenderingProperties *commonRP = CRaptorDisplay::GetCurrentDisplay()->createRenderingProperties();
	commonRP->setTexturing(IRenderingProperties::ENABLE);
	commonRP->setCullFace(IRenderingProperties::DISABLE);
	commonRP->setDepthTest(IRenderingProperties::DISABLE);
	commonRP->setLighting(IRenderingProperties::DISABLE);
	//commonRP->setFloatClamping(IRenderingProperties::DISABLE);
	commonRP->clear(CGL_NULL);

    for (unsigned int i=0; i<nLevels; i++)
    {
        width = MAX(1,width/2);
        height = MAX(1,height/2);

        rda.width = width;
        rda.height = height;

        m_pDownSizedDisplay[i] = Raptor::glCreateDisplay(rda);
		IRenderingProperties &rp = m_pDownSizedDisplay[i]->getRenderingProperties();
        rp = *commonRP;
        m_pDownSizedDisplay[i]->setViewPoint(NULL);
        ITextureObject::TEXTURE_FILTER filter = ( i < 2 ? ITextureObject::CGL_BILINEAR : ITextureObject::CGL_UNFILTERED);

		if (m_fModel == RENDER_BUFFER)
		{
			m_pDownSizedBuffer[i] = filterFactory.glCreateTexture(	ITextureObject::CGL_COLOR_FLOAT16_ALPHA,
																	CTextureObject::CGL_OPAQUE,
																	filter);
			filterFactory.glResizeTexture(m_pDownSizedBuffer[i],width,height);
			CTextureSet *tset = new CTextureSet("HDR_TSet");
			tset->addTexture(m_pDownSizedBuffer[i]);
			m_pDownSizedDisplay[i]->glvkBindDisplay(*tset);
			m_pDownSizedAttachments[i] = tset;
		}
		else if (m_fModel == RENDER_TEXTURE)
		{
			m_pDownSizedBuffer[i] = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
																			CTextureObject::CGL_OPAQUE,
																			filter, 
																			m_pDownSizedDisplay[i]);
		}
    }

    rda.width = MAX(1,filter_cs_width * BLUR_BUFFER_SIZE_FACTOR);
    rda.height = MAX(1,filter_cs_height * BLUR_BUFFER_SIZE_FACTOR);
    m_pDownBlurXDisplay = Raptor::glCreateDisplay(rda);
	IRenderingProperties &rp = m_pDownBlurXDisplay->getRenderingProperties();
    rp = *commonRP;
    m_pDownBlurXDisplay->setViewPoint(NULL);
	if (m_fModel == RENDER_BUFFER)
	{
		m_pDownBlurXBuffer = filterFactory.glCreateTexture(	ITextureObject::CGL_COLOR_FLOAT16_ALPHA,
															CTextureObject::CGL_OPAQUE,
															ITextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(m_pDownBlurXBuffer,rda.width,rda.height);
		m_pDownBlurXBuffer->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
		CTextureSet *tset = new CTextureSet("HDR_XBlur");
		tset->addTexture(m_pDownBlurXBuffer);
		m_pDownBlurXDisplay->glvkBindDisplay(*tset);
	}
	else if (m_fModel == RENDER_TEXTURE)
	{
		m_pDownBlurXBuffer = filterFactory.glCreateDynamicTexture(ITextureObject::CGL_COLOR24_ALPHA,	
																  CTextureObject::CGL_OPAQUE,
																  ITextureObject::CGL_BILINEAR,
																  m_pDownBlurXDisplay);
	}

    m_pDownBlurYDisplay = Raptor::glCreateDisplay(rda);
	IRenderingProperties &rp2 = m_pDownBlurYDisplay->getRenderingProperties();
    rp2 = *commonRP;
    m_pDownBlurYDisplay->setViewPoint(NULL);
	if (m_fModel == RENDER_BUFFER)
	{
		m_pDownBlurYBuffer = filterFactory.glCreateTexture(	ITextureObject::CGL_COLOR_FLOAT16_ALPHA,
															CTextureObject::CGL_OPAQUE,
															ITextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(m_pDownBlurYBuffer,rda.width,rda.height);
		m_pDownBlurYBuffer->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
		CTextureSet *tset = new CTextureSet("HDR_YBlur");
		tset->addTexture(m_pDownBlurYBuffer);
		m_pDownBlurYDisplay->glvkBindDisplay(*tset);
	}
	else if (m_fModel == RENDER_TEXTURE)
	{
		m_pDownBlurYBuffer = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
																	CTextureObject::CGL_OPAQUE,
																	ITextureObject::CGL_BILINEAR,
																	m_pDownBlurYDisplay);
	}

    m_pDownHighFreqs  = Raptor::glCreateDisplay(rda);
	IRenderingProperties &rp3 = m_pDownHighFreqs->getRenderingProperties();
    rp3 = *commonRP;
    m_pDownHighFreqs->setViewPoint(NULL);
	if (m_fModel == RENDER_BUFFER)
	{
		m_pDownHFBuffer = filterFactory.glCreateTexture(ITextureObject::CGL_COLOR_FLOAT16_ALPHA,
														CTextureObject::CGL_OPAQUE,
														ITextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(m_pDownHFBuffer,rda.width,rda.height);
		m_pDownHFBuffer->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
		CTextureSet *tset = new CTextureSet("HDR_HighFrequencies");
		tset->addTexture(m_pDownHFBuffer);
		m_pDownHighFreqs->glvkBindDisplay(*tset);
	}
	else if (m_fModel == RENDER_TEXTURE)
	{
		m_pDownHFBuffer = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
																CTextureObject::CGL_OPAQUE,
																ITextureObject::CGL_BILINEAR,
																m_pDownHighFreqs);
	}

	delete commonRP;

    //
    //  Create shaders
    //
	blurOffsets.addParameter("offset", GL_COORD_VERTEX(	1.0f / (filter_cs_width * BLUR_BUFFER_SIZE_FACTOR),
														1.0f / (filter_cs_height * BLUR_BUFFER_SIZE_FACTOR),
														0.0f, 1.0f));
	blurOffsets.addParameter("color", CTextureUnitSetup::IMAGE_UNIT_0);

	thresholdParams.addParameter("offset", GL_COORD_VERTEX(	1.0f / (filter_cs_width * 2.0f * BLUR_BUFFER_SIZE_FACTOR),
															1.0f / (filter_cs_height * 2.0f * BLUR_BUFFER_SIZE_FACTOR),
															-1.0f / (filter_cs_width * 2.0f * BLUR_BUFFER_SIZE_FACTOR),
															0.0f));
	thresholdParams.addParameter("treshhold", thresholdParam.p);
	thresholdParams.addParameter("color", CTextureUnitSetup::IMAGE_UNIT_0);

	maxLuminanceParams.addParameter("offset", luminanceParams.p);
	maxLuminanceParams.addParameter("color", CTextureUnitSetup::IMAGE_UNIT_0);

	res = res && glBuildShaders();

    rda.width = filter_cs_width;
    rda.height = filter_cs_height;

	filterFactory.getConfig().useTextureResize(previousResize);

    return res;
}


#if defined(GL_ARB_geometry_shader4)
bool CHDRFilter::glBuildShaders(void)
{
	m_pBlenderX = new CShader("HDR_BLENDER_X");
	COpenGLShaderStage *stage = m_pBlenderX->glGetOpenGLShader();
	stage->glGetVertexShader("EMPTY_PROGRAM");
	stage->glGetGeometryShader("FULL_SCREEN_GEO_PROGRAM");
	stage->glGetFragmentShader("BLENDER_8X_TEX_PROGRAM");
	bool res = stage->glCompileShader();

	m_pBlenderY = new CShader("HDR_BLENDER_Y");
	stage = m_pBlenderY->glGetOpenGLShader();
	stage->glGetVertexShader("EMPTY_PROGRAM");
	stage->glGetGeometryShader("FULL_SCREEN_GEO_PROGRAM");
	stage->glGetFragmentShader("BLENDER_8Y_TEX_PROGRAM");
	res = res && stage->glCompileShader();
	
	m_pTreshholdFreqs = new CShader("HDR_TRESHOLDS");
	stage = m_pTreshholdFreqs->glGetOpenGLShader();
	stage->glGetVertexShader("EMPTY_PROGRAM");
	stage->glGetGeometryShader("FULL_SCREEN_GEO_PROGRAM");
	stage->glGetFragmentShader("HDR_THRESHOLD_TEX_SHADER");
	res = res && stage->glCompileShader();

	m_pComposite = new CShader("HDR_COMPOSITE");
	stage = m_pComposite->glGetOpenGLShader();
	stage->glGetVertexShader("EMPTY_PROGRAM");
	stage->glGetGeometryShader("FULL_SCREEN_GEO_PROGRAM");
	stage->glGetFragmentShader("HDR_COMPOSER_TEX_SHADER");
	
	CProgramParameters	fp_params;
	fp_params.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
	fp_params.addParameter("blur",CTextureUnitSetup::IMAGE_UNIT_1);
	fp_params.addParameter("lwhite",CTextureUnitSetup::IMAGE_UNIT_2);
	stage->setProgramParameters(fp_params);
	res = res && stage->glCompileShader();

	m_maxLuminance = new CShader("HDR_MAXLUMINANCE");
	stage = m_maxLuminance->glGetOpenGLShader();
	stage->glGetVertexShader("EMPTY_PROGRAM");
	stage->glGetGeometryShader("FULL_SCREEN_GEO_PROGRAM");
	stage->glGetFragmentShader("HDR_LUMINANCE_TEX_SHADER");
	res = res && stage->glCompileShader();

	m_lastMaxLuminance = new CShader("HDR_LASTMAXLUMINANCE");
	stage = m_lastMaxLuminance->glGetOpenGLShader();
	stage->glGetVertexShader("EMPTY_PROGRAM");
	stage->glGetGeometryShader("FULL_SCREEN_GEO_PROGRAM");
	stage->glGetFragmentShader("HDR_LUMINANCELAST_TEX_SHADER");
	res = res && stage->glCompileShader();

	return res;
}

#elif defined(GL_ARB_vertex_shader)

	bool CHDRFilter::glBuildShaders()
	{
		m_pBlenderX = new CShader("HDR_BLENDER_X");
		COpenGLProgramStage *stage = m_pBlenderX->glGetOpenGLProgram();
		stage->glGetFragmentProgram("BLENDER_8X");
		stage->glGetVertexProgram("BLENDER_8X_XOFFSETS");
		
		m_pBlenderY = new CShader("HDR_BLENDER_Y");
		stage = m_pBlenderY->glGetOpenGLProgram();
		stage->glGetFragmentProgram("BLENDER_8X");
		stage->glGetVertexProgram("BLENDER_8X_YOFFSETS");


		m_pTreshholdFreqs = new CShader("HDR_TRESHOLDS");
		COpenGLShaderStage *stage2 = m_pTreshholdFreqs->glGetOpenGLShader();
		stage2->glGetFragmentShader("HDR_THRESHOLD_OLD_TEX_SHADER");
		bool res = stage2->glCompileShader();

		m_pComposite = new CShader("HDR_COMPOSITE");
		stage2 = m_pComposite->glGetOpenGLShader();
		stage2->glGetFragmentShader("HDR_COMPOSER_OLD_TEX_SHADER");
		
		CProgramParameters	fp_params;
		fp_params.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
		fp_params.addParameter("blur",CTextureUnitSetup::IMAGE_UNIT_1);
		fp_params.addParameter("lwhite",CTextureUnitSetup::IMAGE_UNIT_2);
		stage2->setProgramParameters(fp_params);
		res = res && stage2->glCompileShader();

		m_maxLuminance = new CShader("HDR_MAXLUMINANCE");
		stage2 = m_maxLuminance->glGetOpenGLShader();
		stage2->glGetFragmentShader("HDR_LUMINANCE_OLD_TEX_SHADER");
		res = res && stage2->glCompileShader();

		m_lastMaxLuminance = new CShader("HDR_LASTMAXLUMINANCE");
		stage2 = m_lastMaxLuminance->glGetOpenGLShader();
		stage2->glGetFragmentShader("HDR_LUMINANCELAST_OLD_TEX_SHADER");
		res = res && stage2->glCompileShader();

		return res;
	}

#elif defined(GL_ARB_vertex_program)

	bool CHDRFilter::glBuildShaders()
	{
		m_pBlenderX = new CShader("HDR_BLENDER_X");
		COpenGLProgramStage *stage = m_pBlenderX->glGetOpenGLProgram();
		stage->glGetFragmentProgram("BLENDER_8X");
		stage->glGetVertexProgram("BLENDER_8X_XOFFSETS");

		m_pBlenderY = new CShader("HDR_BLENDER_Y");
		stage = m_pBlenderY->glGetOpenGLProgram();
		stage->glGetFragmentProgram("BLENDER_8X");
		stage->glGetVertexProgram("BLENDER_8X_YOFFSETS");

		m_pTreshholdFreqs = new CShader("HDR_TRESHOLDS");
		stage = m_pTreshholdFreqs->glGetOpenGLProgram();
		stage->glGetFragmentProgram("HDR_THRESHOLD_TEX_PROGRAM");
		
		m_pComposite = new CShader("HDR_COMPOSITE");
		stage = m_pComposite->glGetOpenGLProgram();
		stage->glGetFragmentProgram("HDR_COMPOSER_TEX_PROGRAM");

		m_maxLuminance = new CShader("HDR_MAXLUMINANCE");
		stage = m_maxLuminance->glGetOpenGLProgram();
		stage->glGetFragmentProgram("HDR_LUMINANCE_TEX_PROGRAM");

		m_lastMaxLuminance = new CShader("HDR_LASTMAXLUMINANCE");
		stage = m_lastMaxLuminance->glGetOpenGLProgram();
		stage->glGetFragmentProgram("HDR_LUMINANCELAST_TEX_PROGRAM");

		return res;
	}

#else

	bool CHDRFilter::glBuildShaders()
	{ 
		return false; 
	}
#endif

void CHDRFilter::glDestroyFilter(void)
{
    CRaptorDisplayFilter::glDestroyFilter();

    if (m_pDownSizedBuffer != NULL)
    {
        for (unsigned int i=0;i<nLevels;i++)
            m_pDownSizedBuffer[i]->releaseReference();

        delete [] m_pDownSizedBuffer;
		m_pDownSizedBuffer = NULL;
    }

    if (m_pDownSizedDisplay != NULL)
    {
        for (unsigned int i=0;i<nLevels;i++)
            Raptor::glDestroyDisplay(m_pDownSizedDisplay[i]);

        delete [] m_pDownSizedDisplay;
		m_pDownSizedDisplay = NULL;
    }

	if (m_pDownSizedAttachments != NULL)
    {
        for (unsigned int i=0;i<nLevels;i++)
            delete m_pDownSizedAttachments[i];

        delete [] m_pDownSizedAttachments;
		m_pDownSizedAttachments = NULL;
    }

	if (NULL != m_pDownBlurXDisplay)
        Raptor::glDestroyDisplay(m_pDownBlurXDisplay);
	if (NULL != m_pDownBlurYDisplay)
        Raptor::glDestroyDisplay(m_pDownBlurYDisplay);
	if (NULL != m_pDownHighFreqs)
		Raptor::glDestroyDisplay(m_pDownHighFreqs);
	if (NULL != m_pDownBlurXBuffer)
		m_pDownBlurXBuffer->releaseReference();
	if (NULL != m_pDownBlurYBuffer)
		m_pDownBlurYBuffer->releaseReference();
	if (NULL != m_pDownHFBuffer)
		m_pDownHFBuffer->releaseReference();
	if (NULL != m_pComposite)
		m_pComposite->releaseReference();
	if (NULL != m_pTreshholdFreqs)
		m_pTreshholdFreqs->releaseReference();
	if (NULL != m_maxLuminance)
		m_maxLuminance->releaseReference();
	if (NULL != m_lastMaxLuminance)
		m_lastMaxLuminance->releaseReference();

	m_pDownBlurYDisplay = NULL;
	m_pDownBlurXDisplay = NULL;
	m_pDownBlurXBuffer = NULL;
	m_pDownBlurYBuffer = NULL;
	m_pDownHighFreqs = NULL;
	m_pDownHFBuffer = NULL;
    m_pComposite = NULL;
	m_pTreshholdFreqs = NULL;
	m_maxLuminance = NULL;
	m_lastMaxLuminance = NULL;
}

//
// For debug purpose, cpu computing functions to compare
//	with gpu results.
//
/*
static float maxc = 0;
static float maxl = 0;
void maxL(void)
{
    int w = 0;
    int h = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&h);

    maxl = 0;
	maxc = 0;
    float *pixels = new float[w*h*4];

    glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_FLOAT,pixels);

    for (int j=0;j<h;j++)
    {
        for (int k=0;k<w*4;k+=4)
        {
            CColor::RGBA rgba(pixels[j*w*4 + k],pixels[j*w*4 + k+1],pixels[j*w*4 + k+2],pixels[j*w*4 + k+3]);
			
			maxc = MAX(maxc,MAX(rgba.r,MAX(rgba.g,rgba.b)));

			CColor::YUVA yuva = rgba;
            if (yuva.y > maxl)
                maxl = yuva.y;
        }
    }
    delete [] pixels;
}

static float minr0 = 0;
static float ming0 = 0;
static float minb0 = 0;
static float mina0 = 0;
static float maxl = 0;

void mean(void)
{
    int w = 0;
    int h = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&h);

    float minr=0, ming=0,minb=0,mina=0;
    float maxr=0, maxg=0,maxb=0,maxa=0;
    float *pixels = new float[w*h*4];

	GLuint err = glGetError();
    glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_FLOAT,pixels);
	err = glGetError();

    for (int j=0;j<h;j++)
    {
        float sumr = 0;
        float sumg = 0;
        float sumb = 0;
        float suma = 0;
        for (int k=0;k<w*4;k+=4)
        {
            if (pixels[j*w*4 + k] > maxr)
                maxr = pixels[j*w*4 + k];
            if (pixels[j*w*4 + k+1] > maxg)
                maxg = pixels[j*w*4 + k+1];
            if (pixels[j*w*4 + k+2] > maxb)
                maxb = pixels[j*w*4 + k+2];
            if (pixels[j*w*4 + k+3] > maxa)
                maxa = pixels[j*w*4 + k+3];
            sumr += pixels[j*w*4 + k];
            sumg += pixels[j*w*4 + k+1];
            sumb += pixels[j*w*4 + k+2];
            suma += pixels[j*w*4 + k+3];
        }

        minr += sumr / w;
        ming += sumg / w;
        minb += sumb / w;
        mina += suma / w;
    }

    minr0 = minr = minr / h;
    ming0 = ming = ming / h;
    minb0 = minb = minb / h;
    mina0 = mina = mina / h;

    delete [] pixels;
}
*/

