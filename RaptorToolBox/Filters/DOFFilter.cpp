// DOFFilter.cpp: implementation of the CDOFFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
    #include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
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
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif
#if !defined(AFX_DOFFILTER_H__A4FA0FE9_04AA_4887_9B4A_3CFAF930D840__INCLUDED_)
    #include "DOFFilter.h"
#endif



#if defined(GL_ARB_vertex_shader)
	static const string dof_vp =
	"uniform vec4 offset; \n\
	uniform vec4 vector; \n\
	void main (void) \n\
	{\n\
		gl_TexCoord[0] = gl_MultiTexCoord0; \n\
		gl_TexCoord[1] = gl_MultiTexCoord0 + offset.x*vector; \n\
		gl_TexCoord[2] = gl_MultiTexCoord0 - offset.x*vector; \n\
		gl_TexCoord[3] = gl_MultiTexCoord0 + offset.y*vector; \n\
		gl_TexCoord[4] = gl_MultiTexCoord0 - offset.y*vector; \n\
		gl_TexCoord[5] = gl_MultiTexCoord0 + offset.z*vector; \n\
		gl_TexCoord[6] = gl_MultiTexCoord0 - offset.z*vector; \n\
		gl_Position = ftransform(); \n\
	}\n\
	";

	//	C = A * m * |S2 - S1| / S2
	static const string dof_fp = 
	"uniform sampler2D colorMap;\n\
	uniform sampler2D depthMap;\n\
	uniform vec4 dofParams; \n\
	const float CoC_mag = 2.66; \n\
	\n\
	void main (void) \n\
	{\n\
		vec4 color_base  = texture2D(colorMap,gl_TexCoord[0].xy);\n\
		float depth_base = texture2D(depthMap,gl_TexCoord[0].xy).r;\n\
	\n\
		float CoC_radius = dofParams.z * CoC_mag * abs(depth_base - dofParams.y) / depth_base;\n\
	\n\
		if (CoC_radius > 1.0) \n\
		{\n\
			vec4 color = color_base;\n\
			color += texture2D(colorMap,gl_TexCoord[1].xy);\n\
			color += texture2D(colorMap,gl_TexCoord[2].xy);\n\
			color_base = mix(color_base,color / 3.0,(CoC_radius - 1.0)); \n\
	//		color_base = vec4(1.0, 0.0, 0.0, 1.0); \n\
	\n\
			if (CoC_radius > 2.0) \n\
			{\n\
				vec4 color2 = color; \n\
				color2 += texture2D(colorMap,gl_TexCoord[3].xy);\n\
				color2 += texture2D(colorMap,gl_TexCoord[4].xy);\n\
				color_base = mix(color / 3.0,color2 / 5.0,(CoC_radius - 2.0)); \n\
	//			color_base = vec4(0.0, 1.0, 0.0, 1.0); \n\
	\n\
				if (CoC_radius > 3.0) \n\
				{\n\
					vec4 color3 = color2; \n\
					color3 += texture2D(colorMap,gl_TexCoord[5].xy);\n\
					color3 += texture2D(colorMap,gl_TexCoord[6].xy);\n\
					color_base = mix(color2 / 5.0,color3 / 7.0,(min(CoC_radius,4.0) - 3.0)); \n\
	//				color_base = vec4(0.0, 0.0, 1.0, 1.0); \n\
				} \n\
			}\n\
	\n\
		}\n\
	\n\
		gl_FragColor = color_base; \n\
	}\n\
	";

//	Depth of field color ( eq. fog ):
//	Not applicable as a post processing if RenderBuffer is antialiased.
//	It could be feasible if FSAA is also applied as a (final) postprocess ( quite unlikely )
//
//	float f = smoothstep(dofParams.y,1.0,depth_base); \n\
//	gl_FragColor = mix(dofColor,color_base,exp(-f*f));\n\

#elif defined(GL_ARB_vertex_program)
	static const string dof_vs = 
	"!!ARBvp1.0 \
	ATTRIB iPos = vertex.position; \
	ATTRIB iTexCoord = vertex.texcoord[0]; \
	PARAM mvp[4] = { state.matrix.mvp }; \
	PARAM offset = program.local[0]; \
	PARAM vector = program.local[1]; \
	TEMP pos; \
	OUTPUT oPos = result.position; \
	OUTPUT oTex0 = result.texcoord[0]; \
	OUTPUT oTex2 = result.texcoord[2]; \
	OUTPUT oTex3 = result.texcoord[3]; \
	OUTPUT oTex4 = result.texcoord[4]; \
	OUTPUT oTex5 = result.texcoord[5]; \
	OUTPUT oTex6 = result.texcoord[6]; \
	OUTPUT oTex7 = result.texcoord[7]; \
	DP4 oPos.x , mvp[0] , iPos; \
	DP4 oPos.y , mvp[1] , iPos; \
	DP4 oPos.z , mvp[2] , iPos; \
	DP4 oPos.w, mvp[3] ,iPos; \
	MOV oTex0, iTexCoord; \
	MAD oTex2 , offset.x, vector, iTexCoord ; \
	MAD oTex3 , offset.y, vector, iTexCoord ; \
	MAD oTex4 , offset.z, vector, iTexCoord ; \
	MAD oTex5 , -offset.x, vector, iTexCoord ; \
	MAD oTex6 , -offset.y, vector, iTexCoord ; \
	MAD oTex7 , -offset.z, vector, iTexCoord ; \
	END";



	static const string dof_fs = 
	"!!ARBfp1.0 \
	OPTION ARB_precision_hint_nicest; \
	ATTRIB iTex0 = fragment.texcoord[0]; \
	ATTRIB iTex2 = fragment.texcoord[2]; \
	ATTRIB iTex3 = fragment.texcoord[3]; \
	ATTRIB iTex4 = fragment.texcoord[4]; \
	ATTRIB iTex5 = fragment.texcoord[5]; \
	ATTRIB iTex6 = fragment.texcoord[6]; \
	ATTRIB iTex7 = fragment.texcoord[7]; \
	PARAM one = { 1.0, 1.0, 1.0, 1.0 }; \
	PARAM one2 = { 1.0, 1.0, 0.0, 0.0 }; \
	PARAM factor = { 0.125, 0.95, 50.0, 0.1666 }; \
	PARAM dofParams = program.local[0]; \
	TEMP color; \
	TEMP depth_base; \
	TEMP filter_base; \
	TEMP filter_CoC; \
	TEMP depth; \
	TEMP depth_factor; \
	TEMP depth_factor2; \
	OUTPUT finalColor = result.color; \
	TEX depth_base, iTex0, texture[1], 2D; \
	TEX filter_base, iTex0, texture[0] , 2D ; \
	TEX color, iTex2, texture[0] , 2D ; \
	TEX depth, iTex2, texture[1] , 2D ; \
	SGE depth_factor.x, depth, depth_base; \
	MUL filter_CoC, color, depth_factor.xxxx; \
	TEX color, iTex3, texture[0] , 2D ; \
	TEX depth, iTex3, texture[1] , 2D ; \
	SGE depth_factor.y, depth, depth_base; \
	MAD filter_CoC, color, depth_factor.yyyy, filter_CoC; \
	TEX color, iTex4, texture[0] , 2D ; \
	TEX depth, iTex4, texture[1] , 2D ; \
	SGE depth_factor.z, depth, depth_base; \
	MAD filter_CoC, color, depth_factor.zzzz, filter_CoC; \
	TEX color, iTex5, texture[0] , 2D ; \
	TEX depth, iTex5, texture[1] , 2D ; \
	SGE depth_factor.w, depth, depth_base; \
	MAD filter_CoC, color, depth_factor.wwww, filter_CoC; \
	TEX color, iTex6, texture[0] , 2D ; \
	TEX depth, iTex6, texture[1] , 2D ; \
	SGE depth_factor2.x, depth, depth_base; \
	MAD filter_CoC, color, depth_factor2.xxxx, filter_CoC; \
	TEX color, iTex7, texture[0] , 2D ; \
	TEX depth, iTex7, texture[1] , 2D ; \
	SGE depth_factor2.y, depth, depth_base; \
	MAD filter_CoC, color, depth_factor2.yyyy, filter_CoC; \
	DP4 depth_factor, depth_factor, one; \
	DP4 depth_factor2, depth_factor2, one2; \
	ADD depth_factor, depth_factor, depth_factor2; \
	#                           \
	# final equation     \
	#                           \n\
	SUB_SAT depth_base, depth_base, dofParams.yyyy; \
	MUL_SAT depth_base, depth_base, dofParams.zzzz; \
	MUL filter_CoC, filter_CoC, depth_base; \
	MUL depth_factor, depth_factor, factor.xxxx; \
	MUL depth_factor, depth_factor, -depth_base; \
	MAD filter_CoC, filter_CoC, factor.xxxx, filter_base; \
	MAD finalColor, filter_base, depth_factor, filter_CoC; \
	MOV finalColor.w, one.w; \
	END" ;
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
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    //  Render X-blur in pixel buffer
    RAPTOR_HANDLE noDevice;
    tmpDisplay->glBindDisplay(noDevice);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
	depthInput->glvkRender();
    glActiveTextureARB(GL_TEXTURE0_ARB);
	getColorInput()->glvkRender();


#if defined(GL_ARB_vertex_shader)
	DOFShader->glGetVertexProgram()->setProgramParameters(vp_paramsX);
	DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_program)
	DOFShader->glGetVertexShader()->setProgramParameters(vp_paramsX);
	DOFShader->glGetFragmentShader()->setProgramParameters(fp_params);
#endif

	DOFShader->glRender();
    glDrawBuffer();
	DOFShader->glStop();

    tmpDisplay->glUnBindDisplay();

	for (unsigned int i=2;i<=m_nbBlur;i++)
	{
		//  Render Y-blur in current buffer
		tmpDisplay2->glBindDisplay(noDevice);
		tmpTexture->glvkRender();

	#if defined(GL_ARB_vertex_shader)
		DOFShader->glGetVertexProgram()->setProgramParameters(vp_paramsY);
		DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
	#elif defined(GL_ARB_vertex_program)
		DOFShader->glGetVertexShader()->setProgramParameters(vp_paramsY);
		DOFShader->glGetFragmentShader()->setProgramParameters(fp_params);
	#endif

		DOFShader->glRender();
		glDrawBuffer();
		DOFShader->glStop();
		tmpDisplay2->glUnBindDisplay();

		//  Render X-blur in pixel buffer
		tmpDisplay->glBindDisplay(noDevice);
		tmpTexture2->glvkRender();

#if defined(GL_ARB_vertex_shader)
		DOFShader->glGetVertexProgram()->setProgramParameters(vp_paramsX);
		DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_program)
		DOFShader->glGetVertexShader()->setProgramParameters(vp_paramsX);
		DOFShader->glGetFragmentShader()->setProgramParameters(fp_params);
#endif

		DOFShader->glRender();
		glDrawBuffer();
		DOFShader->glStop();

		tmpDisplay->glUnBindDisplay();
	}
}

void CDOFFilter::glRenderFilterOutput()
{
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

	//  Render Y-blur in current buffer
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	depthInput->glvkRender();
	glActiveTextureARB(GL_TEXTURE0_ARB);
	tmpTexture->glvkRender();

#if defined(GL_ARB_vertex_shader)
	DOFShader->glGetVertexProgram()->setProgramParameters(vp_paramsY);
	DOFShader->glGetFragmentProgram()->setProgramParameters(fp_params);
#elif defined(GL_ARB_vertex_program)
	DOFShader->glGetVertexShader()->setProgramParameters(vp_paramsY);
	DOFShader->glGetFragmentShader()->setProgramParameters(fp_params);
#endif

	DOFShader->glRender();
	glDrawBuffer();
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
		depthInput = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_DEPTH24,
															CTextureObject::CGL_OPAQUE,
															CTextureObject::CGL_UNFILTERED,
															depthExternalSource);
		colorInput = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															CTextureObject::CGL_UNFILTERED,
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

		tmpTexture = filterFactory.glCreateTexture(	CTextureObject::CGL_COLOR24_ALPHA,
			                                        CTextureObject::CGL_OPAQUE,
				                                    CTextureObject::CGL_UNFILTERED);
		filterFactory.glResizeTexture(tmpTexture,state.width,state.height);
		tmpTexture->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
		m_pRenderTextures->addTexture(tmpTexture);

		tmpTexture2 = filterFactory.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,
			                                        CTextureObject::CGL_OPAQUE,
				                                    CTextureObject::CGL_UNFILTERED);
		filterFactory.glResizeTexture(tmpTexture2,state.width,state.height);
		tmpTexture2->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
		m_pRenderTextures2->addTexture(tmpTexture2);
	}

    tmpDisplay = Raptor::glCreateDisplay(state);
    CRenderingProperties *rp = tmpDisplay->getRenderingProperties();
    rp->setTexturing(CRenderingProperties::ENABLE);
    rp->setCullFace(CRenderingProperties::DISABLE);
    rp->setDepthTest(CRenderingProperties::DISABLE);
    rp->setLighting(CRenderingProperties::DISABLE);
    rp->clear(CGL_NULL);
    tmpDisplay->setViewPoint(NULL);

	tmpDisplay2 = Raptor::glCreateDisplay(state);
    rp = tmpDisplay2->getRenderingProperties();
    rp->setTexturing(CRenderingProperties::ENABLE);
    rp->setCullFace(CRenderingProperties::DISABLE);
    rp->setDepthTest(CRenderingProperties::DISABLE);
    rp->setLighting(CRenderingProperties::DISABLE);
    rp->clear(CGL_NULL);
    tmpDisplay2->setViewPoint(NULL);

	if (m_fModel == RENDER_BUFFER)
	{
		tmpDisplay->glBindDisplay(*m_pRenderTextures);
		tmpDisplay2->glBindDisplay(*m_pRenderTextures2);
	}

	if (m_fModel == RENDER_TEXTURE)
	{
		tmpTexture = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															CTextureObject::CGL_UNFILTERED,
															tmpDisplay);
		tmpTexture2 = filterFactory.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															CTextureObject::CGL_UNFILTERED,
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
#if defined(GL_ARB_vertex_shader)
	CVertexProgram *vp = DOFShader->glGetVertexProgram("dof_vp");
	bool res = vp->glLoadProgram(dof_vp);
	if (res)
		vp->setProgramParameters(vp_paramsX);

	CFragmentProgram *fp = DOFShader->glGetFragmentProgram("dof_fp");
	res = res && fp->glLoadProgram(dof_fp);
	if (res)
	{
		fp_params.addParameter("colorMap",CTextureUnitSetup::IMAGE_UNIT_0);
		fp_params.addParameter("depthMap",CTextureUnitSetup::IMAGE_UNIT_1);
		fp->setProgramParameters(fp_params);
	}

	res = res && DOFShader->glCompileShader();
#elif defined(GL_ARB_vertex_program)
	CVertexShader *vs = DOFShader->glGetVertexShader("dof_vs");
	bool res = vs->glLoadProgram(dof_vs);
	CFragmentShader *fs = DOFShader->glGetFragmentShader("dof_fs");
	res = res && fs->glLoadProgram(dof_fs);
#endif
}


