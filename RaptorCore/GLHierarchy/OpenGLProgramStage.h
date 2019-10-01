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

class CVertexProgram_old;
class CFragmentProgram_old;


class RAPTOR_API COpenGLProgramStage : public CShaderProgram
{
public:
	COpenGLProgramStage(const std::string& name);

	virtual ~COpenGLProgramStage(void);

	//! Implements base class
	virtual bool glLoadProgram(const std::string &program)
	{
		return NULL;
	};

	//! Implements base class
	virtual void glRender(void)
	{
	};

	//! Implements base class
	virtual void glStop(void) {};

	//! Implements base class
	virtual bool glGetProgramStatus(void)
	{
		return m_bValid;
	};

	//! Implements base class
	virtual std::string glGetProgramString(void) { return ""; }

	virtual void glProgramParameter(unsigned int numParam,
									const GL_COORD_VERTEX &v) const
	{
	};

	virtual void glProgramParameter(unsigned int numParam,
									const CColor::RGBA &v) const
	{
	};

	//! Provide gl-like shader parameters from RaptorCore.
	//!	Attention : parameter shall be copied before next call because return value is reused.
	CProgramParameters::CParameterBase& getDefaultParameter(const std::string& parameter_name, int locationIndex);


	//!	@see CShaderProgram
	virtual void setProgramParameters(const CProgramParameters &v);


	//! Inherited from CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(COpenGLProgramStageClassID, "OpenGLProgramStage", CShaderProgram)


private:
	//! Denied operators
	COpenGLProgramStage();
	COpenGLProgramStage& operator=(const COpenGLProgramStage&);

	CVertexProgram_old	*m_pVShader;
	CFragmentProgram_old *m_pFShader;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OPENGLPROGRAMSTAGE_H__0BCE3B42_6E10_4F50_BB27_1993345ADBCF__INCLUDED_)
