#ifndef __vkext_macros_h_
#define __vkext_macros_h_

#ifndef __RAPTOR_VKEXT_H__
	#include "vkext.h"
#endif


//!
//!
//!     Macro definition for extensions declaration
//!
//!

#if defined(VK_VERSION_1_0)
	#ifndef IMPLEMENT_RAPTOR_VK_instance
	#define IMPLEMENT_RAPTOR_VK_instance(target) \
		PFN_vkEnumeratePhysicalDevices					target::vkEnumeratePhysicalDevices = NULL; \
		PFN_vkEnumerateDeviceExtensionProperties		target::vkEnumerateDeviceExtensionProperties = NULL; \
		PFN_vkEnumerateDeviceLayerProperties			target::vkEnumerateDeviceLayerProperties = NULL; \
		PFN_vkGetPhysicalDeviceProperties				target::vkGetPhysicalDeviceProperties = NULL; \
		PFN_vkGetPhysicalDeviceFeatures					target::vkGetPhysicalDeviceFeatures = NULL; \
		PFN_vkGetPhysicalDeviceQueueFamilyProperties	target::vkGetPhysicalDeviceQueueFamilyProperties = NULL; \
		PFN_vkGetPhysicalDeviceMemoryProperties			target::vkGetPhysicalDeviceMemoryProperties = NULL; \
		PFN_vkDestroyInstance							target::vkDestroyInstance = NULL; \
		PFN_vkCreateDevice								target::vkCreateDevice = NULL; \
		PFN_vkGetDeviceProcAddr							target::vkGetDeviceProcAddr = NULL;
	#endif
#else
	#define IMPLEMENT_RAPTOR_VK_instance(target)
#endif

//!
//!
//!     Macro definition for extensions initialisation
//!
//!

#ifndef IMPLEMENT_VK_instance
	#if defined(VK_VERSION_1_0)
		#define IMPLEMENT_VK_instance(target,instance)\
		{\
			target##vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices")); \
			target##vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)(vkGetInstanceProcAddr(instance, "vkEnumerateDeviceExtensionProperties")); \
			target##vkEnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties)(vkGetInstanceProcAddr(instance, "vkEnumerateDeviceLayerProperties")); \
			target##vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties")); \
			target##vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures")); \
			target##vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties")); \
			target##vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties")); \
			target##vkDestroyInstance = (PFN_vkDestroyInstance)(vkGetInstanceProcAddr(instance, "vkDestroyInstance")); \
			target##vkCreateDevice = (PFN_vkCreateDevice)(vkGetInstanceProcAddr(instance, "vkCreateDevice")); \
			target##vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)(vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr")); \
		}
	#else
		#define IMPLEMENT_VK_instance(target)
	#endif	// defined(VK_VERSION_1_0)
#endif

#ifndef IMPLEMENT_VK_win32
	#if defined(VK_VERSION_1_0) && defined(VK_USE_PLATFORM_WIN32_KHR) && defined(VK_KHR_win32_surface)
		#define IMPLEMENT_VK_win32(target,instance)\
		{\
			target##vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)(vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR")); \
			target##vkGetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR"));\
		}
	#else
		#define IMPLEMENT_VK_win32(target,instance)
	#endif // defined(VK_VERSION_1_0) && defined(VK_USE_PLATFORM_WIN32_KHR) defined(VK_KHR_win32_surface)
#endif

#ifndef IMPLEMENT_VK_xlib
	#if defined(VK_VERSION_1_0) && defined(VK_USE_PLATFORM_XLIB_KHR) && defined(VK_KHR_win32_surface)
		#define IMPLEMENT_VK_xlib(target,instance)\
		{\
			target##vkCreateXlibSurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)(vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR")); \
			target##vkGetPhysicalDeviceXlibPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceXlibPresentationSupportKHR"));\
		}
	#else
		#define IMPLEMENT_VK_xlib(target,instance)
	#endif // defined(VK_VERSION_1_0) && defined(VK_USE_PLATFORM_XLIB_KHR) && defined(VK_KHR_win32_surface)
#endif

#endif	// __vkext_macros_h_

