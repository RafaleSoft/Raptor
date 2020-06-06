/***************************************************************************/
/*                                                                         */
/*  MagnifierFilter.cpp                                                    */
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
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
    #include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__DD0AD51D_3BFF_4C65_8099_BA7696D7BDDF__INCLUDED_)
	#include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "GLHierarchy/VertexProgram.h"
#endif
#if !defined(AFX_GEOMETRYSHADER_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
	#include "GLHierarchy/GeometryShader.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_MAGNIFIERFILTER_H__3660D446_2F92_4D02_A795_BFF8336D61D2__INCLUDED_)
    #include "MagnifierFilter.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_OPENGLPROGRAMSTAGE_H__0BCE3B42_6E10_4F50_BB27_1993345ADBCF__INCLUDED_)
	#include "GLHierarchy/OpenGLProgramStage.h"
#endif


RAPTOR_NAMESPACE

static const int KERNEL_SIZE = 256;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMagnifierFilter::CMagnifierFilter():
    m_bRebuild(false),kernelTexture(NULL),
    xKernelPass(NULL),xBuffer(NULL),m_pXKernelShader(NULL),m_pYKernelShader(NULL)
{
	kernelBuilder = &CMagnifierFilter::sincKernel;
	kernelParams.x = 0.75f;
	kernelParams.y = 0.0f;
	kernelParams.z = 0.0f;
	kernelParams.h = 0.0f;
}

CMagnifierFilter::~CMagnifierFilter()
{
	glDestroyFilter();
}


void CMagnifierFilter::setSincKernel(float A)
{
    kernelBuilder = &CMagnifierFilter::sincKernel;
    kernelParams.x = A;
    kernelParams.y = 0.0f;
	m_bRebuild = true;
}

void CMagnifierFilter::setMitchellNetravaliKernel(float B, float C)
{
    kernelBuilder = &CMagnifierFilter::mitchellNetravaliKernel;
    kernelParams.x = B;
    kernelParams.y = C;
	m_bRebuild = true;
}

//!
//! Default installed kernels : sinc approximation & mitchell netravelli
//!
float CMagnifierFilter::sincKernel(float x,float A,float ) const
{
    float ax = fabs(x);
    if (ax < 1)
        return (A+2)*ax*ax*ax - (A+3)*ax*ax + 1.0f;
    else if ((ax >=1) && (ax < 2))
        return A*ax*ax*ax - 5*A*ax*ax + 8*A*ax - 4*A;
    else 
        return 0;
}

float CMagnifierFilter::mitchellNetravaliKernel(float x,float B,float C) const
{
    float ax = fabs(x);
    if (ax < 1)
        return ((12 - 9*B - 6*C)*ax*ax*ax + (-18 + 12*B + 6*C)*ax*ax + (6 - 2*B)) / 6;
    else if ((ax >=1) && (ax < 2))
        return ((-B - 6*C)*ax*ax*ax + (6*B + 30*C)*ax*ax +  (-12*B - 48*C)*ax + (8*B + 24*C)) / 6;
    else 
        return 0;
}

void CMagnifierFilter::computeKernel(void)
{
	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();

	CImage kernelImage;
	kernelImage.allocatePixels(KERNEL_SIZE, 1, CImage::CGL_COLOR_FLOAT32_ALPHA);
	float *kernel = kernelImage.getFloatPixels();
	
    for (unsigned int i=0; i<KERNEL_SIZE ; i++)
    {
        float x = i / ((float)KERNEL_SIZE - 1.0f);
        *kernel++ = (this->*kernelBuilder)(x+1,	kernelParams.x, kernelParams.y);
        *kernel++ = (this->*kernelBuilder)(x,	kernelParams.x, kernelParams.y);
        *kernel++ = (this->*kernelBuilder)(1-x,	kernelParams.x, kernelParams.y);
        *kernel++ = (this->*kernelBuilder)(2-x,	kernelParams.x, kernelParams.y);
    }

	filterFactory.glLoadTexture(kernelTexture, kernelImage);
	m_bRebuild = false;
}

void CMagnifierFilter::glRenderFilter()
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    //! First pass : xPass of the kernel assuming it is separable
    RAPTOR_HANDLE noDevice;
	xBuffer->glvkBindDisplay(noDevice);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
	kernelTexture->glvkRender();
	if (m_bRebuild)
		computeKernel();
    glActiveTextureARB(GL_TEXTURE0_ARB);
	colorInput->glvkRender();

#if defined(GL_ARB_geometry_shader4) || defined(GL_ARB_vertex_shader)
	m_pYKernelShader->glGetOpenGLShader()->setProgramParameters(params_y);
#elif defined(GL_ARB_vertex_program)
	m_pYKernelShader->glGetOpenGLProgram()->setProgramParameters(params_y);
#endif
	m_pYKernelShader->glRender();

	glDrawFilter();

	m_pYKernelShader->glStop();

	xBuffer->glvkUnBindDisplay();

	glBindTexture(GL_TEXTURE_2D,0);
}

void CMagnifierFilter::glRenderFilterOutput()
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    //! Second (last) pass : yPass of the kernel, assuming it is separable
	glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
	kernelTexture->glvkRender();
    glActiveTextureARB(GL_TEXTURE0_ARB);
	xKernelPass->glvkRender();

#if defined(GL_ARB_geometry_shader4) || defined(GL_ARB_vertex_shader)
	m_pXKernelShader->glGetOpenGLShader()->setProgramParameters(params_x);
#elif defined(GL_ARB_vertex_program)
	m_pXKernelShader->glGetOpenGLProgram()->setProgramParameters(params_x);
#endif
	m_pXKernelShader->glRender();

	glDrawFilter();
	
	m_pXKernelShader->glStop();

	glBindTexture(GL_TEXTURE_2D,0);
}


bool CMagnifierFilter::glInitFilter(void)
{
    if (!CRaptorDisplayFilter::glInitFilter())
        return false;

    if (kernelTexture != NULL)
        kernelTexture->releaseReference();

	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();
	bool previousResize = filterFactory.getConfig().useTextureResize();
	filterFactory.getConfig().useTextureResize(false);

	if ((colorExternalSource != NULL) && (m_fModel == RENDER_TEXTURE))
	{
		//! See remark below regarding texture filtering.
		colorInput = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															ITextureObject::CGL_UNFILTERED, //CGL_BILINEAR,
															colorExternalSource);
	}
	else if (m_fModel == RENDER_BUFFER)
	{
		//!    Source is unfiltered to avoid artifacts ( see comment below ).
		colorInput->glvkRender();
		colorInput->glvkUpdateFilter(ITextureObject::CGL_UNFILTERED);
	}


    //! 256 interpolated values are enough for good results.
    //! For high quality filtering, future release may allow a user defined size.
    kernelTexture = filterFactory.glCreateTexture( ITextureObject::CGL_COLOR_FLOAT16_ALPHA,
                                                   ITextureObject::CGL_OPAQUE,
                                                   ITextureObject::CGL_UNFILTERED);
    kernelTexture->setSize(KERNEL_SIZE,1);
    computeKernel();

    //! A buffer to render the first pass of the kernel
    //! ( the two available kernels are separable, so it is more efficient
    //! to proceed in 2 passes : x-y buffers are created to render intermediare results.
    CRaptorDisplayConfig state;
    state.width = m_fXfactor * colorInput->getWidth();
    state.height = m_fYfactor * colorInput->getHeight();
	state.acceleration = CRaptorDisplayConfig::HARDWARE;
	state.bind_to_texture = true;
	state.double_buffer = false;
	state.depth_buffer = false;
    state.display_mode = CGL_RGBA;
	state.renderer = CRaptorDisplayConfig::PIXEL_BUFFER;

	if (m_fModel == RENDER_BUFFER)
	{
		state.renderer = CRaptorDisplayConfig::RENDER_BUFFER;

		xKernelPass = filterFactory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
			                                        ITextureObject::CGL_OPAQUE,
				                                    ITextureObject::CGL_UNFILTERED);
		filterFactory.glResizeTexture(xKernelPass,state.width,state.height);
		xKernelPass->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
		m_pRenderTextures->addTexture(xKernelPass);
	}


    xBuffer = Raptor::glCreateDisplay(state);
	IRenderingProperties &rp = xBuffer->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setCullFace(IRenderingProperties::DISABLE);
	rp.setDepthTest(IRenderingProperties::DISABLE);
	rp.setLighting(IRenderingProperties::DISABLE);
    rp.clear(CGL_NULL);
    xBuffer->setViewPoint(NULL);

	if (m_fModel == RENDER_BUFFER)
		xBuffer->glvkBindDisplay(*m_pRenderTextures);

	if (m_fModel == RENDER_TEXTURE)
	{
		//! Do not use bilinear filtering because the colors fetched may not correspond to the kernel factors with some texture sizes non power of 2.
		//! ( Specifically where tex coord is near a texel edge, and also because it is shifted to work in texels' center and should be faster ).
		xKernelPass = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															ITextureObject::CGL_UNFILTERED,//CGL_BILINEAR,
															xBuffer);
	}

    GL_COORD_VERTEX	offsetParams;
    GL_COORD_VERTEX	sizeParams;
    offsetParams.x = 1.0f / colorInput->getWidth();
    offsetParams.y = 1.0f / colorInput->getHeight();
    sizeParams.x = colorInput->getWidth();
    sizeParams.y = colorInput->getHeight();
    sizeParams.h = 1.0f;
	
	//!	0.5 factor for offset to locate texel center
    GL_COORD_VERTEX ofsy(0.0, 0.5f * offsetParams.y, 0.0, 0.0);
	params_y.addParameter("center",ofsy);
	params_y.addParameter("offset", offsetParams);
	params_y.addParameter("size", sizeParams);
	params_y.addParameter("color", CTextureUnitSetup::IMAGE_UNIT_0);
	params_y.addParameter("factor", CTextureUnitSetup::IMAGE_UNIT_1);

	GL_COORD_VERTEX ofsx(0.5f * offsetParams.x, 0.0, 0.0, 0.0);
	params_x.addParameter("center",ofsx);
	params_x.addParameter("offset",offsetParams);
	params_x.addParameter("size",sizeParams);
	params_x.addParameter("color", CTextureUnitSetup::IMAGE_UNIT_0);
	params_x.addParameter("factor", CTextureUnitSetup::IMAGE_UNIT_1);

    m_pXKernelShader = new CShader("XKERNEL_SHADER");
	m_pYKernelShader = new CShader("YKERNEL_SHADER");

#if defined(GL_ARB_geometry_shader4)
	m_pXKernelShader->glGetOpenGLShader()->glGetVertexShader("EMPTY_PROGRAM");
	CGeometryShader *gp = m_pXKernelShader->glGetOpenGLShader()->glGetGeometryShader("MAGNIFIER_GEO_SHADER");
	bool res = gp->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);
	m_pXKernelShader->glGetOpenGLShader()->glGetFragmentShader("MAGNIFIER_X_TEX_SHADER");
	res = res && m_pXKernelShader->glGetOpenGLShader()->glCompileShader();

	m_pYKernelShader->glGetOpenGLShader()->glGetVertexShader("EMPTY_PROGRAM");
	m_pYKernelShader->glGetOpenGLShader()->glGetGeometryShader("MAGNIFIER_GEO_SHADER");
	m_pYKernelShader->glGetOpenGLShader()->glGetFragmentShader("MAGNIFIER_Y_TEX_SHADER");
	res = res && m_pYKernelShader->glGetOpenGLShader()->glCompileShader();
#elif defined(GL_ARB_vertex_shader)
	m_pXKernelShader->glGetOpenGLShader()->glGetVertexShader("MAGNIFIER_VTX_SHADER");
	m_pXKernelShader->glGetOpenGLShader()->glGetFragmentShader("MAGNIFIER_OLDX_TEX_SHADER");
	bool res = m_pXKernelShader->glGetOpenGLShader()->glCompileShader();

	m_pYKernelShader->glGetOpenGLShader()->glGetVertexShader("MAGNIFIER_VTX_SHADER");
	m_pYKernelShader->glGetOpenGLShader()->glGetFragmentShader("MAGNIFIER_OLDY_TEX_SHADER");
	res = res && m_pYKernelShader->glGetOpenGLShader()->glCompileShader();
#elif defined(GL_ARB_vertex_program)
	m_pXKernelShader->glGetOpenGLProgram()->glGetVertexProgram("MAGNIFIER_VTX_PROGRAM");
	m_pXKernelShader->glGetOpenGLProgram()->glGetFragmentProgram("MAGNIFIER_X_TEX_PROGRAM");

	m_pYKernelShader->glGetOpenGLProgram()->glGetVertexProgram("MAGNIFIER_VTX_PROGRAM");
	m_pYKernelShader->glGetOpenGLProgram()->glGetFragmentProgram("MAGNIFIER_Y_TEX_PROGRAM");
	bool res = true;
#endif

	filterFactory.getConfig().useTextureResize(previousResize);

    return res;
}

 
void CMagnifierFilter::glDestroyFilter(void)
{
    CRaptorDisplayFilter::glDestroyFilter();

    if (kernelTexture != NULL)
	{
        kernelTexture->releaseReference();
		kernelTexture = NULL;
	}
    if (xKernelPass != NULL)
	{
        xKernelPass->releaseReference();
		xKernelPass = NULL;
	}

    if (xBuffer != NULL)
	{
        Raptor::glDestroyDisplay(xBuffer);
		xBuffer = NULL;
	}

    if (m_pXKernelShader != NULL)
	{
        m_pXKernelShader->releaseReference();
		m_pXKernelShader = NULL;
	}
    if (m_pYKernelShader != NULL)
	{
        m_pYKernelShader->releaseReference();
		m_pYKernelShader = NULL;
	}
}

