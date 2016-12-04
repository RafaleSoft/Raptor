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
#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CVulkanPipeline;
class CVulkanShader;

class CVulkanDevice
{
public:
	static const unsigned int NB_RENDERING_RESOURCES = 3;


	CVulkanDevice(void);
	virtual ~CVulkanDevice(void);

#if defined(VK_VERSION_1_0)
	//!	Creates and initialise a logical device and linked resources
	bool vkCreateLogicalDevice(	const VkPhysicalDevice &device,
								const VkPhysicalDeviceFeatures &features,
								uint32_t graphicsQueueFamilyIndex,
								uint32_t graphicsQueueCount,
								uint32_t presentQueueFamilyIndex,
								uint32_t presentQueueCount,
								uint32_t transferQueueFamilyIndex,
								uint32_t transferQueueCount);

	//!	Creates and initialises the swap chain.
	bool vkCreateSwapChain(	VkSurfaceKHR surface,
							VkSurfaceFormatKHR format,
							VkSurfaceCapabilitiesKHR surfaceCapabilities,
							VkPresentModeKHR presentMode,
							uint32_t width,
							uint32_t height);

	bool vkDestroySwapChain();

	//!	Provides a rendering image from swap chain for next frame render.
	//!	Currently, only a single image per frame is managed.
	//!	TODO : handle multiple acquired images.
	bool acquireSwapChainImage(uint64_t timeout);
	bool presentSwapChainImage();

	//!	Creates and initialises a render pass
	bool vkCreateRenderPassResources(	VkSurfaceFormatKHR format,
										uint32_t nbSamples,
										uint32_t width,
										uint32_t height);

	//!	Creates and initialise rendering resources for each rendering pass.
	//!	Rendering resources are:
	//!	- CommandBuffer
	//!	- Semaphores for image available and rendering finished
	//!	- Fence for queue submission
	//!	- Framebuffer.
	//!	NB_RENDERING_RESOURCES set of resources are created.
	bool vkCreateRenderingResources(void);

	//!	Transfer unsynchronised buffer object data to corresponding 
	//!	device buffer objects. Buffer object data may already be synchronised.
	bool vkSynchroniseBufferObjects(void);

	//! Returns the memory wrapper managing this device.
	IDeviceMemoryManager* getMemory(void) const { return pDeviceMemory; };


	CVulkanPipeline*	createPipeline(void) const;
	CVulkanShader*		createShader(void) const;

	//!	Binds the provided Pipeline to the current grahics command buffer
	//!	and initialise the render pass.
	bool vkBindPipeline(const CVulkanPipeline& pipeline,
						const VkRect2D& scissor,
						const CColor::RGBA& clearColor,
						VkDeviceSize offset,
						VkDeviceSize offset2);

	//! Destroy or Release all device linked Vulkan resources, including swap chain
	bool vkDestroyLogicalDevice(void);


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

	CVulkanMemory::CVulkanMemoryWrapper* pDeviceMemory;

	VkCommandPool	graphicsCommandPool;
	VkCommandPool	presentCommandPool;
	VkCommandPool	transferCommandPool;
	uint32_t		graphics_queueFamilyIndex;
	uint32_t		present_queueFamilyIndex;
	uint32_t		transfer_queueFamilyIndex;

	typedef struct
	{
		VkCommandBuffer	commandBuffer;
		VkSemaphore		imageAvailableSemaphore;
		VkSemaphore		renderingCompleteSemaphore;
		VkFence			queueExecutionComplete;
		VkFramebuffer	frameBuffer;
		VkQueue			queue;
	} VK_RENDERING_RESOURCE;

	VkCommandBuffer	transferBuffer;
	VkQueue			transferQueue;

	uint32_t			currentRenderingResources;
	VK_RENDERING_RESOURCE	*renderingResources;
	
#ifdef VK_KHR_swapchain
	VkSwapchainKHR		swapChain;
	uint32_t			swapchainImageCount;
	uint32_t			currentImage;
#endif
	
	typedef struct
	{
		VkImage			image;
		VkImageView		view;
		VkExtent2D		extent;
	} VK_RENDERING_IMAGE;

	VkRenderPass	renderPass;
	VK_RENDERING_IMAGE	*renderImages;
#endif
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
