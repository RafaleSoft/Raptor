// GeometryProgram.h: interface for the CGeometryProgram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMETRYPROGRAM_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
#define AFX_GEOMETRYPROGRAM_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_

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
class RAPTOR_API CGeometryProgram : public CUnifiedProgram  
{
public:
    typedef struct GL_GEOMETRY_PROGRAM_CAPS_t
	{
		int		max_geometry_texture_image_units;
		int		max_geometry_varying_components;
		int		max_vertex_varying_components;
		int		max_varying_components;
		int		max_geometry_uniform_components;
		int		max_geometry_output_vertices;
		int		max_geometry_total_output_components;
	} GL_GEOMETRY_PROGRAM_CAPS;


public:
	CGeometryProgram(const std::string& name="GEOMETRY_PROGRAM");
	virtual ~CGeometryProgram();

    //! The real rendering cannot be performed here, as it relies on
    //! CShader program object invocation. Nevertheless, this method is
    //! necessary to perform 'after-link' steps ( such as uniform locations
    virtual void glRender(void);

    //! See base class
    virtual void glStop(void);

    //! Loads an OpenGL 2.0 vertex shader program.
	virtual bool glLoadProgram(const std::string &program);

    //!	Implements CShaderProgram.
	virtual bool glGetProgramStatus(void);

	//! This method returns the underlying hardware capabilities to render a vertex program. the capabilities fill
	//! the structure given in caps parameter.
	//!	Refer to official ARB documentation for detailed information on each capability item.
	static bool glGetProgramCaps(GL_GEOMETRY_PROGRAM_CAPS& caps);

    //! This method attaches the vertex program to a program object for
    //! linking and validation. Checking is performed on the handle to accept only valid programs.
    //! CShader use this method only when necessary
    //! @return : true if binding is done without errors, false otherwise.
    bool glBindProgram(RAPTOR_HANDLE program);

    //! Symetric as above : detaches the vertex program from the program object, performing checking.
    bool glUnbindProgram(RAPTOR_HANDLE program);

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CGeometryProgramClassID,"GeometryProgram",CShaderProgram)

private:
    //! Specific init of shader parameters
    virtual void	glInitShaders();
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GEOMETRYPROGRAM_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)

