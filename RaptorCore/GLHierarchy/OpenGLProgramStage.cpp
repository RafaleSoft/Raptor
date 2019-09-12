/***************************************************************************/
/*                                                                         */
/*  OpenGLProgramStage.cpp                                                 */
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

#if !defined(AFX_OPENGLPROGRAMSTAGE_H__0BCE3B42_6E10_4F50_BB27_1993345ADBCF__INCLUDED_)
	#include "OpenGLProgramStage.h"
#endif



//////////////////////////////////////////////////////////////////////
// Static data
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE_BEGIN

static COpenGLProgramStage::COpenGLProgramStageClassID stageId;
const CPersistence::CPersistenceClassID& COpenGLProgramStage::COpenGLProgramStageClassID::GetClassId(void)
{
	return stageId;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

COpenGLProgramStage::COpenGLProgramStage(const std::string& name)
	:CShaderProgram(stageId, name),
	m_pVShader(NULL), m_pFShader(NULL)
{
}

COpenGLProgramStage::~COpenGLProgramStage(void)
{
}

bool COpenGLProgramStage::exportObject(CRaptorIO& o)
{
	return true;
}

bool COpenGLProgramStage::importObject(CRaptorIO& i)
{
	return true;
}

void COpenGLProgramStage::setProgramParameters(const CProgramParameters &v)
{
	CShaderProgram::setProgramParameters(v);
}

