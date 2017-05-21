// FragmentShader.h: interface for the CFragmentShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
#define AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
#include "ShaderProgram.h"
#endif

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CFragmentShader : public CShaderProgram  
{
public:
	typedef struct GL_FRAGMENT_SHADER_CAPS_t
	{
		int		max_instructions;
		int		max_native_instructions;
		int		max_temporaries;
		int		max_native_temporaries;
		int		max_parameters;
		int		max_local_parameters;
		int		max_env_parameters;
		int		max_native_parameters;
		int		max_attribs;
		int		max_native_attribs;
		int		max_alu_instructions;
		int		max_tex_instructions;
		int		max_tex_indirections;
		int		max_native_alu_instructions;
		int		max_native_tex_instructions;
		int		max_native_tex_indirections;
		int		max_program_matrices;
		int		max_program_matrix_stack_depth;
		int		max_texture_coords;
		int		max_texture_image_units;
	} GL_FRAGMENT_SHADER_CAPS;


public:
	//!	Default Constructor.
	CFragmentShader(const std::string& name="FRAGMENT_SHADER");

	//!	Destructor.
	virtual ~CFragmentShader();

	//!	Clone this shader.
	virtual CFragmentShader* glClone();

    //! Implements subclass of CShaderProgram : loads a valid ASM shader code source
	//!	@see CShaderProgram.
	virtual bool glLoadProgram(const std::string &program);

    //! Renders the shader : i.e. it is made the current fragment shader bound to the pipeline
	//!	@see CShaderProgram.
	virtual void glRender(void);

    //! Disables the fragment shader, the fixed pipeline is reactivated ( unboud )
	//!	@see CShaderProgram.
	virtual void glStop(void);

    //! Returns the status of this shader
	//!	@see CShaderProgram.
	virtual bool glGetProgramStatus(void);

	//! This method returns the underlying hardware capabilities
	//! to render a vertex program. the capabilities fill
	//! the structure given in caps parameter.
	//!	Refer to official ARB documentation for detailed information
	//! on each capability item.
	static bool glGetProgramCaps(GL_FRAGMENT_SHADER_CAPS& caps);
	
	//!	Direct access to program parameters
	virtual void glProgramParameter(unsigned int numParam,const GL_COORD_VERTEX &v) const;
	virtual void glProgramParameter(unsigned int numParam,const CColor::RGBA &v) const;

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CFragmentShaderClassID,"FragmentShader",CShaderProgram)


private:
	//!	Forbidden operators
	CFragmentShader& operator=(const CFragmentShader&);

	//! Copy constructor.
	CFragmentShader(const CFragmentShader& shader);

    //! Specific init of shader parameters
    virtual void	glInitShaders();

	static bool		m_bFragmentReady;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)

