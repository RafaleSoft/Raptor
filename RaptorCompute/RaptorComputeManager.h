/***************************************************************************/
/*                                                                         */
/*  RaptorComputeManager.h                                                 */
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


#if !defined(AFX_RAPTOR_COMPUTE_MANAGER_H__876B757E_63F1_4E8A_847E_205C3AE2E132__INCLUDED_)
#define AFX_RAPTOR_COMPUTE_MANAGER_H__876B757E_63F1_4E8A_847E_205C3AE2E132__INCLUDED_

#pragma once

#if !defined(AFX_RAPTOR_COMPUTE_H__7FDED2CB_7AA2_4D6E_BF22_50923FD45212__INCLUDED_)
	#include "RaptorCompute.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif

#include "CL/cl.h"
#include "CL/cl_gl.h"


class CRaptorComputeManager
{
public:
	typedef struct PLATFORM_IDt
	{
		cl_platform_id				platformID;
		std::vector<cl_device_id>	deviceIDs;
		std::vector<cl_context>		contexts;
	} PLATFORM_ID;


public:
	//!	Single instance accessor.
	static CRaptorComputeManager& GetInstance(void);

	//!	Desctructor : release all used resources.
	bool clRelease(void);

	//! Initialize OCL platforms
	bool clInitPlatforms(void);

	//!	Initialize computing context
	bool clCreateContext(	unsigned int numPlatform = 0,
							unsigned int numDevice = 0,
							CRaptorDisplay *dsp = NULL);

	//! Returns the context associated with a platform/device pair.
	cl_context getContext(	unsigned int numPlatform = 0,
							unsigned int numDevice = 0) const
	{
		if ((numPlatform < m_platformIDs.size()) &&
			(numDevice < m_platformIDs[numPlatform].contexts.size()))
			return m_platformIDs[numPlatform].contexts[numDevice];
		else
			return NULL;
	}

	//!	TODO: reserve work-units by providing sub-devices
	//! allocation and management as 'standard' devices

	//!	@return the list of OpenCL platforms available for external usage
	const std::vector<CRaptorCompute::PLATFORM>& getPlatforms(void) const 
	{ return m_platforms; };

	//!	@return the list of platform ids for internal usage
	const std::vector<PLATFORM_ID>& getPlatformIDs(void) const 
	{ return m_platformIDs; };

	//! Emulates CPersistence for global opengl classes IDs & global namespace
	DECLARE_API_CLASS_ID(RAPTOR_API,COpenCLClassID,"OpenCL",CPersistence);


private:
	//! Constructor, forbidden.
	CRaptorComputeManager(void);

	//! Destructor : forbidden.
	~CRaptorComputeManager(void);

	//! The unique instance of the global manager
	static CRaptorComputeManager	*s_pManager;

	//!	The list of platform descriptors
	std::vector<CRaptorCompute::PLATFORM> m_platforms;

	//!	The list of platform IDs for OpenCL API direct calls
	std::vector<PLATFORM_ID> m_platformIDs;
};

#endif	// defined(AFX_RAPTOR_COMPUTE_MANAGER_H__876B757E_63F1_4E8A_847E_205C3AE2E132__INCLUDED_)


