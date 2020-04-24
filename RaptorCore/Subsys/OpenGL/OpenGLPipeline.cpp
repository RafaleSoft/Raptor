/***************************************************************************/
/*                                                                         */
/*  OpenGLPipeline.cpp                                                     */
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


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOROPENGLPIPELINE_H__E386000E_D01F_4FD9_B15B_ED5E14685241__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLPipeline.h"
#endif


RAPTOR_NAMESPACE


COpenGLPipeline::COpenGLPipeline(void)
{
}

COpenGLPipeline::~COpenGLPipeline(void)
{
}


bool COpenGLPipeline::destroyPipeline()
{
	return true;
}


bool COpenGLPipeline::initPipeline(const CShaderProgram* shaderStages, const CGeometry* geometry)
{
	return true;
}
