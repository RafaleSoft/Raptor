/***************************************************************************/
/*                                                                         */
/*  FragmentShader.h                                                       */
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


#if !defined(AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
#define AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_UNIFIEDSHADER_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)
	#include "UnifiedShader.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//!
//! This class handles OpenGL2.0 fragment shaders.
//!
class RAPTOR_API CFragmentShader : public CUnifiedShader  
{
public:
    typedef struct GL_FRAGMENT_SHADER_CAPS_t
	{
        int		max_fragment_uniform_components;
        int		max_texture_coords;
		int		max_texture_image_units;
		int		max_fragment_uniform_blocks;
	} GL_FRAGMENT_SHADER_CAPS;

public:
	//!	Default Constructor.
	CFragmentShader(const std::string& name = "FRAGMENT_SHADER");

	//!	Destructor.
	virtual ~CFragmentShader();

	//!	Clone this shader.
	virtual CFragmentShader* glClone();

    //! Loads an OpenGL 2.0 vertex shader program.
	bool glLoadProgram(const std::string &program);

    //! Returns current status of this fragment program
    //!	@see CShaderProgram.
	virtual bool glGetProgramStatus(void) const;

	//! This method returns the underlying hardware capabilities to render a vertex program. the capabilities fill
	//! the structure given in caps parameter.
	//!	Refer to official ARB documentation for detailed information on each capability item.
	static bool glGetShaderCaps(GL_FRAGMENT_SHADER_CAPS& caps);

	//! This method attaches the vertex program to a program object for
	//! linking and validation. Checking is performed on the handle to accept only valid programs.
	//! CShader use this method only when necessary
	//! @return : true if binding is done without errors, false otherwise.
	virtual bool glBindProgram(RAPTOR_HANDLE program);

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CFragmentShaderClassID,"FragmentShader",CShaderProgram)
	

private:
	//!	Forbidden operators
	CFragmentShader& operator=(const CFragmentShader&);

	//! Copy constructor.
	CFragmentShader(const CFragmentShader& shader);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)

