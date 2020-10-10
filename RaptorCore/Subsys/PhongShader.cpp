/***************************************************************************/
/*                                                                         */
/*  PhongShader.cpp                                                        */
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
	return new CPhongShader(*this);
}

void CPhongShader::glInit(void)
{
	COpenGLShaderStage *stage = glGetOpenGLShader("PHONG_SHADER_PROGRAM");

	stage->glGetVertexShader("PPIXEL_PHONG_VTX_PROGRAM");
	stage->glGetFragmentShader("PPIXEL_PHONG_TEX_PROGRAM");

	CProgramParameters params;
	params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);

	CLightAttributes::light_order L;
	CProgramParameters::CParameterArray<int, CLightAttributes::MAX_LIGHTS> lights("lightEnable", L);
	params.addParameter(lights);

	stage->setProgramParameters(params);

	stage->glCompileShader();
}

void CPhongShader::glRender(void)
{
	COpenGLShaderStage *stage = glGetOpenGLShader();
	CProgramParameters params;

	CLightAttributes::light_order const &L = CLightAttributes::getLightOrder();
	CProgramParameters::CParameterArray<int, CLightAttributes::MAX_LIGHTS> lights("lightEnable", L);
	params.addParameter(lights);

	stage->updateProgramParameters(params);

	CShader::glRender();
}

