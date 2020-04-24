/***************************************************************************/
/*                                                                         */
/*  VertexShader.h                                                         */
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


#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
#define AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_UNIFIEDSHADER_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)
	#include "UnifiedShader.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CVertexShader : public CUnifiedShader
{
public:
    typedef struct GL_VERTEX_SHADER_CAPS_t
	{
		int		max_vertex_uniform_components;
		int		max_varying_floats;
		int		max_vertex_attribs;
		int		max_texture_image_units;
		int		max_vertex_texture_image_units;
		int		max_combined_texture_image_units;
		int		max_texture_coords;
		bool	vertex_program_point_size;
		bool	vertex_program_two_side;
		int		max_combined_vertex_uniform_components;
		int		max_uniform_block_size;
		int		max_vertex_uniform_blocks;
	} GL_VERTEX_SHADER_CAPS;


public:
	//!	Default Constructor.
	CVertexShader(const std::string& name = "VERTEX_SHADER");

	//!	Destructor.
	virtual ~CVertexShader();

	//!	Clone this shader.
	virtual CVertexShader* glClone();

    //! Loads an OpenGL vertex shader program.
	virtual bool glLoadProgram(const std::string &program);

    //!	Implements CShaderProgram.
	virtual bool glGetProgramStatus(void) const;

	//! This method returns the underlying hardware capabilities to render a vertex program. the capabilities fill
	//! the structure given in caps parameter.
	//!	Refer to official ARB documentation for detailed information on each capability item.
	static bool glGetShaderCaps(GL_VERTEX_SHADER_CAPS& caps);

    //! This method attaches the vertex program to a program object for
    //! linking and validation. Checking is performed on the handle to accept only valid programs.
    //! CShader use this method only when necessary
    //! @return : true if binding is done without errors, false otherwise.
    virtual bool glBindProgram(RAPTOR_HANDLE program);

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CVertexShaderClassID, "VertexShader", CShaderProgram)


private:
	//!	Forbidden operators
	CVertexShader& operator=(const CVertexShader&);

	//! Copy constructor.
	CVertexShader(const CVertexShader& shader);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)

