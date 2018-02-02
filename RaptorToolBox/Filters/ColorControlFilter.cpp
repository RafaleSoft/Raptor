// BWFilter.cpp: implementation of the CBWFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
    #include "System/RaptorExtensions.h"
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
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif
#if !defined(AFX_COLORCONTROLFILTER_H__CD5D6DA8_0CD8_43AD_A2BF_F9CE419A50FE__INCLUDED_)
    #include "ColorControlFilter.h"
#endif



#if defined(GL_ARB_vertex_shader)
	const char *colorcontrol_fprogram = 
	"	#version 120						\n\
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
	void main (void) \n\
	{\n\
		vec3 color = texture2D(source,vec2(gl_TexCoord[0].st)).xyz; \n\
		vec3 yuv = vec3(0.0,0.0,0.0); \n\
		yuv.x = dot(luminance,color);	\n\
		yuv.y = percentage.z * dot(u_chrominance,color);	\n\
		yuv.z = percentage.z * dot(v_chrominance,color);	\n\
	\n\
		color.x = dot(yuv,red);		\n\
		color.y = dot(yuv,green);	\n\
		color.z = dot(yuv,blue);	\n\
	\n\
		gl_FragColor.xyz = mix(percentage.y * color,yuv.x * baseColor.xyz,percentage.x);	\n\
		gl_FragColor.w = 1.0; \n\
	}\n\
	";
#elif defined(GL_ARB_vertex_program)
	static const string bw_fp = 
	"!!ARBfp1.0 \
	ATTRIB iTex0 = fragment.texcoord[0]; \
	PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };\
	PARAM percentage = program.local[0]; \
	PARAM baseColor = program.local[1]; \
	TEMP color; \
	TEMP bw; \
	OUTPUT finalColor = result.color; \
	TEX color, iTex0, texture[0] , 2D ; \
	DP3 bw, color, luminance; \
	MUL bw, bw, baseColor; \
	LRP finalColor, percentage.xxxx, bw, color; \
	MOV finalColor.w, percentage.w; \
	END"; 

	static const string toyuv_fp = 
	"!!ARBfp1.0 \
	ATTRIB iTex0 = fragment.texcoord[0]; \
	PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };\
	PARAM u_chrominance = { -0.14713, -0.28886, 0.436, 0.0 };\
	PARAM v_chrominance = { 0.615, -0.51498, -0.10001, 0.0 };\
	TEMP color; \
	OUTPUT finalColor = result.color; \
	TEX color, iTex0, texture[0] , 2D ; \
	DP3 finalColor.x, color, luminance; \
	DP3 finalColor.y, color, u_chrominance; \
	DP3 finalColor.z, color, v_chrominance; \
	MOV finalColor.w, color.w; \
	END"; 


	static const string torgb_fp = 
	"!!ARBfp1.0 \
	ATTRIB iTex0 = fragment.texcoord[0]; \
	PARAM red = { 1.0, 0.0, 1.13983, 1.0 };\
	PARAM green = { 1.0, -0.39465, 0.-58060, 0.0 };\
	PARAM blue = { 1.0, 2.03211, 0.0, 0.0 };\
	TEMP color; \
	OUTPUT finalColor = result.color; \
	TEX color, iTex0, texture[0] , 2D ; \
	DP3 finalColor.x, color, red; \
	DP3 finalColor.y, color, green; \
	DP3 finalColor.z, color, blue; \
	MOV finalColor.w, color.w; \
	END"; 

	static const string colorcontrol_fp = 
	"!!ARBfp1.0 \
	ATTRIB iTex0 = fragment.texcoord[0]; \
	PARAM percentage = program.local[0]; \
	PARAM baseColor = program.local[1]; \
	PARAM luminance = { 0.299, 0.587, 0.114, 0.0 };\
	PARAM u_chrominance = { -0.14713, -0.28886, 0.436, 0.0 };\
	PARAM v_chrominance = { 0.615, -0.51498, -0.10001, 0.0 };\
	PARAM red = { 1.0, 0.0, 1.13983, 1.0 };\
	PARAM green = { 1.0, -0.39465, -0.58060, 0.0 };\
	PARAM blue = { 1.0, 2.03211, 0.0, 0.0 };\
	TEMP color; \
	TEMP yuvcolor;\
	OUTPUT finalColor = result.color; \
	TEX color, iTex0, texture[0] , 2D ; \
	DP3 yuvcolor.x, color, luminance; \
	DP3 yuvcolor.y, color, u_chrominance; \
	DP3 yuvcolor.z, color, v_chrominance; \
	MOV yuvcolor.w, color.w; \
	MUL yuvcolor, yuvcolor, percentage.wzzw; \
	DP3 color.x, yuvcolor, red; \
	DP3 color.y, yuvcolor, green; \
	DP3 color.z, yuvcolor, blue; \
	MUL color, color, percentage.yyyy; \
	MUL yuvcolor, baseColor, yuvcolor.xxxx; \
	LRP finalColor, percentage.xxxx, yuvcolor, color; \
	MOV finalColor.w, red.w; \
	END";
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
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    //! Filter shaders Rendering
#if defined(GL_ARB_vertex_shader)
	BWShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_program)
	BWShader->glGetFragmentShader()->setProgramParameters(fp_params);
#endif

    glActiveTextureARB(GL_TEXTURE0_ARB);
	getColorInput()->glvkRender();
	
	BWShader->glRender();
    glDrawBuffer();
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
#if defined(GL_ARB_vertex_shader)
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






