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
#if !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)
	#include "Subsys/UniformAllocator.h"
#endif
#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "Subsys/LightAttributes.h"
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
	:CShaderProgram(stageId, name), m_bReLinked(false),
	m_pVShader(NULL), m_pFShader(NULL), m_pGShader(NULL), m_bUpdateLocations(false),
	m_bDeleteFShader(false), m_bDeleteVShader(false), m_bDeleteGShader(false)
{
	m_handle.hClass(COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId().ID());
}

COpenGLShaderStage::COpenGLShaderStage(const COpenGLShaderStage& stage)
	:CShaderProgram(stageId, stage.getName()), m_bReLinked(stage.m_bReLinked),
	m_pVShader(NULL), m_pFShader(NULL), m_pGShader(NULL), m_bUpdateLocations(stage.m_bUpdateLocations)
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

	//!	Do not compile the shader at this stage because 
	//!	derived classes will likely modify the shaders after this point.
	//if (0 != stage.m_handle.glhandle())
	//	glCompileShader();

	m_bDeleteVShader = stage.m_bDeleteVShader;
	m_bDeleteFShader = stage.m_bDeleteFShader;
	m_bDeleteGShader = stage.m_bDeleteGShader;
}

COpenGLShaderStage::~COpenGLShaderStage(void)
{
	if (m_handle.glhandle() != 0)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_VERSION_2_0)
		if (!pExtensions->glIsProgram(m_handle.glhandle()))
#elif defined(GL_ARB_shader_objects)
		GLint value = 0;
		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_TYPE_ARB, &value);
		if (value != GL_PROGRAM_OBJECT_ARB)
#endif
		{
			RAPTOR_WARNING(	COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId(),
							"Shader Program is invalid in this context");

			CATCH_GL_ERROR
			return;
		}
	}

	// TODO : delete program only if not shared !!!

	if ((m_handle.glhandle() != 0) &&
		(m_bDeleteVShader || m_bDeleteFShader || m_bDeleteGShader))
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		GLsizei maxCount = 0;
		GLsizei count = 0;

#if defined(GL_VERSION_2_0)
		pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ATTACHED_SHADERS, &maxCount);
		GLuint *pHandles = new GLuint[maxCount];
		pExtensions->glGetAttachedShaders(m_handle.glhandle(), maxCount, &count, pHandles);
		for (GLsizei i = 0; ((i<count) && (i<maxCount)); i++)
			pExtensions->glDetachShader(m_handle.glhandle(), pHandles[i]);
#elif defined(GL_ARB_shader_objects)
		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_ATTACHED_OBJECTS_ARB, &maxCount);
		GLhandleARB *pHandles = new GLhandleARB[maxCount];
		pExtensions->glGetAttachedObjectsARB(m_handle.glhandle(), maxCount, &count, pHandles);
		for (GLsizei i = 0; ((i<count) && (i<maxCount)); i++)
			pExtensions->glDetachObjectARB(m_handle.glhandle(), pHandles[i]);
#endif

		delete[] pHandles;
	}

	glRemoveVertexShader();
	glRemoveFragmentShader();
	glRemoveGeometryShader();

	if (m_handle.glhandle() != 0)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_VERSION_2_0)
		pExtensions->glDeleteProgram(m_handle.glhandle());
#elif defined(GL_ARB_shader_objects)
		pExtensions->glDeleteObjectARB(m_handle.glhandle());
#endif
	}
}

COpenGLShaderStage* COpenGLShaderStage::glClone() const
{
	return new COpenGLShaderStage(*this);
}

bool COpenGLShaderStage::glLoadProgram(const std::string &program)
{
	RAPTOR_ERROR(	stageId,
					"OpenGLShaderStage cannot load programs directly. Use Vertex, Fragment or Geometry shaders instead.");

	return NULL;
}

void COpenGLShaderStage::unLink(const CPersistence* p)
{
	if (p == static_cast<CPersistence*>(m_pVShader))
	{
		m_bReLinked = false;
		m_pVShader = NULL;
		if (m_handle.glhandle() == 0)
			m_bValid = false;
	}
	else if (p == static_cast<CPersistence*>(m_pFShader))
	{
		m_bReLinked = false;
		m_pFShader = NULL;
		if (m_handle.glhandle() == 0)
			m_bValid = false;
	}
	else if (p == static_cast<CPersistence*>(m_pGShader))
	{
		m_bReLinked = false;
		m_pGShader = NULL;
		if (m_handle.glhandle() == 0)
			m_bValid = false;
	}
}

bool COpenGLShaderStage::glGetProgramStatus(void) const
{
	CVertexShader::GL_VERTEX_SHADER_CAPS vcaps;
	CGeometryShader::GL_GEOMETRY_SHADER_CAPS gcaps;
	CFragmentShader::GL_FRAGMENT_SHADER_CAPS fcaps;
	bool valid = m_bValid;

	if (valid && (NULL != m_pVShader))
	{
		valid = m_pVShader->glGetProgramStatus();
		m_pVShader->glGetShaderCaps(vcaps);
	}
	if (valid && (NULL != m_pFShader))
	{
		valid = m_pFShader->glGetProgramStatus();
		m_pFShader->glGetShaderCaps(fcaps);
	}
	if (valid && (NULL != m_pGShader))
	{
		valid = m_pGShader->glGetProgramStatus();
		m_pGShader->glGetShaderCaps(gcaps);
	}
	
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
#if defined(GL_VERSION_2_0)
	GLint nbShaders = 0;
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ATTACHED_SHADERS, &nbShaders);
	GLint nbAttributes = 0;
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_ATTRIBUTES, &nbAttributes);
	GLint nbUniforms = 0;
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_UNIFORMS, &nbUniforms);
	GLint nbUniformsBlocks = 0;
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_UNIFORM_BLOCKS, &nbUniformsBlocks);
#endif

	return valid;
}

std::string COpenGLShaderStage::glGetProgramString(void) const
{
	return "";
}


void COpenGLShaderStage::setProgramParameters(const CProgramParameters &v)
{
	CShaderProgram::setProgramParameters(v);

	size_t nbParams = m_parameters.getNbParameters();
	for (size_t i = 0; i<nbParams; i++)
	{
		CProgramParameters::CParameterBase& value = m_parameters[i];
		if (value.locationIndex < 0)
			m_bUpdateLocations = true;
	}
}


void COpenGLShaderStage::glRender(void)
{
	if (m_handle.glhandle() != 0)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
#if defined(GL_VERSION_2_0)
		pExtensions->glUseProgram(m_handle.glhandle());
#elif defined(GL_ARB_shader_objects)
		pExtensions->glUseProgramObjectARB(m_handle.glhandle());
#endif

		if (m_bUpdateLocations || m_bReLinked)
		{
			glGetBufferMemoryRequirements();
			glQueryUniformLocations();
			glQueryAttributeLocations();
			m_bUpdateLocations = false;
			m_bReLinked = false;
		}

		if (m_bApplyParameters)
		{
			glSetProgramParameters();
			m_bApplyParameters = false;
		}
	
		if (m_pVShader != NULL)
			m_pVShader->glRender();

		if (m_pGShader != NULL)
			m_pGShader->glRender();

		if (m_pFShader != NULL)
			m_pFShader->glRender();
	}
}

void COpenGLShaderStage::glStop(void)
{
	if (m_handle.glhandle() != 0)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
#if defined(GL_VERSION_2_0)
		pExtensions->glUseProgram(0);
#elif defined(GL_ARB_shader_objects)
		pExtensions->glUseProgramObjectARB(0);
#endif
	}
}


CVertexShader* const COpenGLShaderStage::glGetVertexShader(const std::string& name)
{
	if (m_pVShader == NULL)
	{
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
		m_bReLinked = false;

		if (m_handle.glhandle() == 0)
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
		m_pVShader->glUnbindProgram(m_handle);

		if (m_bDeleteVShader)
			delete m_pVShader;
		m_pVShader = NULL;
		m_bDeleteVShader = false;
		m_bReLinked = false;

		if (m_handle.glhandle() == 0)
			m_bValid = false;

		CATCH_GL_ERROR

		return true;
	}
}

CFragmentShader * const COpenGLShaderStage::glGetFragmentShader(const std::string& name)
{
	if (m_pFShader == NULL)
	{
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
		m_bReLinked = false;
		
		if (m_handle.glhandle() == 0)
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
		m_pFShader->glUnbindProgram(m_handle);

		if (m_bDeleteFShader)
			delete m_pFShader;
		m_pFShader = NULL;
		m_bDeleteFShader = false;
		m_bReLinked = false;
		
		if (m_handle.glhandle() == 0)
			m_bValid = false;

		CATCH_GL_ERROR

		return true;
	}
}


CGeometryShader* const COpenGLShaderStage::glGetGeometryShader(const std::string& name)
{
	if (m_pGShader == NULL)
	{
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
		m_bReLinked = false;

		if (m_handle.glhandle() == 0)
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
		m_pGShader->glUnbindProgram(m_handle);

		if (m_bDeleteGShader)
			delete m_pGShader;
		m_pGShader = NULL;
		m_bDeleteGShader = false;
		m_bReLinked = false;
		
		if (m_handle.glhandle() == 0)
			m_bValid = false;

		CATCH_GL_ERROR

		return true;
	}
}


bool COpenGLShaderStage::glCompileShader()
{
	m_bValid = false;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_VERSION_2_0) || defined(GL_ARB_shader_objects)
	// First try to generate programs.
	// This step is mandatory and must succeed if there are programs.
	if ((m_pFShader != NULL) || (m_pVShader != NULL) || (m_pGShader != NULL))
	{
		bool abort = false;

		// create a program object
		if (m_handle.glhandle() == 0)
		{
#if defined(GL_VERSION_2_0)
			m_handle.handle(pExtensions->glCreateProgram());
#elif defined(GL_ARB_shader_objects)
			m_handle.handle(pExtensions->glCreateProgramObjectARB());
#endif
			if (m_handle.glhandle() == 0)
			{
				abort = true;
				RAPTOR_ERROR(COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId(),
							 CRaptorMessages::ID_CREATE_FAILED)
			}

#if defined(GL_VERSION_2_0)
			if (!pExtensions->glIsProgram(m_handle.glhandle()))
#elif defined(GL_ARB_shader_objects)
			GLint value = 0;
			pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_TYPE_ARB, &value);
			if (value != GL_PROGRAM_OBJECT_ARB)
#endif
			{
				CATCH_GL_ERROR
				return false;
			}
			else
			{
				std::stringstream msg;
				msg << "Creation of shader program [";
				msg << m_handle.handle();
				msg << "] is: ";
				msg << getName();
				msg << std::ends;
				RAPTOR_NO_ERROR(COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId(),
								msg.str());
			}
		}

		// Attach programs before linking.
		// Query attributes is done before linking (in glBindProgram)
		// to be able to bind attributes locations
		if ((!abort) && (m_pVShader != NULL))
		{
			if ((!m_pVShader->isValid()) || (!m_pVShader->glBindProgram(m_handle)))
				abort = true;
		}

		if ((!abort) && (m_pFShader != NULL))
		{
			if ((!m_pFShader->isValid()) || (!m_pFShader->glBindProgram(m_handle)))
				abort = true;
		}

		if ((!abort) && (m_pGShader != NULL))
		{
			if ((!m_pGShader->isValid()) || (!m_pGShader->glBindProgram(m_handle)))
				abort = true;
		}

		//	Bind attribute variables
		if (!abort)
		{
			for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
			{
				const CProgramParameters::CParameterBase& value = m_parameters[idx];
				CProgramParameters::GL_VERTEX_ATTRIB p;
				if (value.isA(p))
				{
					p = ((const CProgramParameters::CParameter<CProgramParameters::GL_VERTEX_ATTRIB>&)value).p;
#if defined(GL_VERSION_2_0)
					pExtensions->glBindAttribLocation(m_handle.glhandle(), p, value.name().data());
#elif defined(GL_ARB_shader_objects)
					pExtensions->glBindAttribLocationARB(m_handle.glhandle(), p, value.name().data());
#endif
				}
			}
		}

		// link the program with bound shaders
		if (!abort)
		{
			
			GLint linkStatus = GL_FALSE;
#if defined(GL_VERSION_2_0)
			pExtensions->glLinkProgram(m_handle.glhandle());
			pExtensions->glGetProgramiv(m_handle.glhandle(), GL_LINK_STATUS, &linkStatus);
#elif defined(GL_ARB_shader_objects)
			pExtensions->glLinkProgramARB(m_handle.glhandle());
			pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_LINK_STATUS_ARB, &linkStatus);
#endif
			if (linkStatus == GL_FALSE)
			{
				CATCH_GL_ERROR
				abort = true;
			}
		}

		if (!abort)
		{
			GLint validateStatus = GL_FALSE;
#if defined(GL_VERSION_2_0)
			pExtensions->glValidateProgram(m_handle.handle());
			pExtensions->glGetProgramiv(m_handle.handle(), GL_VALIDATE_STATUS, &validateStatus);
#elif defined(GL_ARB_shader_objects)
			pExtensions->glValidateProgramARB(m_handle.handle());
			pExtensions->glGetObjectParameterivARB(m_handle.handle(), GL_OBJECT_VALIDATE_STATUS_ARB, &validateStatus);
#endif
			if (validateStatus == GL_FALSE)
				abort = true;
		}

		if ((abort) && (m_handle.glhandle() != 0))
		{
			GLint maxLength = 255;
			GLint length = 0;
			CRaptorMessages::MessageArgument arg;
			char *pInfoLog = NULL;

#if defined(GL_VERSION_2_0)
			pExtensions->glGetProgramiv(m_handle.handle(), GL_INFO_LOG_LENGTH, &maxLength);
#elif defined(GL_ARB_shader_objects)
			pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
#endif
			if (maxLength > 0)
			{
				pInfoLog = (char*)malloc(maxLength * sizeof(char));
#if defined(GL_VERSION_2_0)
				pExtensions->glGetProgramInfoLog(m_handle.glhandle(), maxLength, &length, pInfoLog);
#elif defined(GL_ARB_shader_objects)
				pExtensions->glGetInfoLogARB(m_handle.glhandle(), maxLength, &length, pInfoLog);
#endif
				arg.arg_sz = pInfoLog;
			}
			else
				arg.arg_sz = "Unknown Error";

			vector<CRaptorMessages::MessageArgument> args;
			args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_ERROR,
														   CRaptorMessages::ID_PROGRAM_ERROR,
														   __FILE__, __LINE__, args);
			if ((maxLength > 0) && (NULL != pInfoLog))
				free(pInfoLog);
		}
		else
		{
			m_bReLinked = true;
			m_bValid = true;
			m_bValid = glGetProgramStatus();
		}

		CATCH_GL_ERROR
		return !abort;
	}
#endif

	return false;
}

void COpenGLShaderStage::glQueryUniformLocations(void)
{
	if (m_handle.glhandle() == 0)
		return;

	// Query the size of uniforms
	GLint attrMaxLength = 0;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_VERSION_2_0)
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &attrMaxLength);
#elif defined(GL_ARB_shader_objects)
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &attrMaxLength);
#endif

	//	Predefined uniforms are not taken into account ! (e.g. gl_ModelViewProjectionMatrix)
	//	So, take a bit of space.
	attrMaxLength = MAX(32, attrMaxLength);
	GLcharARB *name = new GLcharARB[attrMaxLength];

	// Query the number of active uniforms
	GLint count = 0;
#if defined(GL_VERSION_2_0)
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_UNIFORMS, &count);
#elif defined(GL_ARB_shader_objects)
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_ACTIVE_UNIFORMS_ARB, &count);
#endif

	// Loop over each of the active uniforms, and set their value
	for (GLint i = 0; i < count; i++)
	{
		GLint size = 0;
		GLenum type = GL_FLOAT_VEC4_ARB;

#if defined(GL_VERSION_2_0)
		pExtensions->glGetActiveUniform(m_handle.glhandle(), i, attrMaxLength, NULL, &size, &type, name);
#elif defined(GL_ARB_shader_objects)
		pExtensions->glGetActiveUniformARB(m_handle.glhandle(), i, attrMaxLength, NULL, &size, &type, name);
#endif

		if (strlen(name) > 0)
		{
#if defined(GL_VERSION_2_0)
			GLint location = pExtensions->glGetUniformLocation(m_handle.glhandle(), name);
#elif defined(GL_ARB_shader_objects)
			GLint location = pExtensions->glGetUniformLocationARB(m_handle.glhandle(), name);
#endif

			if (location >= 0)
			{
				for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
				{
					CProgramParameters::CParameterBase& value = m_parameters[idx];

					if (value.name() == name)
					{
						value.locationIndex = location;
						value.locationType = type;
						value.locationSize = size;
						break;
					}
				}
			}
		}
	}

	delete[] name;

	CATCH_GL_ERROR
}

static bool isTypeVector(unsigned int shaderKind)
{
#if defined(GL_ARB_shader_objects)
	switch (shaderKind)
	{
		case GL_FLOAT:
		case GL_FLOAT_VEC2_ARB:
		case GL_FLOAT_VEC3_ARB:
		case GL_FLOAT_VEC4_ARB:
		case GL_INT:
		case GL_INT_VEC2_ARB:
		case GL_INT_VEC3_ARB:
		case GL_INT_VEC4_ARB:
		{
			return true;
			break;
		}
		default:
		{
			//  boolean values are not handled, they are too far from Raptor float vectors.
			return false;
		}
	}
#else
	return false;
#endif
}

void COpenGLShaderStage::glQueryAttributeLocations(void)
{
	if (m_handle.handle() == 0)
		return;

	// Query the size of attributes
	GLint attrMaxLength = 0;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_VERSION_2_0)
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attrMaxLength);
#elif defined(GL_ARB_shader_objects)
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB, &attrMaxLength);
#endif

	attrMaxLength = MAX(32, attrMaxLength);
	GLcharARB *name = new GLcharARB[attrMaxLength];

	// Query the number of active attributes
	GLint count = 0;
#if defined(GL_VERSION_2_0)
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_ATTRIBUTES, &count);
#elif defined(GL_ARB_shader_objects)
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_ACTIVE_ATTRIBUTES_ARB, &count);
#endif
	
	GLint maxAttribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &maxAttribs);

	// Loop over each of the active attributes, and set their value
	for (GLint i = 0; i < count; i++)
	{
		GLint size = 0;
		GLenum type = GL_FLOAT_VEC4_ARB;

#if defined(GL_VERSION_2_0)
		pExtensions->glGetActiveAttrib(m_handle.glhandle(), i, attrMaxLength, NULL, &size, &type, name);
#elif defined(GL_ARB_shader_objects)
		pExtensions->glGetActiveAttribARB(m_handle.glhandle(), i, attrMaxLength, NULL, &size, &type, name);
#endif

		if (strlen(name) > 0)
		{
#if defined(GL_VERSION_2_0)
			GLint location = pExtensions->glGetAttribLocation(m_handle.glhandle(), name);
#elif defined(GL_ARB_shader_objects)
			GLint location = pExtensions->glGetAttribLocationARB(m_handle.glhandle(), name);
#endif
			if (location >= 0)
			{
				for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
				{
					CProgramParameters::CParameterBase& value = m_parameters[idx];
					CProgramParameters::GL_VERTEX_ATTRIB userLocation = CProgramParameters::POSITION;
					if ((value.name() == name) && value.isA(userLocation) && isTypeVector(type))
					{
						userLocation = ((const CProgramParameters::CParameter<CProgramParameters::GL_VERTEX_ATTRIB>&)value).p;

						// the location retrieved will only be used if the user value is invalid.
						value.locationType = type;
						if ((userLocation < maxAttribs) && (userLocation != location))
						{
							value.locationIndex = userLocation;

							CRaptorMessages::MessageArgument arg;
							arg.arg_sz = name;
							vector<CRaptorMessages::MessageArgument> args;
							args.push_back(arg);

							//	Vertex attribute index inconsistency with user expectation after link
							Raptor::GetErrorManager()->generateRaptorError(	COpenGLShaderStage::COpenGLShaderStageClassID::GetClassId(),
																			CRaptorErrorManager::RAPTOR_WARNING,
																			CRaptorMessages::ID_UPDATE_FAILED,
																			__FILE__, __LINE__, args);
						}
						else
							value.locationIndex = location; // No effect, but consistent.
					}
				}
			}
		}
	}

	delete[] name;

	CATCH_GL_ERROR
}

#if defined(GL_VERSION_2_0)
void COpenGLShaderStage::glSetProgramParameters()
{
	CTextureUnitSetup::TEXTURE_IMAGE_UNIT sampler = CTextureUnitSetup::IMAGE_UNIT_0;
	GL_COORD_VERTEX vector(0.0f, 0.0f, 0.0f, 0.0f);
	CColor::RGBA color(0.0f, 0.0f, 0.0f, 0.0f);
	GL_MATRIX matrix;
	std::vector<float> float_vector;
	float f;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
	{
		const CProgramParameters::CParameterBase& param_value = m_parameters[idx];
		if (param_value.locationIndex >= 0)
		{
			if ((param_value.isA(f)) && (GL_FLOAT == param_value.locationType))
			{
				f = ((const CProgramParameters::CParameter<float>&)param_value).p;
				pExtensions->glUniform1f(param_value.locationIndex, f);
			}
			else if (param_value.isA(vector))
			{
				vector = ((const CProgramParameters::CParameter<GL_COORD_VERTEX>&)param_value).p;
				switch (param_value.locationType)
				{
					case GL_FLOAT:
					{
						pExtensions->glUniform1fv(param_value.locationIndex, 1, vector);
						break;
					}
					case GL_FLOAT_VEC2_ARB:
					{
						pExtensions->glUniform2fv(param_value.locationIndex, 1, vector);
						break;
					}
					case GL_FLOAT_VEC3_ARB:
					{
						pExtensions->glUniform3fv(param_value.locationIndex, 1, vector);
						break;
					}
					case GL_FLOAT_VEC4_ARB:
					{
						pExtensions->glUniform4fv(param_value.locationIndex, 1, vector);
						break;
					}
					case GL_INT:
					{
						int val = vector.x;
						pExtensions->glUniform1i(param_value.locationIndex, val);
						break;
					}
					case GL_INT_VEC2_ARB:
					{
						int val[2];
						val[0] = vector.x;
						val[1] = vector.y;
						pExtensions->glUniform2i(param_value.locationIndex, val[0], val[1]);
						break;
					}
					case GL_INT_VEC3_ARB:
					{
						int val[3];
						val[0] = vector.x;
						val[1] = vector.y;
						val[2] = vector.z;
						pExtensions->glUniform3iv(param_value.locationIndex, 1, &val[0]);
						break;
					}
					case GL_INT_VEC4_ARB:
					{
						int val[4];
						val[0] = vector.x;
						val[1] = vector.y;
						val[2] = vector.z;
						val[3] = vector.h;
						pExtensions->glUniform4iv(param_value.locationIndex, 1, &val[0]);
						break;
					}
				}
			}
			//	TODO: add type checking here: locationType should be GL_SAMPLER_xxx_ARB
			else if (param_value.isA(sampler))
			{
				sampler = ((const CProgramParameters::CParameter<CTextureUnitSetup::TEXTURE_IMAGE_UNIT>&)param_value).p;
				GLint s = sampler;
				pExtensions->glUniform1i(param_value.locationIndex, s);
			}
			else if (param_value.isA(matrix))
			{
				matrix = ((const CProgramParameters::CParameter<GL_MATRIX>&)param_value).p;
				pExtensions->glUniformMatrix4fvARB(param_value.locationIndex, 1, GL_TRUE, matrix);
			}
			else if (param_value.isA(color))
			{
				color = ((const CProgramParameters::CParameter<CColor::RGBA>&)param_value).p;
				pExtensions->glUniform4fv(param_value.locationIndex, 1, color);
			}
			else if (param_value.isA(float_vector) && (param_value.locationType == GL_FLOAT))
			{
				const std::vector<float> &fvector = ((const CProgramParameters::CParameter<std::vector<float>>&)param_value).p;
				size_t fsize = min(param_value.locationSize, fvector.size());
				pExtensions->glUniform1fv(param_value.locationIndex, (GLsizei)fsize, &fvector[0]);
			}
			//else if (param_value.isA(lorder)) : add a tempalte isA without const &T ?
			else if (param_value.getTypeId() == CProgramParameters::CParameterArray<int, CLightAttributes::MAX_LIGHTS>::TypeId())
			{
				const CLightAttributes::light_order &lvector = ((const CProgramParameters::CParameterArray<int, CLightAttributes::MAX_LIGHTS>&)param_value).p;
				size_t lsize = min(param_value.locationSize, CLightAttributes::MAX_LIGHTS);
				pExtensions->glUniform1iv(param_value.locationIndex, (GLsizei)lsize, &lvector[0]);
			}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			else
			{
				RAPTOR_ERROR(CShaderProgram::CShaderProgramClassID::GetClassId(),
							 CRaptorMessages::ID_UPDATE_FAILED);
			}
#endif
		}
	}
}
#elif defined(GL_ARB_vertex_shader)
void COpenGLShaderStage::glSetProgramParameters()
{
	CTextureUnitSetup::TEXTURE_IMAGE_UNIT sampler = CTextureUnitSetup::IMAGE_UNIT_0;
	GL_COORD_VERTEX vector(0.0f, 0.0f, 0.0f, 0.0f);
	CColor::RGBA color(0.0f, 0.0f, 0.0f, 0.0f);
	GL_MATRIX matrix;
	std::vector<float> float_vector;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
	{
		const CProgramParameters::CParameterBase& param_value = m_parameters[idx];
		if (param_value.locationIndex >= 0)
		{
			if (param_value.isA(vector))
			{
				vector = ((const CProgramParameters::CParameter<GL_COORD_VERTEX>&)param_value).p;
				switch (param_value.locationType)
				{
				case GL_FLOAT:
				{
					pExtensions->glUniform1fvARB(param_value.locationIndex, 1, vector);
					break;
				}
				case GL_FLOAT_VEC2_ARB:
				{
					pExtensions->glUniform2fvARB(param_value.locationIndex, 1, vector);
					break;
				}
				case GL_FLOAT_VEC3_ARB:
				{
					pExtensions->glUniform3fvARB(param_value.locationIndex, 1, vector);
					break;
				}
				case GL_FLOAT_VEC4_ARB:
				{
					pExtensions->glUniform4fvARB(param_value.locationIndex, 1, vector);
					break;
				}
				case GL_INT:
				{
					int val = vector.x;
					pExtensions->glUniform1iARB(param_value.locationIndex, val);
					break;
				}
				case GL_INT_VEC2_ARB:
				{
					int val[2];
					val[0] = vector.x;
					val[1] = vector.y;
					pExtensions->glUniform2iARB(param_value.locationIndex, val[0], val[1]);
					break;
				}
				case GL_INT_VEC3_ARB:
				{
					int val[3];
					val[0] = vector.x;
					val[1] = vector.y;
					val[2] = vector.z;
					pExtensions->glUniform3ivARB(param_value.locationIndex, 1, &val[0]);
					break;
				}
				case GL_INT_VEC4_ARB:
				{
					int val[4];
					val[0] = vector.x;
					val[1] = vector.y;
					val[2] = vector.z;
					val[3] = vector.h;
					pExtensions->glUniform4ivARB(param_value.locationIndex, 1, &val[0]);
					break;
				}
				}
			}
			//	TODO: add type checking here: locationType should be GL_SAMPLER_xxx_ARB
			else if (param_value.isA(sampler))
			{
				sampler = ((const CProgramParameters::CParameter<CTextureUnitSetup::TEXTURE_IMAGE_UNIT>&)param_value).p;
				GLint s = sampler;
				pExtensions->glUniform1iARB(param_value.locationIndex, s);
			}
			else if (param_value.isA(matrix))
			{
				matrix = ((const CProgramParameters::CParameter<GL_MATRIX>&)param_value).p;
				pExtensions->glUniformMatrix4fvARB(param_value.locationIndex, 1, GL_TRUE, matrix);
			}
			else if (param_value.isA(color))
			{
				color = ((const CProgramParameters::CParameter<CColor::RGBA>&)param_value).p;
				pExtensions->glUniform4fvARB(param_value.locationIndex, 1, color);
			}
			else if (param_value.isA(float_vector) && (param_value.locationType == GL_FLOAT))
			{
				const std::vector<float> &fvector = ((const CProgramParameters::CParameter<std::vector<float>>&)param_value).p;
				size_t fsize = min(param_value.locationSize, fvector.size());
				pExtensions->glUniform1fvARB(param_value.locationIndex, (GLsizei)fsize, &fvector[0]);
			}
			//else if (param_value.isA(lorder)) : add a tempalte isA without const &T ?
			else if (param_value.getTypeId() == CProgramParameters::CParameterArray<int, CLightAttributes::MAX_LIGHTS>::TypeId())
			{
				const CLightAttributes::light_order &lvector = ((const CProgramParameters::CParameterArray<int, CLightAttributes::MAX_LIGHTS>&)param_value).p;
				size_t lsize = min(param_value.locationSize, CLightAttributes::MAX_LIGHTS);
				pExtensions->glUniform1ivARB(param_value.locationIndex, (GLsizei)fsize, &lvector[0]);
			}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			else
			{
				RAPTOR_ERROR(CShaderProgram::CShaderProgramClassID::GetClassId(),
					CRaptorMessages::ID_UPDATE_FAILED);
			}
#endif
		}
	}
}
#else
void COpenGLShaderStage::glSetProgramParameters()
{
}
#endif

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
	while (io.hasMoreValues())
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

