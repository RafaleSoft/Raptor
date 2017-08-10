// VulkanDevice.cpp: implementation of the CVulkanDisplay class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
	#include "Subsys/Vulkan/VulkanDevice.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_)
	#include "Subsys/Vulkan/VulkanPipeline.h"
#endif
#if !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
	#include "Subsys/Vulkan/VulkanShader.h"
#endif
#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif
#if !defined(AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_)
	#include "Subsys/Vulkan/VulkanCommandBuffer.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif

RAPTOR_NAMESPACE

CVulkanDevice::CVulkanDevice(void)
{
	device = VK_NULL_HANDLE;
	renderPass = VK_NULL_HANDLE;
	renderImages = NULL;
	graphics_queueFamilyIndex = MAXUINT;
	present_queueFamilyIndex = MAXUINT;
	graphicsCommandPool = VK_NULL_HANDLE;
	presentCommandPool = VK_NULL_HANDLE;

	currentRenderingResources = 0;
	renderingResources = NULL;

#ifdef VK_KHR_swapchain
	swapChain = VK_NULL_HANDLE;
	currentImage = MAXUINT;
	swapchainImageCount = 0;
#endif

	vkCreateDevice = NULL;
	vkGetDeviceProcAddr = NULL;

	vkDeviceWaitIdle = NULL;
	vkDestroyDevice = NULL;
	vkCreateCommandPool = NULL;
	vkDestroyCommandPool = NULL;
	vkResetCommandPool = NULL;
	vkGetDeviceQueue = NULL;
	vkCreateSemaphore = NULL;
	vkDestroySemaphore = NULL;
	vkCreateFence = NULL;
	vkWaitForFences = NULL;
	vkResetFences = NULL;
	vkDestroyFence = NULL;
	vkAllocateCommandBuffers = NULL;
	vkFreeCommandBuffers = NULL;
	vkCreateRenderPass = NULL;
	vkDestroyRenderPass = NULL;
	vkCreateImageView = NULL;
	vkDestroyImageView = NULL;
	vkCreateFramebuffer = NULL;
	vkDestroyFramebuffer = NULL;
	vkQueueSubmit = NULL;
	vkQueueWaitIdle = NULL;

#if defined(VK_KHR_swapchain)
	vkCreateSwapchainKHR = NULL;
	vkDestroySwapchainKHR = NULL;
	vkGetSwapchainImagesKHR = NULL;
	vkAcquireNextImageKHR = NULL;
	vkQueuePresentKHR = NULL;
#endif
}

CVulkanDevice::~CVulkanDevice(void)
{
	if (NULL != renderImages)
		delete [] renderImages;

	if (NULL != renderingResources)
		delete [] renderingResources;
}

CVulkanDevice	*CVulkanDevice::pCurrentDevice = NULL;

bool CVulkanDevice::setCurrentDevice(CVulkanDevice* current)
{
	if (NULL != pCurrentDevice)
	{
		if (NULL != current)
			return false;
		else
			pCurrentDevice = NULL;
	}
	else
		pCurrentDevice = current;

	return true;
}

CVulkanDevice* CVulkanDevice::getCurrentDevice()
{
	return pCurrentDevice;
}

CVulkanPipeline* CVulkanDevice::createPipeline(void) const
{
	if ((VK_NULL_HANDLE != device) &&
		(VK_NULL_HANDLE != renderPass))
	{
		CVulkanPipeline *pipeline = new CVulkanPipeline(device,renderPass,vkGetDeviceProcAddr);
		return pipeline;
	}
	else
		return NULL;
}

CVulkanShader* CVulkanDevice::createShader(void) const
{
	if (VK_NULL_HANDLE != device)
	{
		CVulkanShader *shader = new CVulkanShader(device, vkGetDeviceProcAddr);
		return shader;
	}
	else
		return NULL;
}

bool CVulkanDevice::acquireSwapChainImage(uint64_t timeout)
{
	//! Only a single image is managed for the moment.
	//!	Future improvement will manage several images per frame.
	if (currentImage != MAXUINT)
	{
		CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
		pErrMgr->generateRaptorError(	Global::CVulkanClassID::GetClassId(),
										CRaptorErrorManager::RAPTOR_WARNING,
										"Vulkan Device aready has a rendering image available!");
		return true;
	}

	VkResult res = VK_NOT_READY;

	VK_RENDERING_RESOURCE &resource = renderingResources[currentRenderingResources];
	res = vkWaitForFences(device, 1, &resource.queueExecutionComplete, VK_FALSE, timeout);
	if(VK_SUCCESS != res)
	{
		CATCH_VK_ERROR(res)
		return false;
	}
	vkResetFences(device, 1, &resource.queueExecutionComplete);
	
#ifdef VK_KHR_swapchain
	res = vkAcquireNextImageKHR(device,
								swapChain,
								timeout,
								resource.imageAvailableSemaphore,
								VK_NULL_HANDLE,
								&currentImage);
	switch (res)
	{
		case VK_SUCCESS:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
		case VK_SUBOPTIMAL_KHR:
		{
			CATCH_VK_ERROR(res)
			break;
		}
		default:
		{
			CATCH_VK_ERROR(res)
			break;
		}
	}
#endif
	if (MAXUINT == currentImage)
		return false;

	return (VK_SUCCESS == res);
}

bool CVulkanDevice::presentSwapChainImage()
{
	if (MAXUINT == currentImage)
		return false;

	VK_RENDERING_RESOURCE &resource = renderingResources[currentRenderingResources];

	VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO,
								NULL,
								1, &resource.imageAvailableSemaphore,
								&wait_dst_stage_mask,
								1, &resource.commandBuffers[0],
								1, &resource.renderingCompleteSemaphore };
								
	VkResult res = vkQueueSubmit(resource.queue, 1, &submit_info, resource.queueExecutionComplete);
	if(VK_SUCCESS != res)
	{
		CATCH_VK_ERROR(res)
		return false;
	}

	VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, NULL,
									1, &resource.renderingCompleteSemaphore,
									1, &swapChain,
									&currentImage,
									NULL};

	res = vkQueuePresentKHR(resource.queue,&presentInfo);
	CATCH_VK_ERROR(res)
	
	currentImage = MAXUINT;
	currentRenderingResources = (currentRenderingResources + 1) % NB_RENDERING_RESOURCES;
	return (VK_SUCCESS == res);
}

bool CVulkanDevice::vkSynchroniseBufferObjects(bool blocking)
{
	if (!pDeviceMemory->needBufferObjectDataSynchro())
		return true;

	{
		CVulkanCommandBuffer displayList(transferBuffer);
		pDeviceMemory->synchroniseBufferObjectData(displayList);
	}

	VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO,
								NULL,
								0, NULL,
								NULL,
								1, &transferBuffer,
								0, NULL };
	VkResult res = vkQueueSubmit(transferQueue, 1, &submit_info, NULL );
	CATCH_VK_ERROR(res);
	
	if (blocking)
	{
		res = vkQueueWaitIdle(transferQueue);
		CATCH_VK_ERROR(res)
	}

	return (VK_SUCCESS == res);
}

bool CVulkanDevice::vkRender(	C3DScene *pScene,
								const VkRect2D& scissor,
								const CColor::RGBA& clearColor)
{
	vkSynchroniseBufferObjects();

	// TODO : check currentRenderingResources is valid ?
	if ((currentRenderingResources >= NB_RENDERING_RESOURCES) ||
		(currentImage == MAXUINT))
		return false;
	VK_RENDERING_RESOURCE &resource = renderingResources[currentRenderingResources];
	VK_RENDERING_IMAGE &image =  renderImages[currentImage];

	bool bResize = false;
	if ((image.view != resource.linkedView) && (VK_NULL_HANDLE != resource.frameBuffer))
	{
		vkDestroyFramebuffer(device, resource.frameBuffer, NULL);
		resource.frameBuffer = VK_NULL_HANDLE;
		resource.linkedView = VK_NULL_HANDLE;
	}
	if (VK_NULL_HANDLE == resource.frameBuffer)
	{
		resource.linkedView = image.view;
		VkFramebufferCreateInfo pFrameBufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
															NULL, 0,
															renderPass,
															1, &image.view,
															image.extent.width,
															image.extent.height, 1 };

		VkResult res = vkCreateFramebuffer(device, &pFrameBufferCreateInfo, NULL, &resource.frameBuffer);
		bResize = true;
		CATCH_VK_ERROR(res)
	}

	CVulkanCommandBuffer displayList(resource.commandBuffers[0],scissor);
	if (bResize)
		displayList.resize();
	displayList.imageBarrier(present_queueFamilyIndex, graphics_queueFamilyIndex, image.image);
	displayList.renderPass(renderPass, resource.frameBuffer, clearColor);
	
	VkBuffer binding = pDeviceMemory->getLockedBuffer(IDeviceMemoryManager::IBufferObject::VERTEX_BUFFER);
	VkBuffer binding2 = pDeviceMemory->getLockedBuffer(IDeviceMemoryManager::IBufferObject::INDEX_BUFFER);
	VkBuffer binding3 = pDeviceMemory->getLockedBuffer(IDeviceMemoryManager::IBufferObject::UNIFORM_BUFFER);
	pScene->vkRender(displayList, binding, binding2, binding3);

	return true;
}


bool CVulkanDevice::vkCreateRenderingResources(void)
{
	if (NULL != renderingResources)
		return false;

	//!	Allocate a Queue and Command buffer for transfer operations
	vkGetDeviceQueue(	device,
						transfer_queueFamilyIndex,
						NB_RENDERING_RESOURCES,		// should not use a grphicsQueue here, but NB_RENDERING_RESOURCES+1+1 (present+transfer)
						&transferQueue);
	VkCommandBufferAllocateInfo pTransferBufferInfo = {	VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
														NULL,
														transferCommandPool,
														VK_COMMAND_BUFFER_LEVEL_PRIMARY,
														1	};
	
	VkResult res = vkAllocateCommandBuffers(device,&pTransferBufferInfo,&transferBuffer);
	if (VK_SUCCESS != res)
	{
		CATCH_VK_ERROR(res)
		return false;
	}


	renderingResources = new VK_RENDERING_RESOURCE[NB_RENDERING_RESOURCES];

	VkCommandBuffer	*pCommandBuffer = new VkCommandBuffer[NB_RENDERING_RESOURCES];
	VkCommandBufferAllocateInfo pCommandBufferInfo = {	VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
														NULL,
														graphicsCommandPool,
														VK_COMMAND_BUFFER_LEVEL_PRIMARY,
														NB_RENDERING_RESOURCES	};
	
	res = vkAllocateCommandBuffers(device,&pCommandBufferInfo,&pCommandBuffer[0]);
	if (VK_SUCCESS != res)
	{
		CATCH_VK_ERROR(res)
		return false;
	}
	else
	{
		VkSemaphoreCreateInfo pSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, NULL, 0};
		VkFenceCreateInfo pFenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, NULL, VK_FENCE_CREATE_SIGNALED_BIT };
		for (unsigned int i=0;i<NB_RENDERING_RESOURCES;i++)
		{
			res = vkCreateSemaphore(device,
									&pSemaphoreInfo,
									CVulkanMemory::GetAllocator(),
									&renderingResources[i].imageAvailableSemaphore);
			if (VK_SUCCESS != res)
			{
				CATCH_VK_ERROR(res)
				return false;
			}

			res = vkCreateSemaphore(device,
									&pSemaphoreInfo,
									CVulkanMemory::GetAllocator(),
									&renderingResources[i].renderingCompleteSemaphore);
			if (VK_SUCCESS != res)
			{
				CATCH_VK_ERROR(res)
				return false;
			}

			res = vkCreateFence(device,
								&pFenceInfo,
								CVulkanMemory::GetAllocator(),
								&renderingResources[i].queueExecutionComplete);
			if (VK_SUCCESS != res)
			{
				CATCH_VK_ERROR(res)
				return false;
			}

			renderingResources[i].frameBuffer = VK_NULL_HANDLE;
			renderingResources[i].linkedView = VK_NULL_HANDLE;
			renderingResources[i].commandBuffers.push_back(pCommandBuffer[i]);

			vkGetDeviceQueue(	device,
								graphics_queueFamilyIndex,
								i,
								&renderingResources[i].queue);
		}
	}

	delete [] pCommandBuffer;
/*
	if (graphics_queueFamilyIndex != present_queueFamilyIndex)
	{
		presentCommandBuffer = new VkCommandBuffer[1];
		pCommandBufferInfo.commandPool = presentCommandPool;
		pCommandBufferInfo.commandBufferCount = 1;
		res = vkAllocateCommandBuffers(device,&pCommandBufferInfo,&presentCommandBuffer[0]);
		if (VK_SUCCESS != res)
		{
			CATCH_VK_ERROR(res)
			return false;
		}
	}
	else
		presentCommandBuffer = graphicsCommandBuffer;
*/
	return true;
}


bool CVulkanDevice::vkCreateRenderPassResources(VkSurfaceFormatKHR format,
												uint32_t nbSamples,
												uint32_t width,
												uint32_t height)
{
	if (NULL != renderImages)
		return false;

	VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
	if (nbSamples == 2)
		samples = VK_SAMPLE_COUNT_2_BIT;
	else if ((nbSamples == 4) || (nbSamples == 5))
		samples = VK_SAMPLE_COUNT_4_BIT;
	else if ((nbSamples == 6) || (nbSamples == 8))
		samples = VK_SAMPLE_COUNT_8_BIT;
	else if (nbSamples == 16)
		samples = VK_SAMPLE_COUNT_16_BIT;
	else if (nbSamples == 32)
		samples = VK_SAMPLE_COUNT_32_BIT;
	else if (nbSamples == 64)
		samples = VK_SAMPLE_COUNT_64_BIT;

	VkAttachmentDescription pAttachments = {0, // or 1 = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT
											format.format,samples,
											VK_ATTACHMENT_LOAD_OP_CLEAR,
											VK_ATTACHMENT_STORE_OP_STORE,
											VK_ATTACHMENT_LOAD_OP_CLEAR,
											VK_ATTACHMENT_STORE_OP_DONT_CARE,
											VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, //	Is this correct ?
											VK_IMAGE_LAYOUT_PRESENT_SRC_KHR }; // is this correct ?
	VkAttachmentReference pColorAttachments = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	//VkAttachmentReference pDepthStencilAttachment = { 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
	VkSubpassDescription pSubpasses = {	0,
										VK_PIPELINE_BIND_POINT_GRAPHICS,
										0, NULL,
										1, &pColorAttachments,
										NULL,
										NULL, //&pDepthStencilAttachment,
										0, NULL };
	VkSubpassDependency dependencies[2] = {{VK_SUBPASS_EXTERNAL,
											0,
											VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
											VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
											VK_ACCESS_MEMORY_READ_BIT,
											VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
											VK_DEPENDENCY_BY_REGION_BIT},
										   {0,
											VK_SUBPASS_EXTERNAL,
											VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
											VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
											VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
											VK_ACCESS_MEMORY_READ_BIT,
											VK_DEPENDENCY_BY_REGION_BIT}};


	VkRenderPassCreateInfo pRenderPassCreateInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
													NULL, 0,
													1, &pAttachments,
													1, &pSubpasses,
													2, &dependencies[0]};

	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	VkResult res = vkCreateRenderPass(	device,
										&pRenderPassCreateInfo,
										CVulkanMemory::GetAllocator(),
										&renderPass);
	if (VK_SUCCESS != res)
	{
		CATCH_VK_ERROR(res)
		return false;
	}

	VkImage *pImages = new VkImage[swapchainImageCount];
	res = vkGetSwapchainImagesKHR(	device,
									swapChain,
									&swapchainImageCount,
									pImages);
	if (VK_SUCCESS != res)
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
	else
	{
		renderImages = new VK_RENDERING_IMAGE[swapchainImageCount];
		for (uint32_t i=0;i<swapchainImageCount;i++)
			renderImages[i].view = VK_NULL_HANDLE;
		for (uint32_t i=0;i<swapchainImageCount;i++)
		{
			renderImages[i].extent.width = width;
			renderImages[i].extent.height = height;
			renderImages[i].image = pImages[i];
			VkImageViewCreateInfo image_view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
															NULL,
															0,
															renderImages[i].image,
															VK_IMAGE_VIEW_TYPE_2D,
															format.format,
															{	VK_COMPONENT_SWIZZLE_IDENTITY,
																VK_COMPONENT_SWIZZLE_IDENTITY,
																VK_COMPONENT_SWIZZLE_IDENTITY,
																VK_COMPONENT_SWIZZLE_IDENTITY	}, 
															{	VK_IMAGE_ASPECT_COLOR_BIT,
																0,
																1,
																0,
																1	} };
			
			res = vkCreateImageView(device,
									&image_view_create_info,
									CVulkanMemory::GetAllocator(),
									&renderImages[i].view);
			CATCH_VK_ERROR(res)
		}
	}
	if (NULL != pImages)
		delete [] pImages;

	if (VK_SUCCESS != res)
	{
		if (NULL != renderImages)
		{
			for (uint32_t i=0;i<swapchainImageCount;i++)
				if (VK_NULL_HANDLE != renderImages[i].view)
					vkDestroyImageView(	device,
										renderImages[i].view,
										CVulkanMemory::GetAllocator());
			delete [] renderImages;
			renderImages = NULL;
		}
	}

	return (VK_SUCCESS == res);
}

bool CVulkanDevice::vkCreateSwapChain(VkSurfaceKHR surface,
									VkSurfaceFormatKHR format,
									VkSurfaceCapabilitiesKHR surfaceCapabilities,
									VkPresentModeKHR presentMode,
									uint32_t width,
									uint32_t height)
{
	VkResult res = VK_NOT_READY;
	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	VkExtent2D extent = surfaceCapabilities.currentExtent;

	// width and height are either both -1, or both not -1.
	if (extent.width == -1)
	{
		extent.width = max(	min(width,surfaceCapabilities.maxImageExtent.width),
								surfaceCapabilities.minImageExtent.width);
		extent.height = max(min(height,surfaceCapabilities.maxImageExtent.height),
								surfaceCapabilities.minImageExtent.height);
	}
	VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;	// add transfer flag for clear
	VkSurfaceTransformFlagBitsKHR transform = surfaceCapabilities.currentTransform;
	if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

	//	mailbox will require at least 3 images for non blocking acquire of one image for the application.
	uint32_t numImages = min (max(3,surfaceCapabilities.minImageCount + 1),
								  surfaceCapabilities.maxImageCount);

	//	Opaque alpha if supported
	VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	//if (context.surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
	//	composite = context.surfaceCapabilities.supportedCompositeAlpha;

	VkSwapchainCreateInfoKHR pCreateInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
											NULL, 0, surface,
											numImages,
											format.format,format.colorSpace,
											extent,
											1,	// imageArrayLayers in multiview/stereo surface
											usage,
											VK_SHARING_MODE_EXCLUSIVE, 0, NULL, // Exclusive => no queues share access
											transform, // VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
											composite, // VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR alpha ignored, final alpha set to 1
											presentMode,
											VK_TRUE,	// Hard clipping, no read back
											swapChain };
	VkSwapchainKHR newSwapChain = VK_NULL_HANDLE;
	res = vkCreateSwapchainKHR(	device,
								&pCreateInfo,
								CVulkanMemory::GetAllocator(),
								&newSwapChain);
	if (VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}

	if (VK_NULL_HANDLE != swapChain)
		vkDestroySwapChain();
	swapChain = newSwapChain;

	res = vkGetSwapchainImagesKHR(	device,
									swapChain,
									&swapchainImageCount,
									NULL);
	if ((VK_SUCCESS != res) || (swapchainImageCount == 0))
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}

	return true;
}

bool CVulkanDevice::vkCreateLogicalDevice(	const VkPhysicalDevice &physicalDevice,
											const VkPhysicalDeviceFeatures &features,
											uint32_t graphicsQueueFamilyIndex,
											uint32_t graphicsQueueCount,
											uint32_t presentQueueFamilyIndex,
											uint32_t presentQueueCount,
											uint32_t transferQueueFamilyIndex,
											uint32_t transferQueueCount)
{
#if defined RAPTOR_DEBUG_MODE_GENERATION
	if (graphicsQueueCount < NB_RENDERING_RESOURCES)
	{
		RAPTOR_VKERROR(	Global::CVulkanClassID::GetClassId(),
						"Vulkan Device has not enough graphics queues !");
	}
#endif

	graphics_queueFamilyIndex = graphicsQueueFamilyIndex;
	present_queueFamilyIndex = presentQueueFamilyIndex;
	transfer_queueFamilyIndex = transferQueueFamilyIndex;

	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	VkResult res = VK_NOT_READY;

	float *queuePriorities = new float[graphicsQueueCount];
		for (unsigned int i=0;i<graphicsQueueCount;i++)
			queuePriorities[i] = 1.0f;

	uint32_t nb_queues = 0;
	VkDeviceQueueCreateInfo queueCreateInfo[3];

	VkDeviceQueueCreateInfo graphicsQueue = {	VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
												NULL,
												0,
												graphics_queueFamilyIndex,
												graphicsQueueCount,
												queuePriorities
											};
	queueCreateInfo[nb_queues++] = graphicsQueue;
	
	if (present_queueFamilyIndex != graphics_queueFamilyIndex)
	{
		VkDeviceQueueCreateInfo presentQueue = {	VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
													NULL,
													0,
													present_queueFamilyIndex,
													presentQueueCount,
													queuePriorities
												};
		queueCreateInfo[nb_queues++] = presentQueue;
	}
	
	if (transfer_queueFamilyIndex != graphics_queueFamilyIndex)
	{
		VkDeviceQueueCreateInfo transferQueue = {	VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
													NULL,
													0,
													transfer_queueFamilyIndex,
													transferQueueCount,
													queuePriorities
												};
		queueCreateInfo[nb_queues++] = transferQueue;
	}

	uint32_t nb_extensions = 0;
#if defined(VK_KHR_swapchain)
	const char* extensions[1] = { "VK_KHR_swapchain" };
	nb_extensions = 1;
#else
	const char* extensions[0] = NULL;
#endif
	VkDeviceCreateInfo deviceCreateInfo = {	VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
											NULL, 0,
											nb_queues, queueCreateInfo,
											0, NULL,	//	layers
											nb_extensions, extensions,	//	extensions
											//	full available features, many not specifically necessary here
											&features };

	res = vkCreateDevice(	physicalDevice,
							&deviceCreateInfo,
							CVulkanMemory::GetAllocator(),
							&device);

	delete [] queuePriorities;
	if (VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}

	if (!CVulkanMemory::ManageDevice(physicalDevice,device))
		return false;
	else
		pDeviceMemory = CVulkanMemory::CreateMemoryManager(device);


	vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)(vkGetDeviceProcAddr(device,"vkDeviceWaitIdle"));
	vkDestroyDevice = (PFN_vkDestroyDevice)(vkGetDeviceProcAddr(device,"vkDestroyDevice"));
	vkCreateCommandPool = (PFN_vkCreateCommandPool)(vkGetDeviceProcAddr(device,"vkCreateCommandPool"));
	vkDestroyCommandPool = (PFN_vkDestroyCommandPool)(vkGetDeviceProcAddr(device,"vkDestroyCommandPool"));
	vkResetCommandPool = (PFN_vkResetCommandPool)(vkGetDeviceProcAddr(device,"vkResetCommandPool"));
	vkGetDeviceQueue = (PFN_vkGetDeviceQueue)(vkGetDeviceProcAddr(device,"vkGetDeviceQueue"));
	vkCreateSemaphore = (PFN_vkCreateSemaphore)(vkGetDeviceProcAddr(device,"vkCreateSemaphore"));
	vkDestroySemaphore = (PFN_vkDestroySemaphore)(vkGetDeviceProcAddr(device,"vkDestroySemaphore"));
	vkCreateFence = (PFN_vkCreateFence)(vkGetDeviceProcAddr(device,"vkCreateFence"));
	vkWaitForFences = (PFN_vkWaitForFences)(vkGetDeviceProcAddr(device,"vkWaitForFences"));
	vkResetFences = (PFN_vkResetFences)(vkGetDeviceProcAddr(device,"vkResetFences"));
	vkDestroyFence = (PFN_vkDestroyFence)(vkGetDeviceProcAddr(device,"vkDestroyFence"));
	vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)(vkGetDeviceProcAddr(device,"vkAllocateCommandBuffers"));
	vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)(vkGetDeviceProcAddr(device,"vkFreeCommandBuffers"));
	vkCreateRenderPass = (PFN_vkCreateRenderPass)(vkGetDeviceProcAddr(device,"vkCreateRenderPass"));
	vkDestroyRenderPass = (PFN_vkDestroyRenderPass)(vkGetDeviceProcAddr(device,"vkDestroyRenderPass"));
	vkCreateImageView = (PFN_vkCreateImageView)(vkGetDeviceProcAddr(device,"vkCreateImageView"));
	vkDestroyImageView = (PFN_vkDestroyImageView)(vkGetDeviceProcAddr(device,"vkDestroyImageView"));
	vkCreateFramebuffer = (PFN_vkCreateFramebuffer)(vkGetDeviceProcAddr(device,"vkCreateFramebuffer"));
	vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)(vkGetDeviceProcAddr(device,"vkDestroyFramebuffer"));
	

	CVulkanCommandBuffer::vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)(vkGetDeviceProcAddr(device,"vkCmdPipelineBarrier"));
	CVulkanCommandBuffer::vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)(vkGetDeviceProcAddr(device,"vkCmdBeginRenderPass"));
	CVulkanCommandBuffer::vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)(vkGetDeviceProcAddr(device,"vkCmdEndRenderPass"));
	CVulkanCommandBuffer::vkCmdBindPipeline = (PFN_vkCmdBindPipeline)(vkGetDeviceProcAddr(device,"vkCmdBindPipeline"));
	CVulkanCommandBuffer::vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)(vkGetDeviceProcAddr(device,"vkBeginCommandBuffer"));
	CVulkanCommandBuffer::vkEndCommandBuffer = (PFN_vkEndCommandBuffer)(vkGetDeviceProcAddr(device,"vkEndCommandBuffer"));
	CVulkanCommandBuffer::vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)(vkGetDeviceProcAddr(device,"vkCmdBindVertexBuffers"));
	CVulkanCommandBuffer::vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)(vkGetDeviceProcAddr(device,"vkCmdBindIndexBuffer"));
	CVulkanCommandBuffer::vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)(vkGetDeviceProcAddr(device, "vkCmdBindDescriptorSets"));
	CVulkanCommandBuffer::vkCmdSetViewport = (PFN_vkCmdSetViewport)(vkGetDeviceProcAddr(device,"vkCmdSetViewport"));
	CVulkanCommandBuffer::vkCmdSetScissor = (PFN_vkCmdSetScissor)(vkGetDeviceProcAddr(device,"vkCmdSetScissor"));
	CVulkanCommandBuffer::vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)(vkGetDeviceProcAddr(device,"vkCmdCopyBuffer"));
	CVulkanCommandBuffer::vkCmdCopyImage = (PFN_vkCmdCopyImage)(vkGetDeviceProcAddr(device,"vkCmdCopyImage"));
	CVulkanCommandBuffer::vkCmdBlitImage = (PFN_vkCmdBlitImage)(vkGetDeviceProcAddr(device,"vkCmdBlitImage"));
	CVulkanCommandBuffer::vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)(vkGetDeviceProcAddr(device,"vkCmdCopyBufferToImage"));
	CVulkanCommandBuffer::vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)(vkGetDeviceProcAddr(device,"vkCmdCopyImageToBuffer"));
	CVulkanCommandBuffer::vkCmdDraw = (PFN_vkCmdDraw)(vkGetDeviceProcAddr(device,"vkCmdDraw"));
	CVulkanCommandBuffer::vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)(vkGetDeviceProcAddr(device,"vkCmdDrawIndexed"));

	vkQueueSubmit = (PFN_vkQueueSubmit)(vkGetDeviceProcAddr(device,"vkQueueSubmit"));
	vkQueueWaitIdle = (PFN_vkQueueWaitIdle)(vkGetDeviceProcAddr(device,"vkQueueWaitIdle"));

#if defined(VK_KHR_swapchain)
	vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)(vkGetDeviceProcAddr(device,"vkCreateSwapchainKHR"));
	vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)(vkGetDeviceProcAddr(device,"vkDestroySwapchainKHR"));
	vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)(vkGetDeviceProcAddr(device,"vkGetSwapchainImagesKHR"));
	vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)(vkGetDeviceProcAddr(device,"vkAcquireNextImageKHR"));
	vkQueuePresentKHR = (PFN_vkQueuePresentKHR)(vkGetDeviceProcAddr(device,"vkQueuePresentKHR"));
#endif


	//!
	//!	Create Graphics command pool
	//!
	VkCommandPoolCreateInfo commandPoolCreateInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
													NULL,
													VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT|VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
													graphics_queueFamilyIndex };
	res = vkCreateCommandPool(	device,
								&commandPoolCreateInfo,
								CVulkanMemory::GetAllocator(),
								&graphicsCommandPool);
	if (VK_SUCCESS != res)
	{
		CATCH_VK_ERROR(res)
		return false;
	}
	else
	{
		res = vkResetCommandPool(	device,
									graphicsCommandPool,
									VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
		if (VK_SUCCESS != res)
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			return false;
		}
	}

	//!
	//!	Create Present command pool
	//!
	if (graphics_queueFamilyIndex != present_queueFamilyIndex)
	{
		commandPoolCreateInfo.queueFamilyIndex = present_queueFamilyIndex;
		res = vkCreateCommandPool(	device,
									&commandPoolCreateInfo,
									CVulkanMemory::GetAllocator(),
									&presentCommandPool);
		if (VK_SUCCESS != res)
		{
			CATCH_VK_ERROR(res)
			return false;
		}
		else
		{
			res = vkResetCommandPool(	device,
										presentCommandPool,
										VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
			if (VK_SUCCESS != res)
			{
				CATCH_VK_ERROR(res)
				return false;
			}
		}
	}
	else
		presentCommandPool = graphicsCommandPool;

	//!
	//!	Create Transfer command pool
	//!
	if (graphics_queueFamilyIndex != transfer_queueFamilyIndex)
	{
		commandPoolCreateInfo.queueFamilyIndex = transfer_queueFamilyIndex;
		res = vkCreateCommandPool(	device,
									&commandPoolCreateInfo,
									CVulkanMemory::GetAllocator(),
									&transferCommandPool);
		if (VK_SUCCESS != res)
		{
			CATCH_VK_ERROR(res)
			return false;
		}
		else
		{
			res = vkResetCommandPool(	device,
										transferCommandPool,
										VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
			if (VK_SUCCESS != res)
			{
				CATCH_VK_ERROR(res)
				return false;
			}
		}
	}
	else
		transferCommandPool = graphicsCommandPool;

	return true;
}


bool CVulkanDevice::vkDestroySwapChain()
{
	VkResult res = vkDeviceWaitIdle(device);
	switch (res)
	{
		case VK_SUCCESS:
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		case VK_ERROR_DEVICE_LOST:
		{
			CATCH_VK_ERROR(res)
			return false;
			break;
		}
		default:
		{
			CATCH_VK_ERROR(res)
			return false;
			break;
		}
	}

#ifdef VK_KHR_swapchain
	if (VK_NULL_HANDLE != swapChain)
		vkDestroySwapchainKHR(	device,swapChain,
								CVulkanMemory::GetAllocator());
#endif

	if (NULL != renderImages)
	{
		// Images should be cleared upon swap chain destruction
		for (uint32_t i=0;i<swapchainImageCount;i++)
			vkDestroyImageView(	device,renderImages[i].view,
								CVulkanMemory::GetAllocator());
		delete [] renderImages;
		renderImages = NULL;
	}
	swapchainImageCount = 0;

	if (VK_NULL_HANDLE != renderPass)
	{
		vkDestroyRenderPass(device,renderPass,
							CVulkanMemory::GetAllocator());
		renderPass = VK_NULL_HANDLE;
	}

	if (VK_NULL_HANDLE != transferBuffer)
	{
		vkFreeCommandBuffers(	device,
								transferCommandPool,
								1,
								&transferBuffer);
		transferBuffer = VK_NULL_HANDLE;
	}

	if (NULL != renderingResources)
	{
		for (unsigned int i=0;i<NB_RENDERING_RESOURCES;i++)
		{
			VK_RENDERING_RESOURCE& rr = renderingResources[i];
			if (VK_NULL_HANDLE != rr.frameBuffer)
				vkDestroyFramebuffer(	device,
										rr.frameBuffer,
										NULL);

			if (VK_NULL_HANDLE != rr.commandBuffers[0])
				vkFreeCommandBuffers(	device,
										graphicsCommandPool,
										1,
										&rr.commandBuffers[0]);
			rr.commandBuffers.clear();

			//if ((graphics_queueFamilyIndex != present_queueFamilyIndex) &&
			//	(graphicsCommandPool != presentCommandPool) &&
			//	(VK_NULL_HANDLE != presentCommandBuffer))
			//	vkFreeCommandBuffers(device,presentCommandPool,1,&renderingResources[i].presentCommandBuffer);

			if (VK_NULL_HANDLE != rr.imageAvailableSemaphore)
				vkDestroySemaphore(	device,
									rr.imageAvailableSemaphore,
									CVulkanMemory::GetAllocator());
			if (VK_NULL_HANDLE != rr.renderingCompleteSemaphore)
				vkDestroySemaphore(	device,
									rr.renderingCompleteSemaphore,
									CVulkanMemory::GetAllocator());
			if (VK_NULL_HANDLE != rr.queueExecutionComplete)
				vkDestroyFence(	device,
								rr.queueExecutionComplete,
								CVulkanMemory::GetAllocator());
		}

		delete [] renderingResources;
		renderingResources = NULL;
	}

	currentRenderingResources = 0;
	currentImage = MAXUINT;

	return true;
}

bool CVulkanDevice::vkDestroyLogicalDevice(void)
{
	if (VK_NULL_HANDLE == device)
		return false;

	if (!vkDestroySwapChain())
		return false;

	if (VK_NULL_HANDLE != graphicsCommandPool)
		vkDestroyCommandPool(	device,
								graphicsCommandPool,
								CVulkanMemory::GetAllocator());

	if ((graphics_queueFamilyIndex != present_queueFamilyIndex) &&
		(graphicsCommandPool != presentCommandPool) &&
		(VK_NULL_HANDLE != presentCommandPool))
		vkDestroyCommandPool(	device,
								presentCommandPool,
								CVulkanMemory::GetAllocator());

	if ((graphics_queueFamilyIndex != transfer_queueFamilyIndex) &&
		(graphicsCommandPool != transferCommandPool) &&
		(VK_NULL_HANDLE != transferCommandPool))
		vkDestroyCommandPool(	device,
								transferCommandPool,
								CVulkanMemory::GetAllocator());

	graphicsCommandPool = VK_NULL_HANDLE;
	presentCommandPool = VK_NULL_HANDLE;
	transferCommandPool = VK_NULL_HANDLE;

	vkDestroyDevice(device,
					CVulkanMemory::GetAllocator());
	device = VK_NULL_HANDLE;

	return true;
}
