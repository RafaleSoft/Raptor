//!	This header provides acces to VULKAN API.

#ifndef __RAPTOR_VKEXT_H__
#define __RAPTOR_VKEXT_H__

#include "vk_platform.h"

//!	Provide Windows WSI
#if defined(_WIN32)
	#define VK_USE_PLATFORM_WIN32_KHR 1
#elif defined(LINUX)
	#define VK_USE_PLATFORM_XLIB_KHR 1
#endif

#include "vulkan.h"

/*
	Vulkan extensions features for Raptor framework
*/
#include "Subsys/CodeGeneration.h"

#ifdef __cplusplus
	extern "C" {
#endif

#if defined(VK_VERSION_1_0)

	#if !defined(DECLARE_VK_win32)
		#if defined(VK_USE_PLATFORM_WIN32_KHR)
			#define DECLARE_VK_win32(LINKAGE) \
				LINKAGE PFN_vkCreateWin32SurfaceKHR	vkCreateWin32SurfaceKHR; \
				LINKAGE PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR	vkGetPhysicalDeviceWin32PresentationSupportKHR;
		#else
			#define DECLARE_VK_win32(LINKAGE)
		#endif
	#endif

	#if !defined(DECLARE_VK_xlib)
		#if defined(VK_USE_PLATFORM_XLIB_KHR)
			#define DECLARE_VK_xlib(LINKAGE) \
				LINKAGE PFN_vkCreateXlibSurfaceKHR	vkCreateXlibSurfaceKHR; \
				LINKAGE PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR	vkGetPhysicalDeviceXlibPresentationSupportKHR;
		#else
			#define DECLARE_VK_xlib(LINKAGE)
		#endif
	#endif

	#if !defined(DECLARE_VK_KHR_surface)
		#if defined(VK_KHR_surface)
			#define DECLARE_VK_KHR_surface(LINKAGE) \
				LINKAGE PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR; \
				LINKAGE PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR; \
				LINKAGE PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR; \
				LINKAGE PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR; \
				LINKAGE PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
		#else
			#define DECLARE_VK_KHR_surface(LINKAGE)
		#endif
	#endif

	#if !defined(DECLARE_VK_KHR_display)
		#if defined(VK_KHR_display)
			#define DECLARE_VK_KHR_display(LINKAGE) \
				LINKAGE PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR; \
				LINKAGE PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR; \
				LINKAGE PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR; \
				LINKAGE PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR; \
				LINKAGE PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR; \
				LINKAGE PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR; \
				LINKAGE PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR;
		#else
			#define DECLARE_VK_KHR_display(LINKAGE)
		#endif
	#endif

	#if !defined(DECLARE_VK_KHR_swapchain)
		#if defined(VK_KHR_swapchain)
			#define DECLARE_VK_KHR_swapchain(LINKAGE) \
			LINKAGE  PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR; \
			LINKAGE  PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR; \
			LINKAGE  PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR; \
			LINKAGE  PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR; \
			LINKAGE  PFN_vkQueuePresentKHR vkQueuePresentKHR;
		#else
			#define DECLARE_VK_KHR_swapchain(LINKAGE)
		#endif
	#endif


	#ifndef DECLARE_VK_core
	#define DECLARE_VK_core(LINKAGE) \
		LINKAGE PFN_vkCreateInstance	vkCreateInstance; \
		LINKAGE PFN_vkDestroyInstance	vkDestroyInstance; \
		LINKAGE PFN_vkCreateDevice vkCreateDevice; \
		LINKAGE PFN_vkDestroyDevice vkDestroyDevice; \
		LINKAGE PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices; \
		LINKAGE PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties; \
		LINKAGE PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties; \
		LINKAGE PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties; \
		LINKAGE PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties; \
		LINKAGE PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures; \
		LINKAGE PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties; \
		LINKAGE PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties; \
		LINKAGE PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties; \
		LINKAGE PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties; \
		\
		LINKAGE PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties; \
		LINKAGE PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;\
		LINKAGE PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr; \
		LINKAGE PFN_vkGetDeviceQueue vkGetDeviceQueue; \
		LINKAGE PFN_vkQueueSubmit vkQueueSubmit; \
		LINKAGE PFN_vkQueueWaitIdle vkQueueWaitIdle; \
		LINKAGE PFN_vkDeviceWaitIdle vkDeviceWaitIdle; \
		LINKAGE PFN_vkAllocateMemory vkAllocateMemory; \
		LINKAGE PFN_vkFreeMemory vkFreeMemory; \
		LINKAGE PFN_vkMapMemory vkMapMemory; \
		LINKAGE PFN_vkUnmapMemory vkUnmapMemory; \
		LINKAGE PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges; \
		LINKAGE PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges; \
		LINKAGE PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment; \
		LINKAGE PFN_vkBindBufferMemory vkBindBufferMemory; \
		LINKAGE PFN_vkBindImageMemory vkBindImageMemory; \
		LINKAGE PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements; \
		LINKAGE PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements; \
		LINKAGE PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements; \
		LINKAGE PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties; \
		LINKAGE PFN_vkQueueBindSparse vkQueueBindSparse; \
		LINKAGE PFN_vkCreateFence vkCreateFence; \
		LINKAGE PFN_vkDestroyFence vkDestroyFence; \
		LINKAGE PFN_vkResetFences vkResetFences; \
		LINKAGE PFN_vkGetFenceStatus vkGetFenceStatus; \
		LINKAGE PFN_vkWaitForFences vkWaitForFences; \
		LINKAGE PFN_vkCreateSemaphore vkCreateSemaphore; \
		LINKAGE PFN_vkDestroySemaphore vkDestroySemaphore; \
		LINKAGE PFN_vkCreateEvent vkCreateEvent; \
		LINKAGE PFN_vkDestroyEvent vkDestroyEvent; \
		LINKAGE PFN_vkGetEventStatus vkGetEventStatus; \
		LINKAGE PFN_vkSetEvent vkSetEvent; \
		LINKAGE PFN_vkResetEvent vkResetEvent; \
		LINKAGE PFN_vkCreateQueryPool vkCreateQueryPool; \
		LINKAGE PFN_vkDestroyQueryPool vkDestroyQueryPool; \
		LINKAGE PFN_vkGetQueryPoolResults vkGetQueryPoolResults; \
		LINKAGE PFN_vkCreateBuffer vkCreateBuffer; \
		LINKAGE PFN_vkDestroyBuffer vkDestroyBuffer; \
		LINKAGE PFN_vkCreateBufferView vkCreateBufferView; \
		LINKAGE PFN_vkDestroyBufferView vkDestroyBufferView; \
		LINKAGE PFN_vkCreateImage vkCreateImage; \
		LINKAGE PFN_vkDestroyImage vkDestroyImage; \
		LINKAGE PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout; \
		LINKAGE PFN_vkCreateImageView vkCreateImageView; \
		LINKAGE PFN_vkDestroyImageView vkDestroyImageView; \
		LINKAGE PFN_vkCreateShaderModule vkCreateShaderModule; \
		LINKAGE PFN_vkDestroyShaderModule vkDestroyShaderModule; \
		LINKAGE PFN_vkCreatePipelineCache vkCreatePipelineCache; \
		LINKAGE PFN_vkDestroyPipelineCache vkDestroyPipelineCache; \
		LINKAGE PFN_vkGetPipelineCacheData vkGetPipelineCacheData; \
		LINKAGE PFN_vkMergePipelineCaches vkMergePipelineCaches; \
		LINKAGE PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines; \
		LINKAGE PFN_vkCreateComputePipelines vkCreateComputePipelines; \
		LINKAGE PFN_vkDestroyPipeline vkDestroyPipeline; \
		LINKAGE PFN_vkCreatePipelineLayout vkCreatePipelineLayout; \
		LINKAGE PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout; \
		LINKAGE PFN_vkCreateSampler vkCreateSampler; \
		LINKAGE PFN_vkDestroySampler vkDestroySampler; \
		LINKAGE PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout; \
		LINKAGE PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout; \
		LINKAGE PFN_vkCreateDescriptorPool vkCreateDescriptorPool; \
		LINKAGE PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool; \
		LINKAGE PFN_vkResetDescriptorPool vkResetDescriptorPool; \
		LINKAGE PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets; \
		LINKAGE PFN_vkFreeDescriptorSets vkFreeDescriptorSets; \
		LINKAGE PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets; \
		LINKAGE PFN_vkCreateFramebuffer vkCreateFramebuffer; \
		LINKAGE PFN_vkDestroyFramebuffer vkDestroyFramebuffer; \
		LINKAGE PFN_vkCreateRenderPass vkCreateRenderPass; \
		LINKAGE PFN_vkDestroyRenderPass vkDestroyRenderPass; \
		LINKAGE PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity; \
		LINKAGE PFN_vkCreateCommandPool vkCreateCommandPool; \
		LINKAGE PFN_vkDestroyCommandPool vkDestroyCommandPool; \
		LINKAGE PFN_vkResetCommandPool vkResetCommandPool; \
		LINKAGE PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers; \
		LINKAGE PFN_vkFreeCommandBuffers vkFreeCommandBuffers; \
		LINKAGE PFN_vkBeginCommandBuffer vkBeginCommandBuffer; \
		LINKAGE PFN_vkEndCommandBuffer vkEndCommandBuffer; \
		LINKAGE PFN_vkResetCommandBuffer vkResetCommandBuffer; \
		LINKAGE PFN_vkCmdBindPipeline vkCmdBindPipeline; \
		LINKAGE PFN_vkCmdSetViewport vkCmdSetViewport; \
		LINKAGE PFN_vkCmdSetScissor vkCmdSetScissor; \
		LINKAGE PFN_vkCmdSetLineWidth vkCmdSetLineWidth; \
		LINKAGE PFN_vkCmdSetDepthBias vkCmdSetDepthBias; \
		LINKAGE PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants; \
		LINKAGE PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds; \
		LINKAGE PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask; \
		LINKAGE PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask; \
		LINKAGE PFN_vkCmdSetStencilReference vkCmdSetStencilReference; \
		LINKAGE PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets; \
		LINKAGE PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer; \
		LINKAGE PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers; \
		LINKAGE PFN_vkCmdDraw vkCmdDraw; \
		LINKAGE PFN_vkCmdDrawIndexed vkCmdDrawIndexed; \
		LINKAGE PFN_vkCmdDrawIndirect vkCmdDrawIndirect; \
		LINKAGE PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect; \
		LINKAGE PFN_vkCmdDispatch vkCmdDispatch; \
		LINKAGE PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect; \
		LINKAGE PFN_vkCmdCopyBuffer vkCmdCopyBuffer; \
		LINKAGE PFN_vkCmdCopyImage vkCmdCopyImage; \
		LINKAGE PFN_vkCmdBlitImage vkCmdBlitImage; \
		LINKAGE PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage; \
		LINKAGE PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer; \
		LINKAGE PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer; \
		LINKAGE PFN_vkCmdFillBuffer vkCmdFillBuffer; \
		LINKAGE PFN_vkCmdClearColorImage vkCmdClearColorImage; \
		LINKAGE PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage; \
		LINKAGE PFN_vkCmdClearAttachments vkCmdClearAttachments; \
		LINKAGE PFN_vkCmdResolveImage vkCmdResolveImage; \
		LINKAGE PFN_vkCmdSetEvent vkCmdSetEvent; \
		LINKAGE PFN_vkCmdResetEvent vkCmdResetEvent; \
		LINKAGE PFN_vkCmdWaitEvents vkCmdWaitEvents; \
		LINKAGE PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier; \
		LINKAGE PFN_vkCmdBeginQuery vkCmdBeginQuery; \
		LINKAGE PFN_vkCmdEndQuery vkCmdEndQuery; \
		LINKAGE PFN_vkCmdResetQueryPool vkCmdResetQueryPool; \
		LINKAGE PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp; \
		LINKAGE PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults; \
		LINKAGE PFN_vkCmdPushConstants vkCmdPushConstants; \
		LINKAGE PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass; \
		LINKAGE PFN_vkCmdNextSubpass vkCmdNextSubpass; \
		LINKAGE PFN_vkCmdEndRenderPass vkCmdEndRenderPass; \
		LINKAGE PFN_vkCmdExecuteCommands vkCmdExecuteCommands;
	#endif
#endif

#ifdef __cplusplus
}
#endif

#endif	/* __RAPTOR_VKEXT_H__ */