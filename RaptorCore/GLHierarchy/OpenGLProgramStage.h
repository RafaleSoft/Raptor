/***************************************************************************/
/*                                                                         */
/*  OpenGLProgramStage.h                                                   */
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


#if !defined(AFX_OPENGLPROGRAMSTAGE_H__0BCE3B42_6E10_4F50_BB27_1993345ADBCF__INCLUDED_)
#define AFX_OPENGLPROGRAMSTAGE_H__0BCE3B42_6E10_4F50_BB27_1993345ADBCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "ShaderProgram.h"
#endif



RAPTOR_NAMESPACE_BEGIN

class CVertexProgram;
class CFragmentProgram;


class RAPTOR_API COpenGLProgramStage : public CShaderProgram
{
public:
	COpenGLProgramStage(const std::string& name);

	virtual ~COpenGLProgramStage(void);

	//!	Clone current stage.
	COpenGLProgramStage* glClone() const;


	//! Implements base class
	virtual bool glLoadProgram(const std::string &program)
	{
		return false;
	};

	//! Implements base class
	virtual void glRender(void);

	//! Implements base class
	virtual void glStop(void);

	//! Implements base class
	virtual bool glGetProgramStatus(void) const;
	
	//! Implements base class
	virtual std::string glGetProgramString(void) const;


	virtual void glProgramParameter(unsigned int numParam,
									const GL_COORD_VERTEX &v) const
	{
	};

	virtual void glProgramParameter(unsigned int numParam,
									const CColor::RGBA &v) const
	{
	};

	//!
	//! Shaders configuration
	//!
	//!	Returns the vertex shader
	//!	Allocate a new one if necessary
	CVertexProgram * const glGetVertexProgram(const std::string& name = "");

	//!	Returns true if shader has a Vertex Shader already
	bool hasVertexProgram(void) const { return m_pVProgram != NULL; };

	//! Removes the vertex shader.
	//! @return true if the vertex shader has been deleted
	bool glRemoveVertexProgram(void);

	//!	Returns the fragment shader
	//!	Allocate a new one if necessary
	CFragmentProgram * const glGetFragmentProgram(const std::string& name = "");

	//!	Returns true if shader has a Fragment Shader already
	bool hasFragmentProgram(void) const { return m_pFProgram != NULL; };

	//! Removes the fragment shader.
	//! @return true if the fragment shader has been deleted
	bool glRemoveFragmentProgram(void);


	//! Provide gl-like shader parameters from RaptorCore.
	//!	Attention : parameter shall be copied before next call because return value is reused.
	CProgramParameters::CParameterBase& getDefaultParameter(const std::string& parameter_name, int locationIndex);


	//!	@see CShaderProgram
	virtual void setProgramParameters(const CProgramParameters &v);


	//! Inherited from CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(COpenGLProgramStageClassID, "OpenGLProgramStage", CShaderProgram)


protected:
	//! Copy constructor.
	COpenGLProgramStage(const COpenGLProgramStage&);


private:
	//! Denied operators
	COpenGLProgramStage();
	COpenGLProgramStage& operator=(const COpenGLProgramStage&);

	//! Implements CPersistence
	virtual void unLink(const CPersistence* p);


	bool				m_bDeleteVProgram;
	bool				m_bDeleteFProgram;

	CVertexProgram		*m_pVProgram;
	CFragmentProgram	*m_pFProgram;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OPENGLPROGRAMSTAGE_H__0BCE3B42_6E10_4F50_BB27_1993345ADBCF__INCLUDED_)

