/***************************************************************************/
/*                                                                         */
/*  VertexShader.cpp                                                       */
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

#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
    #include "GLHierarchy/VertexShader.h"
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

IMPLEMENT_CLASS_ID(CVertexShader, vertexId)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVertexShader::CVertexShader(const std::string& name)
    :CUnifiedShader(vertexId,name)
{
    m_handle.handle(0);	// default openGL vertex processing pipeline
	m_handle.hClass(CVertexShader::CVertexShaderClassID::GetClassId().ID());
}

CVertexShader::CVertexShader(const CVertexShader& shader)
	:CUnifiedShader(shader)
{
}

CVertexShader* CVertexShader::glClone(void)
{
	return new CVertexShader(*this);
}

CVertexShader::~CVertexShader()
{
	if (!CRaptorInstance::GetInstance().isVertexShaderReady())
	{
        Raptor::GetErrorManager()->generateRaptorError(	CVertexShader::CVertexShaderClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
	}
}

bool CVertexShader::glLoadProgram(const std::string &program)
{
	bool load = false;

#if defined(GL_VERSION_2_0)
	load = CUnifiedShader::glLoadProgram(	program,
											GL_VERTEX_SHADER,
											CVertexShader::CVertexShaderClassID::GetClassId());
#elif defined(GL_ARB_fragment_shader)
	load = CUnifiedShader::glLoadProgram(	program,
											GL_VERTEX_SHADER_ARB,
											CVertexShader::CVertexShaderClassID::GetClassId());
#endif

	return load;
}

bool CVertexShader::glBindProgram(RAPTOR_HANDLE program)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	GLint value = 0;

#if defined(GL_VERSION_2_0)
	pExtensions->glGetShaderiv(m_handle.glhandle(), GL_SHADER_TYPE, &value);
	if ((!m_bValid) || (value != GL_VERTEX_SHADER))
#elif defined(GL_ARB_vertex_shader)
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_SUBTYPE_ARB, &value);
	if ((!m_bValid) || (value != GL_VERTEX_SHADER_ARB))
#endif
	{
		Raptor::GetErrorManager()->generateRaptorError(CVertexShader::CVertexShaderClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "Vertex Shader is invalid in this context");
		CATCH_GL_ERROR
		return false;
	}

	if (CUnifiedShader::glBindProgram(program))
	{
		CATCH_GL_ERROR
		return true;
	}
	else
		return false;
}


bool CVertexShader::glGetShaderCaps(GL_VERTEX_SHADER_CAPS& caps)
{
	if (CRaptorInstance::GetInstance().isVertexShaderReady())
	{
#if defined(GL_VERSION_2_0)
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &caps.max_vertex_uniform_components);
		glGetIntegerv(GL_MAX_VARYING_FLOATS, &caps.max_varying_floats);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &caps.max_vertex_attribs);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &caps.max_texture_image_units);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &caps.max_vertex_texture_image_units);
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &caps.max_combined_texture_image_units);
		glGetIntegerv(GL_MAX_TEXTURE_COORDS, &caps.max_texture_coords);
		GLboolean bVal = GL_FALSE;
		glGetBooleanv(GL_VERTEX_PROGRAM_POINT_SIZE, &bVal);
		caps.vertex_program_point_size = (bVal == GL_TRUE);
		glGetBooleanv(GL_VERTEX_PROGRAM_TWO_SIDE, &bVal);
		caps.vertex_program_two_side = (bVal == GL_TRUE);
#elif defined(GL_ARB_vertex_shader)
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

#if defined(GL_VERSION_3_1)
		glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &caps.max_combined_vertex_uniform_components);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &caps.max_uniform_block_size);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &caps.max_vertex_uniform_blocks);
#elif defined(GL_ARB_uniform_buffer_object)
		glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS_ARB, &caps.max_combined_vertex_uniform_components);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE_ARB, &caps.max_uniform_block_size);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS_ARB, &caps.max_vertex_uniform_blocks);
#endif
		return true;
	}
	else
		return false;
}

bool CVertexShader::glGetProgramStatus(void) const
{
	if (m_handle.glhandle() == 0)
		return false;

	if (!CRaptorInstance::GetInstance().isVertexShaderReady())
		return false;


	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	GL_VERTEX_SHADER_CAPS caps;
	if (glGetShaderCaps(caps))
	{
        //  Check program status and compare to shader caps to return global status
        GLint value = 0;

#if defined(GL_VERSION_2_0)
		if (!pExtensions->glIsShader(m_handle.glhandle()))
			return false;

		pExtensions->glGetShaderiv(m_handle.glhandle(), GL_SHADER_TYPE, &value);
		if (value != GL_VERTEX_SHADER)
			return false;

		pExtensions->glGetShaderiv(m_handle.glhandle(), GL_COMPILE_STATUS, &value);
		if (value == 0)
			return false;

		pExtensions->glGetShaderiv(m_handle.glhandle(), GL_DELETE_STATUS, &value);
		if (value == 1)
			return false;
#elif defined(GL_ARB_vertex_shader)
		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_TYPE_ARB, &value);
        if (value != GL_SHADER_OBJECT_ARB)
            return false;

		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_SUBTYPE_ARB, &value);
        if (value != GL_VERTEX_SHADER_ARB)
            return false;

		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_COMPILE_STATUS_ARB, &value);
        if (value == 0)
            return false;

		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_DELETE_STATUS_ARB, &value);
        if (value == 1)
            return false;
#endif
		return true;
	}
	else
		return false;
}

