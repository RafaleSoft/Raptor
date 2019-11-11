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

}

void CUnifiedShader::glProgramParameter(unsigned int numParam, const GL_COORD_VERTEX &v) const
{
    if (m_handle.handle() == 0)
        return;

    glParameter(numParam,v);
}

void CUnifiedShader::glProgramParameter(unsigned int numParam, const CColor::RGBA &v) const
{
    if (m_handle.handle() == 0)
        return;

    glParameter(numParam,v);
}


void CUnifiedShader::glParameter(unsigned int numParam, const float *v) const
{
#if defined(GL_ARB_shader_objects)
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
            pExtensions->glUniform4fvARB(location,1,v);
        }
    }
#endif

    CATCH_GL_ERROR
}


std::string CUnifiedShader::glGetProgramString(void) const
{
	if (m_handle.handle() == 0)
		return "";

#if defined(GL_ARB_shader_objects)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	int value = 0;
	pExtensions->glGetObjectParameterivARB(m_handle, GL_OBJECT_SHADER_SOURCE_LENGTH_ARB, &value);
	char *source = new char[value];
	GLsizei length = 0;
	pExtensions->glGetShaderSourceARB(m_handle, value, &length, source);

	std::string program_source = source;
	delete[] source;
	return program_source;
#else
	return "";
#endif
}

bool CUnifiedShader::glBindProgram(RAPTOR_HANDLE program)
{
#if defined(GL_ARB_shader_objects)
	if (program.handle() == 0)
		return false;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	GLint value = 0;
	pExtensions->glGetObjectParameterivARB(program.handle(), GL_OBJECT_TYPE_ARB, &value);
	if (value != GL_PROGRAM_OBJECT_ARB)
		return false;

	pExtensions->glAttachObjectARB(program.handle(), m_handle.handle());

	CATCH_GL_ERROR

	return true;
#else
	return false;
#endif
}

bool CUnifiedShader::glUnbindProgram(RAPTOR_HANDLE program)
{
#if defined(GL_ARB_shader_objects)
	if ((program.handle() == 0) || (m_handle.handle() == 0))
		return false;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	GLint value = 0;
	pExtensions->glGetObjectParameterivARB(program.handle(), GL_OBJECT_TYPE_ARB, &value);
	if (value != GL_PROGRAM_OBJECT_ARB)
		return false;

	pExtensions->glDetachObjectARB(program.handle(), m_handle.handle());

	CATCH_GL_ERROR

	return true;
#else
	return false;
#endif
}

void CUnifiedShader::glRender(void)
{
	if (m_handle.handle() == 0)
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


uint64_t CUnifiedShader::glGetBufferMemoryRequirements(RAPTOR_HANDLE program)
{
	if (program.handle() == 0)
		return 0;

	uint64_t uniform_size = 0;

#if defined(GL_ARB_uniform_buffer_object)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	GLint active_blocks_count = 0;

	//! Despite the fact that the GL_VERSION text is greater than 3.1, it seems there is a bug in the call
	//! below : the actual value should be returned by glGetProgramiv and not by glGetObjectParameteriv.
	//pExtensions->glGetProgramivARB(program.handle, GL_ACTIVE_UNIFORM_BLOCKS_ARB, &active_blocks_count);
	pExtensions->glGetObjectParameterivARB(program.handle(), GL_ACTIVE_UNIFORM_BLOCKS_ARB, &active_blocks_count);

	for (GLint i = 0; i < active_blocks_count; i++)
	{
		GLint block_size = 0;
		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_DATA_SIZE_ARB, &block_size);

		uniform_size += block_size;

		GLint active_uniforms = 0;
		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS_ARB, &active_uniforms);
		GLint indices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES_ARB, &indices[0]);

		GLint active_uniform_max_length = 0;
		//pExtensions->glGetProgramivARB(program.handle, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH_ARB, &active_uniform_max_length);
		pExtensions->glGetObjectParameterivARB(program.handle(), GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH_ARB, &active_uniform_max_length);

		GLint active_uniform_length = 0;
		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_NAME_LENGTH_ARB, &active_uniform_length);

		GLsizei length = 0;
		char uniformBlockName[256];
		pExtensions->glGetActiveUniformBlockNameARB(program.handle(), i, 256, &length, uniformBlockName);

		GLuint uniformBlockIndex = pExtensions->glGetUniformBlockIndexARB(program.handle(), uniformBlockName);

		GLint binding = 0;
		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_BINDING_ARB, &binding);

		GLint max_bindings = 0;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS_ARB, &max_bindings);

		pExtensions->glGetActiveUniformBlockivARB(program.handle(), i, GL_UNIFORM_BLOCK_BINDING_ARB, &binding);
	}
#endif

	return uniform_size;
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

