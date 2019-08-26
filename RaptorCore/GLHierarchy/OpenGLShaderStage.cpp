/***************************************************************************/
/*                                                                         */
/*  OpenGLShaderStage.cpp                                                 */
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

#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "OpenGLShaderStage.h"
#endif



//////////////////////////////////////////////////////////////////////
// Static data
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE_BEGIN

static COpenGLShaderStage::COpenGLShaderStageClassID stageId;
const CPersistence::CPersistenceClassID& COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId(void)
{
	return stageId;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

COpenGLShaderStage::COpenGLShaderStage(const std::string& name)
	:CShaderProgram(stageId, name),
	m_pVShader(NULL), m_pFShader(NULL), m_pGShader(NULL)
{
}

COpenGLShaderStage::~COpenGLShaderStage(void)
{
}

bool COpenGLShaderStage::exportObject(CRaptorIO& o)
{
	return true;
}

bool COpenGLShaderStage::importObject(CRaptorIO& i)
{
	return true;
}

void COpenGLShaderStage::setProgramParameters(const CProgramParameters &v)
{
	CShaderProgram::setProgramParameters(v);
}

