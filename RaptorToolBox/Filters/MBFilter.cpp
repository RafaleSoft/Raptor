/***************************************************************************/
/*                                                                         */
/*  MBFilter.cpp                                                           */
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
#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexProgram.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
	#include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_GEOMETRYPROGRAM_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
	#include "GLHierarchy/GeometryProgram.h"
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
	static const string accum_gp;
	static const string accum_fp;
	static const string accum_fp2;

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


#if defined(GL_ARB_geometry_shader4)
	const string CAccumulator::accum_gp =
	"#version 440\n\
	\n\
	//	Expect the geometry shader extension to be available, warn if not. \n\
	#extension GL_ARB_geometry_shader4 : enable \n\
	\n\
	layout(points) in; \n\
	layout(triangle_strip, max_vertices=4) out; \n\
	layout(location = 1) out vec4 g_TexCoord; \n\
	\n\
	void main() \n\
	{\n\
		gl_Position = vec4(-1.0, -1.0, 0.0, 1.0); \n\
		g_TexCoord = vec4(0.0,0.0,0.0,0.0); \n\
		EmitVertex(); \n\
		\n\
		gl_Position = vec4(1.0, -1.0, 0.0, 1.0); \n\
		g_TexCoord = vec4(1.0,0.0,0.0,0.0); \n\
		EmitVertex(); \n\
		\n\
		gl_Position = vec4(-1.0, 1.0, 0.0, 1.0); \n\
		g_TexCoord = vec4(0.0, 1.0, 0.0, 0.0); \n\
		EmitVertex(); \n\
		\n\
		gl_Position = vec4(1.0, 1.0, 0.0, 1.0); \n\
		g_TexCoord = vec4(1.0, 1.0, 0.0, 0.0); \n\
		EmitVertex(); \n\
		\n\
		EndPrimitive(); \n\
	}";

	const string CAccumulator::accum_fp =
	"#version 440 			\n\
	\n\
	uniform sampler2D color;	\n\
	uniform sampler2D accum;	\n\
	uniform vec4 percentage;	\n\
	\n\
	layout(location = 1) in vec4 g_TexCoord; \n\
	layout(location = 0) out vec4 o_Color;	\n\
	void main(void)			\n\
	{						\n\
		vec4 c = texture(color,g_TexCoord.xy); \n\
		vec4 a = texture(accum,g_TexCoord.xy); \n\
		o_Color = mix(c,a,percentage); \n\
	}";

	const string CAccumulator::accum_fp2 =
	"#version 440\n\
	\n\
	uniform	sampler2D diffuseMap; \n\
	\n\
	in vec4 g_color; \n\
	layout(location = 1) in vec4 g_TexCoord; \n\
	layout(location = 0) out vec4 o_Color;	\n\
	\n\
	void main (void) \n\
	{\n\
		o_Color = texture(diffuseMap,vec2(g_TexCoord.st)); \n\
	}";
#elif defined(GL_ARB_vertex_shader)
	#include "MBFilter.programs"
#elif defined(GL_ARB_vertex_program)
	#error "Unsupported deprecated vertex program feature."
#endif



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

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    // Accumulate previous render with colorSource
    RAPTOR_HANDLE noDevice;
	pAccum->pCurrentDisplay->glvkBindDisplay(noDevice);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
	pAccum->m_pPreviousColorAccum->glvkRender();
    glActiveTextureARB(GL_TEXTURE0_ARB);
	getColorInput()->glvkRender();

	m_pMotionBlurShader->glGetFragmentProgram()->setProgramParameters(f_params);
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

	m_pFinalShader->glGetFragmentProgram()->setProgramParameters(f_params2);
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

	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();

	bool previousResize = filterFactory.getConfig().useTextureResize();
	filterFactory.getConfig().useTextureResize(false);

	if ((colorExternalSource != NULL) && (m_fModel == RENDER_TEXTURE))
	{
		colorInput = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
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

		CTextureObject *T = NULL;
		T = filterFactory.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
											CTextureObject::CGL_OPAQUE,
											ITextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(T,state.width,state.height);
		T->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
		accumulator->m_pCurrentColorAccum = T;
		m_pRenderTextures->addTexture(T);

		m_pRenderTextures2 = new CTextureSet();
		T = filterFactory.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
											CTextureObject::CGL_OPAQUE,
											ITextureObject::CGL_BILINEAR);
		filterFactory.glResizeTexture(T,state.width,state.height);	
		T->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
		accumulator->m_pPreviousColorAccum = T;
		m_pRenderTextures2->addTexture(T);
	}

    CRaptorDisplay* pDisplay1 = Raptor::glCreateDisplay(state);
	IRenderingProperties &rp1 = pDisplay1->getRenderingProperties();
	rp1.setTexturing(IRenderingProperties::ENABLE);
	rp1.setLighting(IRenderingProperties::DISABLE);
	rp1.setCullFace(IRenderingProperties::DISABLE);
	rp1.setDepthTest(IRenderingProperties::DISABLE);
    rp1.clear(CGL_NULL);
    pDisplay1->setViewPoint(NULL);

	if (m_fModel == RENDER_BUFFER)
		pDisplay1->glvkBindDisplay(*m_pRenderTextures);

    CRaptorDisplay* pDisplay2 = Raptor::glCreateDisplay(state);
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
																					CTextureObject::CGL_OPAQUE,
																					ITextureObject::CGL_BILINEAR,
																					accumulator->pCurrentDisplay);
		accumulator->m_pPreviousColorAccum = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
																					CTextureObject::CGL_OPAQUE,
																					ITextureObject::CGL_BILINEAR,
																					accumulator->pPreviousDisplay);
	}

	m_pMotionBlurShader = new CShader("MB_SHADER");

#if defined(GL_ARB_geometry_shader4)
	m_pFinalShader = new CShader("MotionBlurShader");
	CVertexProgram *vp = m_pMotionBlurShader->glGetVertexProgram("EMPTY_PROGRAM");
	CGeometryProgram *gp = m_pMotionBlurShader->glGetGeometryProgram("mb_gp");
	bool res = gp->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);
	res = res & gp->glLoadProgram(CAccumulator::accum_gp);
	CFragmentProgram *fp = m_pMotionBlurShader->glGetFragmentProgram("mb_fp");
	res = res && fp->glLoadProgram(CAccumulator::accum_fp);
	res = res && m_pMotionBlurShader->glCompileShader();

	vp = m_pFinalShader->glGetVertexProgram("EMPTY_PROGRAM");
	gp = m_pFinalShader->glGetGeometryProgram("mb_gp");
	fp = m_pFinalShader->glGetFragmentProgram("mb_fp2");
	res = res && fp->glLoadProgram(CAccumulator::accum_fp2);
	res = res && m_pFinalShader->glCompileShader();
#elif defined(GL_ARB_vertex_shader)
	CFragmentProgram *fs = m_pMotionBlurShader->glGetFragmentProgram("mb_fp");
	bool res = fs->glLoadProgram(CAccumulator::accum_fp);
	res = res && m_pMotionBlurShader->glCompileShader();
#elif defined(GL_ARB_vertex_program)
	#error "Unsupported deprecated vertex program feature."
#endif

    filterFactory.getConfig().useTextureResize(previousResize);

    return res;
}




