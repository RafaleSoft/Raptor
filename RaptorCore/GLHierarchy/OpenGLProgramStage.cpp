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
#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "Subsys/ShaderLibrary.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_OLD_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "VertexProgram_old.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_OLD_H__DD0AD51D_3BFF_4C65_8099_BA7696D7BDDF__INCLUDED_)
	#include "FragmentProgram_old.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
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
	m_pVProgram(NULL), m_pFProgram(NULL),
	m_bDeleteVProgram(false), m_bDeleteFProgram(false)
{
}

COpenGLProgramStage::COpenGLProgramStage(const COpenGLProgramStage& stage)
	:CShaderProgram(stageId, stage.getName()),
	m_pVProgram(NULL), m_pFProgram(NULL)
{
	if (NULL != stage.m_pVProgram)
	{
		glRemoveVertexProgram();
		if (stage.m_bDeleteVProgram)
			m_pVProgram = stage.m_pVProgram->glClone();
		else
			m_pVProgram = stage.m_pVProgram;
		m_pVProgram->registerDestruction(this);
	}
	if (NULL != stage.m_pFProgram)
	{
		glRemoveFragmentProgram();
		if (stage.m_bDeleteFProgram)
			m_pFProgram = stage.m_pFProgram->glClone();
		else
			m_pFProgram = stage.m_pFProgram;
		m_pFProgram->registerDestruction(this);
	}

	m_bDeleteVProgram = stage.m_bDeleteVProgram;
	m_bDeleteFProgram = stage.m_bDeleteFProgram;
}

COpenGLProgramStage::~COpenGLProgramStage(void)
{
	glRemoveVertexProgram();
	glRemoveFragmentProgram();
}

COpenGLProgramStage* COpenGLProgramStage::glClone() const
{
	return new COpenGLProgramStage(*this);
}

void COpenGLProgramStage::unLink(const CPersistence* p)
{
	if (p == static_cast<CPersistence*>(m_pVProgram))
		m_pVProgram = NULL;
	else if (p == static_cast<CPersistence*>(m_pFProgram))
		m_pFProgram = NULL;
}

bool COpenGLProgramStage::glGetProgramStatus(void) const
{
	return m_bValid;
}

std::string COpenGLProgramStage::glGetProgramString(void) const
{
	return "";
}

void COpenGLProgramStage::glRender(void)
{
	if (NULL != m_pVProgram)
		m_pVProgram->glRender();
	if (NULL != m_pFProgram)
		m_pFProgram->glRender();
}

void COpenGLProgramStage::glStop(void)
{
	if (NULL != m_pVProgram)
		m_pVProgram->glStop();
	if (NULL != m_pFProgram)
		m_pFProgram->glStop();
}

void COpenGLProgramStage::setProgramParameters(const CProgramParameters &v)
{
	CShaderProgram::setProgramParameters(v);
}


CVertexProgram_old * const COpenGLProgramStage::glGetVertexProgram(const std::string& name)
{
	if (m_pVProgram == NULL)
	{
		CShaderLibrary *lib = CShaderLibrary::GetInstance();
		lib->glInitFactory();

		CPersistence *pShader = NULL;
		if (!name.empty())
			pShader = CPersistence::FindObject(name);
		if (pShader == NULL)
		{
			m_pVProgram = new CVertexProgram_old(name);
			m_bDeleteVProgram = true;
		}
		else if (pShader->getId().isSubClassOf(CVertexProgram_old::CVertexProgram_oldClassID::GetClassId()))
			m_pVProgram = (CVertexProgram_old*)pShader;

		m_pVProgram->registerDestruction(this);

		CATCH_GL_ERROR
	}

	return m_pVProgram;
}

bool COpenGLProgramStage::glRemoveVertexProgram(void)
{
	if (m_pVProgram == NULL)
		return false;
	else
	{
		m_pVProgram->unregisterDestruction(this);

		if (m_bDeleteVProgram)
			delete m_pVProgram;
		m_pVProgram = NULL;
		m_bDeleteVProgram = false;

		CATCH_GL_ERROR

		return true;
	}
}

CFragmentProgram_old * const COpenGLProgramStage::glGetFragmentProgram(const std::string& name)
{
	if (m_pFProgram == NULL)
	{
		CShaderLibrary *lib = CShaderLibrary::GetInstance();
		lib->glInitFactory();

		CPersistence *pShader = NULL;
		if (!name.empty())
			pShader = CPersistence::FindObject(name);
		if (pShader == NULL)
		{
			m_pFProgram = new CFragmentProgram_old(name);
			m_bDeleteFProgram = true;
		}
		else if (pShader->getId().isSubClassOf(CFragmentProgram_old::CFragmentProgram_oldClassID::GetClassId()))
			m_pFProgram = (CFragmentProgram_old*)pShader;

		m_pFProgram->registerDestruction(this);

		CATCH_GL_ERROR
	}

	return m_pFProgram;
}

bool COpenGLProgramStage::glRemoveFragmentProgram(void)
{
	if (m_pFProgram == NULL)
		return false;
	else
	{
		m_pFProgram->unregisterDestruction(this);

		if (m_bDeleteFProgram)
			delete m_pFProgram;
		m_pFProgram = NULL;
		m_bDeleteFProgram = false;

		CATCH_GL_ERROR

			return true;
	}
}


bool COpenGLProgramStage::exportObject(CRaptorIO& o)
{
	CPersistence::exportObject(o);
	return true;
}

bool COpenGLProgramStage::importObject(CRaptorIO& io)
{
	string name;
	io >> name;

	string data = io.getValueName();
	while (!data.empty())
	{
		if (data == "name")
			CPersistence::importObject(io);
		else if (data == "VertexProgram")
		{
			glGetVertexProgram();
			m_pVProgram->importObject(io);
		}
		else if (data == "FragmentProgram")
		{
			glGetFragmentProgram();
			m_pFProgram->importObject(io);
		}
		else
			io >> name;

		data = io.getValueName();
	}
	io >> name;

	return true;
}

