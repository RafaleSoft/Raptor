/***************************************************************************/
/*                                                                         */
/*  ShaderProgram.cpp                                                      */
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


#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "ShaderProgram.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE_BEGIN

static CShaderProgram::CShaderProgramClassID shaderId;
const CPersistence::CPersistenceClassID& CShaderProgram::CShaderProgramClassID::GetClassId(void)
{
	return shaderId;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShaderProgram::CShaderProgram(const CPersistence::CPersistenceClassID& id,const std::string& name):
	CPersistence(id,name),
	m_bValid(false),
	m_handle(),
	m_bApplyParameters(false),
	m_parameters()
{
}

CShaderProgram::CShaderProgram(const CShaderProgram& shader)
    :CPersistence(shaderId,shader.getName())
{
	m_bValid = shader.m_bValid;
	m_handle = shader.m_handle;
	m_bApplyParameters = shader.m_bApplyParameters;
	m_parameters = shader.m_parameters;
}

CShaderProgram::~CShaderProgram()
{
	// TODO : Recycle handle
}


void CShaderProgram::setProgramParameters(const CProgramParameters &v)
{
    m_parameters = v;
    m_bApplyParameters = true;
}


void CShaderProgram::updateProgramParameters(const CProgramParameters &v)
{
	bool apply = m_parameters.updateParameters(v);
	m_bApplyParameters = apply;
}

bool CShaderProgram::glLoadProgramFromFile(const std::string &program)
{
	CRaptorIO *shdr = CRaptorIO::Create(program, CRaptorIO::DISK_READ);
	if (NULL == shdr)
		return false;

	if (shdr->getStatus() == CRaptorIO::IO_OK)
	{
		string programstr;
		while (shdr->getStatus() == CRaptorIO::IO_OK)
		{
			string line;
			*shdr >> line;
			if (shdr->getStatus() == CRaptorIO::IO_OK)
				programstr = programstr + line + "\n";
		}

		return glLoadProgram(programstr);
	}
	else
		return false;
}
