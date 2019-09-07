/***************************************************************************/
/*                                                                         */
/*  ShaderProgram.h                                                        */
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


#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
#define AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
	#include "GLHierarchy/ProgramParameters.h"
#endif

RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CShaderProgram : public CPersistence  
{
public:
	virtual ~CShaderProgram();

    //! Program status: program may not be usable until valid state is true.
    //! isValid is equivalent to glGetProgramStatus but can be used without a gfx context.
	bool isValid(void) const { return m_bValid; };

	//!	Loads the assembler program describing OGL attributes
	//! processing program. The program must be correct,any other program
	//! loaded in a single objet is deleted and replaced by the one
	//! newly loaded.
	//! One must use several ShaderProgram objects to call several programs.
	//!	A default program is always valid however, it is the GL standard
	//! processing pipeline. It can be specified passing "opengl" 
	//! as the program argument ( the default value for any new object )
	virtual bool glLoadProgram(const std::string &program) = 0;

	//!	Same behavior as above, but program source is external.
	virtual bool glLoadProgramFromFile(const std::string &program);

	//!	The Render method has the typical bahaviour
	//!	of all Raptor Render methods : if the object
	//!	has no geometry info, Render sets GL state
	//!	for the next GL primitives rendered.
	//!	This behaviour is the same with vertex shaders
	virtual void glRender(void) = 0;

	//!	This method returns true if the program loaded fits
	//!	within the capabilities computed in the method below.
	//!	If result is true, it means the program will run properly
	//!	giving expected result.
	virtual bool glGetProgramStatus(void) const = 0;

	//! This method returns the shader program source loaded
	//! with glLoadProgram or glLoadProgramFromFile.
	//! If the shader is not valid or if any error prevent accessing 
	//! the source, then an empty string is returned.
	virtual std::string glGetProgramString(void) const = 0;

	//!	This method sets a program parameter ( 4 components vector ):
	//! The number of parameters is limited by hardware ( get shader caps in derived classes ). 
    //! The first parameter selects the program paramater, the second is the actual data.
    //! CAUTION : Accessing parameters directly is unsafe and should be avoided, except for performance issues.
	//virtual void glProgramParameter( unsigned int numParam,
    //                                 const GL_COORD_VERTEX &v) const = 0;

	//! Same as above but passes a color instead of a geo vector
	//virtual void glProgramParameter( unsigned int numParam,
    //                                 const CColor::RGBA &v) const = 0;

    //! This method can be used to pass in the whole parameter set.
    //! They will be actually applied after a successfull link is issued.
    virtual void setProgramParameters(const CProgramParameters &v);

	//! This method can be used to update only a subset of the actual parameter set.
	//! They will be actually applied after a successfull link is issued.
	void updateProgramParameters(const CProgramParameters &v);


	//! Inherited from CPersistence
	DECLARE_CLASS_ID(CShaderProgramClassID,"ShaderProgram",CPersistence)


protected:
	//!	Base Constructor.
	CShaderProgram(const CPersistence::CPersistenceClassID& id,const std::string& name="SHADER_PROGRAM");

	//!	Copy constructor.
	CShaderProgram(const CShaderProgram&);
	
	//! Valid status
	bool					m_bValid;

	RAPTOR_HANDLE			m_handle;
	bool					m_bApplyParameters;
	CProgramParameters	    m_parameters;


private:
    //! Denied operators
	CShaderProgram();
    CShaderProgram& operator=(const CShaderProgram& ) { return *this;};
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)

