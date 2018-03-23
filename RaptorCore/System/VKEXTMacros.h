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
		PFN_vkEnumeratePhysicalDevices					target::vkEnumeratePhysicalDevices = VK_NULL_HANDLE; \
		PFN_vkEnumerateDeviceExtensionProperties		target::vkEnumerateDeviceExtensionProperties = VK_NULL_HANDLE; \
		PFN_vkEnumerateDeviceLayerProperties			target::vkEnumerateDeviceLayerProperties = VK_NULL_HANDLE; \
		PFN_vkGetPhysicalDeviceProperties				target::vkGetPhysicalDeviceProperties = VK_NULL_HANDLE; \
		PFN_vkGetPhysicalDeviceFeatures					target::vkGetPhysicalDeviceFeatures = VK_NULL_HANDLE; \
		PFN_vkGetPhysicalDeviceQueueFamilyProperties	target::vkGetPhysicalDeviceQueueFamilyProperties = VK_NULL_HANDLE; \
		PFN_vkGetPhysicalDeviceMemoryProperties			target::vkGetPhysicalDeviceMemoryProperties = VK_NULL_HANDLE; \
		PFN_vkDestroyInstance							target::vkDestroyInstance = VK_NULL_HANDLE; \
		PFN_vkCreateDevice								target::vkCreateDevice = VK_NULL_HANDLE; \
		PFN_vkGetDeviceProcAddr							target::vkGetDeviceProcAddr = VK_NULL_HANDLE;
	#endif
#else
	#define IMPLEMENT_RAPTOR_VK_instance(target)
#endif

#if defined(VK_VERSION_1_0)
	#ifndef IMPLEMENT_RAPTOR_VK_device_memory
	#define IMPLEMENT_RAPTOR_VK_device_memory(target) \
		PFN_vkGetBufferMemoryRequirements		target::vkGetBufferMemoryRequirements = VK_NULL_HANDLE; \
		PFN_vkGetImageMemoryRequirements		target::vkGetImageMemoryRequirements = VK_NULL_HANDLE; \
		PFN_vkGetImageSparseMemoryRequirements	target::vkGetImageSparseMemoryRequirements = VK_NULL_HANDLE; \
		PFN_vkCreateBuffer						target::vkCreateBuffer = VK_NULL_HANDLE; \
		PFN_vkDestroyBuffer						target::vkDestroyBuffer = VK_NULL_HANDLE; \
		PFN_vkAllocateMemory					target::vkAllocateMemory = VK_NULL_HANDLE; \
		PFN_vkBindBufferMemory					target::vkBindBufferMemory = VK_NULL_HANDLE; \
		PFN_vkCreateImage						target::vkCreateImage = VK_NULL_HANDLE; \
		PFN_vkDestroyImage						target::vkDestroyImage = VK_NULL_HANDLE; \
		PFN_vkBindImageMemory					target::vkBindImageMemory = VK_NULL_HANDLE; \
		PFN_vkFreeMemory						target::vkFreeMemory = VK_NULL_HANDLE; \
		PFN_vkMapMemory							target::vkMapMemory = VK_NULL_HANDLE; \
		PFN_vkFlushMappedMemoryRanges			target::vkFlushMappedMemoryRanges = VK_NULL_HANDLE; \
		PFN_vkUnmapMemory						target::vkUnmapMemory = VK_NULL_HANDLE;
	#endif
#else
	#define IMPLEMENT_RAPTOR_VK_device_memory(target)
#endif

#if defined(VK_VERSION_1_0)
	#ifndef IMPLEMENT_RAPTOR_VK_command_buffer
	#define IMPLEMENT_RAPTOR_VK_command_buffer(target) \
		PFN_vkCmdPipelineBarrier	target::vkCmdPipelineBarrier = VK_NULL_HANDLE; \
		PFN_vkCmdBeginRenderPass	target::vkCmdBeginRenderPass = VK_NULL_HANDLE; \
		PFN_vkCmdEndRenderPass		target::vkCmdEndRenderPass = VK_NULL_HANDLE; \
		PFN_vkCmdBindPipeline		target::vkCmdBindPipeline = VK_NULL_HANDLE; \
		PFN_vkBeginCommandBuffer	target::vkBeginCommandBuffer = VK_NULL_HANDLE; \
		PFN_vkEndCommandBuffer		target::vkEndCommandBuffer = VK_NULL_HANDLE; \
		PFN_vkCmdBindVertexBuffers	target::vkCmdBindVertexBuffers = VK_NULL_HANDLE; \
		PFN_vkCmdBindIndexBuffer	target::vkCmdBindIndexBuffer = VK_NULL_HANDLE; \
		PFN_vkCmdBindDescriptorSets	target::vkCmdBindDescriptorSets = VK_NULL_HANDLE; \
		PFN_vkCmdSetViewport		target::vkCmdSetViewport = VK_NULL_HANDLE; \
		PFN_vkCmdSetScissor			target::vkCmdSetScissor = VK_NULL_HANDLE; \
		PFN_vkCmdCopyBuffer			target::vkCmdCopyBuffer = VK_NULL_HANDLE; \
		PFN_vkCmdCopyImage			target::vkCmdCopyImage = VK_NULL_HANDLE; \
		PFN_vkCmdBlitImage			target::vkCmdBlitImage = VK_NULL_HANDLE; \
		PFN_vkCmdCopyBufferToImage	target::vkCmdCopyBufferToImage = VK_NULL_HANDLE; \
		PFN_vkCmdCopyImageToBuffer	target::vkCmdCopyImageToBuffer = VK_NULL_HANDLE; \
		PFN_vkCmdDraw				target::vkCmdDraw = VK_NULL_HANDLE; \
		PFN_vkCmdDrawIndexed		target::vkCmdDrawIndexed = VK_NULL_HANDLE;
	#endif
#else
	#define IMPLEMENT_RAPTOR_VK_command_buffer(target)
#endif

#if defined(VK_VERSION_1_0)
	#ifndef IMPLEMENT_RAPTOR_VK_pipeline
	#define IMPLEMENT_RAPTOR_VK_pipeline(target) \
		PFN_vkCreateGraphicsPipelines	target::vkCreateGraphicsPipelines = VK_NULL_HANDLE; \
		PFN_vkCreateComputePipelines	target::vkCreateComputePipelines = VK_NULL_HANDLE; \
		PFN_vkDestroyPipeline			target::vkDestroyPipeline = VK_NULL_HANDLE; \
		PFN_vkCreatePipelineLayout		target::vkCreatePipelineLayout = VK_NULL_HANDLE; \
		PFN_vkDestroyPipelineLayout		target::vkDestroyPipelineLayout = VK_NULL_HANDLE; \
		PFN_vkCreateShaderModule		target::vkCreateShaderModule = VK_NULL_HANDLE; \
		PFN_vkDestroyShaderModule		target::vkDestroyShaderModule = VK_NULL_HANDLE; \
		PFN_vkCreateDescriptorSetLayout	target::vkCreateDescriptorSetLayout = VK_NULL_HANDLE; \
		PFN_vkDestroyDescriptorSetLayout	target::vkDestroyDescriptorSetLayout = VK_NULL_HANDLE; \
		PFN_vkGetImageSubresourceLayout	target::vkGetImageSubresourceLayout = VK_NULL_HANDLE; \
		PFN_vkCreatePipelineCache		target::vkCreatePipelineCache = VK_NULL_HANDLE; \
		PFN_vkDestroyPipelineCache		target::vkDestroyPipelineCache = VK_NULL_HANDLE; \
		PFN_vkGetPipelineCacheData		target::vkGetPipelineCacheData = VK_NULL_HANDLE; \
		PFN_vkMergePipelineCaches		target::vkMergePipelineCaches = VK_NULL_HANDLE; \
		PFN_vkCreateDescriptorPool		target::vkCreateDescriptorPool = VK_NULL_HANDLE; \
		PFN_vkDestroyDescriptorPool		target::vkDestroyDescriptorPool = VK_NULL_HANDLE; \
		PFN_vkResetDescriptorPool		target::vkResetDescriptorPool = VK_NULL_HANDLE; \
		PFN_vkAllocateDescriptorSets	target::vkAllocateDescriptorSets = VK_NULL_HANDLE; \
		PFN_vkFreeDescriptorSets		target::vkFreeDescriptorSets = VK_NULL_HANDLE; \
		PFN_vkUpdateDescriptorSets		target::vkUpdateDescriptorSets = VK_NULL_HANDLE;
	#endif
#else
	#define IMPLEMENT_RAPTOR_VK_pipeline(target)
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
	#if defined(VK_VERSION_1_0) && defined(VK_USE_PLATFORM_XLIB_KHR) && defined(VK_KHR_xlib_surface)
		#define IMPLEMENT_VK_xlib(target,instance)\
		{\
			target##vkCreateXlibSurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)(vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR")); \
			target##vkGetPhysicalDeviceXlibPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceXlibPresentationSupportKHR"));\
		}
	#else
		#define IMPLEMENT_VK_xlib(target,instance)
	#endif // defined(VK_VERSION_1_0) && defined(VK_USE_PLATFORM_XLIB_KHR) && defined(VK_KHR_win32_surface)
#endif


#ifndef IMPLEMENT_VK_KHR_surface
	#if defined(VK_VERSION_1_0) && defined(VK_KHR_surface)
		#define IMPLEMENT_VK_KHR_surface(target,instance)\
		{\
			target##vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)(vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR")); \
			target##vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR")); \
			target##vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR")); \
			target##vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR")); \
			target##vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR")); \
		}
	#else
		#define IMPLEMENT_VK_KHR_surface(target,instance)
	#endif // defined(VK_VERSION_1_0) && defined(VK_KHR_surface)
#endif

#ifndef IMPLEMENT_VK_device
	#if defined(VK_VERSION_1_0)
		#define IMPLEMENT_VK_device(target,device)\
		{\
			target##vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)(vkGetDeviceProcAddr(device, "vkDeviceWaitIdle")); \
			target##vkDestroyDevice = (PFN_vkDestroyDevice)(vkGetDeviceProcAddr(device, "vkDestroyDevice")); \
			target##vkCreateCommandPool = (PFN_vkCreateCommandPool)(vkGetDeviceProcAddr(device, "vkCreateCommandPool")); \
			target##vkDestroyCommandPool = (PFN_vkDestroyCommandPool)(vkGetDeviceProcAddr(device, "vkDestroyCommandPool")); \
			target##vkResetCommandPool = (PFN_vkResetCommandPool)(vkGetDeviceProcAddr(device, "vkResetCommandPool")); \
			target##vkGetDeviceQueue = (PFN_vkGetDeviceQueue)(vkGetDeviceProcAddr(device, "vkGetDeviceQueue")); \
			target##vkCreateSemaphore = (PFN_vkCreateSemaphore)(vkGetDeviceProcAddr(device, "vkCreateSemaphore")); \
			target##vkDestroySemaphore = (PFN_vkDestroySemaphore)(vkGetDeviceProcAddr(device, "vkDestroySemaphore")); \
			target##vkCreateFence = (PFN_vkCreateFence)(vkGetDeviceProcAddr(device, "vkCreateFence")); \
			target##vkWaitForFences = (PFN_vkWaitForFences)(vkGetDeviceProcAddr(device, "vkWaitForFences")); \
			target##vkResetFences = (PFN_vkResetFences)(vkGetDeviceProcAddr(device, "vkResetFences")); \
			target##vkDestroyFence = (PFN_vkDestroyFence)(vkGetDeviceProcAddr(device, "vkDestroyFence")); \
			target##vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)(vkGetDeviceProcAddr(device, "vkAllocateCommandBuffers")); \
			target##vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)(vkGetDeviceProcAddr(device, "vkFreeCommandBuffers")); \
			target##vkCreateRenderPass = (PFN_vkCreateRenderPass)(vkGetDeviceProcAddr(device, "vkCreateRenderPass")); \
			target##vkDestroyRenderPass = (PFN_vkDestroyRenderPass)(vkGetDeviceProcAddr(device, "vkDestroyRenderPass")); \
			target##vkCreateImageView = (PFN_vkCreateImageView)(vkGetDeviceProcAddr(device, "vkCreateImageView")); \
			target##vkDestroyImageView = (PFN_vkDestroyImageView)(vkGetDeviceProcAddr(device, "vkDestroyImageView")); \
			target##vkCreateFramebuffer = (PFN_vkCreateFramebuffer)(vkGetDeviceProcAddr(device, "vkCreateFramebuffer")); \
			target##vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)(vkGetDeviceProcAddr(device, "vkDestroyFramebuffer"));\
		}
	#else
		#define IMPLEMENT_VK_device(target,device)
	#endif // defined(VK_VERSION_1_0)
#endif

#ifndef IMPLEMENT_VK_device_memory
	#if defined(VK_VERSION_1_0)
		#define IMPLEMENT_VK_device_memory(target,device)\
		{\
			target##vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)(vkGetInstanceProcAddr(instance, "vkGetBufferMemoryRequirements")); \
			target##vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)(vkGetInstanceProcAddr(instance, "vkGetImageMemoryRequirements")); \
			target##vkGetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)(vkGetInstanceProcAddr(instance, "vkGetImageSparseMemoryRequirements")); \
			target##vkCreateBuffer = (PFN_vkCreateBuffer)(vkGetInstanceProcAddr(instance, "vkCreateBuffer")); \
			target##vkDestroyBuffer = (PFN_vkDestroyBuffer)(vkGetInstanceProcAddr(instance, "vkDestroyBuffer")); \
			target##vkAllocateMemory = (PFN_vkAllocateMemory)(vkGetInstanceProcAddr(instance, "vkAllocateMemory")); \
			target##vkBindBufferMemory = (PFN_vkBindBufferMemory)(vkGetInstanceProcAddr(instance, "vkBindBufferMemory")); \
			target##vkCreateImage = (PFN_vkCreateImage)(vkGetInstanceProcAddr(instance, "vkCreateImage")); \
			target##vkDestroyImage = (PFN_vkDestroyImage)(vkGetInstanceProcAddr(instance, "vkDestroyImage")); \
			target##vkBindImageMemory = (PFN_vkBindImageMemory)(vkGetInstanceProcAddr(instance, "vkBindImageMemory")); \
			target##vkFreeMemory = (PFN_vkFreeMemory)(vkGetInstanceProcAddr(instance, "vkFreeMemory")); \
			target##vkMapMemory = (PFN_vkMapMemory)(vkGetInstanceProcAddr(instance, "vkMapMemory")); \
			target##vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)(vkGetInstanceProcAddr(instance, "vkFlushMappedMemoryRanges")); \
			target##vkUnmapMemory = (PFN_vkUnmapMemory)(vkGetInstanceProcAddr(instance, "vkUnmapMemory")); \
		}
	#else
		#define IMPLEMENT_VK_device_memory(target,device)
	#endif	// defined(VK_VERSION_1_0)
#endif

#ifndef IMPLEMENT_VK_KHR_swapchain
	#if defined(VK_VERSION_1_0) && defined(VK_KHR_swapchain)
		#define IMPLEMENT_VK_KHR_swapchain(target,device) \
		{\
			target##vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)(vkGetDeviceProcAddr(device, "vkCreateSwapchainKHR")); \
			target##vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)(vkGetDeviceProcAddr(device, "vkDestroySwapchainKHR")); \
			target##vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)(vkGetDeviceProcAddr(device, "vkGetSwapchainImagesKHR")); \
			target##vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)(vkGetDeviceProcAddr(device, "vkAcquireNextImageKHR"));\
			target##vkQueuePresentKHR = (PFN_vkQueuePresentKHR)(vkGetDeviceProcAddr(device, "vkQueuePresentKHR"));\
		}
	#else
		#define IMPLEMENT_VK_KHR_swapchain(target,device)
	#endif
#endif

#ifndef IMPLEMENT_VK_command_buffer
	#if defined(VK_VERSION_1_0)
		#define IMPLEMENT_VK_command_buffer(target,device) \
		{\
			target##vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)(vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier")); \
			target##vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)(vkGetDeviceProcAddr(device, "vkCmdBeginRenderPass")); \
			target##vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)(vkGetDeviceProcAddr(device, "vkCmdEndRenderPass")); \
			target##vkCmdBindPipeline = (PFN_vkCmdBindPipeline)(vkGetDeviceProcAddr(device, "vkCmdBindPipeline")); \
			target##vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)(vkGetDeviceProcAddr(device, "vkBeginCommandBuffer")); \
			target##vkEndCommandBuffer = (PFN_vkEndCommandBuffer)(vkGetDeviceProcAddr(device, "vkEndCommandBuffer")); \
			target##vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)(vkGetDeviceProcAddr(device, "vkCmdBindVertexBuffers")); \
			target##vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)(vkGetDeviceProcAddr(device, "vkCmdBindIndexBuffer")); \
			target##vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)(vkGetDeviceProcAddr(device, "vkCmdBindDescriptorSets")); \
			target##vkCmdSetViewport = (PFN_vkCmdSetViewport)(vkGetDeviceProcAddr(device, "vkCmdSetViewport")); \
			target##vkCmdSetScissor = (PFN_vkCmdSetScissor)(vkGetDeviceProcAddr(device, "vkCmdSetScissor")); \
			target##vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)(vkGetDeviceProcAddr(device, "vkCmdCopyBuffer")); \
			target##vkCmdCopyImage = (PFN_vkCmdCopyImage)(vkGetDeviceProcAddr(device, "vkCmdCopyImage")); \
			target##vkCmdBlitImage = (PFN_vkCmdBlitImage)(vkGetDeviceProcAddr(device, "vkCmdBlitImage")); \
			target##vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)(vkGetDeviceProcAddr(device, "vkCmdCopyBufferToImage")); \
			target##vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)(vkGetDeviceProcAddr(device, "vkCmdCopyImageToBuffer")); \
			target##vkCmdDraw = (PFN_vkCmdDraw)(vkGetDeviceProcAddr(device, "vkCmdDraw")); \
			target##vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)(vkGetDeviceProcAddr(device, "vkCmdDrawIndexed")); \
		}
	#else
		#define IMPLEMENT_VK_command_buffer(target,device)
	#endif
#endif

#ifndef IMPLEMENT_VK_pipeline
	#if defined(VK_VERSION_1_0)
	#define IMPLEMENT_VK_pipeline(target,device) \
	{\
		target##vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)(vkGetDeviceProcAddr(device, "vkCreateGraphicsPipelines")); \
		target##vkCreateComputePipelines = (PFN_vkCreateComputePipelines)(vkGetDeviceProcAddr(device, "vkCreateComputePipelines")); \
		target##vkDestroyPipeline = (PFN_vkDestroyPipeline)(vkGetDeviceProcAddr(device, "vkDestroyPipeline")); \
		target##vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)(vkGetDeviceProcAddr(device, "vkCreatePipelineLayout")); \
		target##vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)(vkGetDeviceProcAddr(device, "vkDestroyPipelineLayout")); \
		target##vkCreateShaderModule = (PFN_vkCreateShaderModule)(vkGetDeviceProcAddr(device, "vkCreateShaderModule")); \
		target##vkDestroyShaderModule = (PFN_vkDestroyShaderModule)(vkGetDeviceProcAddr(device, "vkDestroyShaderModule")); \
		target##vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)(vkGetDeviceProcAddr(device, "vkCreateDescriptorSetLayout")); \
		target##vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)(vkGetDeviceProcAddr(device, "vkDestroyDescriptorSetLayout")); \
		target##vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)(vkGetDeviceProcAddr(device, "vkGetImageSubresourceLayout")); \
		target##vkCreatePipelineCache = (PFN_vkCreatePipelineCache)(vkGetDeviceProcAddr(device, "vkCreatePipelineCache")); \
		target##vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)(vkGetDeviceProcAddr(device, "vkDestroyPipelineCache")); \
		target##vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)(vkGetDeviceProcAddr(device, "vkGetPipelineCacheData")); \
		target##vkMergePipelineCaches = (PFN_vkMergePipelineCaches)(vkGetDeviceProcAddr(device, "vkMergePipelineCaches")); \
		target##vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)(vkGetDeviceProcAddr(device, "vkCreateDescriptorPool")); \
		target##vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)(vkGetDeviceProcAddr(device, "vkDestroyDescriptorPool")); \
		target##vkResetDescriptorPool = (PFN_vkResetDescriptorPool)(vkGetDeviceProcAddr(device, "vkResetDescriptorPool")); \
		target##vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)(vkGetDeviceProcAddr(device, "vkAllocateDescriptorSets")); \
		target##vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)(vkGetDeviceProcAddr(device, "vkFreeDescriptorSets")); \
		target##vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)(vkGetDeviceProcAddr(device, "vkUpdateDescriptorSets")); \
	}
	#else
		#define IMPLEMENT_VK_pipeline(target,device)
	#endif
#endif

#endif	// __vkext_macros_h_

