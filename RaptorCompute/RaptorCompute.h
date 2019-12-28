/***************************************************************************/
/*                                                                         */
/*  RaptorCompute.h                                                        */
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


#if !defined(AFX_RAPTOR_COMPUTE_H__7FDED2CB_7AA2_4D6E_BF22_50923FD45212__INCLUDED_)
#define AFX_RAPTOR_COMPUTE_H__7FDED2CB_7AA2_4D6E_BF22_50923FD45212__INCLUDED_


#include "Subsys/CodeGeneration.h"
#include <stdint.h>


RAPTOR_NAMESPACE

class CRaptorComputeManager;
class CRaptorComputeJob;


//! Raptor Compute entry point
//!
class RAPTOR_API CRaptorCompute
{
public:

	//!	Device definition. This structure is public, but it's content
	//! shall become private through opaque identifiers only
	typedef struct DEVICE_t
	{
		std::string			version;
		std::string			driver_version;
		unsigned long		type;
		std::string			extensions;
		std::string			vendor;
		unsigned long		vendor_id;
		std::string			name;
		unsigned long		queue_properties;
		unsigned long		execution_capabilities;
		uint32_t			compiler_available;
		uint32_t			available;
		uint32_t			endian_little;
		size_t				timer_resolution;
		uint32_t			error_correction;
		uint64_t			local_memory_size;
		unsigned long		local_memory_type;
		uint32_t			max_constants;
		uint64_t			max_constant_buffer;
		uint64_t			global_memory_size;
		uint64_t			global_memory_cache_size;
		uint32_t			global_memory_cacheline_size;
		unsigned long		global_memory_cache_type;
		uint64_t			single_fp_config;
		uint32_t			min_align_size;
		uint32_t			base_address_align;
		uint32_t			max_samplers;
		size_t				max_parameter_size;
		size_t				image3d_max_width;
		size_t				image3d_max_height;
		size_t				image3d_max_depth;
		size_t				image2d_max_width;
		size_t				image2d_max_height;
		uint32_t			max_image_write;
		uint32_t			max_image_read;
		unsigned long		image_support;
		uint64_t			max_alloc_size;
		uint32_t			address_size;
		uint32_t			frequency;
		uint32_t			max_compute_units;
		uint32_t			max_work_item_dimensions;
		size_t				*max_work_item_sizes;
		size_t				max_work_item_group_size;
		uint32_t			vector_width_char;
		uint32_t			vector_width_short;
		uint32_t			vector_width_int;
		uint32_t			vector_width_long;
		uint32_t			vector_width_float;
		uint32_t			vector_width_double;
	} DEVICE;
	
	//! Platforms
	typedef struct PLATFORM_t
	{
		std::string			name;
		std::string			profile;
		std::string			version;
		std::string			vendor;
		std::string			extensions;
		std::vector<DEVICE>	devices;
	} PLATFORM;
	

public:
	//! Initializes computing platform.
	//!	If Raptor is not initialised, a default init is done, and
	//! may not be what is expected in a further call to glInitRaptor for OpenGL.
	//!	RaptorCore shall be initialized prior to this call.
	//! @param dsp : if not NULL, a Raptor Display to be shared with Compute Contexts
	//! @return true if initialization without errors.
	static bool clInitRaptorCompute(void);

	//!	Releases computing platform
	//! @return true if all resources released without errors.
	static bool clReleaseRaptorCompute(void);

	//! Retrieve platform description, after initialization.
	//! @return the platforms descriptors.
	static std::vector<CRaptorCompute::PLATFORM> getPlatforms(void);


private:
	//! Contructor : forbidden
	CRaptorCompute(void);

	//! Destructor : forbidden
	virtual ~CRaptorCompute();
};

#endif	// defined(AFX_RAPTOR_COMPUTE_H__7FDED2CB_7AA2_4D6E_BF22_50923FD45212__INCLUDED_)

