/***************************************************************************/
/*                                                                         */
/*  RaptorPipeline.h                                                       */
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

#if !defined(AFX_RAPTORPIPELINE_972B6860_BDAE_4303_962E_3DAC4ECF7F7B__INCLUDED_)
#define AFX_RAPTORPIPELINE_972B6860_BDAE_4303_962E_3DAC4ECF7F7B__INCLUDED_

#pragma once

#include "Subsys/CodeGeneration.h"


RAPTOR_NAMESPACE_BEGIN


class CShaderProgram;
class CGeometry;


RAPTOR_INTERFACE RAPTOR_API IRaptorPipeline
{
public:
	//!	Creation and initialisation of all resources of the pipeline.
	//! @return true if pipeline succesfully initialized, false in case of error.
	virtual bool initPipeline(const CShaderProgram* shaderStages, const CGeometry* geometry) = 0;

	//!	Releation of all resources of the pipeline.
	//! @return true if pipeline succesfully destroyed, false in case of error.
	virtual bool destroyPipeline(void) = 0;
};

#endif	// !defined(AFX_RAPTORPIPELINE_972B6860_BDAE_4303_962E_3DAC4ECF7F7B__INCLUDED_)

RAPTOR_NAMESPACE_END