// MagnifierFilter.cpp: implementation of the CMagnifierFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
    #include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
	#include "GLHierarchy/FragmentProgram.h"
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
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_MAGNIFIERFILTER_H__3660D446_2F92_4D02_A795_BFF8336D61D2__INCLUDED_)
    #include "MagnifierFilter.h"
#endif


static const int KERNEL_SIZE = 256;

#if defined(GL_ARB_vertex_shader)
	static const string kernel_vs = 
	"#version 120			\n\
	uniform vec4 center;	\n\
	void main(void)			\n\
	{						\n\
		gl_Position = ftransform();\n\
		gl_TexCoord[0] = gl_MultiTexCoord0 + center;\n\
	}";

	static const string xk_ps =
	"#version 120				\n\
	\n\
	uniform sampler2D color;	\n\
	uniform sampler2D factor;	\n\
	\n\
	uniform vec4 size;			\n\
	uniform vec4 offset;		\n\
	void main(void)			\n\
	{						\n\
		vec4 subpixels = fract(size * gl_TexCoord[0]); \n\
		vec2 texcoord = gl_TexCoord[0].xy; \n\
		vec4 factors = texture2D(factor,subpixels.xy); \n\
	\n\
		vec4 colors = texture2D(color,texcoord); \n\
		vec4 colorsum = colors * factors.zzzz; \n\
	\n\
		colors = texture2D(color,texcoord + offset.xz); \n\
		colorsum = colorsum + colors * factors.wwww; \n\
	\n\
		colors = texture2D(color,texcoord - offset.xz); \n\
		colorsum = colorsum + colors * factors.yyyy; \n\
	\n\
		colors = texture2D(color,texcoord - offset.xz - offset.xz); \n\
		gl_FragColor = colorsum + colors * factors.xxxx; \n\
		gl_FragColor.w = 1.0; \n\
	}";

	static const string yk_ps =
	"#version 120				\n\
	\n\
	uniform sampler2D color;	\n\
	uniform sampler2D factor;	\n\
	\n\
	uniform vec4 size;			\n\
	uniform vec4 offset;		\n\
	void main(void)			\n\
	{						\n\
		vec4 subpixels = fract(size * gl_TexCoord[0]); \n\
		vec2 texcoord = gl_TexCoord[0].xy; \n\
		vec4 factors = texture2D(factor,subpixels.yx); \n\
	\n\
		vec4 colors = texture2D(color,texcoord); \n\
		vec4 colorsum = colors * factors.zzzz; \n\
	\n\
		colors = texture2D(color,texcoord + offset.zy); \n\
		colorsum = colorsum + colors * factors.wwww; \n\
	\n\
		colors = texture2D(color,texcoord - offset.zy); \n\
		colorsum = colorsum + colors * factors.yyyy; \n\
	\n\
		colors = texture2D(color,texcoord - offset.zy - offset.zy); \n\
		gl_FragColor = colorsum + colors * factors.xxxx; \n\
		gl_FragColor.w = 1.0; \n\
	}";
#elif defined(GL_ARB_vertex_program)
	static const string kernel_vp = 
	"!!ARBvp1.0 \
	ATTRIB iPos = vertex.position; \
	ATTRIB iTexCoord = vertex.texcoord[0]; \
	PARAM mvp[4] = { state.matrix.mvp }; \
	PARAM ofs = program.local[0]; \
	OUTPUT oPos = result.position; \
	OUTPUT oTex0 = result.texcoord[0]; \
	DP4 oPos.x , mvp[0] , iPos; \
	DP4 oPos.y , mvp[1] , iPos; \
	DP4 oPos.z , mvp[2] , iPos; \
	DP4 oPos.w, mvp[3] ,iPos; \
	ADD oTex0, iTexCoord, ofs; \
	END";

	static const string xk_fp =
	"!!ARBfp1.0 \
	ATTRIB iTex0 = fragment.texcoord[0]; \
	PARAM one_plusx = program.local[0];\
	PARAM size = program.local[1];\
	TEMP color; \
	TEMP factors; \
	TEMP subpixels; \
	TEMP colorsum; \
	TEMP offset; \
	OUTPUT finalColor = result.color; \
	MUL subpixels, iTex0, size; \
	FRC subpixels, subpixels; \
	TEX color, iTex0, texture[0] , 2D ; \
	TEX factors, subpixels.xyzw, texture[1], 2D; \
	MUL colorsum, color, factors.zzzz; \
	ADD offset, iTex0, one_plusx.xzzz; \
	TEX color, offset, texture[0] , 2D ; \
	MAD colorsum, color, factors.wwww, colorsum;\
	SUB offset, iTex0, one_plusx.xzzz; \
	TEX color, offset, texture[0] , 2D ; \
	MAD colorsum, color, factors.yyyy, colorsum; \
	SUB offset, offset, one_plusx.xzzz; \
	TEX color, offset, texture[0] , 2D ; \
	MAD finalColor, color, factors.xxxx, colorsum; \
	MOV finalColor.w, size.w; \
	END";

	static const string yk_fp = 
	"!!ARBfp1.0 \
	ATTRIB iTex0 = fragment.texcoord[0]; \
	PARAM one_plusy = program.local[0];\
	PARAM size = program.local[1];\
	TEMP color; \
	TEMP factors; \
	TEMP subpixels; \
	TEMP colorsum; \
	TEMP offset; \
	OUTPUT finalColor = result.color; \
	MUL subpixels, iTex0, size; \
	FRC subpixels, subpixels; \
	TEX color, iTex0, texture[0] , 2D ; \
	TEX factors, subpixels.yxzw, texture[1], 2D; \
	MUL colorsum, color, factors.zzzz; \
	ADD offset, iTex0, one_plusy.zyzz; \
	TEX color, offset, texture[0] , 2D ; \
	MAD colorsum, color, factors.wwww, colorsum;\
	SUB offset, iTex0, one_plusy.zyzz; \
	TEX color, offset, texture[0] , 2D ; \
	MAD colorsum, color, factors.yyyy, colorsum; \
	SUB offset, offset, one_plusy.zyzz; \
	TEX color, offset, texture[0] , 2D ; \
	MAD finalColor, color, factors.xxxx, colorsum; \
	MOV finalColor.w, size.w; \
	END";
#endif

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

#if defined(GL_ARB_vertex_shader)
	m_pYKernelShader->glGetVertexProgram()->setProgramParameters(v_params_y);
	m_pYKernelShader->glGetFragmentProgram()->setProgramParameters(f_params);
#elif defined(GL_ARB_vertex_program)
	m_pYKernelShader->glGetVertexShader()->setProgramParameters(v_params_y);
	m_pYKernelShader->glGetFragmentShader()->setProgramParameters(f_params);
#endif
	m_pYKernelShader->glRender();
    glDrawBuffer();
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

#if defined(GL_ARB_vertex_shader)
	m_pXKernelShader->glGetVertexProgram()->setProgramParameters(v_params_x);
	m_pXKernelShader->glGetFragmentProgram()->setProgramParameters(f_params);
#elif defined(GL_ARB_vertex_program)
	m_pXKernelShader->glGetVertexShader()->setProgramParameters(v_params_x);
	m_pXKernelShader->glGetFragmentShader()->setProgramParameters(f_params);
#endif
	m_pXKernelShader->glRender();
    glDrawBuffer();
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
															CTextureObject::CGL_OPAQUE,
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
                                                   CTextureObject::CGL_OPAQUE,
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
			                                        CTextureObject::CGL_OPAQUE,
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
															CTextureObject::CGL_OPAQUE,
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
	v_params_y.addParameter("center",ofsy);
	GL_COORD_VERTEX ofsx(0.5f * offsetParams.x, 0.0, 0.0, 0.0);
	v_params_x.addParameter("center",ofsx);
	f_params.addParameter("offset",offsetParams);
	f_params.addParameter("size",sizeParams);

    m_pXKernelShader = new CShader("XKERNEL_SHADER");
	m_pYKernelShader = new CShader("YKERNEL_SHADER");

#if defined(GL_ARB_vertex_shader)
	CVertexProgram *vp = m_pXKernelShader->glGetVertexProgram("magnifier_vp");
    bool res = vp->glLoadProgram(kernel_vs);
	CFragmentProgram *ps = m_pXKernelShader->glGetFragmentProgram("xk_ps");
    res = res && ps->glLoadProgram(xk_ps);
	res = res && m_pXKernelShader->glCompileShader();

	vp = m_pYKernelShader->glGetVertexProgram("magnifier_vp");
	ps = m_pYKernelShader->glGetFragmentProgram("yk_ps");
    res = res && ps->glLoadProgram(yk_ps);
	m_pYKernelShader->glCompileShader();

	f_params.addParameter("color",CTextureUnitSetup::IMAGE_UNIT_0);
	f_params.addParameter("factor",CTextureUnitSetup::IMAGE_UNIT_1);
#elif defined(GL_ARB_vertex_program)
    CVertexShader *vs = m_pXKernelShader->glGetVertexShader("magnifier_vp");
    bool res = vs->glLoadProgram(kernel_vp);
    CFragmentShader *fs = m_pXKernelShader->glGetFragmentShader("xk_fp");
    res = res && fs->glLoadProgram(xk_fp);

    vs = m_pYKernelShader->glGetVertexShader("magnifier_vp");
    fs = m_pYKernelShader->glGetFragmentShader("yk_fp");
    res = res && fs->glLoadProgram(yk_fp);
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

