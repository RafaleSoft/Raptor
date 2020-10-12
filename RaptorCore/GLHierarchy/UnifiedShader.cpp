/***************************************************************************/
/*                                                                         */
/*  UnifiedShader.cpp                                                      */
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

#if !defined(AFX_UNIFIEDSHADER_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)
    #include "UnifiedShader.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnifiedShader::CUnifiedShader(const CPersistence::CPersistenceClassID &classId, const std::string& name)
	:CShaderProgram(classId,name)
{
}

CUnifiedShader::CUnifiedShader(const CUnifiedShader& shader)
	:CShaderProgram(shader)
{
}

CUnifiedShader::~CUnifiedShader()
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (m_handle.glhandle() > 0)
	{
#if defined(GL_VERSION_2_0)
		pExtensions->glDeleteShader(m_handle.glhandle());
#elif defined(GL_ARB_vertex_shader)
		pExtensions->glDeleteObjectARB(m_handle.glhandle());
#endif
	
		m_handle.glhandle(0);
	}
}

void CUnifiedShader::glProgramParameter(size_t numParam, const GL_COORD_VERTEX &v) const
{
	if (m_handle.glhandle() == 0)
        return;

    glParameter(numParam,v);
}

void CUnifiedShader::glProgramParameter(size_t numParam, const CColor::RGBA &v) const
{
	if (m_handle.glhandle() == 0)
        return;

    glParameter(numParam,v);
}

void CUnifiedShader::glParameter(size_t numParam, const float *v) const
{
	if (numParam < m_parameters.getNbParameters())
	{
		const CProgramParameters::CParameterBase& param_value = m_parameters[numParam];
		GLint location = param_value.locationIndex;
		if (location == -1)
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			RAPTOR_ERROR(	CShaderProgram::CShaderProgramClassID::GetClassId(),
							CRaptorMessages::ID_NO_RESOURCE);
#endif
		}
		else
		{
			const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_VERSION_2_0)
			pExtensions->glUniform4fv(location, 1, v);
#elif defined(GL_ARB_shader_objects)
			pExtensions->glUniform4fvARB(location, 1, v);
#endif
		}
	}

	CATCH_GL_ERROR
}

bool CUnifiedShader::glLoadProgram(	const std::string &program, 
									GLenum shaderType,
									const CPersistence::CPersistenceClassID& shaderClass)
{
	m_bValid = false;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (CRaptorInstance::GetInstance().isVertexShaderReady())
	{
		if (m_handle.glhandle() > 0)
#if defined(GL_VERSION_2_0)
			pExtensions->glDeleteShader(m_handle.glhandle());
		m_handle.handle(pExtensions->glCreateShader(shaderType));
#elif defined(GL_ARB_vertex_shader)
			pExtensions->glDeleteObjectARB(m_handle.glhandle());
		m_handle.handle(pExtensions->glCreateShaderObjectARB(shaderType));
#endif

		if (m_handle.glhandle() == 0)
		{
			RAPTOR_WARNING(shaderClass, CRaptorMessages::ID_NO_GPU_PROGRAM)
			return false;
		}

		GLint length = (GLint)program.size();
		const char* source = program.data();

#if defined(GL_VERSION_2_0)
		pExtensions->glShaderSource(m_handle.glhandle(), 1, &source, &length);
		pExtensions->glCompileShader(m_handle.glhandle());
#elif defined(GL_ARB_vertex_shader)
		pExtensions->glShaderSourceARB(m_handle.glhandle(), 1, &source, &length);
		pExtensions->glCompileShaderARB(m_handle.glhandle());
#endif

		m_bValid = glGetProgramStatus();

		if (!m_bValid)
		{
			GLint maxLength = 0;

#if defined(GL_VERSION_2_0)
			pExtensions->glGetShaderiv(m_handle.handle(), GL_INFO_LOG_LENGTH, &maxLength);
#elif defined(GL_ARB_shader_objects)
			pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
#endif
			char *pInfoLog = (char*)malloc(maxLength * sizeof(char));

#if defined(GL_VERSION_2_0)
			pExtensions->glGetShaderInfoLog(m_handle.glhandle(), maxLength, &length, pInfoLog);
#elif defined(GL_ARB_shader_objects)
			pExtensions->glGetInfoLogARB(m_handle.glhandle(), maxLength, &length, pInfoLog);
#endif
			CRaptorMessages::MessageArgument arg;
			arg.arg_sz = pInfoLog;
			vector<CRaptorMessages::MessageArgument> args;
			args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(	shaderClass,
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_PROGRAM_ERROR,
															__FILE__, __LINE__, args);
			free(pInfoLog);
			return false;
		}
	}

	CATCH_GL_ERROR

	return m_bValid;
}

std::string CUnifiedShader::glGetProgramString(void) const
{
	if (m_handle.glhandle() == 0)
		return "";

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	int value = 0;
	char *source = NULL;

#if defined(GL_VERSION_2_0)
	pExtensions->glGetShaderiv(m_handle.glhandle(), GL_SHADER_SOURCE_LENGTH, &value);
	source = new char[value];
	GLsizei length = 0;
	pExtensions->glGetShaderSource(m_handle.glhandle(), value, &length, source);
#elif defined(GL_ARB_shader_objects)
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_SHADER_SOURCE_LENGTH_ARB, &value);
	source = new char[value];
	GLsizei length = 0;
	pExtensions->glGetShaderSourceARB(m_handle.glhandle(), value, &length, source);
#else
	return "";
#endif

	std::string program_source = source;
	delete[] source;
	return program_source;
}

bool CUnifiedShader::glBindProgram(RAPTOR_HANDLE program)
{
	if (program.glhandle() == 0)
		return false;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	

#if defined(GL_VERSION_2_0)
	if (!pExtensions->glIsProgram(program.glhandle()))
		return false;

	pExtensions->glAttachShader(program.glhandle(), m_handle.glhandle());

#elif defined(GL_ARB_shader_objects)
	GLint value = 0;
	pExtensions->glGetObjectParameterivARB(program.glhandle(), GL_OBJECT_TYPE_ARB, &value);
	if (value != GL_PROGRAM_OBJECT_ARB)
		return false;

	pExtensions->glAttachObjectARB(program.glhandle(), m_handle.glhandle());

#else
	return false;
#endif

	CATCH_GL_ERROR

	return true;
}

bool CUnifiedShader::glUnbindProgram(RAPTOR_HANDLE program)
{
	if ((program.glhandle() == 0) || (m_handle.glhandle() == 0))
		return false;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_VERSION_2_0)
	if (!pExtensions->glIsProgram(program.glhandle()))
		return false;

	pExtensions->glDetachShader(program.glhandle(), m_handle.glhandle());

#elif defined(GL_ARB_shader_objects)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	GLint value = 0;
	pExtensions->glGetObjectParameterivARB(program.glhandle(), GL_OBJECT_TYPE_ARB, &value);
	if (value != GL_PROGRAM_OBJECT_ARB)
		return false;

	pExtensions->glDetachObjectARB(program.glhandle(), m_handle.glhandle());

#else
	return false;
#endif

	CATCH_GL_ERROR

	return true;

}

void CUnifiedShader::glRender(void)
{
	if (m_handle.glhandle() == 0)
		return;
	
	CATCH_GL_ERROR
}

/*
 *	The API is missing interface for doubles.
 *
void CUnifiedShader::glProgramParameter(unsigned int numParam,GL_HIRES_COORD_VERTEX &v)
{
	if (m_handle.handle == 0)
        return;

#if defined(GL_ARB_shader_objects)
    if (numParam < m_parameters.getNbParameters())
    {
        GLint location = m_locations[numParam].locationIndex;
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        if (location == -1)
        {
            Raptor::GetErrorManager()->generateRaptorError(	getId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_NO_RESOURCE);
        }
        else
#endif
        {
            const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
            pExtensions->glUniform4dvARB(location,1,v);
        }
    }
#endif

    CATCH_GL_ERROR
}
*/

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

static bool isTypeMatrix(unsigned int shaderKind)
{
#if defined(GL_ARB_shader_objects)
	switch (shaderKind)
	{
		case GL_FLOAT_MAT2_ARB:
		case GL_FLOAT_MAT3_ARB:
		case GL_FLOAT_MAT4_ARB:
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

static bool isTypeSampler(unsigned int shaderKind)
{
#if defined(GL_ARB_shader_objects)
	switch (shaderKind)
	{
		case GL_SAMPLER_1D_ARB:
		case GL_SAMPLER_2D_ARB:
		case GL_SAMPLER_3D_ARB:
		case GL_SAMPLER_CUBE_ARB:
		case GL_SAMPLER_1D_SHADOW_ARB:
		case GL_SAMPLER_2D_SHADOW_ARB:
		case GL_SAMPLER_2D_RECT_ARB:
		case GL_SAMPLER_2D_RECT_SHADOW_ARB:
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

