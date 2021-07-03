/***************************************************************************/
/*                                                                         */
/*  BumpShader.h                                                           */
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


#if !defined(AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_)
#define AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CBumpLightObserver;


class CBumpShader : public CShader
{
public:
	//!	Default constructor.
	CBumpShader(const std::string &name = "BUMP_SHADER");

	//! Destructor.
	virtual ~CBumpShader(void);

	//!	Clone current shader, assigning the new name newShaderName
	virtual CShader* glClone(const std::string& newShaderName) const;

	//!	Implements base glRender.
	virtual void glRender(void);

	//! Initialize all shadings, must be called before use.
	void glInit();


protected:
	//!	Copy constructor for cloning.
	CBumpShader(const CBumpShader& shader);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_)
