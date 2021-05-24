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
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
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
	if (hasShaderBloc() && hasMaterial())
	{
		CShaderBloc *pBloc = glGetShaderBloc();
		if (!pBloc->isExternal())
		{
			CMaterial *M = getMaterial();
			glRenderShaderBloc(*pBloc, *M);
		}
	}

	CShader::glRender();

	CATCH_GL_ERROR
}

