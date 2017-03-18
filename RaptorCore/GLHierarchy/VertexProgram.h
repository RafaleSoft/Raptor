// VertexProgram.h: interface for the CVertexProgram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
#define AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_UNIFIEDPROGRAM_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)
	#include "UnifiedProgram.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//!
//! This class handles OpenGL2.0 vertex shaders. It is conceived to handle a program
//! rather than being only a shader because OGL2 shading language relates more to the concept
//! of a program with variables, compilation, linking and so on.
//! CAUTION : this class embbeds a vertex shader extension object, while CVertexShader holds
//! an assembly program only.
class RAPTOR_API CVertexProgram : public CUnifiedProgram  
{
public:
    typedef struct GL_VERTEX_PROGRAM_CAPS_t
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
	} GL_VERTEX_PROGRAM_CAPS;


public:
	//!	Default Constructor.
	CVertexProgram(const std::string& name="VERTEX_PROGRAM");

	//!	Destructor.
	virtual ~CVertexProgram();

	//!	Clone this shader.
	virtual CVertexProgram* glClone();

    //! Loads an OpenGL 2.0 vertex shader program.
	virtual bool glLoadProgram(const std::string &program);

    //!	Implements CShaderProgram.
	virtual bool glGetProgramStatus(void);

	//! This method returns the underlying hardware capabilities to render a vertex program. the capabilities fill
	//! the structure given in caps parameter.
	//!	Refer to official ARB documentation for detailed information on each capability item.
	static bool glGetProgramCaps(GL_VERTEX_PROGRAM_CAPS& caps);

    //! This method attaches the vertex program to a program object for
    //! linking and validation. Checking is performed on the handle to accept only valid programs.
    //! CShader use this method only when necessary
    //! @return : true if binding is done without errors, false otherwise.
    bool glBindProgram(RAPTOR_HANDLE program);

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CVertexProgramClassID,"VertexProgram",CShaderProgram)


private:
	//!	Forbidden operators
	CVertexProgram& operator=(const CVertexProgram&);

	//! Copy constructor.
	CVertexProgram(const CVertexProgram& shader);

    //! Specific init of shader parameters
    virtual void	glInitShaders();
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)

