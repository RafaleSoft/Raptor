#if !defined(AFX_RAPTOR_COMPUTE_H__7FDED2CB_7AA2_4D6E_BF22_50923FD45212__INCLUDED_)
#define AFX_RAPTOR_COMPUTE_H__7FDED2CB_7AA2_4D6E_BF22_50923FD45212__INCLUDED_

#ifdef EXPORT_RAPTOR_COMPUTE
	#define RAPTORCOMPUTE_API __declspec(dllexport)
#else
	#define RAPTORCOMPUTE_API __declspec(dllimport)
#endif


#include <string>
#include <vector>

class CRaptorComputeManager;
class CRaptorComputeJob;


//! Raptor Compute entry point
//!
class RAPTORCOMPUTE_API CRaptorCompute
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
		unsigned __int32	compiler_available;
		unsigned __int32	available;
		unsigned __int32	endian_little;
		size_t				timer_resolution;
		unsigned __int32	error_correction;
		unsigned __int64	local_memory_size;
		unsigned long		local_memory_type;
		unsigned __int32	max_constants;
		unsigned __int64	max_constant_buffer;
		unsigned __int64	global_memory_size;
		unsigned __int64	global_memory_cache_size;
		unsigned __int32	global_memory_cacheline_size;
		unsigned long		global_memory_cache_type;
		unsigned __int64	single_fp_config;
		unsigned __int32	min_align_size;
		unsigned __int32	base_address_align;
		unsigned __int32	max_samplers;
		size_t				max_parameter_size;
		size_t				image3d_max_width;
		size_t				image3d_max_height;
		size_t				image3d_max_depth;
		size_t				image2d_max_width;
		size_t				image2d_max_height;
		unsigned __int32	max_image_write;
		unsigned __int32	max_image_read;
		unsigned long		image_support;
		unsigned __int64	max_alloc_size;
		unsigned __int32	address_size;
		unsigned __int32	frequency;
		unsigned __int32	max_compute_units;
		unsigned __int32	max_work_item_dimensions;
		size_t				*max_work_item_sizes;
		size_t				max_work_item_group_size;
		unsigned __int32	vector_width_char;
		unsigned __int32	vector_width_short;
		unsigned __int32	vector_width_int;
		unsigned __int32	vector_width_long;
		unsigned __int32	vector_width_float;
		unsigned __int32	vector_width_double;
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
	static bool CRaptorCompute::clReleaseRaptorCompute(void);

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

