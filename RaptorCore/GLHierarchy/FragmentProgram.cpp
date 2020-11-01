/***************************************************************************/
/*                                                                         */
/*  FragmentProgram.cpp                                                    */
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
#if !defined(AFX_FRAGMENTPROGRAM_H__DD0AD51D_3BFF_4C65_8099_BA7696D7BDDF__INCLUDED_)
	#include "FragmentProgram.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


RAPTOR_NAMESPACE

IMPLEMENT_CLASS_ID(CFragmentProgram, fragmentId)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFragmentProgram::CFragmentProgram(const std::string& name)
	:CShaderProgram(fragmentId,name)
{
    m_bValid = false;
	m_handle.handle(0);	// default openGL vertex processing pipeline
	m_handle.hClass(CFragmentProgram::CFragmentProgramClassID::GetClassId().ID());
}

CFragmentProgram::CFragmentProgram(const CFragmentProgram& shader)
	:CShaderProgram(shader)
{
	m_bValid = shader.m_bValid;
}

CFragmentProgram* CFragmentProgram::glClone()
{
	return new CFragmentProgram(*this);
}

CFragmentProgram::~CFragmentProgram()
{
#ifdef GL_ARB_fragment_program
	if (CRaptorInstance::GetInstance().isFragmentProgramReady())
	{
		glStop();

		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		GLuint program = m_handle.glhandle();
		if (pExtensions->glIsProgramARB(program))
			pExtensions->glDeleteProgramsARB(1,&program);
	}
#endif
}

bool CFragmentProgram::glGetProgramCaps(GL_FRAGMENT_PROGRAM_CAPS& caps)
{
#ifdef GL_ARB_fragment_program
	if (CRaptorInstance::GetInstance().isFragmentProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_INSTRUCTIONS_ARB,&caps.max_instructions);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB,&caps.max_native_instructions);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_TEMPORARIES_ARB,&caps.max_temporaries);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB,&caps.max_native_temporaries);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_PARAMETERS_ARB,&caps.max_parameters);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,&caps.max_local_parameters);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,&caps.max_env_parameters);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB,&caps.max_native_parameters);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_ATTRIBS_ARB,&caps.max_attribs);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB,&caps.max_native_attribs);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB,&caps.max_alu_instructions);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB,&caps.max_native_alu_instructions);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB,&caps.max_tex_instructions);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB,&caps.max_native_tex_instructions);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB,&caps.max_tex_indirections);
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB,&caps.max_native_tex_indirections);
		
		glGetIntegerv(GL_MAX_PROGRAM_MATRICES_ARB,&caps.max_program_matrices);
		glGetIntegerv(GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB,&caps.max_program_matrix_stack_depth);
		glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,&caps.max_texture_coords);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB,&caps.max_texture_image_units);

		return true;
	}
	else
#endif
		return false;
}


void CFragmentProgram::glRender(void)
{
	if (m_handle.glhandle() == 0)
		return;

#ifdef GL_ARB_fragment_program
	if (CRaptorInstance::GetInstance().isFragmentProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		if (pExtensions->glIsProgramARB(m_handle.glhandle()))
		{
			glEnable(GL_FRAGMENT_PROGRAM_ARB);
			pExtensions->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_handle.glhandle());

			if (m_bApplyParameters)
			{
				for (unsigned int i=0;i<m_parameters.getNbParameters();i++)
				{
					const CProgramParameters::CParameterBase& param_value = m_parameters[i];
					const GL_COORD_VERTEX &vector = ((const CProgramParameters::CParameter<GL_COORD_VERTEX>&)param_value).p;

					pExtensions->glProgramLocalParameter4fvARB(	GL_FRAGMENT_PROGRAM_ARB, i, vector);
				}
				m_bApplyParameters = false;
			}
		}
	}
#endif

	CATCH_GL_ERROR
}

void CFragmentProgram::glStop(void)
{
#ifdef GL_ARB_fragment_program
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,0);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
#endif
}

bool CFragmentProgram::glLoadProgram(const std::string &program)
{
    m_bValid = false;

#ifdef GL_ARB_fragment_program
	if (CRaptorInstance::GetInstance().isFragmentProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

        //!    In case of a previous error, we need to initialize error checking
        //!    to be sure that the error detected will only be due to shader loading.
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
			RAPTOR_WARNING(	fragmentId, "Raptor encountered errors before loading vertex shader, check with debug infos.");
            while (err != GL_NO_ERROR)
                err = ::glGetError();
        }

		GLuint hd = m_handle.glhandle();
		if (pExtensions->glIsProgramARB(hd))
			pExtensions->glDeleteProgramsARB(1,&hd);

		pExtensions->glGenProgramsARB(1,&hd);
		pExtensions->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,hd);
		m_handle.handle(hd);

	 	pExtensions->glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,
										GL_PROGRAM_FORMAT_ASCII_ARB,
										(GLsizei)program.size(),
										(void*)(program.data()));
		err = glGetError();
		
		if (err != GL_NO_ERROR)// && (err == GL_INVALID_OPERATION))
		{
			int pos = 0;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB,&pos);
			const unsigned char * str = glGetString(GL_PROGRAM_ERROR_STRING_ARB);

            vector<CRaptorMessages::MessageArgument> args;
            CRaptorMessages::MessageArgument arg1;
            arg1.arg_int = pos;
            args.push_back(arg1);
            CRaptorMessages::MessageArgument arg2;
            arg2.arg_sz = (const char*)str;
            args.push_back(arg2);

			Raptor::GetErrorManager()->generateRaptorError(fragmentId,
                                                           CRaptorErrorManager::RAPTOR_ERROR,
											               CRaptorMessages::ID_PROGRAM_ERROR,
														   __FILE__, __LINE__, args); 
		}

        m_bValid = ((err == GL_NO_ERROR) && glGetProgramStatus());
        if (!m_bValid)
		{
			RAPTOR_WARNING(fragmentId, CRaptorMessages::ID_NO_GPU_PROGRAM)
			RAPTOR_WARNING(fragmentId, getName().data())
		}

		// Unbind program to avoid side effects
		glStop();
	}
#endif

	CATCH_GL_ERROR

	return m_bValid;
}

std::string CFragmentProgram::glGetProgramString(void) const
{
	if (m_handle.glhandle() == 0)
		return "";

	if (!CRaptorInstance::GetInstance().isFragmentProgramReady())
		return "";

#if defined(GL_ARB_fragment_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glIsProgramARB(m_handle.glhandle()))
		pExtensions->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_handle.glhandle());
	else
		return "";

	int value = 0;
	pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_LENGTH_ARB, &value);
	char *source = new char[value];
	pExtensions->glGetProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_STRING_ARB, source);

	std::string program_source = source;
	delete[] source;
	return program_source;
#else
	return "";
#endif
}

bool CFragmentProgram::glGetProgramStatus(void) const
{
	if (m_handle.glhandle() == 0)
		return false;

	if (!CRaptorInstance::GetInstance().isFragmentProgramReady())
		return false;

#if defined(GL_ARB_fragment_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glIsProgramARB(m_handle.glhandle()))
		pExtensions->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_handle.glhandle());
	else
		return false;

	GL_FRAGMENT_PROGRAM_CAPS caps;
	if (glGetProgramCaps(caps))
	{
		int value = 0;
		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_INSTRUCTIONS_ARB,&value);
		if (value > caps.max_instructions)
			return false;

		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB,&value);
		if (value > caps.max_native_instructions)
			return false;

		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_TEMPORARIES_ARB,&value);
		if (value > caps.max_temporaries)
			return false;

		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_NATIVE_TEMPORARIES_ARB,&value);
		if (value > caps.max_native_temporaries)
			return false;

		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_PARAMETERS_ARB,&value);
		if (value > caps.max_parameters)
			return false;

		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_NATIVE_PARAMETERS_ARB,&value);
		if (value > caps.max_native_parameters)
			return false;

		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_ATTRIBS_ARB,&value);
		if (value > caps.max_attribs)
			return false;

		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_NATIVE_ATTRIBS_ARB,&value);
		if (value > caps.max_native_attribs)
			return false;

		pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB,&value);
		if (value < 1)
			return false;

        pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_ALU_INSTRUCTIONS_ARB,&value);
		if (value > caps.max_alu_instructions)
			return false;

        pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_TEX_INSTRUCTIONS_ARB,&value);
		if (value > caps.max_tex_instructions)
			return false;

        pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_TEX_INDIRECTIONS_ARB,&value);
		if (value > caps.max_tex_indirections)
			return false;

        pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB,&value);
		if (value > caps.max_native_alu_instructions)
			return false;

        pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB,&value);
		if (value > caps.max_native_tex_instructions)
			return false;

        pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB,&value);
		if (value > caps.max_native_tex_indirections)
			return false;
 
		return true;
	}
	else
#endif
	return false;
}

void CFragmentProgram::glProgramParameter(unsigned int numParam, const GL_COORD_VERTEX &v) const
{
#if defined(GL_ARB_fragment_program)
	if (CRaptorInstance::GetInstance().isFragmentProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, numParam, v);
	}
#endif

	CATCH_GL_ERROR
}

void CFragmentProgram::glProgramParameter(unsigned int numParam, const CColor::RGBA &v) const
{
#if defined(GL_ARB_fragment_program)
	if (CRaptorInstance::GetInstance().isFragmentProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, numParam, v);
	}
#endif

	CATCH_GL_ERROR
}

