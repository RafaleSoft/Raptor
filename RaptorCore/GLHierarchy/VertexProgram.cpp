// VertexProgram.cpp: implementation of the CVertexProgram class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
    #include "VertexProgram.h"
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

bool CVertexProgram::m_bVertexProgramReady = false;
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
    m_handle.handle(0);	// default openGL vertex processing pipeline
	m_handle.hClass(CVertexProgram::CVertexProgramClassID::GetClassId().ID());

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
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		if (m_handle.handle() > 0)
			pExtensions->glDeleteObjectARB(m_handle.handle());
	}
#endif
}

void CVertexProgram::glInitShaders()
{
	m_parameters.clear();

	if (!m_bVertexProgramReady)
	{
		if (Raptor::glIsExtensionSupported(GL_ARB_VERTEX_SHADER_EXTENSION_NAME))
		{
#if defined(GL_ARB_vertex_shader)
			const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
			m_bVertexProgramReady = (NULL != pExtensions->glCreateShaderObjectARB);
#else
			m_bVertexProgramReady = false;
#endif
		}
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			CRaptorMessages::MessageArgument arg;
			arg.arg_sz = "GLSL vertex";
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

bool CVertexProgram::glLoadProgram(const std::string &program)
{
    m_bValid = false;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_ARB_vertex_shader)
	if (m_bVertexProgramReady)
	{
		if (m_handle.handle() > 0)
			pExtensions->glDeleteObjectARB(m_handle.handle());

        m_handle.handle(pExtensions->glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB));
        if (m_handle.handle() == 0)
        {
            Raptor::GetErrorManager()->generateRaptorError(	CVertexProgram::CVertexProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_NO_GPU_PROGRAM);
            return false;
        }

        int length = program.size();
        const char* source = program.data();
        pExtensions->glShaderSourceARB(m_handle.handle(),1,&source,&length);

        pExtensions->glCompileShaderARB(m_handle.handle());

        m_bValid = glGetProgramStatus();

	    if (!m_bValid) 
        {
            GLint maxLength = 0;
	        pExtensions->glGetObjectParameterivARB(m_handle.handle(),GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
	        char *pInfoLog = (char*) malloc(maxLength * sizeof(char));
	        pExtensions->glGetInfoLogARB(m_handle.handle(), maxLength, &length, pInfoLog);

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
#if defined(GL_ARB_shader_objects)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (CUnifiedProgram::glBindProgram(program))
	{
		for (unsigned int idx = 0; idx < m_parameters.getNbParameters(); idx++)
		{
			const CProgramParameters::CParameterBase& value = m_parameters[idx];
			CProgramParameters::GL_VERTEX_ATTRIB p;
			if (value.isA(p))
			{
				p = ((const CProgramParameters::CParameter<CProgramParameters::GL_VERTEX_ATTRIB>&)value).p;
				pExtensions->glBindAttribLocationARB(program.handle(), p, value.name().data());
			}
		}

		CATCH_GL_ERROR
		return true;
	}
	else
#endif
		return false;
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
	if (m_handle.handle() == 0)
		return false;

	if (!m_bVertexProgramReady)
		return false;

#if defined(GL_ARB_vertex_shader)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	GL_VERTEX_PROGRAM_CAPS caps;
	if (glGetProgramCaps(caps))
	{
        //  Check program status and compare to shader caps to return global status
        GLint value = 0;
        pExtensions->glGetObjectParameterivARB(m_handle.handle(), GL_OBJECT_TYPE_ARB,&value);
        if (value != GL_SHADER_OBJECT_ARB)
            return false;

        pExtensions->glGetObjectParameterivARB(m_handle.handle(), GL_OBJECT_SUBTYPE_ARB,&value);
        if (value != GL_VERTEX_SHADER_ARB)
            return false;

        pExtensions->glGetObjectParameterivARB(m_handle.handle(),GL_OBJECT_COMPILE_STATUS_ARB, &value);
        if (value == 0)
            return false;

        pExtensions->glGetObjectParameterivARB(m_handle.handle(),GL_OBJECT_DELETE_STATUS_ARB, &value);
        if (value == 1)
            return false;

		return true;
	}
	else
#endif
		return false;
}

