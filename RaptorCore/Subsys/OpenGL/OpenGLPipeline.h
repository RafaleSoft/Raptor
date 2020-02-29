/***************************************************************************/
/*                                                                         */
/*  OpenGLPipeline.h                                                       */
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


#if !defined(AFX_RAPTOROPENGLPIPELINE_H__E386000E_D01F_4FD9_B15B_ED5E14685241__INCLUDED_)
#define AFX_RAPTOROPENGLPIPELINE_H__E386000E_D01F_4FD9_B15B_ED5E14685241__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class COpenGLPipeline
{
public:
	COpenGLPipeline(void);
	virtual ~COpenGLPipeline(void);

	bool initPipeline();

	bool destroyPipeline(void);


private:
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTOROPENGLPIPELINE_H__E386000E_D01F_4FD9_B15B_ED5E14685241__INCLUDED_)
