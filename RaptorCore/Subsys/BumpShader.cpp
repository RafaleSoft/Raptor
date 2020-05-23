/***************************************************************************/
/*                                                                         */
/*  BumpShader.cpp                                                         */
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

#if !defined(AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_)
	#include "BumpShader.h"
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
#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "LightAttributes.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Material.h"
#endif


RAPTOR_NAMESPACE

int CBumpShader::lightEnable = -1;
int CBumpShader::diffuseMap = -1;
int CBumpShader::normalMap = -1;
int CBumpShader::eyePos = -1;


CBumpShader::CBumpShader(const std::string &name)
	:CShader(name)
{
}

CBumpShader::CBumpShader(const CBumpShader& shader)
	:CShader(shader)
{
}

CShader* CBumpShader::glClone(const std::string& newShaderName) const
{
	CBumpShader* bump = new CBumpShader(*this);
	if (!newShaderName.empty())
		bump->setName(newShaderName);

	if (hasOpenGLShader())
		bump->glInit();

	return bump;
}

CBumpShader::~CBumpShader(void)
{
}

void CBumpShader::glInit(void)
{
	COpenGLShaderStage *stage = glGetOpenGLShader("BUMP_SHADER_PROGRAM");

	stage->glGetVertexShader("PPIXEL_BUMP_VTX_PROGRAM");
	stage->glGetFragmentShader("PPIXEL_BUMP_TEX_PROGRAM");
	
	CProgramParameters params;
	params.addParameter("tangent", CProgramParameters::ADDITIONAL_PARAM1);

#if defined(GL_ARB_uniform_buffer_object)
	CMaterial::Material_t M;
	CProgramParameters::CParameter<CMaterial::Material_t> material(M);
	material.name("Material");
	material.locationType = GL_UNIFORM_BLOCK_BINDING_ARB;
	params.addParameter(material);
#endif

	stage->setProgramParameters(params);

	stage->glCompileShader();
}

void CBumpShader::glRender(void)
{
	CShader::glRender();

#if defined(GL_ARB_shader_objects)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if ((lightEnable < 0) || (diffuseMap < 0) || (normalMap < 0) || (eyePos < 0))
	{
		GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
		lightEnable = pExtensions->glGetUniformLocationARB(program, "lightEnable");
		diffuseMap = pExtensions->glGetUniformLocationARB(program, "diffuseMap");
		normalMap = pExtensions->glGetUniformLocationARB(program, "normalMap");
		eyePos = pExtensions->glGetUniformLocationARB(program, "eyePos");
	}

	int *bLights = CLightAttributes::getLightOrder();
	if ((lightEnable >= 0) && (NULL != bLights))
		pExtensions->glUniform1ivARB(lightEnable,CLightAttributes::MAX_LIGHTS,bLights);
		
	if (diffuseMap >= 0)
		pExtensions->glUniform1iARB(diffuseMap,CTextureUnitSetup::IMAGE_UNIT_0);
		
	if (normalMap >= 0)
		pExtensions->glUniform1iARB(normalMap,CTextureUnitSetup::IMAGE_UNIT_1);

	if (eyePos >= 0)
	{
		C3DEngineMatrix T;
		glGetTransposeFloatv(GL_MODELVIEW_MATRIX, T);
		GL_COORD_VERTEX V;
		V.x = -(T[0] * T[3] + T[4] * T[7] + T[8] * T[11]);
		V.y = -(T[1] * T[3] + T[5] * T[7] + T[9] * T[11]);
		V.z = -(T[2] * T[3] + T[6] * T[7] + T[10] * T[11]);
		V.h = 1.0f;

		pExtensions->glUniform4fvARB(eyePos, 1, V);
	}
#endif
}
