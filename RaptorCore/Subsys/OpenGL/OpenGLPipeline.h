/***************************************************************************/
/*                                                                         */
/*  OpenGLPipeline.h                                                       */
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


#if !defined(AFX_RAPTOROPENGLPIPELINE_H__E386000E_D01F_4FD9_B15B_ED5E14685241__INCLUDED_)
#define AFX_RAPTOROPENGLPIPELINE_H__E386000E_D01F_4FD9_B15B_ED5E14685241__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif
#if !defined(AFX_RAPTORPIPELINE_972B6860_BDAE_4303_962E_3DAC4ECF7F7B__INCLUDED_)
	#include "GLHierarchy/IRaptorPipeline.h"
#endif



RAPTOR_NAMESPACE_BEGIN


class COpenGLPipeline : public IRaptorPipeline
{
public:
	COpenGLPipeline(void);
	virtual ~COpenGLPipeline(void);

	//! Implement base class. @see IRaptorPipeline.
	bool initPipeline(const CShaderProgram* shaderStages, const CGeometry* geometry);

	//! Implement base class. @see IRaptorPipeline.
	bool destroyPipeline(void);


private:
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTOROPENGLPIPELINE_H__E386000E_D01F_4FD9_B15B_ED5E14685241__INCLUDED_)
