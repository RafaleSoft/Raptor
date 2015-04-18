// HDRFilter.cpp: implementation of the CHDRFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

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
#if !defined(AFX_VERTEXSHADER_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
  #include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_HDRFILTER_H__6E9B4FC8_154A_46DD_A218_6BC438E45C6A__INCLUDED_)
    #include "HDRFilter.h"
#endif
#if !defined(AFX_COLOR_H__3770AC59_0D0E_49EF_99C8_037268A33CE4__INCLUDED_)
	#include "System/Color.h"
#endif


static const string composer =
"!!ARBfp1.0 \
ATTRIB iTex = fragment.texcoord[0]; \
PARAM one = { 1.0 , 1.0, 1.0, 1.0 }; \
PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };\
TEMP color; \
TEMP blur; \
TEMP lwhite; \
TEMP l; \
TEMP factor; \
OUTPUT finalColor = result.color; \
TEX color, iTex, texture[0], 2D ; \
TEX blur, iTex, texture[1], 2D ; \
TEX lwhite, iTex, texture[2], 2D; \
DP3 l, color, luminance; \
MUL l, l, lwhite; \
MAD finalColor, color, lwhite, blur; \
MOV finalColor.w, one.w; \
END" ;

//  Compute luminance with bilinear components, averaging
//  the 4 surrouding of the texel's center.
static const string treshhold2 = 
"!!ARBfp1.0 \
ATTRIB iTex = fragment.texcoord[0]; \
PARAM offset = program.local[0];\
PARAM treshhold = program.local[1];\
PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };\
TEMP color1; \
TEMP color2; \
TEMP color3; \
TEMP color4; \
TEMP L;\
TEMP tCoord; \
TEMP color; \
OUTPUT finalColor = result.color; \
\
SUB tCoord, iTex, offset.xyww; \
TEX color1, tCoord , texture[0] , 2D ; \
DP3 L.x, color1, luminance; \
\
ADD tCoord, iTex, offset.zyww; \
TEX color2, tCoord , texture[0] , 2D ; \
DP3 L.y, color2, luminance; \
\
SUB tCoord, iTex, offset.zyww; \
TEX color3, tCoord , texture[0] , 2D ; \
DP3 L.z, color3, luminance; \
\
ADD tCoord, iTex, offset.xyww; \
TEX color4, tCoord , texture[0] , 2D ; \
DP3 L.w, color4, luminance; \
\
SUB L, L, treshhold.y; \
MAX L, L, luminance.w; \
MUL color, color1, L.x; \
MAD color, color2, L.y, color; \
MAD color, color3, L.z, color; \
MAD color, color4, L.w, color; \
MOV finalColor, color; \
MOV finalColor.w, treshhold.w ;\
END" ;

//PARAM quart = { 0.25, 0.25, 0.25, 0.25 };\
//MUL finalColor, color, quart; \


static const string luminanceMax = 
"!!ARBfp1.0 \
ATTRIB iTex = fragment.texcoord[0]; \
PARAM offset = program.local[0];\
PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };\
TEMP L;\
TEMP LMax; \
TEMP tCoord; \
TEMP color; \
OUTPUT finalColor = result.color; \
\
SUB tCoord, iTex, offset.xyww; \
TEX color, tCoord , texture[0] , 2D ; \
DP3 LMax, color, luminance; \
\
ADD tCoord, iTex, offset.zyww; \
TEX color, tCoord , texture[0] , 2D ; \
DP3 L, color, luminance; \
MAX LMax, L, LMax; \
\
SUB tCoord, iTex, offset.zyww; \
TEX color, tCoord , texture[0] , 2D ; \
DP3 L, color, luminance; \
MAX LMax, L, LMax; \
\
ADD tCoord, iTex, offset.xyww; \
TEX color, tCoord , texture[0] , 2D ; \
DP3 L, color, luminance; \
MAX finalColor, L, LMax; \
\
MOV finalColor.w, luminance.w ;\
END" ;

static const string lastLuminanceMax = 
"!!ARBfp1.0 \
ATTRIB iTex = fragment.texcoord[0]; \
PARAM offset = program.local[0];\
PARAM luminance = { 0.299, 0.587, 0.114, 1.0 };\
TEMP L;\
TEMP LMax; \
TEMP tCoord; \
TEMP color; \
OUTPUT finalColor = result.color; \
\
SUB tCoord, iTex, offset.xyww; \
TEX color, tCoord , texture[0] , 2D ; \
DP3 LMax, color, luminance; \
\
ADD tCoord, iTex, offset.zyww; \
TEX color, tCoord , texture[0] , 2D ; \
DP3 L, color, luminance; \
MAX LMax, L, LMax; \
\
SUB tCoord, iTex, offset.zyww; \
TEX color, tCoord , texture[0] , 2D ; \
DP3 L, color, luminance; \
MAX LMax, L, LMax; \
\
ADD tCoord, iTex, offset.xyww; \
TEX color, tCoord , texture[0] , 2D ; \
DP3 L, color, luminance; \
MAX LMax, L, LMax; \
MAX LMax.x, LMax.x, luminance.w; \
RCP finalColor, LMax.x; \
\
MOV finalColor.w, luminance.w ;\
END" ;


static const float BLUR_BUFFER_SIZE_FACTOR = 0.125f;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHDRFilter::CHDRFilter(const CRaptorDisplayConfig &da)
{
    rda = da;
    rda.x = 0;
    rda.y = 0;
    rda.refresh_rate.fps = CGL_MAXREFRESHRATE;
    rda.display_mode =	CGL_RENDER_TEXTURE | 
						CGL_HARDWARE |
                        (da.display_mode & (CGL_RGB|CGL_RGBA|CGL_FLOAT));

#if defined(GL_EXT_framebuffer_object)
	if ((Raptor::glIsExtensionSupported("GL_EXT_framebuffer_object")) &&
		((da.display_mode & CGL_RENDER_BUFFER) == CGL_RENDER_BUFFER))
		rda.display_mode |= CGL_RENDER_BUFFER;
#endif

    rda.frame_mode = CGL_NOSTATUS;

	nLevels = 0;
	nBlurPass = 1;
	treshold = 0.5;
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
	treshold = MAX(0,t);
}

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
	{
        Raptor::glDestroyDisplay(m_pDownBlurXDisplay);
		m_pDownBlurXDisplay = NULL;
	}
    if (m_pDownBlurYDisplay != NULL)
	{
        Raptor::glDestroyDisplay(m_pDownBlurYDisplay);
		m_pDownBlurYDisplay = NULL;
	}
    if (m_pDownBlurXBuffer != NULL)
	{
        m_pDownBlurXBuffer->releaseReference();
		m_pDownBlurXBuffer = NULL;
	}
    if (m_pDownBlurYBuffer != NULL)
	{
        m_pDownBlurYBuffer->releaseReference();
		m_pDownBlurYBuffer = NULL;
	}
    if (m_pDownHighFreqs != NULL)
	{
        Raptor::glDestroyDisplay(m_pDownHighFreqs);
		m_pDownHighFreqs = NULL;
	}
    if (m_pDownHFBuffer != NULL)
	{
        m_pDownHFBuffer->releaseReference();
		m_pDownHFBuffer = NULL;
	}

    if (m_pComposite != NULL)
	{
        delete m_pComposite;
		m_pComposite = NULL;
	}
    if (m_pTreshholdFreqs != NULL)
	{
        delete m_pTreshholdFreqs;
		m_pTreshholdFreqs = NULL;
	}
    if (m_maxLuminance != NULL)
	{
        delete m_maxLuminance;
		m_maxLuminance = NULL;
	}
    if (m_lastMaxLuminance != NULL)
	{
        delete m_lastMaxLuminance;
		m_lastMaxLuminance = NULL;
	}
}

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

void CHDRFilter::glRenderFilter()
{
    if (getColorInput() == NULL)
        return;

    //  Downsize buffers to compute global intensity
    RAPTOR_HANDLE nodevice;
    CTextureObject  *currentBuffer = getColorInput();
    for (unsigned int i=0;i<nLevels;i++)
    {
        m_pDownSizedDisplay[i]->glBindDisplay(nodevice);
        currentBuffer->glRender();

        if (i < 2)
            glDrawBuffer();
        else if (i<nLevels-1)
        {
            maxLuminanceParams[0].vector = GL_COORD_VERTEX(	0.5f / currentBuffer->getWidth(), 
																	0.5f / currentBuffer->getHeight() ,
																	-0.5f / currentBuffer->getWidth() ,
																	0.0f);

            m_maxLuminance->setProgramParameters(maxLuminanceParams);
            m_maxLuminance->glRender();
            glDrawBuffer();
            m_maxLuminance->glStop();
        }
        else
        {
            maxLuminanceParams[0].vector = GL_COORD_VERTEX(	0.5f / currentBuffer->getWidth(), 
																	0.5f / currentBuffer->getHeight() ,
																	-0.5f / currentBuffer->getWidth() ,
																	0.0f);

            m_lastMaxLuminance->setProgramParameters(maxLuminanceParams);
            m_lastMaxLuminance->glRender();
            glDrawBuffer();
            m_lastMaxLuminance->glStop();
        }

        m_pDownSizedDisplay[i]->glUnBindDisplay();
        currentBuffer = m_pDownSizedBuffer[i];
    }

    //
    //  Extract high frequencies only
    //
    m_pDownHighFreqs->glBindDisplay(nodevice);
    m_pDownSizedBuffer[1]->glRender();
	threshholdParams[1].vector = GL_COORD_VERTEX(1.0f, treshold, 1.0f, 1.0f);
    m_pTreshholdFreqs->setProgramParameters(threshholdParams);
    m_pTreshholdFreqs->glRender();
    glDrawBuffer();
    m_pTreshholdFreqs->glStop();
    m_pDownHighFreqs->glUnBindDisplay();
  
    //
    //  Blur high frequencies  
    //
    m_pDownBlurXDisplay->glBindDisplay(nodevice);
    m_pDownHFBuffer->glRender();
    m_pBlurXOffsets->setProgramParameters(blurOffsets);
    m_pBlurXOffsets->glRender();
    m_pBlur->glRender();

    glDrawBuffer();

    m_pBlurXOffsets->glStop();
    m_pBlur->glStop();
    m_pDownBlurXDisplay->glUnBindDisplay();

    m_pDownBlurYDisplay->glBindDisplay(nodevice);
    m_pDownBlurXBuffer->glRender();
    m_pBlurYOffsets->setProgramParameters(blurOffsets);
    m_pBlurYOffsets->glRender();
    m_pBlur->glRender();

    glDrawBuffer();

    m_pBlurYOffsets->glStop();
    m_pBlur->glStop();
    m_pDownBlurYDisplay->glUnBindDisplay();

	for (unsigned int i=2;i<=nBlurPass;i++)
	{
		//
		//  Second pass Blur high frequencies  
		//
		m_pDownBlurXDisplay->glBindDisplay(nodevice);
		m_pDownBlurYBuffer->glRender();
		m_pBlurXOffsets->setProgramParameters(blurOffsets);
		m_pBlurXOffsets->glRender();
		m_pBlur->glRender();

		glDrawBuffer();

		m_pBlurXOffsets->glStop();
		m_pBlur->glStop();
		m_pDownBlurXDisplay->glUnBindDisplay();

		m_pDownBlurYDisplay->glBindDisplay(nodevice);
		m_pDownBlurXBuffer->glRender();
		m_pBlurYOffsets->setProgramParameters(blurOffsets);
		m_pBlurYOffsets->glRender();
		m_pBlur->glRender();

		glDrawBuffer();

		m_pBlurYOffsets->glStop();
		m_pBlur->glStop();
		m_pDownBlurYDisplay->glUnBindDisplay();
	}
}

static float minl = 1000.0f;
void CHDRFilter::glRenderFilterOutput()
{
	//
    // Final rendering : composite global illumination with gloom and original HDR scene.
    //
	const CRaptorExtensions*   const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
    m_pDownSizedBuffer[nLevels-1]->glRender();    // texture is a single texel with luminance max.
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    m_pDownBlurYBuffer->glRender();

/*
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	//m_pDownSizedBuffer[nLevels-1]->glRender();
    m_pDownBlurYBuffer->glRender();
	//m_pDownHFBuffer->glRender();
	glDrawBuffer();
*/

    glActiveTextureARB(GL_TEXTURE0_ARB);
    getColorInput()->glRender();

    // compose blur + hdr
    m_pComposite->glRender();
    glDrawBuffer();
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
		colorInput = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															CTextureObject::CGL_BILINEAR,
															colorExternalSource);
	}
	else if (m_fModel == RENDER_BUFFER)
	{
		m_pDownSizedAttachments = new CTextureSet*[nLevels];
	}

    CRenderingProperties commonRP;
    commonRP.setTexturing(CRenderingProperties::ENABLE);
    commonRP.setCullFace(CRenderingProperties::DISABLE);
    commonRP.setDepthTest(CRenderingProperties::DISABLE);
    commonRP.setLighting(CRenderingProperties::DISABLE);
    commonRP.clear(CGL_NULL);

    for (unsigned int i=0; i<nLevels; i++)
    {
        width = MAX(1,width/2);
        height = MAX(1,height/2);

        rda.width = width;
        rda.height = height;

        m_pDownSizedDisplay[i] = Raptor::glCreateDisplay(rda);
        CRenderingProperties *rp = m_pDownSizedDisplay[i]->getRenderingProperties();
        *rp = commonRP;
        m_pDownSizedDisplay[i]->setViewPoint(NULL);
        CTextureObject::TEXTURE_FILTER filter = ( i < 2 ? CTextureObject::CGL_BILINEAR : CTextureObject::CGL_UNFILTERED);

		if (m_fModel == RENDER_BUFFER)
		{
			m_pDownSizedBuffer[i] = filterFactory.glCreateTexture(	CTextureObject::CGL_COLOR_FLOAT16_ALPHA,
																	CTextureObject::CGL_OPAQUE,
																	filter);
			filterFactory.glResizeTexture(m_pDownSizedBuffer[i],width,height);
			CTextureSet *tset = new CTextureSet("HDR_TSet");
			tset->addTexture(m_pDownSizedBuffer[i]);
			m_pDownSizedDisplay[i]->glBindDisplay(*tset);
			m_pDownSizedAttachments[i] = tset;
		}
		else if (m_fModel == RENDER_TEXTURE)
		{
			m_pDownSizedBuffer[i] = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
																			CTextureObject::CGL_OPAQUE,
																			filter, 
																			m_pDownSizedDisplay[i]);
		}
    }

    rda.width = MAX(1,filter_cs_width * BLUR_BUFFER_SIZE_FACTOR);
    rda.height = MAX(1,filter_cs_height * BLUR_BUFFER_SIZE_FACTOR);
    m_pDownBlurXDisplay = Raptor::glCreateDisplay(rda);
    CRenderingProperties *rp = m_pDownBlurXDisplay->getRenderingProperties();
    *rp = commonRP;
    m_pDownBlurXDisplay->setViewPoint(NULL);
	if (m_fModel == RENDER_BUFFER)
	{
		m_pDownBlurXBuffer = filterFactory.glCreateTexture(	CTextureObject::CGL_COLOR_FLOAT16_ALPHA,
															CTextureObject::CGL_OPAQUE,
															CTextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(m_pDownBlurXBuffer,rda.width,rda.height);
		m_pDownBlurXBuffer->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
		CTextureSet *tset = new CTextureSet("HDR_XBlur");
		tset->addTexture(m_pDownBlurXBuffer);
		m_pDownBlurXDisplay->glBindDisplay(*tset);
	}
	else if (m_fModel == RENDER_TEXTURE)
	{
		m_pDownBlurXBuffer = filterFactory.glCreateDynamicTexture(CTextureObject::CGL_COLOR24_ALPHA,	
																  CTextureObject::CGL_OPAQUE,
																  CTextureObject::CGL_BILINEAR,
																  m_pDownBlurXDisplay);
	}

    m_pDownBlurYDisplay = Raptor::glCreateDisplay(rda);
    rp = m_pDownBlurYDisplay->getRenderingProperties();
    *rp = commonRP;
    m_pDownBlurYDisplay->setViewPoint(NULL);
	if (m_fModel == RENDER_BUFFER)
	{
		m_pDownBlurYBuffer = filterFactory.glCreateTexture(	CTextureObject::CGL_COLOR_FLOAT16_ALPHA,
															CTextureObject::CGL_OPAQUE,
															CTextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(m_pDownBlurYBuffer,rda.width,rda.height);
		m_pDownBlurYBuffer->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
		CTextureSet *tset = new CTextureSet("HDR_YBlur");
		tset->addTexture(m_pDownBlurYBuffer);
		m_pDownBlurYDisplay->glBindDisplay(*tset);
	}
	else if (m_fModel == RENDER_TEXTURE)
	{
		m_pDownBlurYBuffer = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
																	CTextureObject::CGL_OPAQUE,
																	CTextureObject::CGL_BILINEAR,
																	m_pDownBlurYDisplay);
	}

    m_pDownHighFreqs  = Raptor::glCreateDisplay(rda);
    rp = m_pDownHighFreqs->getRenderingProperties();
    *rp = commonRP;
    m_pDownHighFreqs->setViewPoint(NULL);
	if (m_fModel == RENDER_BUFFER)
	{
		m_pDownHFBuffer = filterFactory.glCreateTexture(CTextureObject::CGL_COLOR_FLOAT16_ALPHA,
														CTextureObject::CGL_OPAQUE,
														CTextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(m_pDownHFBuffer,rda.width,rda.height);
		m_pDownHFBuffer->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
		CTextureSet *tset = new CTextureSet("HDR_HighFrequencies");
		tset->addTexture(m_pDownHFBuffer);
		m_pDownHighFreqs->glBindDisplay(*tset);
	}
	else if (m_fModel == RENDER_TEXTURE)
	{
		m_pDownHFBuffer = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
																CTextureObject::CGL_OPAQUE,
																CTextureObject::CGL_BILINEAR,
																m_pDownHighFreqs);
	}

    //
    //  Create shaders
    //
    CShader *shaderLib = new CShader();
    m_pBlur = shaderLib->glGetFragmentShader("BLENDER_8X");
    m_pBlurXOffsets = shaderLib->glGetVertexShader("BLENDER_8X_XOFFSETS");
    shaderLib->releaseReference();
    shaderLib = new CShader();
    m_pBlurYOffsets = shaderLib->glGetVertexShader("BLENDER_8X_YOFFSETS");
    shaderLib->releaseReference();
    m_pComposite = new CFragmentShader("HDR_COMPOSITE");
    res = res && m_pComposite->glLoadProgram(composer);
    m_pTreshholdFreqs = new CFragmentShader("HDR_TRESHOLDS");
    res = res && m_pTreshholdFreqs->glLoadProgram(treshhold2);

    blurOffsets.addParameter("",GL_COORD_VERTEX(1.0f / (filter_cs_width * BLUR_BUFFER_SIZE_FACTOR), 
												1.0f / (filter_cs_height * BLUR_BUFFER_SIZE_FACTOR),
												0.0f, 1.0f));
	
	threshholdParams.addParameter("",GL_COORD_VERTEX(1.0f / (filter_cs_width * 2.0f * BLUR_BUFFER_SIZE_FACTOR), 
													1.0f / (filter_cs_height * 2.0f * BLUR_BUFFER_SIZE_FACTOR),
													-1.0f / (filter_cs_width * 2.0f * BLUR_BUFFER_SIZE_FACTOR),
													0.0f));
	threshholdParams.addParameter("",GL_COORD_VERTEX(1.0f, treshold, 1.0f, 1.0f));


    m_maxLuminance = new CFragmentShader("HDR_MAXLUMINANCE");
    res = res && m_maxLuminance->glLoadProgram(luminanceMax);
	maxLuminanceParams.addParameter("",GL_COORD_VERTEX(0.0f,0.0f,0.0f,1.0f));

    m_lastMaxLuminance = new CFragmentShader("HDR_LASTMAXLUMINANCE");
    res = res && m_lastMaxLuminance->glLoadProgram(lastLuminanceMax);

    rda.width = filter_cs_width;
    rda.height = filter_cs_height;

	filterFactory.getConfig().useTextureResize(previousResize);

    return res;
}



/*
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

