/***************************************************************************/
/*                                                                         */
/*  UnifiedShader.h                                                        */
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


#if !defined(AFX_UNIFIEDSHADER_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)
#define AFX_UNIFIEDSHADER_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
    #include "GLHierarchy/ShaderProgram.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CUnifiedShader : public CShaderProgram  
{
public:
	//!	Destructor.
	virtual ~CUnifiedShader();

	//!	Load OpenGL parameters all at once
	virtual void glRender(void);

    //! Loads a unique parameter immediately
    virtual void glProgramParameter(size_t numParam,
									const GL_COORD_VERTEX &v) const;

	//! Same as above but passes a color instead of a geo vector
	virtual void glProgramParameter(size_t numParam,
									const CColor::RGBA &v) const;

	//! Implements CShaderProgram.
	virtual std::string glGetProgramString(void) const;

	//! This method attaches the vertex program to a program object for
	//! linking and validation. Checking is performed on the handle to accept only valid programs.
	//! CShader use this method only when necessary
	//! @return : true if binding is done without errors, false otherwise.
	virtual bool glBindProgram(RAPTOR_HANDLE program);

	//! Symetric as above : detaches the vertex program from the program object, performing checking.
	bool glUnbindProgram(RAPTOR_HANDLE program);



protected:
	//!	Default Constructor.
	CUnifiedShader(const CPersistence::CPersistenceClassID &classId,
					const std::string& name);

	//! Copy constructor.
	CUnifiedShader(const CUnifiedShader& shader);

	//! Generic shader source loading.
	bool CUnifiedShader::glLoadProgram(	const std::string &program,
										GLenum shaderType,
										const CPersistence::CPersistenceClassID& shaderClass);

    //! Updates locations of uniform variables.
    void glQueryUniformLocations(RAPTOR_HANDLE program);

    //! Updates locations of attributes variables.
    void glQueryAttributeLocations(RAPTOR_HANDLE program);


private:
	//!	Forbidden operators
	CUnifiedShader& operator=(const CUnifiedShader&);

    //! factorize uniform settings
    void glParameter(size_t numParam,const float *v) const;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_UNIFIEDSHADER_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)

