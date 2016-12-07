// VertexProgram.cpp: implementation of the CVertexProgram class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GEOMETRYPROGRAM_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
    #include "GeometryProgram.h"
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

static CGeometryProgram::CGeometryProgramClassID geometryId;
const CPersistence::CPersistenceClassID& CGeometryProgram::CGeometryProgramClassID::GetClassId(void)
{
	return geometryId;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeometryProgram::CGeometryProgram(const std::string& name)
	:CUnifiedProgram(geometryId,name)
{
    m_handle.handle = 0;	// default openGL vertex processing pipeline
	m_handle.hClass = CGeometryProgram::CGeometryProgramClassID::GetClassId().ID();

    glInitShaders();
}

CGeometryProgram::CGeometryProgram(const CGeometryProgram& shader)
	:CUnifiedProgram(shader)
{
}

CGeometryProgram* CGeometryProgram::glClone()
{
	return new CGeometryProgram(*this);
}

CGeometryProgram::~CGeometryProgram()
{
#if defined(GL_ARB_geometry_shader4)
	if (!m_bGeometryProgramReady)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	CGeometryProgram::CGeometryProgramClassID::GetClassId(),
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

void CGeometryProgram::glInitShaders()
{
	m_iMaxLocation = 0;

#if defined(GL_ARB_geometry_shader4)
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_ARB,&m_iMaxLocation);

    // ! The returned value is the number of individual component. Raptor always use vectors ...
    m_iMaxLocation = m_iMaxLocation / 4;
	m_locations.clear();
#endif

    CShaderProgram::glInitShaders();
}

void CGeometryProgram::glStop(void)
{
}

void CGeometryProgram::glRender(void)
{
    if (m_handle.handle == 0)
        return;

#if defined(GL_ARB_geometry_shader4)
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
            Raptor::GetErrorManager()->generateRaptorError(	CGeometryProgram::CGeometryProgramClassID::GetClassId(),
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
				CProgramParameters::PROGRAM_PARAMETER_VALUE& value = m_parameters[idx];
                switch(value.kind)
                {
                    case CShaderProgram::CProgramParameters::VECTOR:
                        pExtensions->glUniform4fvARB(	m_locations[idx].locationIndex,1,
														value.vector);
                        break;
                    case CShaderProgram::CProgramParameters::MATRIX:
                        pExtensions->glUniformMatrix4fvARB(	m_locations[idx].locationIndex,1,GL_TRUE,
															value.matrix);
                        break;
                    case CShaderProgram::CProgramParameters::ATTRIBUTE:
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


bool CGeometryProgram::glLoadProgram(const std::string &program)
{
    m_bValid = false;
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_ARB_geometry_shader4)
	if (m_bGeometryProgramReady)
	{
		if (m_handle.handle > 0)
			pExtensions->glDeleteObjectARB(m_handle.handle);

        m_handle.handle = pExtensions->glCreateShaderObjectARB(GL_GEOMETRY_SHADER_ARB);
        if (m_handle.handle == 0)
        {
            Raptor::GetErrorManager()->generateRaptorError(	CGeometryProgram::CGeometryProgramClassID::GetClassId(),
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
            Raptor::GetErrorManager()->generateRaptorError(	CGeometryProgram::CGeometryProgramClassID::GetClassId(),
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

bool CGeometryProgram::glBindProgram(RAPTOR_HANDLE program)
{
#if defined(GL_ARB_geometry_shader4)
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


bool CGeometryProgram::glUnbindProgram(RAPTOR_HANDLE program)
{
#if defined(GL_ARB_geometry_shader4)
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

bool CGeometryProgram::glGetProgramCaps(GL_GEOMETRY_PROGRAM_CAPS& caps)
{
	if (m_bGeometryProgramReady)
	{
#if defined(GL_ARB_geometry_shader4)
		glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_ARB,&caps.max_geometry_texture_image_units);
		glGetIntegerv(GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB,&caps.max_geometry_varying_components);
		glGetIntegerv(GL_MAX_VERTEX_VARYING_COMPONENTS_ARB,&caps.max_vertex_varying_components);
		glGetIntegerv(GL_MAX_VARYING_COMPONENTS,&caps.max_varying_components);
		glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_ARB,&caps.max_geometry_uniform_components);
		glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB,&caps.max_geometry_output_vertices);
		glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_ARB,&caps.max_geometry_total_output_components);
#endif
#if defined(GL_ARB_uniform_buffer_object)
		glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS_ARB, &caps.max_geometry_uniform_blocks);
#endif
		return true;
	}
	else
		return false;
}

bool CGeometryProgram::glGetProgramStatus(void)
{
	if (m_handle.handle == 0)
		return false;

	if (!m_bGeometryProgramReady)
		return false;

#if defined(GL_ARB_geometry_shader4)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	GL_GEOMETRY_PROGRAM_CAPS caps;
	if (glGetProgramCaps(caps))
	{
        //  Check program status and compare to shader caps to return global status
        GLint value = 0;
        pExtensions->glGetObjectParameterivARB(m_handle.handle, GL_OBJECT_TYPE_ARB,&value);
        if (value != GL_SHADER_OBJECT_ARB)
            return false;

        pExtensions->glGetObjectParameterivARB(m_handle.handle, GL_OBJECT_SUBTYPE_ARB,&value);
        if (value != GL_GEOMETRY_SHADER_ARB)
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

