/***************************************************************************/
/*                                                                         */
/*  FlatShader.cpp                                                         */
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

#if !defined(AFX_FLATSHADER_H__E2077AC4_6F9F_45F3_85DD_E493AEF57959__INCLUDED_)
#include "FlatShader.h"
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


CFlatShader::CFlatShader(void)
	:CShader("FLAT_SHADER")
{
}

CFlatShader::CFlatShader(const CFlatShader& shader)
	: CShader(shader)
{
}

CShader* CFlatShader::glClone(const std::string& newShaderName) const
{
	CFlatShader* flat = new CFlatShader(*this);
	if (!newShaderName.empty())
		flat->setName(newShaderName);

	if (hasOpenGLShader())
		flat->glInit();

	return flat;
}

CFlatShader::~CFlatShader(void)
{
}

void CFlatShader::glInit()
{
	COpenGLShaderStage *stage = glGetOpenGLShader("FLAT_SHADER_PROGRAM");

	stage->glGetVertexShader("FLAT_VTX_SDHADER");
	stage->glGetFragmentShader("FLAT_TEX_SDHADER");

	CProgramParameters params;
	params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
	stage->setProgramParameters(params);

	stage->glCompileShader();

	CShaderBloc *bloc = glGetShaderBloc("LightProducts");
	if (NULL == bloc)
	{
		RAPTOR_ERROR(CShader::CShaderClassID::GetClassId(),
			"Flat Shader Object cannot find uniform bloc \"LightProducts\", compiled shader source is incorrect.");
	}
}

void CFlatShader::glRender(void)
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


