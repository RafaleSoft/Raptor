// MBFilter.cpp: implementation of the CMBFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
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
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_MBFILTER_H__53A619DD_DBAB_4709_9EAD_72C5D6C401E9__INCLUDED_)
    #include "MBFilter.h"
#endif

// Specific texture generator to flip between buffer display of previous frame
class CAccumulator : public ITextureGenerator
{
public:
    CAccumulator():
      pCurrentDisplay(NULL),pPreviousDisplay(NULL),
       m_pCurrentColorAccum(NULL),m_pPreviousColorAccum(NULL){};
    virtual ~CAccumulator() {};

    //! Implements TextureGenerator
    virtual ITextureGenerator::GENERATOR_KIND    getKind(void) const { return ITextureGenerator::BUFFERED; };

    //! The fragment program to compute accumulation;
    static const string accum_ps;

    //! Generation is switched each frame
    virtual void glGenerate(CTextureObject* t)
    { pCurrentDisplay->glGenerate(t); };

    //! This method returns the width of the generator
    virtual unsigned int getGenerateWidth(void) const
    { return pCurrentDisplay->getGenerateWidth(); };

    //! This method returns the height of the generator
    virtual unsigned int getGenerateHeight(void) const
    { return pCurrentDisplay->getGenerateHeight(); };

    void switchDisplay(void)
    {
        CRaptorDisplay *tmpDisplay = pPreviousDisplay;
        pPreviousDisplay = pCurrentDisplay;
        pCurrentDisplay = tmpDisplay;

        CTextureObject  *tmpColorAccum = m_pPreviousColorAccum;
        m_pPreviousColorAccum = m_pCurrentColorAccum;
        m_pCurrentColorAccum = tmpColorAccum;
    }

    //! Flip-flop color output
    CRaptorDisplay *pCurrentDisplay;
    CTextureObject  *m_pCurrentColorAccum;

    CRaptorDisplay *pPreviousDisplay;
    CTextureObject  *m_pPreviousColorAccum;
};

const string CAccumulator::accum_ps =
"#version 120				\n\
uniform sampler2D color;	\n\
uniform sampler2D accum;	\n\
uniform vec4 percentage;	\n\
void main(void)			\n\
{						\n\
	vec4 c = texture2D(color,gl_TexCoord[0].xy); \n\
	vec4 a = texture2D(accum,gl_TexCoord[0].xy); \n\
	gl_FragColor = mix(c,a,percentage); \n\
}";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMBFilter::CMBFilter():
    m_pMotionBlurShader(NULL),m_pAccumulator(NULL),
	m_pRenderTextures2(NULL)
{
	GL_COORD_VERTEX mbParams(0.75f,0.75f,0.75f,1.0f);
	f_params.addParameter("percentage",mbParams);
	f_params.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
	f_params.addParameter("accum",CTextureUnitSetup::IMAGE_UNIT_1);
}

CMBFilter::~CMBFilter()
{
	glDestroyFilter();
}

void CMBFilter::setPercentage(float r,float g,float b,float a)
{
	GL_COORD_VERTEX mbParams(r,g,b,a);
	f_params[0].vector = mbParams;
}

void CMBFilter::getPercentage(float &r,float &g,float &b,float &a)
{
	GL_COORD_VERTEX& mbParams = f_params[0].vector;
    r = mbParams.x;
    g = mbParams.y;
    b = mbParams.z;
    a = mbParams.h;
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

        Raptor::glDestroyDisplay(pAccum->pCurrentDisplay);
        Raptor::glDestroyDisplay(pAccum->pPreviousDisplay);

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

	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    // Accumulate previous render with colorSource
    RAPTOR_HANDLE noDevice;
    pAccum->pCurrentDisplay->glBindDisplay(noDevice);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    pAccum->m_pPreviousColorAccum->glRender();
    glActiveTextureARB(GL_TEXTURE0_ARB);
    getColorInput()->glRender();

	m_pMotionBlurShader->glGetFragmentProgram()->setProgramParameters(f_params);
    m_pMotionBlurShader->glRender();
    glDrawBuffer();
	m_pMotionBlurShader->glStop();

	glBindTexture(GL_TEXTURE_2D,0);
    pAccum->pCurrentDisplay->glUnBindDisplay();
}

void CMBFilter::glRenderFilterOutput()
{
    //  Render final buffer
    CAccumulator *pAccum = (CAccumulator*)m_pAccumulator;

	pAccum->m_pCurrentColorAccum->glRender();
    glDrawBuffer();

	glBindTexture(GL_TEXTURE_2D,0);

    pAccum->pCurrentDisplay->glUnBindDisplay();
    pAccum->pPreviousDisplay->glUnBindDisplay();

    pAccum->switchDisplay();
}


bool CMBFilter::glInitFilter(void)
{
    if (!CRaptorDisplayFilter::glInitFilter())
        return false;

	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();

	bool previousResize = filterFactory.getConfig().useTextureResize();
	filterFactory.getConfig().useTextureResize(false);

	if ((colorExternalSource != NULL) && (m_fModel == RENDER_TEXTURE))
	{
		colorInput = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															CTextureObject::CGL_BILINEAR,
															colorExternalSource);
	}

    CRaptorDisplayConfig state;
    state.width = colorInput->getWidth();
    state.height = colorInput->getHeight();
	state.acceleration = CRaptorDisplayConfig::HARDWARE;
    state.display_mode = CGL_RENDER_TEXTURE | CGL_RGBA ;

	CAccumulator *accumulator = new CAccumulator;

	if (m_fModel == RENDER_BUFFER)
	{
		state.display_mode |= CGL_RENDER_BUFFER;

		CTextureObject *T = NULL;
		T = filterFactory.glCreateTexture(	CTextureObject::CGL_COLOR24_ALPHA,
											CTextureObject::CGL_OPAQUE,
											CTextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(T,state.width,state.height);
		T->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
		accumulator->m_pCurrentColorAccum = T;
		m_pRenderTextures->addTexture(T);

		m_pRenderTextures2 = new CTextureSet();
		T = filterFactory.glCreateTexture(	CTextureObject::CGL_COLOR24_ALPHA,
											CTextureObject::CGL_OPAQUE,
											CTextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(T,state.width,state.height);	
		T->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
		accumulator->m_pPreviousColorAccum = T;
		m_pRenderTextures2->addTexture(T);
	}

    CRaptorDisplay* pDisplay1 = Raptor::glCreateDisplay(state);
    CRenderingProperties *rp1 = pDisplay1->getRenderingProperties();
    rp1->setTexturing(CRenderingProperties::ENABLE);
    rp1->setLighting(CRenderingProperties::DISABLE);
    rp1->setCullFace(CRenderingProperties::DISABLE);
    rp1->setDepthTest(CRenderingProperties::DISABLE);
    rp1->clear(CGL_NULL);
    pDisplay1->setViewPoint(NULL);

	if (m_fModel == RENDER_BUFFER)
		pDisplay1->glBindDisplay(*m_pRenderTextures);

    CRaptorDisplay* pDisplay2 = Raptor::glCreateDisplay(state);
    CRenderingProperties *rp2 = pDisplay2->getRenderingProperties();
    *rp2 = *rp1;
    pDisplay2->setViewPoint(NULL);

	if (m_fModel == RENDER_BUFFER)
		pDisplay2->glBindDisplay(*m_pRenderTextures2);


    accumulator->pCurrentDisplay = pDisplay1;
    accumulator->pPreviousDisplay = pDisplay2;
    m_pAccumulator = accumulator;

	if (m_fModel == RENDER_TEXTURE)
	{
		accumulator->m_pCurrentColorAccum = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
																					CTextureObject::CGL_OPAQUE,
																					CTextureObject::CGL_BILINEAR,
																					accumulator->pCurrentDisplay);
		accumulator->m_pPreviousColorAccum = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
																					CTextureObject::CGL_OPAQUE,
																					CTextureObject::CGL_BILINEAR,
																					accumulator->pPreviousDisplay);
	}

    m_pMotionBlurShader = new CShader("MB_SHADER");
    CFragmentProgram *fs = m_pMotionBlurShader->glGetFragmentProgram("mb_fp");
    bool res = fs->glLoadProgram(CAccumulator::accum_ps);
	res = res && m_pMotionBlurShader->glCompileShader();

	filterFactory.getConfig().useTextureResize(previousResize);

    return res;
}




