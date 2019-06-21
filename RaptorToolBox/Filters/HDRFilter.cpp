// HDRFilter.cpp: implementation of the CHDRFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexProgram.h"
#endif
#if defined(GL_ARB_geometry_shader4)
	#if !defined(AFX_GEOMETRYPROGRAM_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
		#include "GLHierarchy/GeometryProgram.h"
	#endif
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
#if !defined(AFX_HDRFILTER_H__6E9B4FC8_154A_46DD_A218_6BC438E45C6A__INCLUDED_)
    #include "HDRFilter.h"
#endif

//#undef GL_ARB_geometry_shader4

#if defined(GL_ARB_geometry_shader4)
	//  Compute luminance with bilinear components, averaging
	//  the 4 surrouding of the texel's center.
	static const string treshhold2_ps =
	"#version 460				\n\
	uniform sampler2D color;	\n\
	uniform vec4 offset;		\n\
	uniform vec4 treshhold;		\n\
	\n\
	const vec4 luminance = vec4(0.299, 0.587, 0.114, 0.0);\n\
	\n\
	layout(location = 1) in vec4 g_TexCoord; \n\
	layout(location = 0) out vec4 o_Color;	\n\
	\n\
	void main(void)			\n\
	{						\n\
		vec4 L = vec4(0.0, 0.0, 0.0, 0.0); \n\
		\n\
		vec4 c1 = texture(color,g_TexCoord.xy - offset.xy); \n\
		L.x = dot(c1,luminance); \n\
		\n\
		vec4 c2 = texture(color,g_TexCoord.xy + offset.zy); \n\
		L.y = dot(c2,luminance); \n\
		\n\
		vec4 c3 = texture(color,g_TexCoord.xy - offset.zy); \n\
		L.z = dot(c3,luminance); \n\
		\n\
		vec4 c4 = texture(color,g_TexCoord.xy + offset.xy); \n\
		L.w = dot(c4,luminance); \n\
		\n\
		L = max(vec4(0.0,0.0,0.0,0.0), L - treshhold.yyyy); \n\
		vec4 c = c1 * L.xxxx; \n\
		c = c + c2 * L.yyyy; \n\
		c = c + c3 * L.zzzz; \n\
		o_Color = c + c4 * L.wwww; \n\
		o_Color.w = 1.0; \n\
	}";

	//!	Tone mapping shader
	static const string composer_ps =
	"#version 460				\n\
	uniform sampler2D color;	\n\
	uniform sampler2D blur;		\n\
	uniform sampler2D lwhite;	\n\
	\n\
	layout(location = 1) in vec4 g_TexCoord; \n\
	layout(location = 0) out vec4 o_Color;	\n\
	\n\
	const vec4 luminance = vec4(0.299, 0.587, 0.114, 0.0);\n\
	\n\
	void main(void)			\n\
	{						\n\
		vec4 c = texture(color,g_TexCoord.xy); \n\
		vec4 b = texture(blur,g_TexCoord.xy); \n\
		vec4 l = texture(lwhite,g_TexCoord.xy); \n\
		o_Color = c*l + b; \n\
		o_Color.w = 1.0; \n\
	}";

	static const string luminanceMax_ps =
	"#version 460				\n\
	uniform sampler2D color;	\n\
	uniform vec4 offset;		\n\
	\n\
	layout(location = 1) in vec4 g_TexCoord; \n\
	layout(location = 0) out vec4 o_Color;	\n\
	\n\
	const vec4 luminance = vec4(0.299, 0.587, 0.114, 0.0);\n\
	\n\
	void main(void)			\n\
	{						\n\
		vec4 c = texture(color,g_TexCoord.xy - offset.xy); \n\
		float LMax = dot(c,luminance); \n\
		\n\
		c = texture(color,g_TexCoord.xy + offset.zy); \n\
		LMax = max(LMax,dot(c,luminance)); \n\
		\n\
		c = texture(color,g_TexCoord.xy - offset.zy); \n\
		LMax = max(LMax,dot(c,luminance)); \n\
		\n\
		c = texture(color,g_TexCoord.xy + offset.xy); \n\
		LMax = max(LMax,dot(c,luminance)); \n\
		o_Color = vec4(LMax,LMax,LMax,0.0); \n\
	}";

	//	Luminance max is scaled to 1.0 to avoid darkening a scene
	//	in the case where no pixels are above 1.0
	//	The final luminance is applied to darken the whole scene
	//	for the tone mapping in order to have the brightest parts
	//	at 1.0. (L>1 => x 1/L is in range [0..1])
	static const string lastLuminanceMax_ps =
	"#version 460				\n\
	uniform sampler2D color;	\n\
	uniform vec4 offset;		\n\
	\n\
	const vec4 luminance = vec4(0.299, 0.587, 0.114, 0.0);\n\
	\n\
	layout(location = 1) in vec4 g_TexCoord; \n\
	layout(location = 0) out vec4 o_Color;	\n\
	\n\
	void main(void)			\n\
	{						\n\
		vec4 c = texture(color,g_TexCoord.xy - offset.xy); \n\
		float LMax = dot(c,luminance); \n\
		\n\
		c = texture(color,g_TexCoord.xy + offset.zy); \n\
		LMax = max(LMax,dot(c,luminance)); \n\
		\n\
		c = texture(color,g_TexCoord.xy - offset.zy); \n\
		LMax = max(LMax,dot(c,luminance)); \n\
		\n\
		c = texture(color,g_TexCoord.xy + offset.xy); \n\
		LMax = max(LMax,dot(c,luminance)); \n\
		LMax = 1.0 / max(1.0,LMax); \n\
		o_Color = vec4(LMax,LMax,LMax,1.0); \n\
	}";

#elif defined(GL_ARB_vertex_shader)
	#include "HDR.programs"
#elif defined(GL_ARB_vertex_program)
	#include "HDR.shaders"
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

#if defined(GL_ARB_geometry_shader4)
	m_pBlenderX = NULL;
	m_pBlenderY = NULL;
#else
	m_pBlurXOffsets = NULL;
	m_pBlurYOffsets = NULL;
	m_pBlur = NULL;
#endif
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
			glDrawBuffer();
        else if (i<nLevels-1)
        {
#if defined(GL_ARB_vertex_shader) || defined(GL_ARB_geometry_shader4)
			m_maxLuminance->glGetFragmentProgram()->setProgramParameters(maxLuminanceParams);
#elif defined(GL_ARB_vertex_program)
            m_maxLuminance->glGetFragmentShader()->setProgramParameters(maxLuminanceParams);
#endif
            m_maxLuminance->glRender();
			glDrawFilter();
            m_maxLuminance->glStop();
        }
        else
        {
#if defined(GL_ARB_vertex_shader) || defined(GL_ARB_geometry_shader4)
			m_lastMaxLuminance->glGetFragmentProgram()->setProgramParameters(maxLuminanceParams);
#elif defined(GL_ARB_vertex_program)
            m_lastMaxLuminance->glGetFragmentShader()->setProgramParameters(maxLuminanceParams);
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
	m_pTreshholdFreqs->glGetFragmentProgram()->setProgramParameters(thresholdParams);
#elif defined(GL_ARB_vertex_program)
    m_pTreshholdFreqs->glGetFragmentShader()->setProgramParameters(threshholdParams);
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
	m_pBlenderX->glGetFragmentProgram()->setProgramParameters(blurOffsets);
	m_pBlenderX->glRender();
	glDrawFilter();
	m_pBlenderX->glStop();
#else
    m_pBlurXOffsets->setProgramParameters(blurOffsets);
    m_pBlurXOffsets->glRender();
    m_pBlur->glRender();
	glDrawBuffer();
    m_pBlurXOffsets->glStop();
    m_pBlur->glStop();
#endif

	m_pDownBlurXDisplay->glvkUnBindDisplay();
	m_pDownBlurYDisplay->glvkBindDisplay(nodevice);
	m_pDownBlurXBuffer->glvkRender();

#if defined(GL_ARB_geometry_shader4)
	m_pBlenderY->glGetFragmentProgram()->setProgramParameters(blurOffsets);
	m_pBlenderY->glRender();
	glDrawFilter();
	m_pBlenderY->glStop();
#else
    m_pBlurYOffsets->setProgramParameters(blurOffsets);
    m_pBlurYOffsets->glRender();
    m_pBlur->glRender();
	glDrawBuffer();
    m_pBlurYOffsets->glStop();
    m_pBlur->glStop();
#endif
	m_pDownBlurYDisplay->glvkUnBindDisplay();

	for (unsigned int i=2;i<=nBlurPass;i++)
	{
		//
		//  Second pass Blur high frequencies  
		//
		m_pDownBlurXDisplay->glvkBindDisplay(nodevice);
		m_pDownBlurYBuffer->glvkRender();
#if defined(GL_ARB_geometry_shader4)
		m_pBlenderX->glRender();
		glDrawFilter();
		m_pBlenderX->glStop();
#else
		m_pBlurXOffsets->setProgramParameters(blurOffsets);
		m_pBlurXOffsets->glRender();
		m_pBlur->glRender();
		glDrawBuffer();
		m_pBlurXOffsets->glStop();
#endif
		m_pDownBlurXDisplay->glvkUnBindDisplay();
		m_pDownBlurYDisplay->glvkBindDisplay(nodevice);
		m_pDownBlurXBuffer->glvkRender();

#if defined(GL_ARB_geometry_shader4)
		m_pBlenderY->glRender();
		glDrawFilter();
		m_pBlenderY->glStop();
#else
		m_pBlurYOffsets->setProgramParameters(blurOffsets);
		m_pBlurYOffsets->glRender();
		glDrawBuffer();
		m_pBlurYOffsets->glStop();
		m_pBlur->glStop();
#endif
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
/*
	glEnable(GL_TEXTURE_2D);
	//m_pDownSizedBuffer[nLevels-1]->glRender();
    m_pDownBlurYBuffer->glRender();
	//m_pDownHFBuffer->glRender();
	glDrawBuffer();
*/
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
	thresholdParams.addParameter("offset", GL_COORD_VERTEX(	1.0f / (filter_cs_width * 2.0f * BLUR_BUFFER_SIZE_FACTOR),
															1.0f / (filter_cs_height * 2.0f * BLUR_BUFFER_SIZE_FACTOR),
															-1.0f / (filter_cs_width * 2.0f * BLUR_BUFFER_SIZE_FACTOR),
															0.0f));
	thresholdParams.addParameter("treshhold", thresholdParam.p);

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
	CVertexProgram *vp = m_pBlenderX->glGetVertexProgram("EMPTY_PROGRAM");
	CGeometryProgram *gp = m_pBlenderX->glGetGeometryProgram("FULL_SCREEN_GEO_PROGRAM");
	CFragmentProgram *fp = m_pBlenderX->glGetFragmentProgram("BLENDER_8X_TEX_PROGRAM");
	bool res = m_pBlenderX->glCompileShader();
	m_pBlenderY = new CShader("HDR_BLENDER_Y");
	vp = m_pBlenderY->glGetVertexProgram("EMPTY_PROGRAM");
	gp = m_pBlenderY->glGetGeometryProgram("FULL_SCREEN_GEO_PROGRAM");
	fp = m_pBlenderY->glGetFragmentProgram("BLENDER_8Y_TEX_PROGRAM");
	res = res && m_pBlenderY->glCompileShader();
	blurOffsets.addParameter("color", CTextureUnitSetup::IMAGE_UNIT_0);




	m_pTreshholdFreqs = new CShader("HDR_TRESHOLDS");
	vp = m_pTreshholdFreqs->glGetVertexProgram("EMPTY_PROGRAM");
	gp = m_pTreshholdFreqs->glGetGeometryProgram("FULL_SCREEN_GEO_PROGRAM");
	// Set it only once since it is a library shader (move this call to the rendering stage)
	res = gp->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);
	fp = m_pTreshholdFreqs->glGetFragmentProgram("treshhold2_fp");
	res = fp->glLoadProgram(treshhold2_ps);
	if (res)
	{
		thresholdParams.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
		fp->setProgramParameters(thresholdParams);
	}
	res = res && m_pTreshholdFreqs->glCompileShader();

	m_pComposite = new CShader("HDR_COMPOSITE");
	vp = m_pComposite->glGetVertexProgram("EMPTY_PROGRAM");
	gp = m_pComposite->glGetGeometryProgram("FULL_SCREEN_GEO_PROGRAM");
	fp = m_pComposite->glGetFragmentProgram("composite_fp");
	res = res && fp->glLoadProgram(composer_ps);
	if (res)
	{
		CProgramParameters	fp_params;
		fp_params.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
		fp_params.addParameter("blur",CTextureUnitSetup::IMAGE_UNIT_1);
		fp_params.addParameter("lwhite",CTextureUnitSetup::IMAGE_UNIT_2);
		fp->setProgramParameters(fp_params);
	}
	res = res && m_pComposite->glCompileShader();

	m_maxLuminance = new CShader("HDR_MAXLUMINANCE");
	maxLuminanceParams.addParameter("offset", luminanceParams.p);
	vp = m_maxLuminance->glGetVertexProgram("EMPTY_PROGRAM");
	gp = m_maxLuminance->glGetGeometryProgram("FULL_SCREEN_GEO_PROGRAM");
	fp = m_maxLuminance->glGetFragmentProgram("luminanceMax_fp");
	res = res && fp->glLoadProgram(luminanceMax_ps);
	if (res)
	{
		maxLuminanceParams.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
		fp->setProgramParameters(maxLuminanceParams);
	}
	res = res && m_maxLuminance->glCompileShader();

	m_lastMaxLuminance = new CShader("HDR_LASTMAXLUMINANCE");
	vp = m_lastMaxLuminance->glGetVertexProgram("EMPTY_PROGRAM");
	gp = m_lastMaxLuminance->glGetGeometryProgram("FULL_SCREEN_GEO_PROGRAM");
	fp = m_lastMaxLuminance->glGetFragmentProgram("lastluminanceMax_fp");
	res = res && fp->glLoadProgram(lastLuminanceMax_ps);
	if (res)
		fp->setProgramParameters(maxLuminanceParams);
	res = res && m_lastMaxLuminance->glCompileShader();

	return res;
}

#elif defined(GL_ARB_vertex_shader)

	bool CHDRFilter::glBuildShaders()
	{
		CShader *shaderLib = new CShader();
		m_pBlur = shaderLib->glGetFragmentShader("BLENDER_8X");
		m_pBlurXOffsets = shaderLib->glGetVertexShader("BLENDER_8X_XOFFSETS");
		shaderLib->releaseReference();
		shaderLib = new CShader();
		m_pBlurYOffsets = shaderLib->glGetVertexShader("BLENDER_8X_YOFFSETS");
		shaderLib->releaseReference();

		m_pTreshholdFreqs = new CShader("HDR_TRESHOLDS");
		CFragmentProgram *fp = m_pTreshholdFreqs->glGetFragmentProgram("treshhold2_fp");
		bool res = fp->glLoadProgram(treshhold2_ps);
		if (res)
		{
			thresholdParams.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
			fp->setProgramParameters(thresholdParams);
		}
		res = res && m_pTreshholdFreqs->glCompileShader();

		m_pComposite = new CShader("HDR_COMPOSITE");
		fp = m_pComposite->glGetFragmentProgram("composite_fp");
		res = res && fp->glLoadProgram(composer_ps);
		if (res)
		{
			CProgramParameters	fp_params;
			fp_params.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
			fp_params.addParameter("blur",CTextureUnitSetup::IMAGE_UNIT_1);
			fp_params.addParameter("lwhite",CTextureUnitSetup::IMAGE_UNIT_2);
			fp->setProgramParameters(fp_params);
		}
		res = res && m_pComposite->glCompileShader();

		m_maxLuminance = new CShader("HDR_MAXLUMINANCE");
		maxLuminanceParams.addParameter("offset", luminanceParams.p);
		fp = m_maxLuminance->glGetFragmentProgram("luminanceMax_fp");
		res = res && fp->glLoadProgram(luminanceMax_ps);
		if (res)
		{
			maxLuminanceParams.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
			fp->setProgramParameters(maxLuminanceParams);
		}
		res = res && m_maxLuminance->glCompileShader();

		m_lastMaxLuminance = new CShader("HDR_LASTMAXLUMINANCE");
		fp = m_lastMaxLuminance->glGetFragmentProgram("lastluminanceMax_fp");
		res = res && fp->glLoadProgram(lastLuminanceMax_ps);
		if (res)
			fp->setProgramParameters(maxLuminanceParams);
		res = res && m_lastMaxLuminance->glCompileShader();

		return res;
	}

#elif defined(GL_ARB_vertex_program)

	bool CHDRFilter::glBuildShaders()
	{
		CShader *shaderLib = new CShader();
		m_pBlur = shaderLib->glGetFragmentShader("BLENDER_8X");
		m_pBlurXOffsets = shaderLib->glGetVertexShader("BLENDER_8X_XOFFSETS");
		shaderLib->releaseReference();
		shaderLib = new CShader();
		m_pBlurYOffsets = shaderLib->glGetVertexShader("BLENDER_8X_YOFFSETS");
		shaderLib->releaseReference();

		m_pTreshholdFreqs = new CShader("HDR_TRESHOLDS");
		CFragmentShader *fs = m_pTreshholdFreqs->glGetFragmentShader("treshhold2_fs");
		bool res = res && fs->glLoadProgram(treshhold2_fp);

		m_pComposite = new CShader("HDR_COMPOSITE");
		CFragmentShader *fs = m_pComposite->glGetFragmentShader("composite_fs");
		res = res && fs->glLoadProgram(composer_fp);

		m_maxLuminance = new CShader("HDR_MAXLUMINANCE");
		maxLuminanceParams.addParameter("offset", luminanceParams.p);
		fs = m_maxLuminance->glGetFragmentShader("luminanceMax_fs");
		res = res && fs->glLoadProgram(luminanceMax_fp);

		m_lastMaxLuminance = new CShader("HDR_LASTMAXLUMINANCE");
		fs = m_lastMaxLuminance->glGetFragmentShader("lastluminanceMax_fs");
		res = res && fs->glLoadProgram(lastLuminanceMax_fp);

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

    if (m_pDownBlurXDisplay != NULL)
        Raptor::glDestroyDisplay(m_pDownBlurXDisplay);
    if (m_pDownBlurYDisplay != NULL)
        Raptor::glDestroyDisplay(m_pDownBlurYDisplay);
	if (m_pDownHighFreqs != NULL)
		Raptor::glDestroyDisplay(m_pDownHighFreqs);
	if (m_pDownBlurXBuffer != NULL)
		m_pDownBlurXBuffer->releaseReference();
	if (m_pDownBlurYBuffer != NULL)
		m_pDownBlurYBuffer->releaseReference();
	if (m_pDownHFBuffer != NULL)
		m_pDownHFBuffer->releaseReference();
	if (m_pComposite != NULL)
		m_pComposite->releaseReference();
	if (m_pTreshholdFreqs != NULL)
		m_pTreshholdFreqs->releaseReference();
	if (m_maxLuminance != NULL)
		m_maxLuminance->releaseReference();
	if (m_lastMaxLuminance != NULL)
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

