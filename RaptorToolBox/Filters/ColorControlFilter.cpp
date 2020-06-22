/***************************************************************************/
/*                                                                         */
/*  ColorControlFilter.cpp                                                 */
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

#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
    #include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__DD0AD51D_3BFF_4C65_8099_BA7696D7BDDF__INCLUDED_)
	#include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_COLORCONTROLFILTER_H__CD5D6DA8_0CD8_43AD_A2BF_F9CE419A50FE__INCLUDED_)
    #include "ColorControlFilter.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorControlFilter::CColorControlFilter():
    BWShader(NULL),
	bwParams(GL_COORD_VERTEX(1.0f,1.0f,0.0f,1.0f)),
	baseColor(GL_COORD_VERTEX(1.0f, 1.0f, 1.0f, 1.0f))
{
	fp_params.addParameter("percentage",bwParams.p);
	fp_params.addParameter("baseColor", baseColor.p);
#if defined(GL_ARB_geometry_shader4) || defined(GL_ARB_vertex_shader)
	fp_params.addParameter("source", CTextureUnitSetup::IMAGE_UNIT_0);
#endif
}

CColorControlFilter::~CColorControlFilter()
{
	glDestroyFilter();
}

void CColorControlFilter::glDestroyFilter(void)
{
    CRaptorDisplayFilter::glDestroyFilter();

    if (BWShader != NULL)
	{
        BWShader->releaseReference();
		BWShader = NULL;
	}
}

void CColorControlFilter::setColorBlend(float r,float g,float b,float a,float percentage)
{
    // If we allow unclampped values, they can be used in a float buffer,
    // and we can also produce an overexpose filter.
	baseColor.p.x = r; //MIN(0.0f,MAX(1.0f,r));
    baseColor.p.y = g; //MIN(0.0f,MAX(1.0f,g));
    baseColor.p.z = b; //MIN(0.0f,MAX(1.0f,b));
    baseColor.p.h = a; //MIN(0.0f,MAX(1.0f,a));

	if (percentage < 0.0f)
        bwParams.p.x = 0.0f;
    else if (percentage > 1.0f)
        bwParams.p.x = 1.0f;
    else
        bwParams.p.x = percentage;

	fp_params[0].copy(bwParams);
	fp_params[1].copy(baseColor);
}

void CColorControlFilter::setCorrection(float brightness,float saturation)
{
	bwParams.p.y = brightness;
	bwParams.p.z = saturation;
	fp_params[0].copy(bwParams);
}

void CColorControlFilter::getCorrection(float &brightness,float &saturation)
{
	brightness = bwParams.p.y;
	saturation = bwParams.p.z;
}

void CColorControlFilter::glRenderFilter()
{
}

void CColorControlFilter::glRenderFilterOutput()
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    //! Filter shaders Rendering
#if defined(GL_ARB_geometry_shader4) || defined(GL_ARB_vertex_shader)
	BWShader->glGetOpenGLShader()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_program)
	BWShader->glGetOpenGLProgram()->glGetFragmentProgram()->setProgramParameters(fp_params);
#endif

    glActiveTextureARB(GL_TEXTURE0_ARB);
	getColorInput()->glvkRender();
	
	BWShader->glRender();
    
	glDrawFilter();

    BWShader->glStop();

	glBindTexture(GL_TEXTURE_2D,0);
}

bool CColorControlFilter::glInitFilter(void)
{
    if (!CRaptorDisplayFilter::glInitFilter())
        return false;

	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();

	if ((colorExternalSource != NULL) && (m_fModel == RENDER_TEXTURE))
	{
		colorInput = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															ITextureObject::CGL_BILINEAR,
															colorExternalSource);
	}

	BWShader = new CShader("BW_SHADER");

	bool res = false;
#if defined(GL_ARB_geometry_shader4)
	COpenGLShaderStage *stage = BWShader->glGetOpenGLShader();
	stage->glGetVertexShader("EMPTY_PROGRAM");
	stage->glGetGeometryShader("FULL_SCREEN_GEO_PROGRAM");
	stage->glGetFragmentShader("COLOR_TEX_SHADER");
	res = stage->glCompileShader();
#elif defined(GL_ARB_vertex_shader)
	COpenGLShaderStage *stage = BWShader->glGetOpenGLShader();
	stage->glGetFragmentShader("COLOR_OLD_TEX_SHADER");
	res = stage->glCompileShader();
#elif defined(GL_ARB_vertex_program)
	BWShader->glGetOpenGLProgram()->glGetFragmentProgram("COLOR_TEX_PROGRAM");
#endif

    return res;
}






