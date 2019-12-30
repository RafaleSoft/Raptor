/***************************************************************************/
/*                                                                         */
/*  GeometryShader.cpp                                                     */
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

#if !defined(AFX_GEOMETRYSHADER_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
    #include "GeometryShader.h"
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
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


RAPTOR_NAMESPACE

IMPLEMENT_CLASS_ID(CGeometryShader, geometryId)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeometryShader::CGeometryShader(const std::string& name)
	:CUnifiedShader(geometryId, name), m_inputType(0), m_outputType(0), m_verticesOut(0)
{
    m_handle.handle(0);	// default openGL vertex processing pipeline
	m_handle.hClass(CGeometryShader::CGeometryShaderClassID::GetClassId().ID());
}

CGeometryShader::CGeometryShader(const CGeometryShader& shader)
	:CUnifiedShader(shader)
{
}

CGeometryShader* CGeometryShader::glClone()
{
	return new CGeometryShader(*this);
}

CGeometryShader::~CGeometryShader()
{
#if defined(GL_ARB_geometry_shader4)
	if (!CRaptorInstance::GetInstance().isGeometryShaderReady())
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	CGeometryShader::CGeometryShaderClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
#endif
	}
	else
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		if (m_handle.glhandle() > 0)
			pExtensions->glDeleteObjectARB(m_handle.glhandle());
	}
#endif
}

bool CGeometryShader::setGeometry(uint32_t inputType, uint32_t outputType, uint32_t verticesOut)
{
	if ((inputType == GL_POINTS) ||
		(inputType == GL_LINES) ||
#if defined(GL_ARB_geometry_shader4)
		(inputType == GL_LINES_ADJACENCY_ARB) ||
		(inputType == GL_TRIANGLES_ADJACENCY_ARB) ||
#endif
		(inputType == GL_TRIANGLES))
		m_inputType = inputType; 
	else
		return false;

	if ((outputType == GL_POINTS) ||
		(outputType == GL_LINE_STRIP) ||
		(outputType == GL_TRIANGLE_STRIP))
		m_outputType = outputType; 
	else
		return false;
	
	if (verticesOut != 0)
		m_verticesOut = verticesOut;
	else
		return false;

	return true;
}

bool CGeometryShader::glLoadProgram(const std::string &program)
{
    m_bValid = false;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_ARB_geometry_shader4)
	if (CRaptorInstance::GetInstance().isGeometryShaderReady())
	{
		if (m_handle.glhandle() > 0)
			pExtensions->glDeleteObjectARB(m_handle.glhandle());

        m_handle.handle(pExtensions->glCreateShaderObjectARB(GL_GEOMETRY_SHADER_ARB));
		if (m_handle.glhandle() == 0)
        {
			RAPTOR_WARNING(CGeometryShader::CGeometryShaderClassID::GetClassId(),
						   CRaptorMessages::ID_NO_GPU_PROGRAM)
            return false;
        }

        int length = program.size();
        const char* source = program.data();
		pExtensions->glShaderSourceARB(m_handle.glhandle(), 1, &source, &length);

		pExtensions->glCompileShaderARB(m_handle.glhandle());

        m_bValid = glGetProgramStatus();

	    if (!m_bValid) 
        {
            GLint maxLength = 0;
			pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
	        char *pInfoLog = (char*) malloc(maxLength * sizeof(char));
			pExtensions->glGetInfoLogARB(m_handle.glhandle(), maxLength, &length, pInfoLog);

            CRaptorMessages::MessageArgument arg;
            arg.arg_sz = pInfoLog;
            vector<CRaptorMessages::MessageArgument> args;
            args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(	CGeometryShader::CGeometryShaderClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															CRaptorMessages::ID_PROGRAM_ERROR,
															__FILE__, __LINE__, args);
            free(pInfoLog);
	        return false;
	    }
	}
#endif

    CATCH_GL_ERROR

    return m_bValid;
}

bool CGeometryShader::glBindProgram(RAPTOR_HANDLE program)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_ARB_geometry_shader4)
	GLint value = 0;
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_SUBTYPE_ARB, &value);
	if (value != GL_GEOMETRY_SHADER_ARB)
	{
		Raptor::GetErrorManager()->generateRaptorError(CGeometryShader::CGeometryShaderClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "Geometry Program is invalid in this context");

		CATCH_GL_ERROR
		return false;
	}
#endif

#if defined(GL_ARB_shader_objects)
	if (CUnifiedShader::glBindProgram(program))
	{
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
	// Since OpenGL 3.2, geometry parameters shall be defined with a layout in shader source.
#if defined(GL_VERSION_3_2)
		Raptor::GetErrorManager()->generateRaptorError(CGeometryShader::CGeometryShaderClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "Geometry Program types shall be defined in a geometry shader");

#else
	#if defined(GL_ARB_geometry_shader4)
		pExtensions->glProgramParameteriARB(program.handle(), GL_GEOMETRY_INPUT_TYPE_ARB, m_inputType);
		pExtensions->glProgramParameteriARB(program.handle(), GL_GEOMETRY_OUTPUT_TYPE_ARB, m_outputType);
		pExtensions->glProgramParameteriARB(program.handle(), GL_GEOMETRY_VERTICES_OUT_ARB, m_verticesOut);
	#endif
#endif
#endif

		CATCH_GL_ERROR
		return true;
	}
	else
#endif
	   return false;
}


bool CGeometryShader::glGetShaderCaps(GL_GEOMETRY_SHADER_CAPS& caps)
{
	if (CRaptorInstance::GetInstance().isGeometryShaderReady())
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

bool CGeometryShader::glGetProgramStatus(void) const
{
	if (m_handle.glhandle() == 0)
		return false;

	if (!CRaptorInstance::GetInstance().isGeometryShaderReady())
		return false;

#if defined(GL_ARB_geometry_shader4)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	GL_GEOMETRY_SHADER_CAPS caps;
	if (glGetShaderCaps(caps))
	{
        //  Check program status and compare to shader caps to return global status
        GLint value = 0;
		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_TYPE_ARB, &value);
        if (value != GL_SHADER_OBJECT_ARB)
            return false;

		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_SUBTYPE_ARB, &value);
        if (value != GL_GEOMETRY_SHADER_ARB)
            return false;

		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_COMPILE_STATUS_ARB, &value);
        if (value == 0)
            return false;

		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_DELETE_STATUS_ARB, &value);
        if (value == 1)
            return false;

		return true;
	}
	else
#endif
		return false;
}

