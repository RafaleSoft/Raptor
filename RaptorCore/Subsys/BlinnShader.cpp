/***************************************************************************/
/*                                                                         */
/*  BlinnShader.cpp                                                        */
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

#if !defined(AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_)
	#include "BlinnShader.h"
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


CBlinnShader::CBlinnShader(void)
	:CShader("BLINN_SHADER")
{
}

CBlinnShader::CBlinnShader(const CBlinnShader& shader)
	: CShader(shader)
{
}

CShader* CBlinnShader::glClone(const std::string& newShaderName) const
{
	CBlinnShader* blinn = new CBlinnShader(*this);
	if (!newShaderName.empty())
		blinn->setName(newShaderName);

	if (hasOpenGLShader())
		blinn->glInit();

	return blinn;
}

CBlinnShader::~CBlinnShader(void)
{
}

void CBlinnShader::glInit()
{
	COpenGLShaderStage *stage = glGetOpenGLShader("BLINN_SHADER_PROGRAM");

	stage->glGetVertexShader("PPIXEL_BLINN_VTX_PROGRAM");
	stage->glGetFragmentShader("PPIXEL_BLINN_TEX_PROGRAM");

	CProgramParameters params;
	params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
	stage->setProgramParameters(params);

	stage->glCompileShader();

	CShaderBloc *bloc = glGetShaderBloc("LightProducts");
	if (NULL == bloc)
	{
		RAPTOR_ERROR(CShader::CShaderClassID::GetClassId(),
					"Blinn Shader Object cannot find uniform bloc \"LightProducts\", compiled shader source is incorrect.");
	}
}

void CBlinnShader::glRender(void)
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


