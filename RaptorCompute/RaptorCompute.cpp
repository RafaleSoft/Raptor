/***************************************************************************/
/*                                                                         */
/*  RaptorCompute.cpp                                                      */
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


#include "stdafx.h"

#if !defined(AFX_RAPTOR_COMPUTE_H__7FDED2CB_7AA2_4D6E_BF22_50923FD45212__INCLUDED_)
	#include "RaptorCompute.h"
#endif
#if !defined(AFX_RAPTOR_COMPUTE_MANAGER_H__876B757E_63F1_4E8A_847E_205C3AE2E132__INCLUDED_)
	#include "RaptorComputeManager.h"
#endif
#if !defined(AFX_RAPTOR_COMPUTE_MEMORY_H__525371EA_0090_40AA_9889_333824045A7A__INCLUDED_)
	#include "RaptorComputeMemory.h"
#endif
#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "System/RaptorConfig.h"
#endif
#if !defined(AFX_RAPTOR_COMPUTE_JOB_H__BCAF8B25_FC2C_49BB_A710_A0D51F6DDFD0__INCLUDED_)
	#include "RaptorComputeJob.h"
#endif


RAPTOR_NAMESPACE


CRaptorCompute::CRaptorCompute()
{
}

CRaptorCompute::~CRaptorCompute()
{
	CRaptorComputeManager::GetInstance().clRelease();
}

bool CRaptorCompute::clInitRaptorCompute()
{
	bool res = false;

	CRaptorComputeManager &manager = CRaptorComputeManager::GetInstance();

	//! Initialize Raptor Error Management Layer
	//! If already initialized, it is harmless,
	//! Nevertheless, if not initialised, this default config
	//! may not be compatible with further OpenGL initialisation
	CRaptorConfig config;
	IRaptor::glInitRaptor(config);

	//! Initialize OpenCL implementation
	res = manager.clInitPlatforms();

	if (!res)
	{
		RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),
						"Impossible to find a valid OpenCL platform.");
	}

	return res;
}

bool CRaptorCompute::clReleaseRaptorCompute(void)
{
	CRaptorComputeManager &manager = CRaptorComputeManager::GetInstance();
	return manager.clRelease();
}

std::vector<CRaptorCompute::PLATFORM> CRaptorCompute::getPlatforms(void)
{
	CRaptorComputeManager &manager = CRaptorComputeManager::GetInstance();
	return manager.getPlatforms();
}

