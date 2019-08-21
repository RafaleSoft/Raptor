// BlurFilter.cpp: implementation of the CBlurFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_GEOMETRYSHADER_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
	#include "GLHierarchy/GeometryShader.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
    #include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_BLURFILTER_H__46C7BB3A_6996_4E57_A387_A56F89777EF9__INCLUDED_)
    #include "BlurFilter.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif

// With RGBA32 pixel model, gaussian coefficients can
// be expanded because scaled filter buffer will produce 
// contributions to colors < 1/256 value, i.e. unused.
// By extending this size, we can benefit of a virtually wider filter, but
// this results in a different value of sigma !
static const unsigned int GAUSSIAN_256[] =
//{ 0, 1, 2, 3, 4, 5, 6, 7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33  };
  { 0, 1, 2, 3, 4, 5, 6, 7, 10, 13, 16, 19, 22, 27, 28, 29, 28, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33  };
// With different coefficients, we also need to store the normalizing factor
static const int SCALE_256[] =
//{ 0, 1, 2, 3,  4,  5,  6,   7,   8,    9,    10,     11,      12,       13,        14,        15,        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33  };
  { 1, 2, 4, 8, 16, 32, 64, 128, 510, 4070, 32625, 260168, 2082058, 66481040, 133422540, 267437100, 134014560 };
#define USE_GAUSSIAN_256 1


static const unsigned int MAX_SIZE = 17;
static const unsigned int DEFAULT_SIZE = 9;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlurFilter::CBlurFilter()
    :xKernelPass(NULL),xBuffer(NULL),
	m_size(0),m_bRebuild(true),
	hBlur(NULL),vBlur(NULL),
	m_model(BLUR_BOX)
{
}

CBlurFilter::~CBlurFilter()
{
	glDestroyFilter();
}

void CBlurFilter::setBlurModel(BLUR_MODEL model)
{
	if (model != m_model)
	{
		m_model = model;
		m_bRebuild = true;
	}
}

void CBlurFilter::setBlurSize(unsigned int size)
{
	const unsigned int old_size = m_size;
	// minimal size is above 2, which the lowest level
	// of filtering obtained with bilinear filtering
	if ((size > 2) && (size < MAX_SIZE))
		m_size = size;
	else
		m_size = DEFAULT_SIZE;

	if (old_size == m_size)
		return;

	vector<unsigned int> coeffs;
	coeffs.push_back(1);
	coeffs.push_back(1);

	unsigned int sz = 2;
	// With RGBA32 pixel model, gaussian coefficients can
	// be extanded because scaled filter buffer will produced 
	// contributions to colors < 1/256 value, i.e. unused.
	// By extending this size, we can benefit of a virtually wider filter.
#if defined(USE_GAUSSIAN_256)
	while (sz < GAUSSIAN_256[m_size])
#else
	while (sz < m_size)
#endif
	{
		unsigned int previous = coeffs[0];
		unsigned int current = coeffs[1];
		coeffs.push_back(1);
		for (size_t j=1;j<sz;j++)
		{
			coeffs[j] = previous + current;
			previous = current;
			current = coeffs[j+1];
		}

		sz++;
	}

	
	m_gaussian_coeffs.clear();
	m_gaussian_offsets.clear();
	size_t base = (sz-m_size)/2;
#if defined(USE_GAUSSIAN_256)
	float scale = 1.0f / SCALE_256[m_size];
	for (size_t i=base;i<sz-base;i++)
#else
	float scale = 1.0f / pow(2.0f,(int)sz-1);
	for (size_t i=0;i<m_size;i++)
#endif
	{
		m_gaussian_coeffs.push_back(scale*coeffs[i]);
		m_gaussian_offsets.push_back(-0.5f*float(m_size-1) + (i-base));
	}

	// Optimize coefficients and offsets for using bilinear filtering
	if ((BLUR_GAUSSIAN_LINEAR == m_model) ||
		(BLUR_BOX_LINEAR == m_model))
	{
		// weight for 2 pixels bi-filtered is the sum of weight
		vector<float> linear_coeffs;
		vector<float> linear_offsets;
		float mid_coeff = 0.0f;
		float mid_offset = 0.0f;
		if ((m_size&1) == 1)
		{
			mid_coeff = m_gaussian_coeffs[m_size/2];
			mid_offset = m_gaussian_offsets[m_size/2];
			m_gaussian_coeffs.erase(m_gaussian_coeffs.begin()+(m_size/2));
			m_gaussian_offsets.erase(m_gaussian_offsets.begin()+(m_size/2));
		}

		for (size_t i=0;i<m_gaussian_coeffs.size();i+=2)
		{
			float w1 = m_gaussian_coeffs[i];
			float w2 = m_gaussian_coeffs[i+1];
			linear_coeffs.push_back(w1 + w2);
			// offset for bilinear sampling is:
			// o" = o2*w2 + o1*w1 / (w1 + w2)
			linear_offsets.push_back((m_gaussian_offsets[i]*w1 + m_gaussian_offsets[i+1]*w2) / (w1 + w2));
		}

		if ((m_size&1) == 1)
		{
			linear_coeffs.insert(linear_coeffs.begin()+((m_size-1)/4),mid_coeff);
			linear_offsets.insert(linear_offsets.begin()+((m_size-1)/4),mid_offset);
		}

		m_gaussian_coeffs = linear_coeffs;
		m_gaussian_offsets = linear_offsets;
	}

	m_bRebuild = true;
}

void CBlurFilter::glRenderFilter()
{
	if (m_bRebuild)
	{
		CRaptorDisplayConfig config;
		xBuffer->glQueryStatus(config,GL_CONFIG_STATE_QUERY);
		glBuildFilter(config.width,config.height);
	}

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    //! First pass : xPass of the kernel assuming it is separable
    RAPTOR_HANDLE noDevice;
	xBuffer->glvkBindDisplay(noDevice);
    glActiveTextureARB(GL_TEXTURE0_ARB);
	getColorInput()->glvkRender();

    hBlur->glRender();
	glDrawFilter();
	hBlur->glStop();

	glBindTexture(GL_TEXTURE_2D,0);
	xBuffer->glvkUnBindDisplay();
}

void CBlurFilter::glRenderFilterOutput()
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

    //! Second (last) pass : yPass of the kernel, assuming it is separable
	glActiveTextureARB(GL_TEXTURE0_ARB);
	xKernelPass->glvkRender();

	vBlur->glRender();
	glDrawFilter();
	vBlur->glStop();

	glBindTexture(GL_TEXTURE_2D,0);
}

bool CBlurFilter::glBuildFilter(int width,int height)
{
	bool res = false;

	if ((m_model != BLUR_BOX) &&
		(m_model != BLUR_BOX_LINEAR) &&
		(m_model != BLUR_GAUSSIAN) &&
		(m_model != BLUR_GAUSSIAN_LINEAR))
		return res;

	if (hBlur == NULL)
		hBlur = new CShader("HORIZONTAL_BLUR_SHADER");
	if (vBlur == NULL)
		vBlur = new CShader("VERTICAL_BLUR_SHADER");
		
	const char *ENDL = "\r\n";
	const char *offsets[2] = { "vec2(offset[i],0.0));", "vec2(0.0,offset[i]));" };
	string srcs[2];	// shader source: 0 -> x, 1 -> y

	for (int dirs=0;dirs<2;dirs++)
	{
		stringstream shader_src;
		size_t sz = m_gaussian_coeffs.size();
		float dim = (dirs == 0 ? width : height);

		//	Write texel offsets
		shader_src << "#version 440" << ENDL << ENDL;
		shader_src << "const float offset[" << sz << "] = float[]( ";
		for (size_t i=0;i<sz-1;i++)
			shader_src << m_gaussian_offsets[i] / dim << " , ";
		shader_src << m_gaussian_offsets[sz-1] / dim << " );" << ENDL;
		
		if ((BLUR_BOX == m_model) || (BLUR_BOX_LINEAR == m_model))
		{
			//	Write main code
			shader_src << "uniform sampler2D diffuseMap;" << ENDL << ENDL;
			shader_src << "layout(location = 1) in vec4 g_TexCoord;" << ENDL;
			shader_src << "layout(location = 0) out vec4 o_Color;" << ENDL << ENDL;
			shader_src << "void main (void)" << ENDL;
			shader_src << "{" << ENDL;
			shader_src << "	vec4    finalColor = vec4(0.0,0.0,0.0,0.0);" << ENDL << ENDL;

			shader_src << "	for (int i=0;i<" << sz << ";i++)" << ENDL;
			shader_src << "	{" << ENDL;
			shader_src << "		finalColor += texture(diffuseMap,g_TexCoord.xy + " << offsets[dirs] << ENDL;
			shader_src << "	}" << ENDL << ENDL;
			shader_src << "	o_Color = " << 1.0f / sz << " * finalColor;" << ENDL;
			shader_src << "}" << ENDL;
		}
		else if ((BLUR_GAUSSIAN == m_model) || (BLUR_GAUSSIAN_LINEAR == m_model))
		{
			//	Write gaussian weights
			shader_src << "const float weights[" << sz << "] = float[]( ";
			for (size_t i=0;i<sz-1;i++)
				shader_src << m_gaussian_coeffs[i] << " , ";
			shader_src << m_gaussian_coeffs[sz-1] << " );" << ENDL << ENDL;

			//	Write main code
			shader_src << "uniform sampler2D diffuseMap;" << ENDL << ENDL;
			shader_src << "layout(location = 1) in vec4 g_TexCoord;" << ENDL;
			shader_src << "layout(location = 0) out vec4 o_Color;" << ENDL << ENDL;
			shader_src << "void main (void)" << ENDL;
			shader_src << "{" << ENDL;
			shader_src << "	vec4    finalColor = vec4(0.0,0.0,0.0,0.0);" << ENDL << ENDL;

			shader_src << "	for (int i=0;i<" << sz << ";i++)" << ENDL;
			shader_src << "	{" << ENDL;
			shader_src << "		finalColor += weights[i] * texture(diffuseMap,g_TexCoord.xy + " << offsets[dirs] << ENDL;
			shader_src << "	}" << ENDL << ENDL;
			shader_src << "	o_Color = finalColor;" << ENDL;
			shader_src << "}" << ENDL;
		}
		srcs[dirs] = shader_src.str();
	}
		
	params.clear();
	params.addParameter("diffuseMap",CTextureUnitSetup::IMAGE_UNIT_0);
	
	CVertexProgram *vp = hBlur->glGetVertexProgram("EMPTY_PROGRAM");
	CGeometryShader *gp = hBlur->glGetGeometryProgram("FULL_SCREEN_GEO_PROGRAM");
	CFragmentProgram *fp = hBlur->glGetFragmentProgram("hBlur_fp");
	res = fp->glLoadProgram(srcs[0]);
	fp->setProgramParameters(params);
	res = res && hBlur->glCompileShader();

	vp = vBlur->glGetVertexProgram("EMPTY_PROGRAM");
	gp = vBlur->glGetGeometryProgram("FULL_SCREEN_GEO_PROGRAM");
	fp = vBlur->glGetFragmentProgram("vBlur_fp");
	res = res && fp->glLoadProgram(srcs[1]);
	fp->setProgramParameters(params);

	res = res && vBlur->glCompileShader();

	// Update filtering
	if ((BLUR_BOX_LINEAR == m_model) || (BLUR_GAUSSIAN_LINEAR == m_model))
	{
		colorInput->glvkRender();
		colorInput->glvkUpdateFilter(ITextureObject::CGL_BILINEAR);

		xKernelPass->glvkRender();
		xKernelPass->glvkUpdateFilter(ITextureObject::CGL_BILINEAR);
	}

	m_bRebuild = !res;
	return res;	
}

bool CBlurFilter::glInitFilter(void)
{
    if (!CRaptorDisplayFilter::glInitFilter())
        return false;

	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();

	bool previousResize = filterFactory.getConfig().useTextureResize();
	filterFactory.getConfig().useTextureResize(false);

	if ((colorExternalSource != NULL) && (m_fModel == RENDER_TEXTURE))
	{
		//!    Source is unfiltered to avoid artifacts ( see comment below ).
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

    //! A buffer to render the first pass of the blur
    //! ( the two available kernels are separable, so it is more efficient
    //! to proceed in 2 passes : x-y buffers are created to render intermediare results.
    CRaptorDisplayConfig state;
	state.width = m_fXfactor * colorInput->getWidth();
	state.height = m_fYfactor * colorInput->getHeight();
	state.acceleration = CRaptorDisplayConfig::HARDWARE;
	state.display_mode = CGL_RGBA;
	state.bind_to_texture = true;
	state.depth_buffer = false;
	state.double_buffer = false;
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
		//! Do not use bilinear filtering because the colors fetched may not correspond 
		//! to the kernel factors with some texture sizes non power of 2.
		//! ( Specifically where tex coord is near a texel edge, 
		//! and also because it is shifted to work in texels' center and 
		//! should be faster ).
		xKernelPass = filterFactory.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															ITextureObject::CGL_UNFILTERED, //CGL_BILINEAR,
															xBuffer);
	}

	filterFactory.getConfig().useTextureResize(previousResize);

	// Create & load shaders to perform a 2 pass blur using depth value.
	bool res = true;
	if (m_size == 0)
		setBlurSize(DEFAULT_SIZE);

	if (m_bRebuild)
		res = glBuildFilter(state.width,state.height);

    return res;
}

 
 void CBlurFilter::glDestroyFilter(void)
 {
     CRaptorDisplayFilter::glDestroyFilter();

	if (hBlur != NULL)
	{
        hBlur->releaseReference();
		hBlur = NULL;
	}
	if (vBlur != NULL)
	{
        vBlur->releaseReference();
		vBlur = NULL;
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
 }

