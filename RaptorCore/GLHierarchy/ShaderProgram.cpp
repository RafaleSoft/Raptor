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
#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "Subsys/ShaderLibrary.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)
	#include "Subsys/UniformAllocator.h"
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
	m_parameters(),
	m_uniforms(NULL),
	m_uniforms_size(0)
{
}

CShaderProgram::CShaderProgram(const CShaderProgram& shader)
    :CPersistence(shaderId,shader.getName())
{
	m_bValid = shader.m_bValid;
	m_handle = shader.m_handle;
	m_bApplyParameters = shader.m_bApplyParameters;
	m_parameters = shader.m_parameters;
	m_uniforms = shader.m_uniforms;
	m_uniforms_size = shader.m_uniforms_size;
}

CShaderProgram::~CShaderProgram()
{
	// TODO : Recycle handle
#if defined(GL_ARB_uniform_buffer_object)
	if (NULL != m_uniforms)
	{
		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
		pUAllocator->releaseUniforms(m_uniforms);
	}
#endif
}

bool CShaderProgram::glAddToLibrary(const std::string& shader_name,
									const std::string& shader_source_file,
									const std::string& class_name)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	CShaderLibrary *library = instance.m_pShaderLibraryInstance;
	if (NULL != library)
		return library->glAddToLibrary(shader_name, shader_source_file, class_name);
	else
		return false;
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



uint64_t CShaderProgram::glGetBufferMemoryRequirements(void)
{
	if (m_handle.glhandle() == 0)
		return 0;

	uint64_t uniform_size = 0;

#if defined(GL_ARB_uniform_buffer_object)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	GLint max_bindings = 0;
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS_ARB, &max_bindings);
	
	GLint active_uniform_max_length = 0;
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH_ARB, &active_uniform_max_length);
	char *uniformBlockName = new char[active_uniform_max_length];

	//! Despite the fact that the GL_VERSION text is greater than 3.1, it seems there is a bug in the call
	//! below : the actual value should be returned by glGetProgramiv and not by glGetObjectParameteriv.
	GLint active_blocks_count = 0;
	//pExtensions->glGetProgramivARB(m_handle.glhandle(), GL_ACTIVE_UNIFORM_BLOCKS_ARB, &active_blocks_count2);
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_ACTIVE_UNIFORM_BLOCKS_ARB, &active_blocks_count);

	for (GLint i = 0; i < active_blocks_count; i++)
	{
		GLint block_size = 0;
		pExtensions->glGetActiveUniformBlockivARB(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_DATA_SIZE_ARB, &block_size);
		uniform_size += block_size;

		/**
		 *	Currently not used. Kept for mapping on native attributes with user attributes.
		GLint active_uniforms = 0;
		pExtensions->glGetActiveUniformBlockivARB(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS_ARB, &active_uniforms);
		GLint indices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		pExtensions->glGetActiveUniformBlockivARB(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES_ARB, &indices[0]);
		GLint active_uniform_length = 0;
		pExtensions->glGetActiveUniformBlockivARB(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_NAME_LENGTH_ARB, &active_uniform_length);
		 */

		GLsizei length = 0;
		pExtensions->glGetActiveUniformBlockNameARB(m_handle.glhandle(), i, 256, &length, uniformBlockName);
		uniformBlockName[length] = 0;

		/**
		 *	Currently not used. Kept for mapping on native attributes with user attributes.
		GLuint uniformBlockIndex = pExtensions->glGetUniformBlockIndexARB(m_handle.glhandle(), uniformBlockName);
		 */

		GLint binding = 0;
		pExtensions->glGetActiveUniformBlockivARB(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_BINDING_ARB, &binding);

		for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
		{
			CProgramParameters::CParameterBase& value = m_parameters[idx];
			std::string name(uniformBlockName);
			if ((value.name() == name) && (value.locationType == GL_UNIFORM_BLOCK_BINDING_ARB))
			{
				value.locationIndex = binding;

				if ((binding >= max_bindings) || (block_size != value.size()))
				{
					//	Vertex attribute index inconsistency with user expectation after link
					RAPTOR_WARNING(	CShaderProgram::CShaderProgramClassID::GetClassId(),
									CRaptorMessages::ID_UPDATE_FAILED);
				}
			}
		}
	}
#endif

	delete[] uniformBlockName;
	return uniform_size;
}
