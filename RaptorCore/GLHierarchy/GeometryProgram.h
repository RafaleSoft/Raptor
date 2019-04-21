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
		int		max_geometry_uniform_blocks;
	} GL_GEOMETRY_PROGRAM_CAPS;


public:
	//!	Default constructor.
	CGeometryProgram(const std::string& name="GEOMETRY_PROGRAM");

	//! Destructor.
	virtual ~CGeometryProgram();

	//!	Clone this shader.
	virtual CGeometryProgram* glClone();

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

	//!	Configure the geometry chader.
	//!	The shader code must be conformant to these parameters.
	//! @return false in case of invallid parameter.
	bool setGeometry(uint32_t inputType, uint32_t outputType, uint32_t verticesOut);


	//!	Implements CPersistence
	DECLARE_CLASS_ID(CGeometryProgramClassID,"GeometryProgram",CShaderProgram)


private:
	//!	Forbidden operators
	CGeometryProgram& operator=(const CGeometryProgram&);

	//! Copy constructor.
	CGeometryProgram(const CGeometryProgram& shader);

    //! Specific init of shader parameters
    virtual void	glInitShaders();

	//!	See base class.
	static bool		m_bGeometryProgramReady;

	//! Input primitive type.
	uint32_t	m_inputType;

	//! Output primitive type.
	uint32_t	m_outputType;

	//! Number of vertivces emitted by primitive.
	uint32_t	m_verticesOut;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GEOMETRYPROGRAM_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)

