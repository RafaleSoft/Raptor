// VertexProgram.cpp: implementation of the CVertexProgram class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
    #include "VertexProgram.h"
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

#include <stdlib.h>	//	malloc
RAPTOR_NAMESPACE

static CVertexProgram::CVertexProgramClassID vertexId;
const CPersistence::CPersistenceClassID& CVertexProgram::CVertexProgramClassID::GetClassId(void)
{
	return vertexId;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVertexProgram::CVertexProgram(const std::string& name):
    CUnifiedProgram(vertexId,name)
{
    m_handle.handle = 0;	// default openGL vertex processing pipeline
	m_handle.hClass = CVertexProgram::CVertexProgramClassID::GetClassId().ID();

    glInitShaders();
}

CVertexProgram::CVertexProgram(const CVertexProgram& shader)
	:CUnifiedProgram(shader)
{
}

CVertexProgram* CVertexProgram::glClone(void)
{
	return new CVertexProgram(*this);
}

CVertexProgram::~CVertexProgram()
{
#if defined(GL_ARB_vertex_shader)
	if (!m_bVertexProgramReady)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	CVertexProgram::CVertexProgramClassID::GetClassId(),
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

void CVertexProgram::glInitShaders()
{
	m_iMaxLocation = 0;

#if defined(GL_ARB_vertex_shader)
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB,&m_iMaxLocation);

    //! The returned value is the number of individual component.
	//! Raptor always use vectors ...
    m_iMaxLocation = m_iMaxLocation / 4;
	//m_locations.clear();
	m_parameters.clear();
#endif

    CShaderProgram::glInitShaders();
}

void CVertexProgram::glStop(void)
{
}

void CVertexProgram::glRender(void)
{
    if (m_handle.handle == 0)
        return;

#if defined(GL_ARB_vertex_shader)
    if (m_bReLinked)
    {
		size_t nbParams = m_parameters.getNbParameters();
		for (size_t i=0;i<nbParams;i++)
        {
			CProgramParameters::PROGRAM_PARAMETER_VALUE &pValue = m_parameters[i];
			pValue.locationIndex = -1;
			pValue.locationType = GL_FLOAT_VEC4_ARB;
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
            Raptor::GetErrorManager()->generateRaptorError(	CVertexProgram::CVertexProgramClassID::GetClassId(),
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
			CProgramParameters::PROGRAM_PARAMETER_VALUE& value = m_parameters[idx];
			if (value.locationIndex >= 0)
            {
                switch(value.kind)
                {
                    case CProgramParameters::VECTOR:
						pExtensions->glUniform4fvARB(	value.locationIndex,
														1,value.vector);
                        break;
                    case CProgramParameters::MATRIX:
						pExtensions->glUniformMatrix4fvARB(	value.locationIndex,
															1,GL_TRUE,value.matrix);
                        break;
                    case CProgramParameters::ATTRIBUTE:
                        break;
                    default:
                        break;
                }
            }
        }

        m_bApplyParameters = false;
    } 
#endif

    CATCH_GL_ERROR
}


bool CVertexProgram::glLoadProgram(const std::string &program)
{
    m_bValid = false;
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_ARB_vertex_shader)
	if (m_bVertexProgramReady)
	{
		if (m_handle.handle > 0)
			pExtensions->glDeleteObjectARB(m_handle.handle);

        m_handle.handle = pExtensions->glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        if (m_handle.handle == 0)
        {
            Raptor::GetErrorManager()->generateRaptorError(	CVertexProgram::CVertexProgramClassID::GetClassId(),
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
            Raptor::GetErrorManager()->generateRaptorError(	CVertexProgram::CVertexProgramClassID::GetClassId(),
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

bool CVertexProgram::glBindProgram(RAPTOR_HANDLE program)
{
#if defined(GL_ARB_vertex_shader)
    if (program.handle == 0)
        return false;

    const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
    GLint value = 0;
    pExtensions->glGetObjectParameterivARB(program.handle, GL_OBJECT_TYPE_ARB,&value);
    if (value != GL_PROGRAM_OBJECT_ARB)
        return false;

    pExtensions->glAttachObjectARB(program.handle, m_handle.handle);

	for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
    {
        CProgramParameters::PROGRAM_PARAMETER_VALUE& pValue = m_parameters[idx];
        if (pValue.kind == CProgramParameters::ATTRIBUTE)
        {
            // the location retrieved will only be used if the user value is invalid.
			pExtensions->glBindAttribLocationARB(program.handle,pValue.attribute,pValue.name.data());
        }
    }

    CATCH_GL_ERROR

    m_bReLinked = true;

    return true;
#else
    return false;
#endif
}


bool CVertexProgram::glUnbindProgram(RAPTOR_HANDLE program)
{
#if defined(GL_ARB_vertex_shader)
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

bool CVertexProgram::glGetProgramCaps(GL_VERTEX_PROGRAM_CAPS& caps)
{
	if (m_bVertexProgramReady)
	{
#if defined(GL_ARB_vertex_shader)
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB,&caps.max_vertex_uniform_components);
		glGetIntegerv(GL_MAX_VARYING_FLOATS_ARB,&caps.max_varying_floats);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB,&caps.max_vertex_attribs);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB,&caps.max_texture_image_units);
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB,&caps.max_vertex_texture_image_units);
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB,&caps.max_combined_texture_image_units);
        glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,&caps.max_texture_coords);
        GLboolean bVal = GL_FALSE;
        glGetBooleanv(GL_VERTEX_PROGRAM_POINT_SIZE_ARB,&bVal);
        caps.vertex_program_point_size = (bVal == GL_TRUE);
        glGetBooleanv(GL_VERTEX_PROGRAM_TWO_SIDE_ARB,&bVal);
        caps.vertex_program_two_side = (bVal == GL_TRUE);
#endif
#if defined(GL_ARB_uniform_buffer_object)
		glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS_ARB, &caps.max_combined_vertex_uniform_components);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE_ARB, &caps.max_uniform_block_size);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS_ARB, &caps.max_vertex_uniform_blocks);
#endif
		return true;
	}
	else
		return false;
}

bool CVertexProgram::glGetProgramStatus(void)
{
	if (m_handle.handle == 0)
		return false;

	if (!m_bVertexProgramReady)
		return false;

#if defined(GL_ARB_vertex_shader)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	GL_VERTEX_PROGRAM_CAPS caps;
	if (glGetProgramCaps(caps))
	{
        //  Check program status and compare to shader caps to return global status
        GLint value = 0;
        pExtensions->glGetObjectParameterivARB(m_handle.handle, GL_OBJECT_TYPE_ARB,&value);
        if (value != GL_SHADER_OBJECT_ARB)
            return false;

        pExtensions->glGetObjectParameterivARB(m_handle.handle, GL_OBJECT_SUBTYPE_ARB,&value);
        if (value != GL_VERTEX_SHADER_ARB)
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

