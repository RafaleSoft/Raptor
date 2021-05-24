/***************************************************************************/
/*                                                                         */
/*  BumpShader.cpp                                                         */
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

#if !defined(AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_)
	#include "BumpShader.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
//#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
//	#include "Engine/3DEngineMatrix.h"
//#endif
//#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
//	#include "GLHierarchy/ITextureObject.h"
//#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_SHADERBLOC_H__56C73DCA_292E_4722_8881_82DC1BF53EA5__INCLUDED_)
	#include "GLHierarchy/ShaderBloc.h"
#endif


RAPTOR_NAMESPACE



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
	params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
	params.addParameter("normalMap", CTextureUnitSetup::IMAGE_UNIT_1);

	//!
	//! For more efficient computation with multiple lights,
	//!	bump shader is computed in eye space, then no need to pass the eyePos here (in object space).
	//!
	//GL_COORD_VERTEX V;
	//params.addParameter("eyePos", V);

	stage->setProgramParameters(params);
	stage->glCompileShader();

	CShaderBloc *bloc = glGetShaderBloc("LightProducts");
	if (NULL == bloc)
	{
		RAPTOR_ERROR(CShader::CShaderClassID::GetClassId(),
					"Bump Shader Object cannot find uniform bloc \"LightProducts\", compiled shader source is incorrect.");
	}
}

void CBumpShader::glRender(void)
{
	//!
	//! For more efficient computation with multiple lights,
	//!	bump shader is computed in eye space, then no need to pass the eyePos here (in object space).
	//!
	/*
	C3DEngineMatrix T;
	glGetTransposeFloatv(GL_MODELVIEW_MATRIX, T);
	GL_COORD_VERTEX V;
	V.x = -(T[0] * T[3] + T[4] * T[7] + T[8] * T[11]);
	V.y = -(T[1] * T[3] + T[5] * T[7] + T[9] * T[11]);
	V.z = -(T[2] * T[3] + T[6] * T[7] + T[10] * T[11]);
	V.h = 1.0f;

	COpenGLShaderStage *stage = glGetOpenGLShader();
	CProgramParameters params;
	params.addParameter("eyePos", V);
	stage->updateProgramParameters(params);
	*/
	
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
