/***************************************************************************/
/*                                                                         */
/*  DOFFilter.cpp                                                          */
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

#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
    #include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
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
#if !defined(AFX_DOFFILTER_H__A4FA0FE9_04AA_4887_9B4A_3CFAF930D840__INCLUDED_)
    #include "DOFFilter.h"
#endif


#if defined(GL_ARB_geometry_shader4)
static const std::string dof_gp =
	"#version 440\n\
	\n\
	//	Expect the geometry shader extension to be available, warn if not. \n\
	#extension GL_ARB_geometry_shader4 : enable \n\
	\n\
	uniform vec4 offset; \n\
	uniform vec4 vector; \n\
	\n\
	layout(points) in; \n\
	layout(triangle_strip, max_vertices=4) out; \n\
	layout(location = 1) out vec4 g_TexCoord[7]; \n\
	\n\
	void main() \n\
	{\n\
		gl_Position = vec4(-1.0, -1.0, 0.0, 1.0); \n\
		g_TexCoord[0] = vec4(0.0,0.0,0.0,0.0); \n\
		g_TexCoord[1] = vec4(0.0,0.0,0.0,0.0) + offset.x*vector; \n\
		g_TexCoord[2] = vec4(0.0,0.0,0.0,0.0) - offset.x*vector; \n\
		g_TexCoord[3] = vec4(0.0,0.0,0.0,0.0) + offset.y*vector; \n\
		g_TexCoord[4] = vec4(0.0,0.0,0.0,0.0) - offset.y*vector; \n\
		g_TexCoord[5] = vec4(0.0,0.0,0.0,0.0) + offset.z*vector; \n\
		g_TexCoord[6] = vec4(0.0,0.0,0.0,0.0) - offset.z*vector; \n\
		EmitVertex(); \n\
		\n\
		gl_Position = vec4(1.0, -1.0, 0.0, 1.0); \n\
		g_TexCoord[0] = vec4(1.0,0.0,0.0,0.0); \n\
		g_TexCoord[1] = vec4(1.0,0.0,0.0,0.0) + offset.x*vector; \n\
		g_TexCoord[2] = vec4(1.0,0.0,0.0,0.0) - offset.x*vector; \n\
		g_TexCoord[3] = vec4(1.0,0.0,0.0,0.0) + offset.y*vector; \n\
		g_TexCoord[4] = vec4(1.0,0.0,0.0,0.0) - offset.y*vector; \n\
		g_TexCoord[5] = vec4(1.0,0.0,0.0,0.0) + offset.z*vector; \n\
		g_TexCoord[6] = vec4(1.0,0.0,0.0,0.0) - offset.z*vector; \n\
		EmitVertex(); \n\
		\n\
		gl_Position = vec4(-1.0, 1.0, 0.0, 1.0); \n\
		g_TexCoord[0] = vec4(0.0, 1.0, 0.0, 0.0); \n\
		g_TexCoord[1] = vec4(0.0, 1.0, 0.0, 0.0) + offset.x*vector; \n\
		g_TexCoord[2] = vec4(0.0, 1.0, 0.0, 0.0) - offset.x*vector; \n\
		g_TexCoord[3] = vec4(0.0, 1.0, 0.0, 0.0) + offset.y*vector; \n\
		g_TexCoord[4] = vec4(0.0, 1.0, 0.0, 0.0) - offset.y*vector; \n\
		g_TexCoord[5] = vec4(0.0, 1.0, 0.0, 0.0) + offset.z*vector; \n\
		g_TexCoord[6] = vec4(0.0, 1.0, 0.0, 0.0) - offset.z*vector; \n\
		EmitVertex(); \n\
		\n\
		gl_Position = vec4(1.0, 1.0, 0.0, 1.0); \n\
		g_TexCoord[0] = vec4(1.0, 1.0, 0.0, 0.0); \n\
		g_TexCoord[1] = vec4(1.0, 1.0, 0.0, 0.0) + offset.x*vector; \n\
		g_TexCoord[2] = vec4(1.0, 1.0, 0.0, 0.0) - offset.x*vector; \n\
		g_TexCoord[3] = vec4(1.0, 1.0, 0.0, 0.0) + offset.y*vector; \n\
		g_TexCoord[4] = vec4(1.0, 1.0, 0.0, 0.0) - offset.y*vector; \n\
		g_TexCoord[5] = vec4(1.0, 1.0, 0.0, 0.0) + offset.z*vector; \n\
		g_TexCoord[6] = vec4(1.0, 1.0, 0.0, 0.0) - offset.z*vector; \n\
		EmitVertex(); \n\
		\n\
		EndPrimitive(); \n\
	}";


	//	C = A * m * |S2 - S1| / S2
	static const string dof_fp =
	"#version 440\n\
	\n\
	uniform sampler2D colorMap;\n\
	uniform sampler2D depthMap;\n\
	uniform vec4 dofParams; \n\
	const float CoC_mag = 2.66; \n\
	\n\
	layout(location = 1) in vec4 g_TexCoord[7]; \n\
	layout(location = 0) out vec4 o_Color;	\n\
	void main (void) \n\
	{\n\
		vec4 color_base  = texture(colorMap,g_TexCoord[0].xy);\n\
		float depth_base = texture(depthMap,g_TexCoord[0].xy).r;\n\
		\n\
		float CoC_radius = dofParams.z * CoC_mag * abs(depth_base - dofParams.y) / depth_base;\n\
		\n\
		if (CoC_radius > 1.0) \n\
		{\n\
			vec4 color = color_base;\n\
			color += texture(colorMap,g_TexCoord[1].xy);\n\
			color += texture(colorMap,g_TexCoord[2].xy);\n\
			color_base = mix(color_base,color / 3.0,(CoC_radius - 1.0)); \n\
			//		color_base = vec4(1.0, 0.0, 0.0, 1.0); \n\
			\n\
			if (CoC_radius > 2.0) \n\
			{\n\
				vec4 color2 = color; \n\
				color2 += texture(colorMap,g_TexCoord[3].xy);\n\
				color2 += texture(colorMap,g_TexCoord[4].xy);\n\
				color_base = mix(color / 3.0,color2 / 5.0,(CoC_radius - 2.0)); \n\
				//			color_base = vec4(0.0, 1.0, 0.0, 1.0); \n\
				\n\
				if (CoC_radius > 3.0) \n\
				{\n\
					vec4 color3 = color2; \n\
					color3 += texture(colorMap,g_TexCoord[5].xy);\n\
					color3 += texture(colorMap,g_TexCoord[6].xy);\n\
					color_base = mix(color2 / 5.0,color3 / 7.0,(min(CoC_radius,4.0) - 3.0)); \n\
					//				color_base = vec4(0.0, 0.0, 1.0, 1.0); \n\
				} \n\
			}\n\
			\n\
		}\n\
		\n\
		o_Color = color_base; \n\
	}\n\
	";

#elif defined(GL_ARB_vertex_shader)
	#include "DOF.programs"
#elif defined(GL_ARB_vertex_program)
	#include "DOF.shaders"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOFFilter::CDOFFilter()
    :DOFShader(NULL),
    tmpTexture(NULL),
	tmpDisplay(NULL),
	tmpTexture2(NULL),
	tmpDisplay2(NULL),
	m_pRenderTextures2(NULL),
	m_nbBlur(1),
	dofParams(GL_COORD_VERTEX(0.0f,0.95f,50.0f,0.0f))
{
	vp_paramsX.addParameter("offset", GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 0.0f));
	vp_paramsX.addParameter("vector", GL_COORD_VERTEX(1.0f, 0.0f, 0.0f, 0.0f));

	vp_paramsY.addParameter("offset", GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 0.0f));
	vp_paramsY.addParameter("vector", GL_COORD_VERTEX(0.0f, 1.0f, 0.0f, 0.0f));

	fp_params.addParameter("dofParams",dofParams.p);
	fp_params.addParameter("colorMap", CTextureUnitSetup::IMAGE_UNIT_0);
	fp_params.addParameter("depthMap", CTextureUnitSetup::IMAGE_UNIT_1);
}

CDOFFilter::~CDOFFilter()
{
	glDestroyFilter();
}

void CDOFFilter::glDestroyFilter(void)
{
    CRaptorDisplayFilter::glDestroyFilter();

    if (DOFShader != NULL)
	{
        DOFShader->releaseReference();
		DOFShader = NULL;
	}
    if (tmpTexture != NULL)
	{
        tmpTexture->releaseReference();
		tmpTexture = NULL;
	}
    if (tmpDisplay != NULL)
	{
        Raptor::glDestroyDisplay(tmpDisplay);
		tmpDisplay = NULL;
	}
	if (tmpTexture2 != NULL)
	{
        tmpTexture2->releaseReference();
		tmpTexture2 = NULL;
	}
    if (tmpDisplay2 != NULL)
	{
        Raptor::glDestroyDisplay(tmpDisplay2);
		tmpDisplay2 = NULL;
	}
	if (m_pRenderTextures2 != NULL)
	{
        delete m_pRenderTextures2;
		m_pRenderTextures2 = NULL;
	}
}

void CDOFFilter::setDOFParams(float percentageOfDepthFiltered, float filterAmplitude)
{
    dofParams.p.y = percentageOfDepthFiltered;

    // set default value if parameter is out of bounds.
    if ((dofParams.p.y < 0) || (dofParams.p.y > 1.0))
        dofParams.p.y = 0.95f;

    dofParams.p.z = filterAmplitude;

    // set default value if parameter is out of bounds
    if (dofParams.p.z < 0)
        dofParams.p.z = 50.0f;

	fp_params[0].copy(dofParams);
}

void CDOFFilter::setBlurNbPass(unsigned int nb)
{
	m_nbBlur = MAX(MIN(nb,4),1);
}

void CDOFFilter::glRenderFilter()
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    //  Render X-blur in pixel buffer
    RAPTOR_HANDLE noDevice;
	tmpDisplay->glvkBindDisplay(noDevice);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
	depthInput->glvkRender();
    glActiveTextureARB(GL_TEXTURE0_ARB);
	getColorInput()->glvkRender();


#if defined(GL_ARB_geometry_shader4)
	DOFShader->glGetGeometryProgram()->setProgramParameters(vp_paramsX);
	DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_shader)
	DOFShader->glGetVertexProgram()->setProgramParameters(vp_paramsX);
	DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_program)
	DOFShader->glGetVertexShader()->setProgramParameters(vp_paramsX);
	DOFShader->glGetFragmentShader()->setProgramParameters(fp_params);
#endif

	DOFShader->glRender();
	
	glDrawFilter();

	DOFShader->glStop();

	tmpDisplay->glvkUnBindDisplay();

	for (unsigned int i=2;i<=m_nbBlur;i++)
	{
		//  Render Y-blur in current buffer
		tmpDisplay2->glvkBindDisplay(noDevice);
		tmpTexture->glvkRender();

	#if defined(GL_ARB_geometry_shader4)
		DOFShader->glGetGeometryProgram()->setProgramParameters(vp_paramsY);
		DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
	#elif defined(GL_ARB_vertex_shader)
		DOFShader->glGetVertexProgram()->setProgramParameters(vp_paramsY);
		DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
	#elif defined(GL_ARB_vertex_program)
		DOFShader->glGetVertexShader()->setProgramParameters(vp_paramsY);
		DOFShader->glGetFragmentShader()->setProgramParameters(fp_params);
	#endif

		DOFShader->glRender();

		glDrawFilter();
		
		DOFShader->glStop();
		tmpDisplay2->glvkUnBindDisplay();

		//  Render X-blur in pixel buffer
		tmpDisplay->glvkBindDisplay(noDevice);
		tmpTexture2->glvkRender();

#if defined(GL_ARB_geometry_shader4)
		DOFShader->glGetGeometryProgram()->setProgramParameters(vp_paramsX);
		DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_shader)
		DOFShader->glGetVertexProgram()->setProgramParameters(vp_paramsX);
		DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_program)
		DOFShader->glGetVertexShader()->setProgramParameters(vp_paramsX);
		DOFShader->glGetFragmentShader()->setProgramParameters(fp_params);
#endif

		DOFShader->glRender();

		glDrawFilter();

		DOFShader->glStop();

		tmpDisplay->glvkUnBindDisplay();
	}
}

void CDOFFilter::glRenderFilterOutput()
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

	//  Render Y-blur in current buffer
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	depthInput->glvkRender();
	glActiveTextureARB(GL_TEXTURE0_ARB);
	tmpTexture->glvkRender();

#if defined(GL_ARB_geometry_shader4)
	DOFShader->glGetGeometryProgram()->setProgramParameters(vp_paramsY);
	DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_shader)
	DOFShader->glGetVertexProgram()->setProgramParameters(vp_paramsY);
	DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_program)
	DOFShader->glGetVertexShader()->setProgramParameters(vp_paramsY);
	DOFShader->glGetFragmentShader()->setProgramParameters(fp_params);
#endif

	DOFShader->glRender();

	glDrawFilter();

	DOFShader->glStop();

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D,0);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D,0);
}

bool CDOFFilter::glInitFilter(void)
{
    if (!CRaptorDisplayFilter::glInitFilter())
        return false;

	if (depthInput == NULL)
	{
		if (depthExternalSource == NULL)
			return false;
		if (depthExternalSource->getKind() != ITextureGenerator::BUFFERED)
			return false;
	}

	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();

	bool previousResize = filterFactory.getConfig().useTextureResize();
	filterFactory.getConfig().useTextureResize(false);

	if ((colorExternalSource != NULL) && 
		(depthExternalSource != NULL) &&
		(m_fModel == RENDER_TEXTURE))
	{
		depthInput = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_DEPTH24,
															CTextureObject::CGL_OPAQUE,
															ITextureObject::CGL_UNFILTERED,
															depthExternalSource);
		colorInput = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															ITextureObject::CGL_UNFILTERED,
															colorExternalSource);
	}

	glInitShaders();	

	m_pRenderTextures2 = new CTextureSet("DOF_BLUR_TSET");

    // Create temporary display and texture to perform the two pass z-blur.
	CRaptorDisplayConfig state;
	state.width = m_fXfactor * getColorInput()->getWidth();
	state.height = m_fYfactor * getColorInput()->getHeight();
	state.acceleration = CRaptorDisplayConfig::HARDWARE;
	state.bind_to_texture = true;
	state.double_buffer = false;
	state.depth_buffer = false;
    state.display_mode = CGL_RGBA;
	state.renderer = CRaptorDisplayConfig::PIXEL_BUFFER;

	if (m_fModel == RENDER_BUFFER)
	{
		state.renderer = CRaptorDisplayConfig::RENDER_BUFFER;

		tmpTexture = filterFactory.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
			                                        CTextureObject::CGL_OPAQUE,
				                                    ITextureObject::CGL_UNFILTERED);
		filterFactory.glResizeTexture(tmpTexture,state.width,state.height);
		tmpTexture->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
		m_pRenderTextures->addTexture(tmpTexture);

		tmpTexture2 = filterFactory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,
			                                        CTextureObject::CGL_OPAQUE,
				                                    ITextureObject::CGL_UNFILTERED);
		filterFactory.glResizeTexture(tmpTexture2,state.width,state.height);
		tmpTexture2->glvkUpdateClamping(ITextureObject::CGL_EDGECLAMP);
		m_pRenderTextures2->addTexture(tmpTexture2);
	}

    tmpDisplay = Raptor::glCreateDisplay(state);
	IRenderingProperties &rp = tmpDisplay->getRenderingProperties();
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setCullFace(IRenderingProperties::DISABLE);
	rp.setDepthTest(IRenderingProperties::DISABLE);
	rp.setLighting(IRenderingProperties::DISABLE);
    rp.clear(CGL_NULL);
    tmpDisplay->setViewPoint(NULL);

	tmpDisplay2 = Raptor::glCreateDisplay(state);
	IRenderingProperties &rp2 = tmpDisplay2->getRenderingProperties();
	rp2.setTexturing(IRenderingProperties::ENABLE);
	rp2.setCullFace(IRenderingProperties::DISABLE);
	rp2.setDepthTest(IRenderingProperties::DISABLE);
	rp2.setLighting(IRenderingProperties::DISABLE);
    rp2.clear(CGL_NULL);
    tmpDisplay2->setViewPoint(NULL);

	if (m_fModel == RENDER_BUFFER)
	{
		tmpDisplay->glvkBindDisplay(*m_pRenderTextures);
		tmpDisplay2->glvkBindDisplay(*m_pRenderTextures2);
	}

	if (m_fModel == RENDER_TEXTURE)
	{
		tmpTexture = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															ITextureObject::CGL_UNFILTERED,
															tmpDisplay);
		tmpTexture2 = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															ITextureObject::CGL_UNFILTERED,
															tmpDisplay2);
	}

	filterFactory.getConfig().useTextureResize(previousResize);

    return true;
}


void CDOFFilter::glInitShaders(void)
{
	DOFShader = new CShader("DOF_SHADER");

	float delta = 1.0f / depthInput->getWidth();
	GL_COORD_VERTEX     vsParameter_Xoffset(1.0f * delta, 2.0f * delta, 3.0f * delta, 0.0f);
	CProgramParameters::CParameter<GL_COORD_VERTEX> px(vsParameter_Xoffset);
	vp_paramsX[0].copy(px);

	delta = 1.0f / depthInput->getHeight();
	GL_COORD_VERTEX     vsParameter_Yoffset(1.0f * delta, 2.0f * delta, 3.0f * delta, 0.0f);
	CProgramParameters::CParameter<GL_COORD_VERTEX> py(vsParameter_Yoffset);
	vp_paramsY[0].copy(py);

	fp_params[0].copy(dofParams);
	

	// Create & load shaders to perform a 2 pass blur using depth value.
#if defined(GL_ARB_geometry_shader4)
	DOFShader->glGetVertexProgram("EMPTY_PROGRAM");
	CGeometryProgram *gp = DOFShader->glGetGeometryProgram("dof_gp");
	bool res = gp->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);
	res = res & gp->glLoadProgram(dof_gp);
	CFragmentProgram *fp = DOFShader->glGetFragmentProgram("dof_fp");
	res = res && fp->glLoadProgram(dof_fp);
	if (res)
		fp->setProgramParameters(fp_params);

	res = res && DOFShader->glCompileShader();
#elif defined(GL_ARB_vertex_shader)
	CVertexProgram *vp = DOFShader->glGetVertexProgram("dof_vp");
	bool res = vp->glLoadProgram(dof_vp);
	if (res)
		vp->setProgramParameters(vp_paramsX);

	CFragmentProgram *fp = DOFShader->glGetFragmentProgram("dof_fp");
	res = res && fp->glLoadProgram(dof_fp);
	if (res)
		fp->setProgramParameters(fp_params);

	res = res && DOFShader->glCompileShader();
#elif defined(GL_ARB_vertex_program)
	CVertexShader *vs = DOFShader->glGetVertexShader("dof_vs");
	bool res = vs->glLoadProgram(dof_vs);
	CFragmentShader *fs = DOFShader->glGetFragmentShader("dof_fs");
	res = res && fs->glLoadProgram(dof_fs);
#endif
}


