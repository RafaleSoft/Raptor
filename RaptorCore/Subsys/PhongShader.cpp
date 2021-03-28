/***************************************************************************/
/*                                                                         */
/*  PhongShader.cpp                                                        */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
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

#if !defined(AFX_PHONGSHADER_H__C6904117_2A9C_42A8_A20B_A71A22A9C00B__INCLUDED_)
	#include "PhongShader.h"
#endif
#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "LightAttributes.h"
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
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_SHADERBLOC_H__56C73DCA_292E_4722_8881_82DC1BF53EA5__INCLUDED_)
	#include "GLHierarchy/ShaderBloc.h"
#endif



RAPTOR_NAMESPACE


CPhongShader::CPhongShader(void)
	:CShader("PHONG_SHADER")
{
}

CPhongShader::CPhongShader(const CPhongShader& shader)
	: CShader(shader)
{
}

CPhongShader::~CPhongShader(void)
{
}

CShader* CPhongShader::glClone(const std::string& newShaderName) const
{
	CPhongShader* phong = new CPhongShader(*this);
	if (!newShaderName.empty())
		phong->setName(newShaderName);

	if (hasOpenGLShader())
		phong->glInit();

	return phong;
}

void CPhongShader::glInit(void)
{
	COpenGLShaderStage *stage = glGetOpenGLShader("PHONG_SHADER_PROGRAM");

	stage->glGetVertexShader("PPIXEL_PHONG_VTX_PROGRAM");
	stage->glGetFragmentShader("PPIXEL_PHONG_TEX_PROGRAM");

	CProgramParameters params;
	params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
	stage->setProgramParameters(params);

	stage->glCompileShader();

	CShaderBloc *bloc = glGetShaderBloc("LightProducts");
	if (NULL == bloc)
	{
		RAPTOR_ERROR(CShader::CShaderClassID::GetClassId(),
			"Phong Shader Object cannot find uniform bloc \"LightProducts\", compiled shader source is incorrect.");
	}
}


void CPhongShader::glRender(void)
{
	CShaderBloc *pBloc = glGetShaderBloc();
	if (!pBloc->isExternal())
	{
		CLight::R_LightProducts products;
		CMaterial *M = getMaterial();
		CLight **olights = CLightAttributes::getOrderedLights();

		for (int i = 0, numl = 0; (i < CLightAttributes::MAX_LIGHTS) && (numl < 5); i++)
		{
			products.lights[i].enable = shader_false;
			CLight *pLight = olights[i];

			if (NULL != pLight)
			{
				CLight::R_LightProduct& lp = products.lights[numl++];
				lp = pLight->computeLightProduct(*M);
			}
		}
		products.scene_ambient = CShader::getAmbient();
		products.shininess = M->getShininess();

		pBloc->glvkUpdateBloc((uint8_t*)&products);
	}

	CShader::glRender();

	CATCH_GL_ERROR
}

