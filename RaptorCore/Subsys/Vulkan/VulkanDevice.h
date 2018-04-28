// VulkanDevice.h: interface for the CVulkanDevice class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
#define AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif
#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CVulkanPipeline;
class CVulkanShader;
class CVulkanSurface;
class CVulkanTextureObject;
class C3DScene;
class CRaptorDisplayConfig;


class CVulkanDevice
{
public:
	static const unsigned int NB_RENDERING_RESOURCES = 3;

	CVulkanDevice(void);
	virtual ~CVulkanDevice(void);

	//!	Store the current device, activated from context glMakeCurrentCurrent
	static const CVulkanDevice& getCurrentDevice();


#if defined(VK_VERSION_1_0)
	//!
	//!	Device creation and initialisation
	//!	These functions shall be called in this order:
	//!	- vkCreateLogicalDevice
	//!	- vkCreateSwapChain
	//!

	//!	Creates and initialise a logical device and linked resources
	bool vkCreateLogicalDevice(	const VkPhysicalDevice &device,
								const VkPhysicalDeviceProperties &props,
								uint32_t graphicsQueueFamilyIndex,
								uint32_t graphicsQueueCount,
								uint32_t presentQueueFamilyIndex,
								uint32_t presentQueueCount,
								uint32_t transferQueueFamilyIndex,
								uint32_t transferQueueCount);

	//!	Creates and initialises the swap chain.
	//!	The previous Swap chain is destroyed if not null.
	bool vkCreateSwapChain(	CVulkanSurface *pSurface,
						   const CRaptorDisplayConfig& config);


	//!
	//!	Accessing data
	//!

	//!	Transfer unsynchronised buffer object data to corresponding 
	//!	device buffer objects. Buffer object data may already be synchronised.
	//! @param blocking : if blocking is true, synchronisation will wait for complete queue execution.
	//! @return true if synchronisation is successful
	bool vkUploadDataToDevice(bool blocking = false);
	VkCommandBuffer getUploadBuffer(void) const { return transferBuffer; };

	//! Returns the memory wrapper managing this device.
	CVulkanMemory::CVulkanMemoryWrapper* getMemory(void) const { return pDeviceMemory; };


	//! Return device features
	const VkPhysicalDeviceFeatures& getFeatures(void) const { return m_features; };

	//! Return device properties
	const VkPhysicalDeviceProperties& getProperties(void) const { return m_properties; };


	CVulkanPipeline*		createPipeline(void) const;
	CVulkanShader*			createShader(void) const;
	CVulkanTextureObject*	createTextureObject(ITextureObject::TEXEL_TYPE type) const;


	//!
	//!	Rendering
	//!

	//!	Provides a rendering image from swap chain for next frame render.
	//!	Currently, only a single image per frame is managed.
	//!	TODO : handle multiple acquired images.
	bool acquireSwapChainImage(uint64_t timeout);
	bool presentSwapChainImage();

	//!	Renders the 3DScene provided to the current grahics command buffer
	//!	and initialise the render pass.
	bool vkRender(	C3DScene *pScene,
					const VkRect2D& scissor,
					const CColor::RGBA& clearColor);

	//! Destroy or Release all device linked Vulkan resources, including swap chain
	bool vkDestroyLogicalDevice(void);

	//!	Destroy Swap Chain, render pass resources and framebuffers
	bool vkDestroySwapChain();



private:
	//!	Forbidden operator
	CVulkanDevice(const CVulkanDevice&);
	CVulkanDevice& operator=(const CVulkanDevice&);

	//!	Creates a command pool on to a Queue Family Index
	VkCommandPool createCommandPool(uint32_t queueFamilyIndex);

	//!	Creates and initialises a render pass and image views
	bool vkCreateRenderPassResources(VkSurfaceFormatKHR format,
									 uint32_t nbSamples,
									 uint32_t width,
									 uint32_t height,
									 VkFormat depth);

	//!	Destroy the render pass resources allocated from vkCreateRenderPassResources.
	bool vkDestroyRenderPassResources(void);

	//!	Creates and initialise rendering resources for each rendering pass.
	//!	Rendering resources are:
	//!	- CommandBuffer
	//!	- Semaphores for image available and rendering finished
	//!	- Fence for queue submission
	//!	- Framebuffer.
	//!	NB_RENDERING_RESOURCES set of resources are created.
	bool vkCreateRenderingResources(void);

	//!	Destroy the rendering resources allocated from vkCreateRenderingResources.
	bool vkDestroyRenderingResources(void);

	//!	Creates the z-buffer for the display context referencing this device
	bool vkCreateZBuffer(uint32_t width,
						 uint32_t height,
						 VkFormat depth);

	//!	Destroy the z-buffer resources allocated by vkCreateZBuffer.
	bool vkDestroyZBuffer(void);


	DECLARE_VK_KHR_swapchain(DEFAULT_LINKAGE)
	DECLARE_VK_device(DEFAULT_LINKAGE)
	DECLARE_VK_queue(DEFAULT_LINKAGE)

	VkDevice					device;
	VkPhysicalDeviceFeatures	m_features;
	VkPhysicalDeviceProperties	m_properties;

	CVulkanMemory::CVulkanMemoryWrapper* pDeviceMemory;

	VkCommandPool	graphicsCommandPool;
	VkCommandPool	presentCommandPool;
	VkCommandPool	transferCommandPool;
	uint32_t		graphics_queueFamilyIndex;
	uint32_t		present_queueFamilyIndex;
	uint32_t		transfer_queueFamilyIndex;

	typedef struct VK_RENDERING_RESOURCE_st
	{
		std::vector<VkCommandBuffer>	commandBuffers;
		VkSemaphore		imageAvailableSemaphore;
		VkSemaphore		renderingCompleteSemaphore;
		VkFence			queueExecutionComplete;
		VkFramebuffer	frameBuffer;
		VkImageView		linkedView;
		VkQueue			queue;
	} VK_RENDERING_RESOURCE;

	VkImage			zBuffer;
	VkImageView		zView;
	VkDeviceMemory	zMemory;
	VkCommandBuffer	transferBuffer;
	VkQueue			transferQueue;

	uint32_t			currentRenderingResources;
	VK_RENDERING_RESOURCE	*renderingResources;
	
#ifdef VK_KHR_swapchain
	VkSwapchainKHR		swapChain;
	uint32_t			swapchainImageCount;
	uint32_t			currentImage;
#endif
	
	typedef struct VK_RENDERING_IMAGE_st
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
