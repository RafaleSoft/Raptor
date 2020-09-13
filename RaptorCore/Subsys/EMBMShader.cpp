/***************************************************************************/
/*                                                                         */
/*  EMBMShader.cpp                                                         */
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
#if !defined(AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "LightAttributes.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Material.h"
#endif


RAPTOR_NAMESPACE


static bool embm_shaders_initialized = false;

//#define PROCEDURAL_PERLIN
#ifdef PROCEDURAL_PERLIN
	static ITextureObject *permutation = NULL;
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

	if (hasOpenGLShader())
		embm->glInit();

	return embm;
}

CEMBMShader::~CEMBMShader(void)
{
}


void CEMBMShader::glInit()
{
	CShader *shaderLib = new CShader();
	COpenGLShaderStage *stage = glGetOpenGLShader("EMBM_SHADER_PROGRAM");

	if (!embm_shaders_initialized)
	{
		CVertexShader *vp = stage->glGetVertexShader("PPIXEL_BUMP_VTX_PROGRAM");
		CFragmentShader *fp = stage->glGetFragmentShader("PPIXEL_BUMP_TEX_PROGRAM");
		std::string embm_vertexshader = vp->glGetProgramString();
		std::string embm_pixelshader = fp->glGetProgramString();
		shaderLib->releaseReference();

		//!	First create the program, to get it with the shader
		//!	and unset auto delete shader
		vp = new CVertexShader("PPIXEL_EMBM_VTX_PROGRAM");
		size_t pos = embm_vertexshader.find("#version");
		if (pos < embm_vertexshader.length())
		{
			size_t pos2 = embm_vertexshader.find("\n", pos);
			embm_vertexshader.insert(pos2 + 1, string("#define EMBM_RENDERING 1\n\n"));
		}
		vp->glLoadProgram(embm_vertexshader);

		//!	First create the program, to get it with the shader
		//!	and unset auto delete shader
		fp = new CFragmentShader("PPIXEL_EMBM_TEX_PROGRAM");
		pos = embm_pixelshader.find("#version");
		if (pos < embm_pixelshader.length())
		{
			size_t pos2 = embm_pixelshader.find("\n", pos);
			embm_pixelshader.insert(pos2 + 1, string("#define EMBM_RENDERING 1\n\n"));
		}
		fp->glLoadProgram(embm_pixelshader);

		embm_shaders_initialized = true;
	}

	CVertexShader *vp = stage->glGetVertexShader("PPIXEL_BUMP_VTX_PROGRAM");
	CFragmentShader *fp = stage->glGetFragmentShader("PPIXEL_BUMP_TEX_PROGRAM");

	CProgramParameters params;
	params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
	params.addParameter("normalMap", CTextureUnitSetup::IMAGE_UNIT_1);
	params.addParameter("environmentMap", CTextureUnitSetup::IMAGE_UNIT_3);

#if defined(GL_ARB_uniform_buffer_object)
	CMaterial::Material_t M;
	CProgramParameters::CParameter<CMaterial::Material_t> material(M);
	material.name("Material");
	material.locationType = GL_UNIFORM_BLOCK_BINDING_ARB;
	params.addParameter(material);
#endif

	stage->setProgramParameters(params);

	stage->glCompileShader();

#ifdef PROCEDURAL_PERLIN
	CTextureFactory &filterFactory = CTextureFactory::getDefaultFactory();

	//! 256 interpolated values are enough for good results.
    //! For high quality filtering, future release may allow a user defined size.
    permutation = filterFactory.glCreateTexture(ITextureObject::CGL_LIGHTMAP,
												ITextureObject::CGL_OPAQUE,
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
		//const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

		//if (environmentMap < 0)
		//{
		//	GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
		//	environmentMap = pExtensions->glGetUniformLocationARB(program, "environmentMap");
		//}
		//if (environmentMap >= 0)
		//	pExtensions->glUniform1iARB(environmentMap,CTextureUnitSetup::IMAGE_UNIT_3);

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
		COpenGLShaderStage *stage = glGetOpenGLShader();

		stage->glRemoveVertexShader();
		stage->glRemoveFragmentShader();

		CProgramParameters params;
		params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
		params.addParameter("normalMap", CTextureUnitSetup::IMAGE_UNIT_1);
		params.addParameter("environmentMap", CTextureUnitSetup::IMAGE_UNIT_3);
		stage->setProgramParameters(params);

		if (enable)
		{
			stage->glGetVertexShader("PPIXEL_EMBM_VTX_PROGRAM");
			stage->glGetFragmentShader("PPIXEL_EMBM_TEX_PROGRAM");
		}
		else
		{
			stage->glGetVertexShader("PPIXEL_BUMP_VTX_PROGRAM");
			stage->glGetFragmentShader("PPIXEL_BUMP_TEX_PROGRAM");
		}

		stage->glCompileShader();
		m_bEnabled = enable;
	}
}