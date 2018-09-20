// VulkanSurface.h: interface for the CVulkanDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORVULKANSURFACE_H__C377C267_32A8_4963_BC2A_4694F4299A68__INCLUDED_)
#define AFX_RAPTORVULKANSURFACE_H__C377C267_32A8_4963_BC2A_4694F4299A68__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __vkext_macros_h_
	#include "System/VKEXTMacros.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CRaptorVKExtensions;


class CVulkanSurface
{
public:
#if defined(VK_VERSION_1_0) && defined(VK_KHR_surface)
	CVulkanSurface(VkSurfaceKHR surface);
#else
	CVulkanSurface();
#endif

	virtual ~CVulkanSurface();

#if defined(VK_VERSION_1_0)
	//!
	//!	Initialise surface formats, capabilities and present modes
	//!
	bool vkInitSurface(VkPhysicalDevice device,
					   CRaptorVKExtensions *pExtensions);

	//!
	//!	Update surface capabilities when device surface is resized.
	//!
	bool vkResize(VkPhysicalDevice device);

	//!
	//!	Determine if queue family supports presentation on this surface.
	//!
	bool supportPresentation(VkPhysicalDevice device, uint32_t queueFamily, CRaptorVKExtensions *pExtensions);

	//!
	//!	Finds a suitable present mode matching as close as possible the preferred mode.
	//!
	VkPresentModeKHR getPresentMode(VkPresentModeKHR preferedMode) const;

	//!
	//!	Finds a suitable surface format matching as close as possible the display mode.
	//!
	VkSurfaceFormatKHR getSurfaceFormat(uint32_t display_mode) const;

	//!
	//!	Finds a suitable depth format matching as close as possible the display mode.
	//!
	VkFormat getDepthFormat(uint32_t display_mode, bool stencil_buffer) const;

	//!
	//!	Returns surface physical dimensions
	//!
	VkExtent2D getExtend(void) const;


	//!
	//!	Returns swap chain info conformant with surface capabilities
	//!
	VkSwapchainCreateInfoKHR CVulkanSurface::getSwapChainRequirements(	uint32_t display_mode,
																		uint32_t width,
																		uint32_t height);
#endif


private:
	//!
	//!	Forbidden constructor and operators
	//!
	CVulkanSurface();
	CVulkanSurface(const CVulkanSurface&);
	CVulkanSurface& operator=(const CVulkanSurface&);

	//!
	//!	Clean surface resources.
	//!
	void cleanSurface(void);

	CRaptorVKExtensions *m_pExtensions;

	//!	Number of supported depth formats
	size_t		num_formats;

#if defined(VK_VERSION_1_0) 
	#if defined(VK_KHR_surface)
	VkSurfaceKHR m_surface;

	VkSurfaceCapabilitiesKHR	surfaceCapabilities;
	uint32_t					pSurfaceFormatCount;
	VkSurfaceFormatKHR			*pSurfaceFormats;
	uint32_t					pPresentModeCount;
	VkPresentModeKHR			*pPresentModes;
	#endif

	//!	Supported depth formats
	VkFormat	*depth_formats;
#endif
};

RAPTOR_NAMESPACE_END


#endif // !defined(AFX_RAPTORVULKANSURFACE_H__C377C267_32A8_4963_BC2A_4694F4299A68__INCLUDED_)