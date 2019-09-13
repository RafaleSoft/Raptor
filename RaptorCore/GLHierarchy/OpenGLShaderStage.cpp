/***************************************************************************/
/*                                                                         */
/*  OpenGLShaderStage.cpp                                                  */
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
#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_GEOMETRYSHADER_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
	#include "GLHierarchy/GeometryShader.h"
#endif
#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "Subsys/ShaderLibrary.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
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
	m_pVShader(NULL), m_pFShader(NULL), m_pGShader(NULL),
	m_bDeleteFShader(false), m_bDeleteVShader(false), m_bDeleteGShader(false)
{
	m_shaderProgram.handle(0);
	m_shaderProgram.hClass(COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId().ID());
}

COpenGLShaderStage::COpenGLShaderStage(const COpenGLShaderStage& stage)
	:CShaderProgram(stageId, stage.getName()),
	m_pVShader(NULL), m_pFShader(NULL), m_pGShader(NULL)
{
	if (NULL != stage.m_pVShader)
	{
		glRemoveVertexShader();
		if (stage.m_bDeleteVShader)
			m_pVShader = stage.m_pVShader->glClone();
		else
			m_pVShader = stage.m_pVShader;
		m_pVShader->registerDestruction(this);
	}
	if (NULL != stage.m_pFShader)
	{
		glRemoveFragmentShader();
		if (stage.m_bDeleteFShader)
			m_pFShader = stage.m_pFShader->glClone();
		else
			m_pFShader = stage.m_pFShader;
		m_pFShader->registerDestruction(this);
	}
	if (NULL != stage.m_pGShader)
	{
		glRemoveGeometryShader();
		if (stage.m_bDeleteGShader)
			m_pGShader = stage.m_pGShader->glClone();
		else
			m_pGShader = stage.m_pGShader;
		m_pGShader->registerDestruction(this);
	}

	if (0 != stage.m_shaderProgram.handle())
		glCompileShader();

	m_bDeleteVShader = stage.m_bDeleteVShader;
	m_bDeleteFShader = stage.m_bDeleteFShader;
	m_bDeleteGShader = stage.m_bDeleteGShader;
}

COpenGLShaderStage::~COpenGLShaderStage(void)
{
	if (m_shaderProgram.handle() != 0)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		GLint value = 0;
		pExtensions->glGetObjectParameterivARB(m_shaderProgram.handle(), GL_OBJECT_TYPE_ARB, &value);
		if (value != GL_PROGRAM_OBJECT_ARB)
		{
			Raptor::GetErrorManager()->generateRaptorError(COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   "Shader Program is invalid in this context");

			CATCH_GL_ERROR
				return;
		}
	}

	// TODO : delete program only if not shared !!!
#if defined(GL_ARB_shader_objects)
	if ((m_shaderProgram.handle() != 0) &&
		(m_bDeleteVShader || m_bDeleteFShader || m_bDeleteGShader))
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

		GLsizei maxCount = 0;
		pExtensions->glGetObjectParameterivARB(m_shaderProgram.handle(), GL_OBJECT_ATTACHED_OBJECTS_ARB, &maxCount);

		GLsizei count = 0;
		GLhandleARB *pHandles = new GLhandleARB[maxCount];
		pExtensions->glGetAttachedObjectsARB(m_shaderProgram.handle(), maxCount, &count, pHandles);

		for (GLsizei i = 0; ((i<count) && (i<maxCount)); i++)
			pExtensions->glDetachObjectARB(m_shaderProgram.handle(), pHandles[i]);

		delete[] pHandles;
	}
#endif

	glRemoveVertexShader();
	glRemoveFragmentShader();
	glRemoveGeometryShader();

	if (m_shaderProgram.handle() != 0)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glDeleteObjectARB(m_shaderProgram.handle());
	}
}

COpenGLShaderStage* COpenGLShaderStage::glClone() const
{
	return new COpenGLShaderStage(*this);
}


void COpenGLShaderStage::unLink(const CPersistence* p)
{
	if (p == static_cast<CPersistence*>(m_pVShader))
	{
		m_pVShader = NULL;
		if (m_shaderProgram.handle() == 0)
			m_bValid = false;
	}
	else if (p == static_cast<CPersistence*>(m_pFShader))
	{
		m_pFShader = NULL;
		if (m_shaderProgram.handle() == 0)
			m_bValid = false;
	}
	else if (p == static_cast<CPersistence*>(m_pGShader))
	{
		m_pGShader = NULL;
		if (m_shaderProgram.handle() == 0)
			m_bValid = false;
	}
}

bool COpenGLShaderStage::glGetProgramStatus(void) const
{
	bool valid = m_bValid;
	if (valid && (NULL != m_pVShader))
		valid = m_pVShader->glGetProgramStatus();
	if (valid && (NULL != m_pFShader))
		valid = m_pFShader->glGetProgramStatus();
	if (valid && (NULL != m_pGShader))
		valid = m_pGShader->glGetProgramStatus();

	return valid;
}

std::string COpenGLShaderStage::glGetProgramString(void) const
{
	return "";
}


void COpenGLShaderStage::setProgramParameters(const CProgramParameters &v)
{
	CShaderProgram::setProgramParameters(v);
}

void COpenGLShaderStage::glRender(void)
{
	if (m_shaderProgram.handle() != 0)
	{
#if defined(GL_ARB_shader_objects)
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glUseProgramObjectARB(m_shaderProgram.handle());

		if (m_pVShader != NULL)
			m_pVShader->glRender();

		if (m_pGShader != NULL)
			m_pGShader->glRender();

		if (m_pFShader != NULL)
			m_pFShader->glRender();
#endif
	}
}

void COpenGLShaderStage::glStop(void)
{
	if (m_shaderProgram.handle() != 0)
	{
#if defined(GL_ARB_shader_objects)
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glUseProgramObjectARB(0);
#endif
	}
}


CVertexShader* const COpenGLShaderStage::glGetVertexShader(const std::string& name)
{
	if (m_pVShader == NULL)
	{
		CShaderLibrary *lib = CShaderLibrary::GetInstance();
		lib->glInitFactory();

		CPersistence *pProgram = NULL;
		if (!name.empty())
			pProgram = CPersistence::FindObject(name);
		if (pProgram == NULL)
		{
			m_pVShader = new CVertexShader(name);
			m_bDeleteVShader = true;
		}
		else if (pProgram->getId().isSubClassOf(CVertexShader::CVertexShaderClassID::GetClassId()))
			m_pVShader = (CVertexShader*)pProgram;

		m_pVShader->registerDestruction(this);

		if (m_shaderProgram.handle() == 0)
			m_bValid = false;

		CATCH_GL_ERROR
	}

	return m_pVShader;
}

bool COpenGLShaderStage::glRemoveVertexShader(void)
{
	if (m_pVShader == NULL)
		return false;
	else
	{
		m_pVShader->unregisterDestruction(this);
		RAPTOR_HANDLE handle(0, m_shaderProgram.handle());
		m_pVShader->glUnbindProgram(handle);

		if (m_bDeleteVShader)
			delete m_pVShader;
		m_pVShader = NULL;
		m_bDeleteVShader = false;

		if (m_shaderProgram.handle() == 0)
			m_bValid = false;

		CATCH_GL_ERROR

		return true;
	}
}

CFragmentShader * const COpenGLShaderStage::glGetFragmentShader(const std::string& name)
{
	if (m_pFShader == NULL)
	{
		CShaderLibrary *lib = CShaderLibrary::GetInstance();
		lib->glInitFactory();

		CPersistence *pProgram = NULL;
		if (!name.empty())
			pProgram = CPersistence::FindObject(name);
		if (pProgram == NULL)
		{
			m_pFShader = new CFragmentShader(name);
			m_bDeleteFShader = true;
		}
		else if (pProgram->getId().isSubClassOf(CFragmentShader::CFragmentShaderClassID::GetClassId()))
			m_pFShader = (CFragmentShader*)pProgram;

		m_pFShader->registerDestruction(this);

		if (m_shaderProgram.handle() == 0)
			m_bValid = false;

		CATCH_GL_ERROR
	}

	return m_pFShader;
}

bool COpenGLShaderStage::glRemoveFragmentShader(void)
{
	if (m_pFShader == NULL)
		return false;
	else
	{
		m_pFShader->unregisterDestruction(this);
		RAPTOR_HANDLE handle(0, m_shaderProgram.handle());
		m_pFShader->glUnbindProgram(handle);

		if (m_bDeleteFShader)
			delete m_pFShader;
		m_pFShader = NULL;
		m_bDeleteFShader = false;

		if (m_shaderProgram.handle() == 0)
			m_bValid = false;

		CATCH_GL_ERROR

		return true;
	}
}


CGeometryShader* const COpenGLShaderStage::glGetGeometryShader(const std::string& name)
{
	if (m_pGShader == NULL)
	{
		CShaderLibrary *lib = CShaderLibrary::GetInstance();
		lib->glInitFactory();

		CPersistence *pProgram = NULL;
		if (!name.empty())
			pProgram = CPersistence::FindObject(name);
		if (pProgram == NULL)
		{
			m_pGShader = new CGeometryShader(name);
			m_bDeleteGShader = true;
		}
		else if (pProgram->getId().isSubClassOf(CGeometryShader::CGeometryShaderClassID::GetClassId()))
			m_pGShader = (CGeometryShader*)pProgram;

		m_pGShader->registerDestruction(this);

		if (m_shaderProgram.handle() == 0)
			m_bValid = false;

		CATCH_GL_ERROR
	}

	return m_pGShader;
}

bool COpenGLShaderStage::glRemoveGeometryShader(void)
{
	if (m_pGShader == NULL)
		return false;
	else
	{
		m_pGShader->unregisterDestruction(this);
		RAPTOR_HANDLE handle(0, m_shaderProgram.handle());
		m_pGShader->glUnbindProgram(handle);

		if (m_bDeleteGShader)
			delete m_pGShader;
		m_pGShader = NULL;
		m_bDeleteGShader = false;

		if (m_shaderProgram.handle() == 0)
			m_bValid = false;

		CATCH_GL_ERROR

		return true;
	}
}


bool COpenGLShaderStage::glCompileShader()
{
	m_bValid = false;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	// First try to generate programs.
	// This step is mandatory and must succeed if there are programs.
	if ((m_pFShader != NULL) || (m_pVShader != NULL) || (m_pGShader != NULL))
	{
#if defined(GL_ARB_shader_objects)
		bool abort = false;

		// create a program object
		if (m_shaderProgram.handle() == 0)
		{
			m_shaderProgram.handle(pExtensions->glCreateProgramObjectARB());
			if (m_shaderProgram.handle() == 0)
			{
				abort = true;
				Raptor::GetErrorManager()->generateRaptorError(COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId(),
															   CRaptorErrorManager::RAPTOR_ERROR,
															   CRaptorMessages::ID_CREATE_FAILED, CRaptorMessages::no_args);
			}

			GLint value = 0;
			pExtensions->glGetObjectParameterivARB(m_shaderProgram.handle(), GL_OBJECT_TYPE_ARB, &value);
			if (value != GL_PROGRAM_OBJECT_ARB)
			{
				CATCH_GL_ERROR
				return false;
			}
		}

		// Attach programs before linking.
		// Query attributes is done before linking (in glBindProgram)
		// to be able to bind attributes locations
		if ((!abort) && (m_pVShader != NULL))
		{
			if ((!m_pVShader->isValid()) || (!m_pVShader->glBindProgram(m_shaderProgram)))
				abort = true;
		}

		if ((!abort) && (m_pFShader != NULL))
		{
			if ((!m_pFShader->isValid()) || (!m_pFShader->glBindProgram(m_shaderProgram)))
				abort = true;
		}

		if ((!abort) && (m_pGShader != NULL))
		{
			if ((!m_pGShader->isValid()) || (!m_pGShader->glBindProgram(m_shaderProgram)))
				abort = true;
		}

		// link the program with bound shaders
		if (!abort)
		{
			pExtensions->glLinkProgramARB(m_shaderProgram.handle());
			GLint linkStatus = GL_FALSE;

			pExtensions->glGetObjectParameterivARB(m_shaderProgram.handle(), GL_OBJECT_LINK_STATUS_ARB, &linkStatus);
			if (linkStatus == GL_FALSE)
			{
				CATCH_GL_ERROR
				abort = true;
			}
		}

#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (!abort)
		{
			pExtensions->glValidateProgramARB(m_shaderProgram.handle());
			GLint validateStatus = GL_FALSE;
			pExtensions->glGetObjectParameterivARB(m_shaderProgram.handle(), GL_OBJECT_VALIDATE_STATUS_ARB, &validateStatus);
			if (validateStatus == GL_FALSE)
				abort = true;
		}
#endif

		if ((abort) && (m_shaderProgram.handle() != 0))
		{
			GLint maxLength = 255;
			GLint length = 0;
			CRaptorMessages::MessageArgument arg;
			char *pInfoLog = NULL;
			pExtensions->glGetObjectParameterivARB(m_shaderProgram.handle(), GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
			if (maxLength > 0)
			{
				char *pInfoLog = (char*)malloc(maxLength * sizeof(char));
				pExtensions->glGetInfoLogARB(m_shaderProgram.handle(), maxLength, &length, pInfoLog);
				arg.arg_sz = pInfoLog;
			}
			else
				arg.arg_sz = "Unknown Error";

			vector<CRaptorMessages::MessageArgument> args;
			args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_ERROR,
														   CRaptorMessages::ID_PROGRAM_ERROR,
														   args);
			if (maxLength > 0)
				free(pInfoLog);
		}

		CATCH_GL_ERROR

		m_bValid = true;
		return !abort;
#endif
	}

	return false;
}

bool COpenGLShaderStage::exportObject(CRaptorIO& o)
{
	CPersistence::exportObject(o);
	return true;
}

bool COpenGLShaderStage::importObject(CRaptorIO& io)
{
	string name;
	io >> name;

	string data = io.getValueName();
	while (!data.empty())
	{
		if (data == "name")
			CPersistence::importObject(io);
		else if (data == "VertexShader")
		{
			glGetVertexShader();
			m_pVShader->importObject(io);
		}
		else if (data == "GeometryShader")
		{
			glGetGeometryShader();
			m_pGShader->importObject(io);
		}
		else if (data == "FragmentShader")
		{
			glGetFragmentShader();
			m_pFShader->importObject(io);
		}
		else
			io >> name;

		data = io.getValueName();
	}
	io >> name;

	return true;
}

