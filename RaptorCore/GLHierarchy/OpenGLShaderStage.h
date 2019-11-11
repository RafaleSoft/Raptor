/***************************************************************************/
/*                                                                         */
/*  OpenGLShaderStage.h                                                    */
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


#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
#define AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "ShaderProgram.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CVertexShader;
class CFragmentShader;
class CGeometryShader;


class RAPTOR_API COpenGLShaderStage : public CShaderProgram
{
public:
	COpenGLShaderStage(const std::string& name);

	virtual ~COpenGLShaderStage(void);

		//!	Clone current stage.
	COpenGLShaderStage* glClone() const;


	//! Implements base class
	virtual bool glLoadProgram(const std::string &program)
	{
		return NULL;
	};

	//! Implements base class
	virtual void glRender(void);

	//! Implements base class
	virtual void glStop(void);

	//! Implements base class
	virtual bool glGetProgramStatus(void) const;

	//! Implements base class
	virtual std::string glGetProgramString(void) const;


	//! Generate the shader as a compiled object ( program objects / display lists ).
	//! The compile state must be correct to render the shader.
	//! @return true if successfully compiled, false otherwise.
	bool glCompileShader(void);


	//!	Returns the vertex Program, allocate a new one if necessary.
	//!	EMPTY_PROGRAM is a special name for a void program (i.e. doing nothing)
	CVertexShader * const glGetVertexShader(const std::string& name = "");

	//!	Returns true if Program has a Vertex Program already
	bool hasVertexShader(void) const { return m_pVShader != NULL; };

	//! Removes the vertex program.
	//! @return true if the vertex program has been deleted
	bool glRemoveVertexShader(void);


	//!	Returns the fragment Program
	//!	Allocate a new one if necessary
	CFragmentShader * const glGetFragmentShader(const std::string& name = "");

	//!	Returns true if Program has a Fragment Program already
	bool hasFragmentShader(void) const { return m_pFShader != NULL; };

	//! Removes the fragment program.
	//! @return true if the fragment program has been deleted
	bool glRemoveFragmentShader(void);


	//!	Returns the geometry Program
	//!	Allocate a new one if necessary
	CGeometryShader * const glGetGeometryShader(const std::string& name = "");

	//!	Returns true if Program has a Vertex Program already
	bool hasGeometryShader(void) const { return m_pGShader != NULL; };

	//! Removes the geometry program.
	//! @return true if the geometry program has been deleted
	bool glRemoveGeometryShader(void);


		//! Provide gl-like shader parameters from RaptorCore.
	//!	Attention : parameter shall be copied before next call because return value is reused.
	CProgramParameters::CParameterBase& getDefaultParameter(const std::string& parameter_name, int locationIndex);


	//!	@see CShaderProgram
	virtual void setProgramParameters(const CProgramParameters &v);

	//!	@see CShaderProgram
	virtual void updateProgramParameters(const CProgramParameters &v);

	//! Inherited from CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(COpenGLShaderStageClassID, "OpenGLShaderStage", CShaderProgram)


protected:
	//! Copy constructor.
	COpenGLShaderStage(const COpenGLShaderStage&);

	//! Program has been re-linked, perform post processing again
	bool    m_bReLinked;


private:
	//! Denied operators
	COpenGLShaderStage();
	COpenGLShaderStage& operator=(const COpenGLShaderStage&);

	//! Implements CPersistence.
	virtual void unLink(const CPersistence* p);

	//!	Initialise uniforms binding of program.
	void glQueryUniformLocations(void);

	//!	Initialise attributes binding of program.
	void glQueryAttributeLocations(void);

	//!	Set parameters values.
	void glSetProgramParameters();


	bool			m_bDeleteVShader;
	bool			m_bDeleteFShader;
	bool			m_bDeleteGShader;
	bool			m_bUpdateLocations;

	RAPTOR_HANDLE	m_shaderProgram;

	CVertexShader	*m_pVShader;
	CFragmentShader	*m_pFShader;
	CGeometryShader	*m_pGShader;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)

