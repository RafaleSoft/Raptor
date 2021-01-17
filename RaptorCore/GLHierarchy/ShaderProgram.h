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

	//!	Adds a shader to current runtime library.
	//! Instead of loading the shader with the methods above, 
	//! provide the name of shader, the program source, and the Raptor shader class.
	//!	The loaded shader can then be reused with CShader::getShader
	//! @param shader_name: the name of the shader to be retrieved by glGet[Fragment|Geometry|Vertex]Shader.
	//! @param shader_source_file: the filename to load that contains the shader source.
	//! @param class_name: the name of the shader class (@see CPersistence)
	static bool glAddToLibrary(	const std::string& shader_name,
								const std::string& shader_source_file,
								const std::string& class_name);

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

	//! Computes and returns and size required to allocate a uniform
	//! buffer object backing uniform blocks
	//! @return the required size in bytes.
	uint64_t glGetBufferMemoryRequirements(void);

	//!	Shader uniform blocks.
	typedef struct shader_bloc_t
	{
		uint64_t	size;
		uint32_t	binding;
	} shader_bloc;
	CShaderProgram::shader_bloc glGetShaderBloc(const std::string& bloc_name) const;

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
	
	//! This method can be used to pass in the whole parameter set.
	//! They will be actually applied after a successfull link is issued.
	virtual void setProgramParameters(const CProgramParameters &v);


	//! Valid status
	bool					m_bValid;

	//!	Shader program handle.
	RAPTOR_HANDLE			m_handle;

	//!	Shader parameters.
	bool					m_bApplyParameters;
	CProgramParameters	    m_parameters;


private:
    //! Denied operators
	CShaderProgram();
    CShaderProgram& operator=(const CShaderProgram& ) { return *this;};

	//!	A helper to recursively read included files
	std::string readFile(const std::string filename);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)

