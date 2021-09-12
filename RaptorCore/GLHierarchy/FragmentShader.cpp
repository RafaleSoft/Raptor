/***************************************************************************/
/*                                                                         */
/*  FragmentShader.cpp                                                     */
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

#if !defined(AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
    #include "FragmentShader.h"
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

IMPLEMENT_CLASS_ID(CFragmentShader, fragmentId)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFragmentShader::CFragmentShader(const std::string& name)
	:CUnifiedShader(fragmentId, name)
{
	m_handle.handle(0);	// default openGL vertex processing pipeline
	m_handle.hClass(CFragmentShader::CFragmentShaderClassID::GetClassId().ID());
}


CFragmentShader::CFragmentShader(const CFragmentShader& shader)
	:CUnifiedShader(shader)
{
}

CFragmentShader* CFragmentShader::glClone()
{
	return new CFragmentShader(*this);
}

CFragmentShader::~CFragmentShader()
{

	if (!CRaptorInstance::GetInstance().isFragmentShaderReady())
	{
		IRaptor::GetErrorManager()->generateRaptorError(	fragmentId,
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_NO_GPU_PROGRAM);
	}
}

bool CFragmentShader::glLoadProgram(const std::string &program)
{
	bool load = false;

#if defined(GL_VERSION_2_0)
	load = CUnifiedShader::glLoadProgram(	program, 
											GL_FRAGMENT_SHADER, 
											fragmentId);
#elif defined(GL_ARB_fragment_shader)
	load = CUnifiedShader::glLoadProgram(	program, 
											GL_FRAGMENT_SHADER_ARB, 
											fragmentId);
#endif

    return load;
}

bool CFragmentShader::glBindProgram(RAPTOR_HANDLE program)
{
	const CRaptorGLExtensions *const pExtensions = IRaptor::glGetExtensions();
	GLint value = 0;

#if defined(GL_VERSION_2_0)
	pExtensions->glGetShaderiv(m_handle.glhandle(), GL_SHADER_TYPE, &value);
	if ((!m_bValid) || (value != GL_FRAGMENT_SHADER))
#elif defined(GL_ARB_fragment_shader)
	pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_SUBTYPE_ARB, &value);
	if (value != GL_FRAGMENT_SHADER_ARB)
#endif
	{
		IRaptor::GetErrorManager()->generateRaptorError(fragmentId,
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "Fragment Shader is invalid in this context",
													   __FILE__, __LINE__);
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

bool CFragmentShader::glGetShaderCaps(GL_FRAGMENT_SHADER_CAPS& caps)
{
	if (CRaptorInstance::GetInstance().isFragmentShaderReady())
	{
#if defined(GL_VERSION_2_0)
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &caps.max_fragment_uniform_components);
		glGetIntegerv(GL_MAX_TEXTURE_COORDS, &caps.max_texture_coords);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &caps.max_texture_image_units);
#elif defined(GL_ARB_fragment_shader)
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB,&caps.max_fragment_uniform_components);
        glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,&caps.max_texture_coords);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB,&caps.max_texture_image_units);
#endif

#if defined(GL_VERSION_3_1)
		glGetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &caps.max_combined_fragment_uniform_components);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &caps.max_uniform_block_size);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &caps.max_fragment_uniform_blocks);
#elif defined(GL_ARB_uniform_buffer_object)
		glGetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS_ARB, &caps.max_combined_fragment_uniform_components);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE_ARB, &caps.max_uniform_block_size);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS_ARB, &caps.max_fragment_uniform_blocks);
#endif
		return true;
	}
	else
		return false;
}

bool CFragmentShader::glGetProgramStatus(void) const
{
	if (m_handle.glhandle() == 0)
		return false;

	if (!CRaptorInstance::GetInstance().isFragmentShaderReady())
		return false;

	const CRaptorGLExtensions *const pExtensions = IRaptor::glGetExtensions();

	GL_FRAGMENT_SHADER_CAPS caps;
	if (glGetShaderCaps(caps))
	{
        //  Check program status and compare to shader caps to return global status
        GLint value = 0;

#if defined(GL_VERSION_2_0)
		if (!pExtensions->glIsShader(m_handle.glhandle()))
			return false;

		pExtensions->glGetShaderiv(m_handle.glhandle(), GL_SHADER_TYPE, &value);
		if (value != GL_FRAGMENT_SHADER)
			return false;

		pExtensions->glGetShaderiv(m_handle.glhandle(), GL_COMPILE_STATUS, &value);
		if (value == 0)
			return false;

		pExtensions->glGetShaderiv(m_handle.glhandle(), GL_DELETE_STATUS, &value);
		if (value == 1)
			return false;
#elif defined(GL_ARB_fragment_shader)
		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_TYPE_ARB, &value);
        if (value != GL_SHADER_OBJECT_ARB)
            return false;

		pExtensions->glGetObjectParameterivARB(m_handle.glhandle(), GL_OBJECT_SUBTYPE_ARB, &value);
        if (value != GL_FRAGMENT_SHADER_ARB)
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

