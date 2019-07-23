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
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
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
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_COLORCONTROLFILTER_H__CD5D6DA8_0CD8_43AD_A2BF_F9CE419A50FE__INCLUDED_)
    #include "ColorControlFilter.h"
#endif


#if defined(GL_ARB_geometry_shader4)
	static const string colorcontrol_fp =
	"#version 440 			\n\
	\n\
	const vec3 luminance = vec3(0.299, 0.587, 0.114);	\n\
	const vec3 u_chrominance = vec3(-0.14713, -0.28886, 0.436);	\n\
	const vec3 v_chrominance = vec3(0.615, -0.51498, -0.10001);	\n\
	const vec3 red = vec3(1.0, 0.0, 1.13983);	\n\
	const vec3 green = vec3(1.0, -0.39465, -0.58060);	\n\
	const vec3 blue = vec3(1.0, 2.03211, 0.0);	\n\
	uniform	sampler2D	source;			\n\
	uniform vec4		percentage;		\n\
	uniform vec4		baseColor;		\n\
	\n\
	layout(location = 1) in vec4 g_TexCoord; \n\
	layout(location = 0) out vec4 o_Color;	\n\
	void main(void)			\n\
	{						\n\
		vec3 color = texture(source,vec2(g_TexCoord.st)).xyz; \n\
		vec3 yuv = vec3(0.0,0.0,0.0); \n\
		yuv.x = dot(luminance,color);	\n\
		yuv.y = percentage.z * dot(u_chrominance,color);	\n\
		yuv.z = percentage.z * dot(v_chrominance, color);	\n\
		\n\
		color.x = dot(yuv, red);		\n\
		color.y = dot(yuv, green);	\n\
		color.z = dot(yuv, blue);	\n\
		\n\
		o_Color.xyz = mix(percentage.y * color, yuv.x * baseColor.xyz, percentage.x);	\n\
		o_Color.w = 1.0; \n\
	}";
#elif defined(GL_ARB_vertex_shader)
	#include "ColorControlFilter.programs"
#elif defined(GL_ARB_vertex_program)
	#include "ColorControlFilter.shaders"
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
	BWShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_program)
	BWShader->glGetFragmentShader()->setProgramParameters(fp_params);
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
															CTextureObject::CGL_OPAQUE,
															ITextureObject::CGL_BILINEAR,
															colorExternalSource);
	}

	BWShader = new CShader("BW_SHADER");

	bool res = false;
#if defined(GL_ARB_geometry_shader4)
	BWShader->glGetVertexProgram("EMPTY_PROGRAM");
	BWShader->glGetGeometryProgram("FULL_SCREEN_GEO_PROGRAM");
	CFragmentProgram *ps = BWShader->glGetFragmentProgram("bw_fp");
	res = ps->glLoadProgram(colorcontrol_fp);
	res = res && BWShader->glCompileShader();
	fp_params.addParameter("source", CTextureUnitSetup::IMAGE_UNIT_0);
#elif defined(GL_ARB_vertex_shader)
	CFragmentProgram *fp = BWShader->glGetFragmentProgram("bw_fp");
	res = fp->glLoadProgram(colorcontrol_fprogram);
	if (res)
		fp_params.addParameter("source",CTextureUnitSetup::IMAGE_UNIT_0);
	res = res && BWShader->glCompileShader();
#elif defined(GL_ARB_vertex_program)
	CFragmentShader *fs = BWShader->glGetFragmentShader("bw_fp");
	res = fs->glLoadProgram(colorcontrol_fp);
#endif

    return res;
}






