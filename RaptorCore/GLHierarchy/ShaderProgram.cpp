/***************************************************************************/
/*                                                                         */
/*  ShaderProgram.cpp                                                      */
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

std::string CShaderProgram::readFile(const std::string filename)
{
	CRaptorIO *shdr = CRaptorIO::Create(filename, CRaptorIO::DISK_READ);
	if (NULL == shdr)
		return false;

	if (shdr->getStatus() == CRaptorIO::IO_OK)
	{
		std::string programstr = "";
		while (shdr->getStatus() == CRaptorIO::IO_OK)
		{
			std::string line;
			*shdr >> line;
			
			size_t include_pos = line.find("#include", 0);
			size_t comment_pos = line.find("//", 0);
			if ((std::string::npos != include_pos) && (include_pos < comment_pos))
			{
				size_t file_begin = line.find("\"", include_pos + 8);
				size_t file_end = line.find("\"", file_begin + 1);
				if ((std::string::npos != file_begin) && (std::string::npos != file_end))
				{
					size_t path_end = filename.find_last_of('\\');
					std::string path = filename.substr(0, path_end) + "\\" + line.substr(file_begin+1,file_end-file_begin-1);
					line = readFile(path);
					programstr = programstr + line + "\n";
				}
				else
				{
					RAPTOR_ERROR(shaderId, "Syntax error near include directire: " + line);
				}
			}
			else
			{
				if (shdr->getStatus() == CRaptorIO::IO_OK)
					programstr = programstr + line + "\n";
			}
		}

		delete shdr;
		return programstr;
	}
	else
	{
		vector<CRaptorMessages::MessageArgument> args;
		CRaptorMessages::MessageArgument arg;
		arg.arg_sz = filename.c_str();
		args.push_back(arg);

		//!	Shader file could not be opened.
		IRaptor::GetErrorManager()->generateRaptorError(shaderId,
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_NO_RESOURCE,
														__FILE__, __LINE__, args);

		delete shdr;
		return false;
	}
}

bool CShaderProgram::glLoadProgramFromFile(const std::string &program)
{
	if (program.empty())
		return false;

	std::string programstr = readFile(program);
	if (!programstr.empty())
		return glLoadProgram(programstr);
	else
	{
		vector<CRaptorMessages::MessageArgument> args;
		CRaptorMessages::MessageArgument arg;
		arg.arg_sz = program.c_str();
		args.push_back(arg);

		//!	Shader file could not be opened.
		IRaptor::GetErrorManager()->generateRaptorError(shaderId,
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_NO_RESOURCE,
														__FILE__, __LINE__, args);

		return false;
	}
}

CShaderProgram::shader_bloc CShaderProgram::glGetShaderBloc(const std::string& bloc_name) const
{
	shader_bloc bloc{ 0, INT32_MAX };

#if defined(GL_VERSION_3_1)
	if (m_handle.glhandle() == 0)
		return bloc;

	const CRaptorGLExtensions *const pExtensions = IRaptor::glGetExtensions();

	uint64_t uniform_size = 0;
	GLint max_bindings = 0;


	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_bindings);

	GLint active_uniform_max_length = 0;
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &active_uniform_max_length);

	//! Despite the fact that the GL_VERSION text is greater than 3.1, it seems there is a bug in the call
	//! below : the actual value should be returned by glGetProgramiv and not by glGetObjectParameteriv.
	GLint active_blocks_count = 0;
	char *uniformBlockName = new char[active_uniform_max_length];

	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_UNIFORM_BLOCKS, &active_blocks_count);

	for (GLint i = 0; i < active_blocks_count; i++)
	{
		GLint block_size = 0;
		pExtensions->glGetActiveUniformBlockiv(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);

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
		pExtensions->glGetActiveUniformBlockName(m_handle.glhandle(), i, 256, &length, &uniformBlockName[0]);
		uniformBlockName[length] = 0;

		/**
		 *	Currently not used. Kept for mapping on native attributes with user attributes.
		GLuint uniformBlockIndex = pExtensions->glGetUniformBlockIndexARB(m_handle.glhandle(), uniformBlockName);
		 */

		GLint binding = 0;
		pExtensions->glGetActiveUniformBlockiv(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_BINDING, &binding);

		std::string name(uniformBlockName);
		if (!bloc_name.compare(name))
		{
			bloc.binding = binding;
			bloc.size = block_size;
		}
	}

	delete[] uniformBlockName;

	CATCH_GL_ERROR
#endif

	return bloc;
}

uint64_t CShaderProgram::glGetBufferMemoryRequirements(void)
{
	if (m_handle.glhandle() == 0)
		return 0;

	const CRaptorGLExtensions *const pExtensions = IRaptor::glGetExtensions();

	uint64_t uniform_size = 0;
	GLint max_bindings = 0;

#if defined(GL_VERSION_3_1)
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_bindings);
#elif defined(GL_ARB_uniform_buffer_object)
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS_ARB, &max_bindings);
#endif

	GLint active_uniform_max_length = 0;

#if defined(GL_VERSION_3_1)
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &active_uniform_max_length);
#elif defined(GL_ARB_uniform_buffer_object)
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH_ARB, &active_uniform_max_length);
#endif

	char *uniformBlockName = new char[active_uniform_max_length];

	//! Despite the fact that the GL_VERSION text is greater than 3.1, it seems there is a bug in the call
	//! below : the actual value should be returned by glGetProgramiv and not by glGetObjectParameteriv.
	GLint active_blocks_count = 0;
	
#if defined(GL_VERSION_3_1)
	pExtensions->glGetProgramiv(m_handle.glhandle(), GL_ACTIVE_UNIFORM_BLOCKS, &active_blocks_count);
#elif defined(GL_ARB_uniform_buffer_object)
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_ACTIVE_UNIFORM_BLOCKS_ARB, &active_blocks_count);
#endif

	for (GLint i = 0; i < active_blocks_count; i++)
	{
		GLint block_size = 0;

#if defined(GL_VERSION_3_1)
		pExtensions->glGetActiveUniformBlockiv(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);
#elif defined(GL_ARB_uniform_buffer_object)
		pExtensions->glGetActiveUniformBlockivARB(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_DATA_SIZE_ARB, &block_size);
#endif

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

#if defined(GL_VERSION_3_1)
		pExtensions->glGetActiveUniformBlockName(m_handle.glhandle(), i, 256, &length, &uniformBlockName[0]);
#elif defined(GL_ARB_uniform_buffer_object)
		pExtensions->glGetActiveUniformBlockNameARB(m_handle.glhandle(), i, 256, &length, uniformBlockName);
#endif

		uniformBlockName[length] = 0;

		/**
		 *	Currently not used. Kept for mapping on native attributes with user attributes.
		GLuint uniformBlockIndex = pExtensions->glGetUniformBlockIndexARB(m_handle.glhandle(), uniformBlockName);
		 */

		GLint binding = 0;

#if defined(GL_VERSION_3_1)
		pExtensions->glGetActiveUniformBlockiv(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_BINDING, &binding);
#elif defined(GL_ARB_uniform_buffer_object)
		pExtensions->glGetActiveUniformBlockivARB(m_handle.glhandle(), i, GL_UNIFORM_BLOCK_BINDING_ARB, &binding);
#endif

		for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
		{
			CProgramParameters::CParameterBase& value = m_parameters[idx];
			std::string name(uniformBlockName);
			if ((value.name() == name) && 
				((value.locationType == GL_UNIFORM_BLOCK_BINDING_ARB) || (value.locationType == GL_UNIFORM_BLOCK_BINDING)))
			{
				value.locationIndex = binding;
				value.locationSize = block_size;

				if ((binding >= max_bindings) || (block_size != value.size()))
				{
					vector<CRaptorMessages::MessageArgument> args;
					CRaptorMessages::MessageArgument arg;
					arg.arg_sz = name.c_str();
					args.push_back(arg);
					CRaptorMessages::MessageArgument arg2;
					arg2.arg_int = block_size;
					args.push_back(arg2);
					CRaptorMessages::MessageArgument arg3;
					arg3.arg_int = value.size();
					args.push_back(arg3);

					//	Vertex attribute index inconsistency with user expectation after link
					IRaptor::GetErrorManager()->generateRaptorError(shaderId,
																	CRaptorErrorManager::RAPTOR_WARNING,
																	CRaptorMessages::ID_UPDATE_FAILED,
																	__FILE__, __LINE__, args);
				}
			}
		}
	}

	delete[] uniformBlockName;

	CATCH_GL_ERROR

	return uniform_size;
}
