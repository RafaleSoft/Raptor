/***************************************************************************/
/*                                                                         */
/*  AOComputeShader.cpp                                                    */
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

#if !defined(AFX_AOCOMPUTESHADER_H__7CD66380_1000_47A3_AA98_47E0EDBD728E__INCLUDED_)
	#include "AOComputeShader.h"
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


CAOComputeShader::CAOComputeShader(void)
	:CShader("AOCOMPUTE_SHADER")
{
}

CAOComputeShader::~CAOComputeShader(void)
{
}

void CAOComputeShader::glInit(void)
{
	COpenGLShaderStage *stage = glGetOpenGLShader("AOCOMPUTE_SHADER_PROGRAM");

	CVertexShader *vs = stage->glGetVertexShader("AMBIENT_OCCLUSION_VTX_PROGRAM");
	CFragmentShader *fs = stage->glGetFragmentShader("AMBIENT_OCCLUSION_TEX_PROGRAM");

	CProgramParameters ao_params;
	GL_MATRIX Id;
	IDENT_MATRIX(Id);
	ao_params.addParameter("vertexMat",Id);
	ao_params.addParameter("normalMat",Id);
	ao_params.addParameter("posMap",CTextureUnitSetup::IMAGE_UNIT_0);
	ao_params.addParameter("normalMap",CTextureUnitSetup::IMAGE_UNIT_1);
	ao_params.addParameter("numRows",GL_COORD_VERTEX());
	stage->setProgramParameters(ao_params);

	stage->glCompileShader();
}

void CAOComputeShader::glStop(void)
{
	CShader::glStop();
}


void CAOComputeShader::glRender(void)
{
	CShader::glRender();

#if defined(GL_ARB_shader_objects)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
#endif
}
