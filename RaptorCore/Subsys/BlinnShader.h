/***************************************************************************/
/*                                                                         */
/*  BlinnShader.h                                                          */
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


#if !defined(AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_)
#define AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_LIGHTSHADER_H__0DD43C10_10BE_4516_80CF_83D1DC8B6312__INCLUDED_)
	#include "LightShader.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CBlinnShader : public CShader, public CLightShader
{
public:
	//!	Default constructor.
	CBlinnShader(void);

	//! Destructor.
	virtual ~CBlinnShader(void);

	//!	Clone current shader, assigning the new name newShaderName
	virtual CShader* glClone(const std::string& newShaderName) const;

	//!	Implements base class.
	virtual void glRender();

	//! Initialize all shadings, must be called before use.
	void glInit();

private:
	//!	Copy constructor for cloning.
	CBlinnShader(const CBlinnShader& shader);
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_)
