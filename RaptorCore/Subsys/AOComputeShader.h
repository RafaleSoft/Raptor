/***************************************************************************/
/*                                                                         */
/*  AOComputeShader.h                                                      */
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


#if !defined(AFX_AOCOMPUTESHADER_H__7CD66380_1000_47A3_AA98_47E0EDBD728E__INCLUDED_)
#define AFX_AOCOMPUTESHADER_H__7CD66380_1000_47A3_AA98_47E0EDBD728E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CAOComputeShader : public CShader
{
public:
	CAOComputeShader(void);
	virtual ~CAOComputeShader(void);

	//!	Implements base glRender.
	virtual void glRender(void);
	virtual void glStop(void);

	//! Initialize all shadings, must be called before use.
	void glInit();

private:
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_AOCOMPUTESHADER_H__7CD66380_1000_47A3_AA98_47E0EDBD728E__INCLUDED_)
