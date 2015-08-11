// FragmentProgram.cpp: implementation of the CFragmentProgram class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "FragmentProgram.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
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
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		if (m_handle.handle > 0)
			pExtensions->glDeleteObjectARB(m_handle.handle);
	}
#endif
}


void CFragmentProgram::glInitShaders()
{
    m_iMaxLocation = 0;

#if defined(GL_ARB_fragment_shader)
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB,&m_iMaxLocation);

    //  Raptor use vectors and the returned value above are single float components
    m_iMaxLocation = m_iMaxLocation / 4;
    m_locations.clear();
#endif

    CShaderProgram::glInitShaders();
}


void CFragmentProgram::glStop(void)
{
}

void CFragmentProgram::glRender(void)
{
    if (m_handle.handle == 0)
        return;

#if defined(GL_ARB_vertex_shader)
    if (m_bReLinked)
    {
        size_t nbParams = m_parameters.getNbParameters();
		m_locations.resize(nbParams);
		for (size_t i=0;i<nbParams;i++)
        {
            location_t l;
            l.locationIndex = -1;
            l.locationType = GL_FLOAT_VEC4_ARB;
            m_locations[i] = l;
        }

        const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
        GLhandleARB program = pExtensions->glGetHandleARB(GL_PROGRAM_OBJECT_ARB);

        if (program != 0)
        {
            glQueryUniformLocations(RAPTOR_HANDLE(0,(void*)program));

            glQueryAttributeLocations(RAPTOR_HANDLE(0,(void*)program));
            
            m_bReLinked = false;
        }
    #ifdef RAPTOR_DEBUG_MODE_GENERATION
        else
        {
            Raptor::GetErrorManager()->generateRaptorError(	CFragmentProgram::CFragmentProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_WRONG_RENDERING);
        }
    #endif
    }

    if (m_bApplyParameters)
    {
        const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
        for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
        {
            if (m_locations[idx].locationIndex >= 0)
            {
				CShaderProgram::CProgramParameters::PROGRAM_PARAMETER_VALUE &value = m_parameters[idx];
                if (value.kind == CShaderProgram::CProgramParameters::VECTOR)
                {
                    switch (m_locations[idx].locationType)
                    {
                        case GL_FLOAT:
                            pExtensions->glUniform1fvARB(m_locations[idx].locationIndex,1,
                                                         value.vector);
                            break;
                        case GL_FLOAT_VEC2_ARB:
                            pExtensions->glUniform2fvARB(m_locations[idx].locationIndex,1,
                                                         value.vector);
                            break;
                        case GL_FLOAT_VEC3_ARB:
                            pExtensions->glUniform3fvARB(m_locations[idx].locationIndex,1,
                                                         value.vector);
                            break;
                        case GL_FLOAT_VEC4_ARB:
                            pExtensions->glUniform4fvARB(m_locations[idx].locationIndex,1,
                                                         value.vector);
                            break;
                        case GL_INT:
                        {
                            int val = value.vector.x;
                            pExtensions->glUniform1iARB(m_locations[idx].locationIndex,val);
                            break;
                        }
                        case GL_INT_VEC2_ARB:
                        {
                            int val[2];
                            val[0] = value.vector.x;
                            val[1] = value.vector.y;
                            pExtensions->glUniform2iARB(m_locations[idx].locationIndex, val[0], val[1]);
                            break;
                        }
                        case GL_INT_VEC3_ARB:
                        {
                            int val[3];
                            val[0] = value.vector.x;
                            val[1] = value.vector.y;
                            val[2] = value.vector.z;
                            pExtensions->glUniform3ivARB(m_locations[idx].locationIndex,1, &val[0]);
                            break;
                        }
                        case GL_INT_VEC4_ARB:
                        {
                            int val[4];
                            val[0] = value.vector.x;
                            val[1] = value.vector.y;
                            val[2] = value.vector.z;
                            val[3] = value.vector.h;
                            pExtensions->glUniform4ivARB(m_locations[idx].locationIndex,1, &val[0]);
                            break;
                        }
                    }
                }
                else if (value.kind == CShaderProgram::CProgramParameters::SAMPLER)
                {
					pExtensions->glUniform1iARB(m_locations[idx].locationIndex, value.sampler);
                }
                else if (value.kind == CShaderProgram::CProgramParameters::MATRIX)
                {
					pExtensions->glUniformMatrix4fvARB(m_locations[idx].locationIndex,1,GL_TRUE,value.matrix);
                }
                else if (value.kind == CShaderProgram::CProgramParameters::ATTRIBUTE)
                {
                    // nothing to do : the attribute mapping is bound in glQueryAttributeLocations
                }
            }
        }

        m_bApplyParameters = false;
    }  
#endif

    CATCH_GL_ERROR
}



bool CFragmentProgram::glLoadProgram(const std::string &program)
{
    m_bValid = false;
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

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

bool CFragmentProgram::glBindProgram(RAPTOR_HANDLE program)
{
#if defined(GL_ARB_fragment_shader)
    if (program.handle == 0)
        return false;

    const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
    GLint value = 0;
    pExtensions->glGetObjectParameterivARB(program.handle, GL_OBJECT_TYPE_ARB,&value);
    if (value != GL_PROGRAM_OBJECT_ARB)
        return false;

    pExtensions->glAttachObjectARB(program.handle, m_handle.handle);

    CATCH_GL_ERROR

    m_bReLinked = true;

    return true;
#else
    return false;
#endif
}


bool CFragmentProgram::glUnbindProgram(RAPTOR_HANDLE program)
{
#if defined(GL_ARB_fragment_shader)
    if ((program.handle == 0) || (m_handle.handle == 0))
        return false;

    const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
    GLint value = 0;
    pExtensions->glGetObjectParameterivARB(program.handle, GL_OBJECT_TYPE_ARB,&value);
    if (value != GL_PROGRAM_OBJECT_ARB)
        return false;

    pExtensions->glDetachObjectARB(program.handle, m_handle.handle);

    CATCH_GL_ERROR

    return true;
#else
    return false;
#endif
}

bool CFragmentProgram::glGetProgramCaps(GL_FRAGMENT_PROGRAM_CAPS& caps)
{
	if (m_bFragmentProgramReady)
	{
#if defined(GL_ARB_fragment_shader)
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB,&caps.max_fragment_uniform_components);
        glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,&caps.max_texture_coords);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB,&caps.max_texture_image_units);

		return true;
#else
		return false;
#endif
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
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

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

