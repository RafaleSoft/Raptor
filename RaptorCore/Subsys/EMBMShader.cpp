// PhongShader.cpp: implementation of the CPhongShader class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_EMBMSHADER_H__99A5AF45_D5C7_4F43_851C_A31FC52DB237__INCLUDED_)
	#include "EMBMShader.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "LightAttributes.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif


RAPTOR_NAMESPACE

int CEMBMShader::environmentMap = -1;

//#define PROCEDURAL_PERLIN
#ifdef PROCEDURAL_PERLIN
static CTextureObject *permutation = NULL;
static int permSampler = -1;
#endif


CEMBMShader::CEMBMShader(void)
	:CBumpShader("EMBM_SHADER"),
	m_bEnabled(false)
{
}

CEMBMShader::CEMBMShader(const CEMBMShader& shader)
	:CBumpShader(shader),
	m_bEnabled(shader.m_bEnabled)
{
}

CShader* CEMBMShader::glClone(const std::string& newShaderName) const
{
	CEMBMShader* embm = new CEMBMShader(*this);
	if (!newShaderName.empty())
		embm->setName(newShaderName);
	embm->m_bEnabled = m_bEnabled;
	return embm;
}

CEMBMShader::~CEMBMShader(void)
{
}


void CEMBMShader::glInit()
{
	CShader *shaderLib = new CShader();
	CVertexShader *vp = shaderLib->glGetVertexProgram("PPIXEL_BUMP_VTX_PROGRAM");
	CFragmentProgram *fp = shaderLib->glGetFragmentProgram("PPIXEL_BUMP_TEX_PROGRAM");
	std::string embm_vertexshader = vp->glGetProgramString();
	std::string embm_pixelshader = fp->glGetProgramString();
	shaderLib->releaseReference();

	//!	First create the program, to get it with the shader
	//!	and unset auto delete shader
	vp = new CVertexShader("PPIXEL_EMBM_VTX_PROGRAM");
	size_t pos = embm_vertexshader.find("#version");
	if (pos < embm_vertexshader.length())
	{
		size_t pos2 = embm_vertexshader.find("\n",pos);
		embm_vertexshader.insert(pos2+1,string("#define EMBM_RENDERING 1\n\n"));
	}
	vp->glLoadProgram(embm_vertexshader);

	//!	First create the program, to get it with the shader
	//!	and unset auto delete shader
	fp = new CFragmentProgram("PPIXEL_EMBM_TEX_PROGRAM");
	pos = embm_pixelshader.find("#version");
	if (pos < embm_pixelshader.length())
	{
		size_t pos2 = embm_pixelshader.find("\n",pos);
		embm_pixelshader.insert(pos2+1,string("#define EMBM_RENDERING 1\n\n"));
	}
	fp->glLoadProgram(embm_pixelshader);

	vp = glGetVertexProgram("PPIXEL_BUMP_VTX_PROGRAM");
	fp = glGetFragmentProgram("PPIXEL_BUMP_TEX_PROGRAM");

	glCompileShader();

#ifdef PROCEDURAL_PERLIN
	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();

	//! 256 interpolated values are enough for good results.
    //! For high quality filtering, future release may allow a user defined size.
    permutation = filterFactory.glCreateTexture(ITextureObject::CGL_LIGHTMAP,
												CTextureObject::CGL_OPAQUE,
												ITextureObject::CGL_UNFILTERED);
    permutation->setSize(512,1);

	CImage perlin;
	perlin.allocatePixels(512,1);
	unsigned char* perm = perlin.getPixels();
	for (unsigned int i=0;i<256;i++)
		perm[i] = i;
	for (unsigned int i=0;i<256;i++)
	{
		unsigned int j = rand() % 256;
        unsigned int p = perm[j];
        perm[j] = perm[i];
        perm[i+256] = perm[i] = p;
	}
	filterFactory.glLoadTexture(permutation,perlin);
#endif
}

void CEMBMShader::glRender(void)
{
	CBumpShader::glRender();

#if defined(GL_ARB_shader_objects)
	if (m_bEnabled)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

		if (environmentMap < 0)
		{
			GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			environmentMap = pExtensions->glGetUniformLocationARB(program, "environmentMap");
		}
		if (environmentMap >= 0)
			pExtensions->glUniform1iARB(environmentMap,CTextureUnitSetup::IMAGE_UNIT_3);

#ifdef PROCEDURAL_PERLIN
		if (permSampler < 0)
		{
			GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			permSampler = pExtensions->glGetUniformLocationARB(program, "permSampler");
		}
		if (permSampler >= 0)
		{
			GLint previousTMU = GL_TEXTURE0_ARB;
			glGetIntegerv(GL_ACTIVE_TEXTURE_ARB,&previousTMU);
			pExtensions->glActiveTextureARB(GL_TEXTURE4_ARB);
			permutation->glRender();
			pExtensions->glUniform1iARB(environmentMap,CTextureUnitSetup::IMAGE_UNIT_4);
			pExtensions->glActiveTextureARB(previousTMU);
		}
#endif
	}
#endif
}

void CEMBMShader::enableEmbm(bool enable)
{
	if (enable != m_bEnabled)
	{
		glRemoveVertexProgram();
		glRemoveFragmentProgram();

		CProgramParameters params;
		params.addParameter("tangent", CProgramParameters::ADDITIONAL_PARAM1);
		CProgramParameters params2;
		CVertexShader *vp = NULL;
		CFragmentProgram *fp = NULL;
	
		if (enable)
		{
			vp = glGetVertexProgram("PPIXEL_EMBM_VTX_PROGRAM");
			fp = glGetFragmentProgram("PPIXEL_EMBM_TEX_PROGRAM");
		}
		else
		{
			vp = glGetVertexProgram("PPIXEL_BUMP_VTX_PROGRAM");
			fp = glGetFragmentProgram("PPIXEL_BUMP_TEX_PROGRAM");
		}

		vp->setProgramParameters(params);
		fp->setProgramParameters(params2);

		glCompileShader();
		m_bEnabled = enable;
	}
}