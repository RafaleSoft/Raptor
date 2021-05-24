/***************************************************************************/
/*                                                                         */
/*  LightShader.h                                                          */
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


#if !defined(AFX_LIGHTSHADER_H__0DD43C10_10BE_4516_80CF_83D1DC8B6312__INCLUDED_)
#define AFX_LIGHTSHADER_H__0DD43C10_10BE_4516_80CF_83D1DC8B6312__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN

class CMaterial;
class CShaderBloc;

class CLightShader
{
public:
	//!	Default constructor.
	CLightShader(void);

	//! Destructor.
	virtual ~CLightShader(void);

	//!	Implements base class.
	void glRenderShaderBloc(CShaderBloc& bloc, const CMaterial &material);
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_LIGHTSHADER_H__0DD43C10_10BE_4516_80CF_83D1DC8B6312__INCLUDED_)
