// FragmentShader.cpp: implementation of the CFragmentShader class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "FragmentShader.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif


RAPTOR_NAMESPACE

bool CFragmentShader::m_bFragmentReady = false;
static CFragmentShader::CFragmentShaderClassID fragmentId;
const CPersistence::CPersistenceClassID& CFragmentShader::CFragmentShaderClassID::GetClassId(void)
{
	return fragmentId;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFragmentShader::CFragmentShader(const std::string& name)
	:CShaderProgram(fragmentId,name)
{
    m_bValid = false;
	m_handle.handle = 0;	// default openGL vertex processing pipeline
	m_handle.hClass = CFragmentShader::CFragmentShaderClassID::GetClassId().ID();

    glInitShaders();
}

CFragmentShader::CFragmentShader(const CFragmentShader& shader)
	:CShaderProgram(shader)
{
	m_bValid = shader.m_bValid;
}

CFragmentShader* CFragmentShader::glClone()
{
	return new CFragmentShader(*this);
}

CFragmentShader::~CFragmentShader()
{
#ifdef GL_ARB_fragment_program
	if (m_bFragmentReady)
	{
		glStop();

		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		if (pExtensions->glIsProgramARB(m_handle.handle))
			pExtensions->glDeleteProgramsARB(1,&m_handle.handle);
	}
#endif
}

void CFragmentShader::glInitShaders()
{
    GLint maxLocals = 24;    //  implementation dependant, but at least 24
    GLint maxMats = 8;

#if defined(GL_ARB_fragment_program)
    const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
    pExtensions->glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,&maxLocals);
    glGetIntegerv(GL_MAX_PROGRAM_MATRICES_ARB,&maxMats);
#endif

    CATCH_GL_ERROR

	if (!m_bFragmentReady)
	{
		if (Raptor::glIsExtensionSupported(GL_ARB_FRAGMENT_PROGRAM_EXTENSION_NAME))
		{
#if defined(GL_ARB_fragment_program)
			const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
			m_bFragmentReady = pExtensions->glIsProgramARB != NULL;
#else
			m_bFragmentReady = false;
#endif
		}
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			CRaptorMessages::MessageArgument arg;
			arg.arg_sz = "ASM fragment";
			vector<CRaptorMessages::MessageArgument> args;
			args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(CShaderProgram::CShaderProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_NO_GPU_PROGRAM,
															args);
#endif
		}
	}
}


bool CFragmentShader::glGetProgramCaps(GL_FRAGMENT_SHADER_CAPS& caps)
{
#ifdef GL_ARB_fragment_program
	if (m_bFragmentReady)
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


void CFragmentShader::glRender(void)
{
	if (m_handle.handle == 0)
		return;

#ifdef GL_ARB_fragment_program
	if (m_bFragmentReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		if (pExtensions->glIsProgramARB(m_handle.handle))
		{
			glEnable(GL_FRAGMENT_PROGRAM_ARB);
			pExtensions->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,m_handle.handle);

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

void CFragmentShader::glStop(void)
{
#ifdef GL_ARB_fragment_program
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,0);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
#endif
}

bool CFragmentShader::glLoadProgram(const std::string &program)
{
    m_bValid = false;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#ifdef GL_ARB_fragment_program
	if (m_bFragmentReady)
	{
        //!    In case of a previous error, we need to initialize error checking
        //!    to be sure that the error detected will only be due to shader loading.
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            Raptor::GetErrorManager()->generateRaptorError(	CFragmentShader::CFragmentShaderClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															"Raptor encountered errors before loading vertex shader, check with debug infos.");
            while (err != GL_NO_ERROR)
                err = ::glGetError();
        }

		if (pExtensions->glIsProgramARB(m_handle.handle))
			pExtensions->glDeleteProgramsARB(1,&m_handle.handle);

		pExtensions->glGenProgramsARB(1,&m_handle.handle);
		pExtensions->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,m_handle.handle);

	 	pExtensions->glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,
										GL_PROGRAM_FORMAT_ASCII_ARB,
										program.size(),
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

            Raptor::GetErrorManager()->generateRaptorError(CFragmentShader::CFragmentShaderClassID::GetClassId(),
                                                           CRaptorErrorManager::RAPTOR_ERROR,
											               CRaptorMessages::ID_PROGRAM_ERROR,args); 
		}

        m_bValid = ((err == GL_NO_ERROR) && glGetProgramStatus());
        if (!m_bValid)
		{
			Raptor::GetErrorManager()->generateRaptorError(	CFragmentShader::CFragmentShaderClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_NO_GPU_PROGRAM);
			Raptor::GetErrorManager()->generateRaptorError(CFragmentShader::CFragmentShaderClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   getName().data());
		}

		// Unbind program to avoid side effects
		glStop();
	}
#endif

	CATCH_GL_ERROR

	return m_bValid;
}

bool CFragmentShader::glGetProgramStatus(void)
{
	if (m_handle.handle == 0)
		return false;

	if (!m_bFragmentReady)
		return false;

#if defined(GL_ARB_fragment_program)
const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (pExtensions->glIsProgramARB(m_handle.handle))
		pExtensions->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,m_handle.handle);
	else
		return false;

	GL_FRAGMENT_SHADER_CAPS caps;
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

void CFragmentShader::glProgramParameter(unsigned int numParam,const GL_COORD_VERTEX &v) const
{
#if defined(GL_ARB_fragment_program)
	if (m_bFragmentReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, numParam, v);
	}
#endif

	CATCH_GL_ERROR
}

void CFragmentShader::glProgramParameter(unsigned int numParam,const CColor::RGBA &v) const
{
#if defined(GL_ARB_fragment_program)
	if (m_bFragmentReady)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, numParam, v);
	}
#endif

	CATCH_GL_ERROR
}

