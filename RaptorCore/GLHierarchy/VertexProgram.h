/***************************************************************************/
/*                                                                         */
/*  VertexProgram.h                                                        */
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


#if !defined(AFX_VERTEXPROGRAM_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
#define AFX_VERTEXPROGRAM_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "GLHierarchy/ShaderProgram.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CVertexProgram : public CShaderProgram  
{
public:
	typedef struct GL_VERTEX_PROGRAM_CAPS_t
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
		int		max_address_registers;
		int		max_native_address_registers;
		int		max_vertex_attribs;
		int		max_program_matrices;
		int		max_program_matrix_stack_depth;
        bool	vertex_program_point_size;
		bool	vertex_program_two_side;
	} GL_VERTEX_PROGRAM_CAPS;

public:
	//! Default constructor.
	CVertexProgram(const std::string& name = "VERTEX_PROGRAM");

	//!	Destructor.
	virtual ~CVertexProgram();

	//!	Clone this shader.
	virtual CVertexProgram* glClone();

    //! Implements base class
	//!	@see CShaderProgram.
	virtual bool glLoadProgram(const std::string &program);

    //! Implements base class. The shader is actually invoked and remains active until
    //! a call to glStop is issued. @see CShaderProgram.
	virtual void glRender(void);

    //! Implements base class
	//!	@see CShaderProgram.
	virtual void glStop(void);

    //! Implements base class
	//!	@see CShaderProgram.
	virtual bool glGetProgramStatus(void) const;

	//! Implements base class
	//!	@see CShaderProgram.
	virtual std::string glGetProgramString(void) const;

	//! This method returns the underlying hardware capabilities
	//! to render a vertex program. the capabilities fill
	//! the structure given in caps parameter.
	//!	Refer to official ARB documentation for detailed information
	//! on each capability item.
	static bool glGetProgramCaps(GL_VERTEX_PROGRAM_CAPS& caps);


	//!	These methods will have the same behaviour as standard
	//!	GL primitives except they are rendererd through a vertex shader:
	//! If the underlying API does the correct mapping, specifying a
	//! generic vertex attribute will do the same as the specific function.
	//! ex: CVertexShader::glNormal uses glVertexAttrib(2,n), it should be
	//! equivalent to ::glNormal(n),but are exclusive
	//!	( remember that for a vertex shader, specifying the vertex
	//!	such as glVertex here will actually render a vertex but computed
	//!	by the shader program )
	void RAPTOR_FASTCALL glVertex(const GL_COORD_VERTEX &v);
	void RAPTOR_FASTCALL glVertexWeightEXT(float w);
	void RAPTOR_FASTCALL glNormal(const GL_COORD_VERTEX &n);
	void RAPTOR_FASTCALL glColor(const CColor::RGBA &c);
	void RAPTOR_FASTCALL glSecondaryColorEXT(const CColor::RGBA &c);
	void RAPTOR_FASTCALL glFogCoordEXT(float f);
	void RAPTOR_FASTCALL glMultiTexCoord(CProgramParameters::GL_VERTEX_ATTRIB tmu, const GL_COORD_VERTEX &t);
	
	void RAPTOR_FASTCALL glAdditionalVertexParam(const GL_COORD_VERTEX &v);
	void RAPTOR_FASTCALL glAdditionalVertexParam2(const GL_COORD_VERTEX &v);

	void RAPTOR_FASTCALL glPointParameter(float minSize,float maxSize,const GL_COORD_VERTEX& attenuation);

	//!	A generic vertex attribute function.
	//!	Vertex attributes will be extended and managed
	//!	with generic names ( vertex shaders do so, GL pipe not )
	//! This function is easier to use because no sophisticated
	//!	objets or structures are needed
	void RAPTOR_FASTCALL glSetVertexAttribute(	CProgramParameters::GL_VERTEX_ATTRIB attrib,
												float param1 = 0.0f,
												float param2 = 0.0f,
												float param3 = 0.0f,
												float param4 = 0.0f);


	//!	Direct access to program parameters
	virtual void glProgramParameter(unsigned int numParam,const GL_COORD_VERTEX &v) const;
	virtual void glProgramParameter(unsigned int numParam,const CColor::RGBA &v) const;
	void glProgramParameter(unsigned int numParam,const CGenericVector<float> &v) const;


	//! Implements CPersistence
	DECLARE_CLASS_ID(CVertexProgramClassID, "VertexProgram", CShaderProgram)



private:
	//!	Forbidden operators
	CVertexProgram& operator=(const CVertexProgram&);

	//! Copy constructor.
	CVertexProgram(const CVertexProgram& shader);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VERTEXPROGRAM_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)

