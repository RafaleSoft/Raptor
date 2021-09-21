/***************************************************************************/
/*                                                                         */
/*  RaptorComputeManager.cpp                                               */
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


#include "StdAfx.h"

#if !defined(AFX_RAPTOR_COMPUTE_MANAGER_H__876B757E_63F1_4E8A_847E_205C3AE2E132__INCLUDED_)
	#include "RaptorComputeManager.h"
#endif
#ifdef LINUX
	#ifndef __GLX_RAPTOR_GLEXT_H__
		#include "GLXSpecific/GLXGlext.h"
	#endif
#endif


#include <stdlib.h>


RAPTOR_NAMESPACE 

CRaptorComputeManager	*CRaptorComputeManager::s_pManager = NULL;
static CRaptorComputeManager::COpenCLClassID openclID;
const CPersistence::CPersistenceClassID& CRaptorComputeManager::COpenCLClassID::GetClassId(void)
{
	return openclID;
}

void CL_CALLBACK openCLCallback(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
	// Log Error - wait for pending jobs - ?
	RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),
					errinfo);
}

CRaptorComputeManager& CRaptorComputeManager::GetInstance(void)
{
	if (s_pManager == NULL)
		s_pManager = new CRaptorComputeManager();

	return *s_pManager;
}

CRaptorComputeManager::CRaptorComputeManager(void)
{
}

bool CRaptorComputeManager::clRelease(void)
{
	if (s_pManager != NULL)
		delete s_pManager;

	s_pManager = NULL;

	return true;
}

CRaptorComputeManager::~CRaptorComputeManager(void)
{
	for (size_t i=0;i<m_platformIDs.size();i++)
	{
		for (size_t j=0;j<m_platformIDs[i].contexts.size();j++)
		{
			cl_context ctx = m_platformIDs[i].contexts[j];
			if (ctx != NULL)
			{
				cl_int ciErrNum = CL_SUCCESS;
				cl_uint refCount = 0;
				ciErrNum = clGetContextInfo(ctx,
											CL_CONTEXT_REFERENCE_COUNT,
											sizeof(cl_uint),
											&refCount,NULL);

				if (ciErrNum != CL_SUCCESS)
				{
					// Log error
				}

				if (refCount != 1)
				{
					// Log Error - wait for pending jobs - ?
					RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),
									"Trying to destroy OpenCL context while pending references still use it.");
				}


				clReleaseContext(ctx);
			}
		}
	}
}

bool CRaptorComputeManager::clCreateContext(unsigned int numPlatform,
											unsigned int numDevice,
											CRaptorDisplay *dsp)
{
	if (numPlatform >= m_platformIDs.size())
		return false;
	if (numDevice >= m_platformIDs[numPlatform].deviceIDs.size())
		return false;
	if (m_platformIDs[numPlatform].contexts[numDevice] != 0)
		return true;

	cl_int ciErrNum = CL_SUCCESS;

	cl_context_properties *pProperties = NULL;
	size_t nbProperties = 0;

	if (NULL != dsp)
	{
#ifdef WIN32
			if ((wglGetCurrentDC() == 0) || (wglGetCurrentContext() == 0))
#else	// Linux environment
			if ((glXGetCurrentDisplay() == 0) || (glXGetCurrentContext() == 0))
#endif
		{
			RAPTOR_ERROR(CRaptorComputeManager::COpenCLClassID::GetClassId(),
						"Unable to get current OpenGL Context from CRaptorDisplay.");
		}

		if (string::npos != m_platforms[numPlatform].extensions.find("cl_khr_gl_sharing"))
		{
			nbProperties = 4;
			pProperties = new cl_context_properties[4];
			pProperties[0] = CL_GL_CONTEXT_KHR;

#ifdef WIN32
			pProperties[1] = (cl_context_properties)wglGetCurrentContext();
			pProperties[2] = CL_WGL_HDC_KHR;
			pProperties[3] = (cl_context_properties)wglGetCurrentDC();
#else	// Linux environment
			pProperties[1] = (cl_context_properties)glXGetCurrentContext;
			pProperties[2] = CL_GLX_DISPLAY_KHR;
			pProperties[3] = (cl_context_properties)glXGetCurrentDisplay();
#endif
		}
		else
		{
			RAPTOR_ERROR(CRaptorComputeManager::COpenCLClassID::GetClassId(),
						"Platform requested does not support OpenCL/GL Interop.");
		}
	}

	size_t props_size = sizeof(cl_context_properties) * (nbProperties + 2 + 1);
	cl_context_properties* properties = (cl_context_properties*)malloc(	props_size );
	if (properties == NULL)
		return false;

	properties[0] = CL_CONTEXT_PLATFORM;
	properties[1] = (cl_context_properties)(m_platformIDs[numPlatform].platformID);
	for (size_t i=0;i<nbProperties;i++)
		properties[2+i] = pProperties[i];
	properties[2+nbProperties] = 0;

	// Optimisation : use all possible of the same type of 
	// the requested device ?

	//cl_uint num_devices = m_platformIDs[numPlatform].deviceIDs.size();
	//cl_device_id* clDeviceIDs = (cl_device_id*)malloc(num_devices * sizeof(cl_device_id));
	//for (size_t j=0;j<num_devices;j++)
	//	clDeviceIDs[j] = m_platformIDs[numPlatform].deviceIDs[j];
	cl_device_id clDeviceIDs = m_platformIDs[numPlatform].deviceIDs[numDevice];

	cl_context ctx = ::clCreateContext(	properties,
										1, //num_devices,
										&clDeviceIDs,
										openCLCallback,
										this,
										&ciErrNum);
	m_platformIDs[numPlatform].contexts[numDevice] = ctx;

	//free(clDeviceIDs);

	if (ciErrNum != CL_SUCCESS)
	{
		RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),
						"Unable to create the requested OpenCL context.");
	}

	return (ciErrNum == CL_SUCCESS);
}

bool CRaptorComputeManager::clInitPlatforms(void)
{
	//! Initialize platforms descriptors only once.
	if (m_platforms.size() > 0)
		return true;

    // Get OpenCL platform count
	cl_uint num_platforms = 0;
    cl_int ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);

    if (ciErrNum != CL_SUCCESS)
        return false;
    else 
    {
        if(num_platforms == 0)
		{
			RAPTOR_ERROR(CRaptorComputeManager::COpenCLClassID::GetClassId(),
						"No OpenCL platform found.");
            return false;
		}
        else 
        {
			//	Allocate space for platform Ids
            cl_platform_id* clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id));
			if (clPlatformIDs == NULL)
				return false;

            // get platform info for each platform
            ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);
            for(cl_uint i = 0; i < num_platforms; i++)
	        {
				CRaptorCompute::PLATFORM P;
				PLATFORM_ID PID;
				PID.platformID = clPlatformIDs[i];

				char chBuffer[MAX_PATH];
                ciErrNum = clGetPlatformInfo (PID.platformID, CL_PLATFORM_NAME, MAX_PATH, chBuffer, NULL);
                if (ciErrNum != CL_SUCCESS)
					return false;
				P.name = chBuffer;

				ciErrNum = clGetPlatformInfo (PID.platformID, CL_PLATFORM_PROFILE, MAX_PATH, chBuffer, NULL);
                if (ciErrNum != CL_SUCCESS)
					return false;
				P.profile = chBuffer;

				ciErrNum = clGetPlatformInfo (PID.platformID, CL_PLATFORM_VERSION, MAX_PATH, chBuffer, NULL);
                if (ciErrNum != CL_SUCCESS)
					return false;
				P.version = chBuffer;

				ciErrNum = clGetPlatformInfo (PID.platformID, CL_PLATFORM_VENDOR, MAX_PATH, chBuffer, NULL);
                if (ciErrNum != CL_SUCCESS)
					return false;
				P.vendor = chBuffer;

				size_t extensions_size = 0;
				ciErrNum = clGetPlatformInfo (PID.platformID, CL_PLATFORM_EXTENSIONS, 0, NULL, &extensions_size);
                if (ciErrNum == CL_SUCCESS)
				{
					char *extensions = (char *)calloc(extensions_size+1,1);
					ciErrNum = clGetPlatformInfo (PID.platformID, CL_PLATFORM_EXTENSIONS, extensions_size, extensions, NULL);
					P.extensions = extensions;
					free(extensions);
					if (ciErrNum != CL_SUCCESS)
						return false;
				}
				else
					return false;

				//	Get devices
				cl_uint num_devices = 0;
				ciErrNum = clGetDeviceIDs(PID.platformID,CL_DEVICE_TYPE_ALL,0,NULL,&num_devices);
				if (ciErrNum != CL_SUCCESS)
					return false;

				cl_device_id* clDeviceIDs = NULL;
				clDeviceIDs = (cl_device_id*)malloc(num_devices * sizeof(cl_device_id));
				if (clDeviceIDs == NULL)
					return false;

				// get device info for each platform
				ciErrNum = clGetDeviceIDs(PID.platformID,CL_DEVICE_TYPE_ALL,num_devices,clDeviceIDs,NULL);
				for(cl_uint j = 0; j < num_devices; j++)
				{
					CRaptorCompute::DEVICE D;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DRIVER_VERSION,MAX_PATH,chBuffer,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;
					D.driver_version = chBuffer;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_VERSION,MAX_PATH,chBuffer,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;
					D.version = chBuffer;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_TYPE,sizeof(cl_device_type),&D.type,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					extensions_size = 0;
					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_EXTENSIONS,0, NULL,&extensions_size);
					if (ciErrNum == CL_SUCCESS)
					{
						char *extensions = (char *)calloc(extensions_size+1,1);
						ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_EXTENSIONS,extensions_size,extensions,NULL);
						D.extensions = extensions;
						free(extensions);
						if (ciErrNum != CL_SUCCESS)
							return false;
					}
					else
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_VENDOR,MAX_PATH,chBuffer,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;
					D.vendor = chBuffer;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_NAME,MAX_PATH,chBuffer,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;
					D.name = chBuffer;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_QUEUE_PROPERTIES,sizeof(cl_command_queue_properties),&D.queue_properties,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_EXECUTION_CAPABILITIES,sizeof(cl_device_exec_capabilities),&D.execution_capabilities,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_COMPILER_AVAILABLE,sizeof(uint32_t),&D.compiler_available,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_AVAILABLE, sizeof(uint32_t), &D.available, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_ENDIAN_LITTLE, sizeof(uint32_t), &D.endian_little, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_PROFILING_TIMER_RESOLUTION,sizeof(size_t),&D.timer_resolution,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(uint32_t), &D.error_correction, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(uint64_t), &D.local_memory_size, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_LOCAL_MEM_TYPE,sizeof(cl_device_local_mem_type),&D.local_memory_type,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(uint32_t), &D.max_constants, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(uint64_t), &D.max_constant_buffer, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(uint64_t), &D.global_memory_size, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(uint64_t), &D.global_memory_cache_size, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(uint32_t), &D.global_memory_cacheline_size, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,sizeof(unsigned long),&D.global_memory_cache_type,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_SINGLE_FP_CONFIG, sizeof(uint64_t), &D.single_fp_config, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, sizeof(uint32_t), &D.min_align_size, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(uint32_t), &D.base_address_align, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MAX_SAMPLERS, sizeof(uint32_t), &D.max_samplers, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_MAX_PARAMETER_SIZE,sizeof(size_t),&D.max_parameter_size,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_IMAGE3D_MAX_WIDTH,sizeof(size_t),&D.image3d_max_width,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_IMAGE3D_MAX_HEIGHT,sizeof(size_t),&D.image3d_max_height,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_IMAGE3D_MAX_DEPTH,sizeof(size_t),&D.image3d_max_depth,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_IMAGE2D_MAX_WIDTH,sizeof(size_t),&D.image2d_max_width,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_IMAGE2D_MAX_HEIGHT,sizeof(size_t),&D.image2d_max_height,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(uint32_t), &D.max_image_write, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(uint32_t), &D.max_image_read, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_IMAGE_SUPPORT,sizeof(unsigned long),&D.image_support,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(uint64_t), &D.max_alloc_size, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_ADDRESS_BITS, sizeof(uint32_t), &D.address_size, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(uint32_t), &D.frequency, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_VENDOR_ID,sizeof(unsigned long),&D.vendor_id,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(uint32_t), &D.max_compute_units, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(uint32_t), &D.max_work_item_dimensions, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(size_t),&D.max_work_item_group_size,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					size_t wi_dims = D.max_work_item_dimensions*sizeof(size_t);
					D.max_work_item_sizes = (size_t*)malloc(wi_dims);
					ciErrNum = clGetDeviceInfo(clDeviceIDs[j],CL_DEVICE_MAX_WORK_ITEM_SIZES,wi_dims,D.max_work_item_sizes,NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(uint32_t), &D.vector_width_char, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(uint32_t), &D.vector_width_short, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(uint32_t), &D.vector_width_int, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(uint32_t), &D.vector_width_long, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(uint32_t), &D.vector_width_float, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

					ciErrNum = clGetDeviceInfo(clDeviceIDs[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(uint32_t), &D.vector_width_double, NULL);
					if (ciErrNum != CL_SUCCESS)
						return false;

				
					P.devices.push_back(D);
					PID.deviceIDs.push_back(clDeviceIDs[j]);
					PID.contexts.push_back(0);
				}

				free(clDeviceIDs);

				m_platforms.push_back(P);
				m_platformIDs.push_back(PID);
            }

			free(clPlatformIDs);
        }
    }

	return (ciErrNum == CL_SUCCESS);
}

