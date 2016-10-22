// VulkanDevice.h: interface for the CVulkanDevice class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
#define AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(__RAPTOR_VKEXT_H__)
	#include "System/vkext.h"
#endif
#if !defined(AFX_COLOR_H__3770AC59_0D0E_49EF_99C8_037268A33CE4__INCLUDED_)
	#include "System/Color.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CVulkanPipeline;
class CVulkanShader;

class CVulkanDevice
{
public:
	CVulkanDevice(void);
	virtual ~CVulkanDevice(void);

#if defined(VK_VERSION_1_0)
	//!	Creates and initialise a logical device and linked resources
	bool createDevice(const VkPhysicalDevice &device,
					const VkPhysicalDeviceFeatures &features,
					uint32_t graphicsQueueFamilyIndex,
					uint32_t graphicsQueueCount,
					uint32_t presentQueueFamilyIndex,
					uint32_t presentQueueCount);

	//!	Creates and initialises the swap chain.
	bool createSwapChain(VkSurfaceKHR surface,
						VkSurfaceFormatKHR format,
						VkSurfaceCapabilitiesKHR surfaceCapabilities,
						VkPresentModeKHR presentMode,
						uint32_t width,
						uint32_t height);

	bool acquireSwapChainImage(uint64_t timeout);
	bool presentSwapChainImage();

	//!	Creates and initialises a render pass
	bool createRenderPass(	VkSurfaceFormatKHR format,
							uint32_t nbSamples,
							uint32_t width,
							uint32_t height);

	CVulkanPipeline*	createPipeline(void) const;
	CVulkanShader*		createShader(void) const;

	//!	Binds the provided Pipeline to the current grahics command buffer
	//!	and initialise the render pass.
	bool bindPipeline(	const CVulkanPipeline& pipeline,
						const VkRect2D& scissor,
						const CColor::RGBA& clearColor);

	bool unbindPipeline();


	//! Destroy or Release all device linked Vulkan resources, including swap chain
	bool destroyDevice(void);


	PFN_vkCreateDevice vkCreateDevice;
	PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;

		

private:
	//!	Forbidden operator
	CVulkanDevice(const CVulkanDevice&);
	CVulkanDevice& operator=(const CVulkanDevice&);

	DECLARE_VK_KHR_swapchain(DEFAULT_LINKAGE)
	DECLARE_VK_device(DEFAULT_LINKAGE)
	DECLARE_VK_queue(DEFAULT_LINKAGE)

	VkDevice		device;

	VkCommandPool	graphicsCommandPool;
	VkCommandPool	presentCommandPool;
#ifdef VK_KHR_swapchain
	VkSwapchainKHR		swapChain;
	uint32_t			swapchainImageCount;
	VkCommandBuffer		*graphicsCommandBuffer;
	VkCommandBuffer		*presentCommandBuffer;
	VkSemaphore			imageAvailableSemaphore;
	VkSemaphore			renderingCompleteSemaphore;
	uint32_t			currentImage;
#endif
	uint32_t		graphics_queueFamilyIndex;
	uint32_t		present_queueFamilyIndex;

	typedef struct
	{
		VkImage			image;
		VkImageView		view;
		VkFramebuffer	frameBuffer;
	} VK_FRAMEBUFFER;
	VkRenderPass	renderPass;
	VK_FRAMEBUFFER	*frameBuffers;
#endif
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
