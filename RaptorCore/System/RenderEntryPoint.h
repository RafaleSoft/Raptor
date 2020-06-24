/***************************************************************************/
/*                                                                         */
/*  RenderEntryPoint.h                                                     */
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



#if !defined(AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_)
#define AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

RAPTOR_NAMESPACE_BEGIN

//! This class implements the main render entry point for Raptor.
//! Each instance of this class will asked for glRender from Raptor::glRender
class RAPTOR_API  CRenderEntryPoint  
{
public:
    //! Constructor : register self with entry points collection.
	CRenderEntryPoint();

    //! Destructor : removes self from entry points collection.
	virtual ~CRenderEntryPoint();

    //! Must be implemented in sub-classes 
	virtual void glRender() = 0;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_)

