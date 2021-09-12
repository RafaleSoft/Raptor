/***************************************************************************/
/*                                                                         */
/*  VulkanSurface.cpp                                                      */
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


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORVULKANSURFACE_H__C377C267_32A8_4963_BC2A_4694F4299A68__INCLUDED_)
	#include "Subsys/Vulkan/VulkanSurface.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORVKEXTENSIONS_H__B17D6B7F_5AFC_4E34_9D49_8DC6CE9192D6__INCLUDED_)
	#include "System/RaptorVKExtensions.h"
#endif
#if !defined(AFX_VULKAN_H__625F6BC5_F386_44C2_85C1_EDBA23B16921__INCLUDED_)
	#include "Subsys/Vulkan/RaptorVulkan.h"
#endif


RAPTOR_NAMESPACE


#if defined(VK_VERSION_1_0) && defined(VK_KHR_surface)
CVulkanSurface::CVulkanSurface(VkSurfaceKHR surface)
	:m_surface(surface),
	pSurfaceFormatCount(0),
	pSurfaceFormats(NULL),
	pPresentModeCount(0),
	pPresentModes(NULL),
	m_pExtensions(NULL)
#else
CVulkanSurface::CVulkanSurface()
	:pExtensions(NULL)
#endif
{
	num_formats = 5;
	depth_formats = new VkFormat[num_formats];
	for (size_t i = 0; i < num_formats;i++)
		depth_formats[i] = VK_FORMAT_UNDEFINED;
}


CVulkanSurface::~CVulkanSurface()
{
	cleanSurface();

	if (NULL != depth_formats)
		delete[] depth_formats;
	depth_formats = NULL;

#if defined(VK_VERSION_1_0) && defined(VK_KHR_surface)
	if ((VK_NULL_HANDLE != m_surface) &&
		(NULL != m_pExtensions))
	{
		m_pExtensions->vkDestroySurfaceKHR(CRaptorVKExtensions::getInstance(), m_surface, NULL);
	}
#endif
}

void CVulkanSurface::cleanSurface(void)
{
#if defined(VK_VERSION_1_0) 
	#if defined(VK_KHR_surface)
	if (NULL != pSurfaceFormats)
		delete[] pSurfaceFormats;
	pSurfaceFormats = NULL;

	pSurfaceFormatCount = 0;
	
	if (NULL != pPresentModes)
		delete[] pPresentModes;
	pPresentModes = NULL;
	
	pPresentModeCount = 0;
	#endif

	for (size_t i = 0; i < num_formats; i++)
		depth_formats[i] = VK_FORMAT_UNDEFINED;
#endif
}



#if defined(VK_VERSION_1_0)

bool CVulkanSurface::supportPresentation(VkPhysicalDevice device,
										 uint32_t queueFamily,
										 CRaptorVKExtensions *pExtensions)
{
	VkBool32 present_support = VK_FALSE;

#if defined(VK_KHR_surface)
	if ((NULL != pExtensions) && 
		(VK_NULL_HANDLE != device) &&
		(VK_NULL_HANDLE != m_surface))
		pExtensions->vkGetPhysicalDeviceSurfaceSupportKHR(device,
														  queueFamily,
														  m_surface,
														  &present_support);
#endif

	return (VK_TRUE == present_support);
}


VkSwapchainCreateInfoKHR CVulkanSurface::getSwapChainRequirements(	uint32_t display_mode,
																	uint32_t width,
																	uint32_t height)
{
	VkSurfaceFormatKHR format = getSurfaceFormat(display_mode);
	VkPresentModeKHR presentMode = getPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);

	VkResult res = VK_NOT_READY;
	CRaptorErrorManager *pErrMgr = IRaptor::GetErrorManager();
	
	VkExtent2D extent = getExtend();

#if defined(VK_KHR_surface)
	// width and height are either both -1, or both not -1.
	if (extent.width == -1)
	{
		extent.width = max(min(width,	surfaceCapabilities.maxImageExtent.width),
										surfaceCapabilities.minImageExtent.width);
		extent.height = max(min(height, surfaceCapabilities.maxImageExtent.height),
										surfaceCapabilities.minImageExtent.height);
	}
	VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;	// add transfer flag for clear
	VkSurfaceTransformFlagBitsKHR transform = surfaceCapabilities.currentTransform;
	if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

	//	mailbox will require at least 3 images for non blocking acquire of one image for the application.
	uint32_t numImages = min(max(3, surfaceCapabilities.minImageCount + 1),
									surfaceCapabilities.maxImageCount);

	//	Opaque alpha if supported
	VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	//if (context.surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
	//	composite = context.surfaceCapabilities.supportedCompositeAlpha;
#endif

	VkSwapchainCreateInfoKHR swapChainRequirements = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
											NULL, 0, m_surface,
											numImages,
											format.format, format.colorSpace,
											extent,
											1,	// imageArrayLayers in multiview/stereo surface
											usage,
											VK_SHARING_MODE_EXCLUSIVE, 0, NULL, // Exclusive => no queues share access
											transform, // VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
											composite, // VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR alpha ignored, final alpha set to 1
											presentMode,
											VK_TRUE,	// Hard clipping, no read back
											VK_NULL_HANDLE };

	return swapChainRequirements;
}

VkExtent2D CVulkanSurface::getExtend(void) const
{
	VkExtent2D extend = { 0, 0 };
#if defined(VK_KHR_surface)
	extend = surfaceCapabilities.currentExtent;
#endif

	return extend;
}

VkFormat CVulkanSurface::getDepthFormat(uint32_t display_mode, bool stencil_buffer) const
{
	VkFormat depth_format = VK_FORMAT_UNDEFINED;
	if (NULL == m_pExtensions)
		return depth_format;

	if ((display_mode & CGL_DEPTH_16) == CGL_DEPTH_16)
	{
		depth_format = VK_FORMAT_D16_UNORM;
		if (stencil_buffer)
			depth_format = VK_FORMAT_D16_UNORM_S8_UINT;
	}
	else if ((display_mode & CGL_DEPTH_24) == CGL_DEPTH_24)
	{
		depth_format = VK_FORMAT_D24_UNORM_S8_UINT;
		if (stencil_buffer)
			depth_format = VK_FORMAT_D24_UNORM_S8_UINT;
	}
	else if ((display_mode & CGL_DEPTH_32) == CGL_DEPTH_32)
	{
		depth_format = VK_FORMAT_D32_SFLOAT;
		if (stencil_buffer)
			depth_format = VK_FORMAT_D32_SFLOAT_S8_UINT;
	}

	VkFormat depth = VK_FORMAT_UNDEFINED;
	for (size_t i = 0; i < num_formats; i++)
		if (depth_format == depth_formats[i])
			depth = depth_format;

	return depth;
}

VkSurfaceFormatKHR CVulkanSurface::getSurfaceFormat(uint32_t display_mode) const
{
	VkSurfaceFormatKHR format = { VK_FORMAT_UNDEFINED, VK_COLORSPACE_SRGB_NONLINEAR_KHR };

#if defined(VK_KHR_surface)
	VkFormat requestedFormat = format.format;
	VkFormat requestedFormat2 = format.format;

	if ((display_mode & CGL_RGB) == CGL_RGB)
	{
		requestedFormat = VK_FORMAT_R8G8B8_UNORM;
		requestedFormat2 = VK_FORMAT_B8G8R8_UNORM;
	}
	else if ((display_mode & CGL_RGBA) == CGL_RGBA)
	{
		requestedFormat = VK_FORMAT_R8G8B8A8_UNORM;
		requestedFormat2 = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else if ((display_mode & CGL_FLOAT_32) == CGL_FLOAT_32)
	{
		requestedFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		requestedFormat2 = VK_FORMAT_R32G32B32A32_SFLOAT;
	}
	else if ((display_mode & CGL_FLOAT_16) == CGL_FLOAT_16)
	{
		requestedFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		requestedFormat2 = VK_FORMAT_R16G16B16A16_SFLOAT;
	}

	for (unsigned int i = 0; i<pSurfaceFormatCount; i++)
		if ((requestedFormat == pSurfaceFormats[i].format) ||
			(requestedFormat2 == pSurfaceFormats[i].format))
			format = pSurfaceFormats[i];
#endif

	return format;
}

VkPresentModeKHR CVulkanSurface::getPresentMode(VkPresentModeKHR preferedMode) const
{
	VkPresentModeKHR pMode = VK_PRESENT_MODE_MAX_ENUM_KHR;

#if defined(VK_KHR_surface)
	for (uint32_t i = 0; i<pPresentModeCount; i++)
		if (pPresentModes[i] == preferedMode)
			pMode = preferedMode;
	if (pMode == VK_PRESENT_MODE_MAX_ENUM_KHR)
		for (unsigned int i = 0; i<pPresentModeCount; i++)
			if (pPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				pMode = VK_PRESENT_MODE_MAILBOX_KHR;
	if (pMode == VK_PRESENT_MODE_MAX_ENUM_KHR)
		for (unsigned int i = 0; i<pPresentModeCount; i++)
			if (pPresentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
				pMode = VK_PRESENT_MODE_FIFO_KHR;
#endif
	return pMode;
}

bool CVulkanSurface::vkResize(VkPhysicalDevice device)
{
	if (NULL == m_pExtensions)
		return false;
	if (!m_pExtensions->vkIsExtensionSupported(VK_KHR_SURFACE_EXTENSION_NAME))
		return false;

	VkResult res = VK_NOT_READY;

#if defined(VK_KHR_surface)
	//!	GetSurface Capabilities
	res = m_pExtensions->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,
																 m_surface,
																 &surfaceCapabilities);
	if (VK_SUCCESS != res)
	{
		RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
					 "Failed to obtain Vulkan surface capabilities");
		return false;
	}
#endif

	return true;
}

bool CVulkanSurface::vkInitSurface(VkPhysicalDevice device,
								   CRaptorVKExtensions *pExtensions)
{
	cleanSurface();

	if (NULL == pExtensions)
		return false;
	if (!pExtensions->vkIsExtensionSupported(VK_KHR_SURFACE_EXTENSION_NAME))
		return false;

	m_pExtensions = pExtensions;

	VkResult res = VK_NOT_READY;

#if defined(VK_KHR_surface)
	//!	GetSurface Capabilities
	res = pExtensions->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(	device,
																	m_surface,
																	&surfaceCapabilities);
	if (VK_SUCCESS != res)
	{
		RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
					 "Failed to obtain Vulkan surface capabilities");
		return false;
	}

	//!	Get Surface formats
	res = pExtensions->vkGetPhysicalDeviceSurfaceFormatsKHR(device,
															m_surface,
															&pSurfaceFormatCount,
															NULL);
	if ((VK_SUCCESS != res) || (0 == pSurfaceFormatCount))
	{
		RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
					 "Failed to obtain Vulkan surface formats");
		res = VK_NOT_READY;
		return false;
	}
	else
	{
		pSurfaceFormats = new VkSurfaceFormatKHR[pSurfaceFormatCount];
		res = pExtensions->vkGetPhysicalDeviceSurfaceFormatsKHR(device,
																m_surface,
																&pSurfaceFormatCount,
																pSurfaceFormats);
		if (VK_SUCCESS != res)
		{
			RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
						 "Failed to obtain Vulkan surface formats");
		}
	}

	if (VK_SUCCESS == res)
		res = pExtensions->vkGetPhysicalDeviceSurfacePresentModesKHR(	device,
																		m_surface,
																		&pPresentModeCount,
																		NULL);
	if ((VK_SUCCESS != res) || (0 == pPresentModeCount))
	{
		RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
					 "Failed to obtain Vulkan surface present modes or no present modes available");
		res = VK_NOT_READY;
	}
	else
	{
		pPresentModes = new VkPresentModeKHR[pPresentModeCount];
		res = pExtensions->vkGetPhysicalDeviceSurfacePresentModesKHR(	device,
																		m_surface,
																		&pPresentModeCount,
																		pPresentModes);
		if (VK_SUCCESS != res)
		{
			RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
						 "Failed to obtain Vulkan surface formats");
		}
	}
#endif

	//!
	//!	Depth formats for optimal tiling
	VkFormatProperties props;
	depth_formats[0] = VK_FORMAT_D16_UNORM;
	depth_formats[1] = VK_FORMAT_D16_UNORM_S8_UINT;
	depth_formats[2] = VK_FORMAT_D24_UNORM_S8_UINT;
	depth_formats[3] = VK_FORMAT_D32_SFLOAT;
	depth_formats[4] = VK_FORMAT_D32_SFLOAT_S8_UINT;
	
	for (size_t i = 0; i < num_formats; i++)
	{
		VkFormat depth_format = depth_formats[i];
		CRaptorVKExtensions::vkGetPhysicalDeviceFormatProperties(device, depth_format, &props);
		if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			depth_formats[i] = VK_FORMAT_UNDEFINED;
		}
	}

	//	Clean Surface formats and present modes
	if (VK_SUCCESS != res)
	{
		cleanSurface();
		return false;
	}

	return true;
}
#endif

