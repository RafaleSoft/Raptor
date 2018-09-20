// RaptorExtensions.h: interface for the RaptorExtensions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORVKEXTENSIONS_H__B17D6B7F_5AFC_4E34_9D49_8DC6CE9192D6__INCLUDED_)
#define AFX_RAPTORVKEXTENSIONS_H__B17D6B7F_5AFC_4E34_9D49_8DC6CE9192D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(__RAPTOR_VKEXT_H__)
	#include "vkext.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//! This class handles all entry points to OGL extensions.
//! An instance of this class is the one and only one access to these entry points.
class RAPTOR_API CRaptorVKExtensions
{
public:
	CRaptorVKExtensions(const std::string &ext);
	virtual ~CRaptorVKExtensions();

#if defined(VK_VERSION_1_0)
    //! Use STATIC_LINKAGE for Vulkan module extensions as they are necessary for
    //! all kind of commands for rendering context management
	DECLARE_VK_get_instance_proc_addr(STATIC_LINKAGE)
	DECLARE_VK_global(STATIC_LINKAGE)
	
	//! Vulkan instance entry points are realative to Vulkan library entry point, 
	//!	so need to declare it as STATIC_LINKAGE since we have one Vulkan instance per application
	//!	Multiple instance per application shall come later.
	DECLARE_VK_instance(STATIC_LINKAGE)

	DECLARE_VK_win32(DEFAULT_LINKAGE)
	DECLARE_VK_xlib(DEFAULT_LINKAGE)
	DECLARE_VK_KHR_surface(DEFAULT_LINKAGE)
#endif

	//!	Initialise instance entry points and extensions
#if defined(VK_VERSION_1_0)
	static bool vkInitInstanceExtensions(void);

	bool vkInitDeviceExtensions(VkPhysicalDevice device);

	static VkInstance getInstance(void) { return instance; };
#endif

	
	//! Search for a specific extension string in all available extensions.
	//! Extensions must have been initialized with a valid Vulkan Instance
	//! or Vulkan device from which extensions are extracted.
	//! @return true is ext is found in extensions list, false otherwise.
	bool vkIsExtensionSupported(const std::string &ext) const;
	
	//! Same requirements as above.
	//! @return the list of all loaded extensions.
	const std::string& vkExtensions(void) const
	{ return extensions; }



private:
	//!	Forbidden constructor
	CRaptorVKExtensions();

	//!	Initialise global Vulkan entry points.
	static bool vkInitExtensions(void);

	
#if defined(VK_VERSION_1_0)
	//!	Vulkan application Instance
	static VkInstance instance;
#endif

	//!	String buffer containing instance extensions
	static std::string instance_extensions;
	
	//! String buffers containing capabilities of a Vulkan physical device.
	std::string	extensions;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVKEXTENSIONS_H__B17D6B7F_5AFC_4E34_9D49_8DC6CE9192D6__INCLUDED_)

