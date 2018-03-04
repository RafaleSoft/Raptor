// FragmentProgram.cpp: implementation of the CFragmentProgram class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "FragmentProgram.h"
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

#include <stdlib.h>

RAPTOR_NAMESPACE

static const int MAX_UNIFORM_NAME_LENGTH = 256;
bool CFragmentProgram::m_bFragmentProgramReady = false;
static CFragmentProgram::CFragmentProgramClassID fragmentId;
const CPersistence::CPersistenceClassID& CFragmentProgram::CFragmentProgramClassID::GetClassId(void)
{
	return fragmentId;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFragmentProgram::CFragmentProgram(const std::string& name):
    CUnifiedProgram(fragmentId,name)
{
	m_handle.handle = 0;	// default openGL vertex processing pipeline
	m_handle.hClass = CFragmentProgram::CFragmentProgramClassID::GetClassId().ID();

    glInitShaders();
}


CFragmentProgram::CFragmentProgram(const CFragmentProgram& shader)
	:CUnifiedProgram(shader)
{
}

CFragmentProgram* CFragmentProgram::glClone()
{
	return new CFragmentProgram(*this);
}

CFragmentProgram::~CFragmentProgram()
{
#if defined(GL_ARB_fragment_shader)
	if (!m_bFragmentProgramReady)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	CFragmentProgram::CFragmentProgramClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
#endif
	}
	else
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		if (m_handle.handle > 0)
			pExtensions->glDeleteObjectARB(m_handle.handle);
	}
#endif
}


void CFragmentProgram::glInitShaders()
{
	m_parameters.clear();

	if (!m_bFragmentProgramReady)
	{
		if (Raptor::glIsExtensionSupported("GL_ARB_fragment_shader"))
		{
#if defined(GL_ARB_fragment_shader)
			const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
			m_bFragmentProgramReady = (NULL != pExtensions->glCreateShaderObjectARB);
#else
			m_bFragmentProgramReady = false;
#endif
		}
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			CRaptorMessages::MessageArgument arg;
			arg.arg_sz = "GLSL fragment";
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

bool CFragmentProgram::glLoadProgram(const std::string &program)
{
    m_bValid = false;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_ARB_fragment_shader)
	if (m_bFragmentProgramReady)
	{
		if (m_handle.handle > 0)
			pExtensions->glDeleteObjectARB(m_handle.handle);

        m_handle.handle = pExtensions->glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        if (m_handle.handle == 0)
        {
            Raptor::GetErrorManager()->generateRaptorError(	CFragmentProgram::CFragmentProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_NO_GPU_PROGRAM);
            return false;
        }

        int length = program.size();
        const char* source = program.data();
        pExtensions->glShaderSourceARB(m_handle.handle,1,&source,&length);

        pExtensions->glCompileShaderARB(m_handle.handle);

        m_bValid = glGetProgramStatus();

        if (!m_bValid) 
        {
            GLint maxLength = 0;
	        pExtensions->glGetObjectParameterivARB(m_handle.handle,GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
	        char *pInfoLog = (char*) malloc(maxLength * sizeof(char));
	        pExtensions->glGetInfoLogARB(m_handle.handle, maxLength, &length, pInfoLog);

            CRaptorMessages::MessageArgument arg;
            arg.arg_sz = pInfoLog;
            vector<CRaptorMessages::MessageArgument> args;
            args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(	CFragmentProgram::CFragmentProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_PROGRAM_ERROR,args);
            free(pInfoLog);
	        return false;
	    }
	}
#endif

    CATCH_GL_ERROR

    return m_bValid;
}

bool CFragmentProgram::glGetProgramCaps(GL_FRAGMENT_PROGRAM_CAPS& caps)
{
	if (m_bFragmentProgramReady)
	{
#if defined(GL_ARB_fragment_shader)
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB,&caps.max_fragment_uniform_components);
        glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,&caps.max_texture_coords);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB,&caps.max_texture_image_units);
#endif
#if defined(GL_ARB_uniform_buffer_object)
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS_ARB, &caps.max_fragment_uniform_blocks);
#endif
		return true;
	}
	else
		return false;
}

bool CFragmentProgram::glGetProgramStatus(void)
{
	if (m_handle.handle == 0)
		return false;

	if (!m_bFragmentProgramReady)
		return false;

#if defined(GL_ARB_fragment_shader)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	GL_FRAGMENT_PROGRAM_CAPS caps;
	if (glGetProgramCaps(caps))
	{
        //  Check program status and compare to shader caps to return global status
        GLint value = 0;
        pExtensions->glGetObjectParameterivARB(m_handle.handle, GL_OBJECT_TYPE_ARB,&value);
        if (value != GL_SHADER_OBJECT_ARB)
            return false;

        pExtensions->glGetObjectParameterivARB(m_handle.handle, GL_OBJECT_SUBTYPE_ARB,&value);
        if (value != GL_FRAGMENT_SHADER_ARB)
            return false;

        pExtensions->glGetObjectParameterivARB(m_handle.handle,GL_OBJECT_COMPILE_STATUS_ARB, &value);
        if (value == 0)
            return false;

        pExtensions->glGetObjectParameterivARB(m_handle.handle,GL_OBJECT_DELETE_STATUS_ARB, &value);
        if (value == 1)
            return false;

        return true;
	}
	else
#endif
		return false;
}

