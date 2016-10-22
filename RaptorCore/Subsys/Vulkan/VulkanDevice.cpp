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


RAPTOR_NAMESPACE

CVulkanDevice::CVulkanDevice(void)
{
	device = VK_NULL_HANDLE;
	renderPass = VK_NULL_HANDLE;
	frameBuffers = NULL;
	graphics_queueFamilyIndex = MAXUINT;
	present_queueFamilyIndex = MAXUINT;
	graphicsCommandPool = VK_NULL_HANDLE;
	presentCommandPool = VK_NULL_HANDLE;
#ifdef VK_KHR_swapchain
	swapChain = VK_NULL_HANDLE;
	imageAvailableSemaphore = VK_NULL_HANDLE;
	renderingCompleteSemaphore = VK_NULL_HANDLE;
	currentImage = MAXUINT;
	swapchainImageCount = 0;
	graphicsCommandBuffer = NULL;
	presentCommandBuffer = NULL;
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
	vkAllocateCommandBuffers = NULL;
	vkFreeCommandBuffers = NULL;

	vkCreateRenderPass = NULL;
	vkDestroyRenderPass = NULL;
	vkCreateImageView = NULL;
	vkDestroyImageView = NULL;

	vkCreateFramebuffer = NULL;
	vkDestroyFramebuffer = NULL;
	vkCreateGraphicsPipelines = NULL;
	vkCreateComputePipelines = NULL;
	vkDestroyPipeline = NULL;
	vkCreatePipelineLayout = NULL;
	vkDestroyPipelineLayout = NULL;
	vkCreateShaderModule = NULL;
	vkDestroyShaderModule = NULL;

	vkCmdPipelineBarrier = NULL;
	vkCmdBeginRenderPass = NULL;
	vkCmdEndRenderPass = NULL;
	vkCmdBindPipeline = NULL;
	vkBeginCommandBuffer = NULL;
	vkEndCommandBuffer = NULL;
	vkCmdDraw = NULL;

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
	if (NULL != frameBuffers)
		delete [] frameBuffers;

#ifdef VK_KHR_swapchain
	if (NULL != graphicsCommandBuffer)
		delete [] graphicsCommandBuffer;
	if (NULL != presentCommandBuffer)
		delete [] presentCommandBuffer;
#endif
}

CVulkanPipeline* CVulkanDevice::createPipeline(void) const
{
	if ((VK_NULL_HANDLE != device) &&
		(VK_NULL_HANDLE != renderPass))
	{
		CVulkanPipeline *pipeline = new CVulkanPipeline(device,renderPass);

		pipeline->vkCreateGraphicsPipelines = vkCreateGraphicsPipelines;
		pipeline->vkCreateComputePipelines = vkCreateComputePipelines;
		pipeline->vkDestroyPipeline = vkDestroyPipeline;
		pipeline->vkCreatePipelineLayout = vkCreatePipelineLayout;
		pipeline->vkDestroyPipelineLayout = vkDestroyPipelineLayout;

		return pipeline;
	}
	else
		return NULL;
}

CVulkanShader* CVulkanDevice::createShader(void) const
{
	if (VK_NULL_HANDLE != device)
	{
		CVulkanShader *shader = new CVulkanShader(device);
		shader->vkCreateShaderModule = vkCreateShaderModule;

		return shader;
	}
	else
		return NULL;
}


bool CVulkanDevice::acquireSwapChainImage(uint64_t timeout)
{
	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	if (currentImage != MAXUINT)
		return false;

	VkResult res = VK_NOT_READY;
#ifdef VK_KHR_swapchain
	res = vkAcquireNextImageKHR(device, swapChain, timeout,imageAvailableSemaphore, VK_NULL_HANDLE,&currentImage);
	switch (res)
	{
		case VK_SUCCESS:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
		case VK_SUBOPTIMAL_KHR:
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			break;
		}
		default:
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			break;
		}
	}
#endif

	return (VK_SUCCESS == res);
}

bool CVulkanDevice::presentSwapChainImage()
{
	if (MAXUINT == currentImage)
		return false;

	VkQueue queue = VK_NULL_HANDLE;
	// TODO: check queue index is below queueCount
	vkGetDeviceQueue(device,graphics_queueFamilyIndex,currentImage,&queue);

	VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO,
								NULL,
								1, &imageAvailableSemaphore,
								&wait_dst_stage_mask,
								1, &graphicsCommandBuffer[currentImage],
								1, &renderingCompleteSemaphore };
								
	VkResult res = vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE );
	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	if(VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}

	VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, NULL,
									1, &renderingCompleteSemaphore,
									1, &swapChain,
									&currentImage,
									NULL};

	res = vkQueuePresentKHR(queue,&presentInfo);
	switch (res)
	{
		case VK_SUCCESS:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
		case VK_SUBOPTIMAL_KHR:
		{
			//	onWindowSizeChanged()
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			break;
		}
		default:
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			break;
		}
	}

	currentImage = MAXUINT;
}


bool CVulkanDevice::bindPipeline(const CVulkanPipeline& pipeline,
								 const VkRect2D& scissor,
								 const CColor::RGBA& clearColor)
{
	VkCommandBufferBeginInfo graphics_commandd_buffer_begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
																	NULL,
																	VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
																	NULL };
	VkImageSubresourceRange image_subresource_range = {	VK_IMAGE_ASPECT_COLOR_BIT,
														0, // baseMipLevel;
														1, // levelCount;
														0, // baseArrayLayer;
														1};  // layerCount };

	VkClearValue clear_value;
	clear_value.color.float32[0] = clearColor.r;
	clear_value.color.float32[1] = clearColor.g;
	clear_value.color.float32[2] = clearColor.b;
	clear_value.color.float32[3] = clearColor.a;

	for (uint32_t i=0;i<swapchainImageCount;i++)
	{
		if (VK_SUCCESS != vkBeginCommandBuffer(	graphicsCommandBuffer[i],
												&graphics_commandd_buffer_begin_info ))
			return false;

		if (graphics_queueFamilyIndex != present_queueFamilyIndex)
		{
			VkImageMemoryBarrier barrier_from_present_to_draw = {	VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
																	NULL,
																	VK_ACCESS_MEMORY_READ_BIT, // VkAccessFlagBits
																	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // VkAccessFlags
																	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,	// VkImageLayout
																	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,	// VkImageLayout
																	present_queueFamilyIndex,
																	graphics_queueFamilyIndex,
																	frameBuffers[i].image,
																	image_subresource_range };
			vkCmdPipelineBarrier(	graphicsCommandBuffer[i],
									VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
									VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
									0, // VkDependencyFlags
									0,
									NULL, // const VkMemoryBarrier*
									0,
									NULL, // const VkBufferMemoryBarrier*
									1, &barrier_from_present_to_draw );
		}

		VkRenderPassBeginInfo render_pass_begin_info = {	VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
															NULL,
															renderPass,
															frameBuffers[i].frameBuffer,
															scissor,
															1, &clear_value	};
		vkCmdBeginRenderPass(	graphicsCommandBuffer[i],
								&render_pass_begin_info,
								VK_SUBPASS_CONTENTS_INLINE );

		vkCmdBindPipeline(	graphicsCommandBuffer[i],
							VK_PIPELINE_BIND_POINT_GRAPHICS,
							pipeline.getPipeline() );

		vkCmdDraw( graphicsCommandBuffer[i], 3, 1, 0, 0 );

		vkCmdEndRenderPass(graphicsCommandBuffer[i]);

		if (graphics_queueFamilyIndex != present_queueFamilyIndex)
		{
			VkImageMemoryBarrier barrier_from_draw_to_present = {	VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
																	NULL,
																	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // VkAccessFlags
																	VK_ACCESS_MEMORY_READ_BIT, // VkAccessFlagBits
																	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,	// VkImageLayout
																	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,	// VkImageLayout
																	graphics_queueFamilyIndex,
																	present_queueFamilyIndex,
																	frameBuffers[i].image,
																	image_subresource_range };
			vkCmdPipelineBarrier(	graphicsCommandBuffer[i],
									VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
									VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
									0, // VkDependencyFlags
									0,
									NULL, // const VkMemoryBarrier*
									0,
									NULL, // const VkBufferMemoryBarrier*
									1, &barrier_from_draw_to_present );
		}

		if (VK_SUCCESS != vkEndCommandBuffer(graphicsCommandBuffer[i]))
			return false;
	}

	return true;
}

bool CVulkanDevice::unbindPipeline()
{
	for (uint32_t i=0;i<swapchainImageCount;i++)
	{/*
		vkCmdEndRenderPass(graphicsCommandBuffer[i]);

		if (graphics_queueFamilyIndex != present_queueFamilyIndex)
		{
			VkImageMemoryBarrier barrier_from_draw_to_present = {	VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
																	NULL,
																	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // VkAccessFlags
																	VK_ACCESS_MEMORY_READ_BIT, // VkAccessFlagBits
																	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,	// VkImageLayout
																	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,	// VkImageLayout
																	graphics_queueFamilyIndex,
																	present_queueFamilyIndex,
																	frameBuffers[i].image,
																	image_subresource_range };
			vkCmdPipelineBarrier(	graphicsCommandBuffer[i],
									VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
									VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
									0, // VkDependencyFlags
									0,
									NULL, // const VkMemoryBarrier*
									0,
									NULL, // const VkBufferMemoryBarrier*
									1, &barrier_from_present_to_draw );
		}*/
	}

	return false;
}


bool CVulkanDevice::createRenderPass(	VkSurfaceFormatKHR format,
										uint32_t nbSamples,
										uint32_t width,
										uint32_t height)
{
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
	VkRenderPassCreateInfo pRenderPassCreateInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
													NULL, 0,
													1, &pAttachments,
													1, &pSubpasses,
													0, NULL /*const VkSubpassDependency* pDependencies*/ };

	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	VkResult res = vkCreateRenderPass(device,&pRenderPassCreateInfo,NULL,&renderPass);
	if (VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}

	VkImage *pImages = new VkImage[swapchainImageCount];
	res = vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, pImages);
	if (VK_SUCCESS != res)
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
	else
	{
		frameBuffers = new VK_FRAMEBUFFER[swapchainImageCount];
		for (uint32_t i=0;i<swapchainImageCount;i++)
		{
			frameBuffers[i].image = pImages[i];
			VkImageViewCreateInfo image_view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
															NULL,
															0,
															frameBuffers[i].image,
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
			
			res = vkCreateImageView( device, &image_view_create_info, NULL, &frameBuffers[i].view);
			if (VK_SUCCESS != res)
				pErrMgr->vkGetError(res,__FILE__,__LINE__);

			VkFramebufferCreateInfo pFrameBufferCreateInfo = {	VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
																NULL, 0,
																renderPass,
																1, &frameBuffers[i].view,
																width, height, 1 };

			res = vkCreateFramebuffer(device,&pFrameBufferCreateInfo,NULL,&frameBuffers[i].frameBuffer);
			if (VK_SUCCESS != res)
				pErrMgr->vkGetError(res,__FILE__,__LINE__);
		}
	}

	if (VK_SUCCESS != res)
	{
		if (NULL != frameBuffers)
		{
			for (uint32_t i=0;i<swapchainImageCount;i++)
			{
				vkDestroyFramebuffer(device,frameBuffers[i].frameBuffer,NULL);
				vkDestroyImageView(device,frameBuffers[i].view,NULL);
			}
			delete [] frameBuffers;
			frameBuffers = NULL;
		}
	}
	if (NULL != pImages)
		delete [] pImages;

	return (VK_SUCCESS == res);
}

bool CVulkanDevice::createSwapChain(VkSurfaceKHR surface,
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
											VK_NULL_HANDLE };
	res = vkCreateSwapchainKHR(device,&pCreateInfo,NULL,&swapChain);
	if (VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}

	VkSemaphoreCreateInfo pSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, NULL, 0};
	res = vkCreateSemaphore(device,&pSemaphoreInfo, NULL, &imageAvailableSemaphore);
	if (VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}

	res = vkCreateSemaphore(device,&pSemaphoreInfo, NULL, &renderingCompleteSemaphore);
	if (VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}

	res = vkGetSwapchainImagesKHR(	device, swapChain, &swapchainImageCount, NULL);
	if ((VK_SUCCESS != res) || (swapchainImageCount == 0))
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}
	
	graphicsCommandBuffer = new VkCommandBuffer[swapchainImageCount];
	VkCommandBufferAllocateInfo pCommandBufferInfo = {	VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
														NULL,
														graphicsCommandPool,
														VK_COMMAND_BUFFER_LEVEL_PRIMARY,
														swapchainImageCount	};
	res = vkAllocateCommandBuffers(device,&pCommandBufferInfo,graphicsCommandBuffer);
	if (VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}

	if (graphics_queueFamilyIndex != present_queueFamilyIndex)
	{
		presentCommandBuffer = new VkCommandBuffer[1];
		pCommandBufferInfo.commandPool = presentCommandPool;
		pCommandBufferInfo.commandBufferCount = 1;
		res = vkAllocateCommandBuffers(device,&pCommandBufferInfo,presentCommandBuffer);
		if (VK_SUCCESS != res)
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			return false;
		}
	}
	else
		presentCommandBuffer = graphicsCommandBuffer;

	return true;
}

bool CVulkanDevice::createDevice(const VkPhysicalDevice &physicalDevice,
								const VkPhysicalDeviceFeatures &features,
								uint32_t graphicsQueueFamilyIndex,
								uint32_t graphicsQueueCount,
								uint32_t presentQueueFamilyIndex,
								uint32_t presentQueueCount)
{
	graphics_queueFamilyIndex = graphicsQueueFamilyIndex;
	present_queueFamilyIndex = presentQueueFamilyIndex;

	CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
	VkResult res = VK_NOT_READY;

	float *queuePriorities = new float[graphicsQueueCount];
		for (unsigned int i=0;i<graphicsQueueCount;i++)
			queuePriorities[i] = 1.0f;

	VkDeviceQueueCreateInfo queueCreateInfo[2] =
	{
		{	VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			NULL,
			0,
			graphics_queueFamilyIndex,
			graphicsQueueCount,
			queuePriorities
		},
		{	VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			NULL,
			0,
			present_queueFamilyIndex,
			presentQueueCount,
			queuePriorities
		}
	};
	uint32_t nb_queues = (graphics_queueFamilyIndex == present_queueFamilyIndex ? 1 : 2);
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

	res = vkCreateDevice(physicalDevice,&deviceCreateInfo,NULL,&device);

	delete [] queuePriorities;
	if (VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}
		

	vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)(vkGetDeviceProcAddr(device,"vkDeviceWaitIdle"));
	vkDestroyDevice = (PFN_vkDestroyDevice)(vkGetDeviceProcAddr(device,"vkDestroyDevice"));
	vkCreateCommandPool = (PFN_vkCreateCommandPool)(vkGetDeviceProcAddr(device,"vkCreateCommandPool"));
	vkDestroyCommandPool = (PFN_vkDestroyCommandPool)(vkGetDeviceProcAddr(device,"vkDestroyCommandPool"));
	vkResetCommandPool = (PFN_vkResetCommandPool)(vkGetDeviceProcAddr(device,"vkResetCommandPool"));
	vkGetDeviceQueue = (PFN_vkGetDeviceQueue)(vkGetDeviceProcAddr(device,"vkGetDeviceQueue"));
	vkCreateSemaphore = (PFN_vkCreateSemaphore)(vkGetDeviceProcAddr(device,"vkCreateSemaphore"));
	vkDestroySemaphore = (PFN_vkDestroySemaphore)(vkGetDeviceProcAddr(device,"vkDestroySemaphore"));
	vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)(vkGetDeviceProcAddr(device,"vkAllocateCommandBuffers"));
	vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)(vkGetDeviceProcAddr(device,"vkFreeCommandBuffers"));
	vkCreateRenderPass = (PFN_vkCreateRenderPass)(vkGetDeviceProcAddr(device,"vkCreateRenderPass"));
	vkDestroyRenderPass = (PFN_vkDestroyRenderPass)(vkGetDeviceProcAddr(device,"vkDestroyRenderPass"));
	vkCreateImageView = (PFN_vkCreateImageView)(vkGetDeviceProcAddr(device,"vkCreateImageView"));
	vkDestroyImageView = (PFN_vkDestroyImageView)(vkGetDeviceProcAddr(device,"vkDestroyImageView"));
	vkCreateFramebuffer = (PFN_vkCreateFramebuffer)(vkGetDeviceProcAddr(device,"vkCreateFramebuffer"));
	vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)(vkGetDeviceProcAddr(device,"vkDestroyFramebuffer"));

	vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)(vkGetDeviceProcAddr(device,"vkCreateGraphicsPipelines"));
	vkCreateComputePipelines = (PFN_vkCreateComputePipelines)(vkGetDeviceProcAddr(device,"vkCreateComputePipelines"));
	vkDestroyPipeline = (PFN_vkDestroyPipeline)(vkGetDeviceProcAddr(device,"vkDestroyPipeline"));
	vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)(vkGetDeviceProcAddr(device,"vkCreatePipelineLayout"));
	vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)(vkGetDeviceProcAddr(device,"vkDestroyPipelineLayout"));
	vkCreateShaderModule = (PFN_vkCreateShaderModule)(vkGetDeviceProcAddr(device,"vkCreateShaderModule"));
	vkDestroyShaderModule = (PFN_vkDestroyShaderModule)(vkGetDeviceProcAddr(device,"vkDestroyShaderModule"));

	vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)(vkGetDeviceProcAddr(device,"vkCmdPipelineBarrier"));
	vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)(vkGetDeviceProcAddr(device,"vkCmdBeginRenderPass"));
	vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)(vkGetDeviceProcAddr(device,"vkCmdEndRenderPass"));
	vkCmdBindPipeline = (PFN_vkCmdBindPipeline)(vkGetDeviceProcAddr(device,"vkCmdBindPipeline"));
	vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)(vkGetDeviceProcAddr(device,"vkBeginCommandBuffer"));
	vkEndCommandBuffer = (PFN_vkEndCommandBuffer)(vkGetDeviceProcAddr(device,"vkEndCommandBuffer"));
	vkCmdDraw = (PFN_vkCmdDraw)(vkGetDeviceProcAddr(device,"vkCmdDraw"));

	vkQueueSubmit = (PFN_vkQueueSubmit)(vkGetDeviceProcAddr(device,"vkQueueSubmit"));
	vkQueueWaitIdle = (PFN_vkQueueWaitIdle)(vkGetDeviceProcAddr(device,"vkQueueWaitIdle"));

#if defined(VK_KHR_swapchain)
	vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)(vkGetDeviceProcAddr(device,"vkCreateSwapchainKHR"));
	vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)(vkGetDeviceProcAddr(device,"vkDestroySwapchainKHR"));
	vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)(vkGetDeviceProcAddr(device,"vkGetSwapchainImagesKHR"));
	vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)(vkGetDeviceProcAddr(device,"vkAcquireNextImageKHR"));
	vkQueuePresentKHR = (PFN_vkQueuePresentKHR)(vkGetDeviceProcAddr(device,"vkQueuePresentKHR"));
#endif

	VkCommandPoolCreateInfo commandPoolCreateInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
													NULL,
													VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
													graphics_queueFamilyIndex };
	res = vkCreateCommandPool(device, &commandPoolCreateInfo,NULL, &graphicsCommandPool);
	if (VK_SUCCESS != res)
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}
	else if (VK_SUCCESS != vkResetCommandPool(device,graphicsCommandPool,VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT))
	{
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return false;
	}

	if (graphics_queueFamilyIndex != present_queueFamilyIndex)
	{
		commandPoolCreateInfo.queueFamilyIndex = present_queueFamilyIndex;
		res = vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &presentCommandPool);
		if (VK_SUCCESS != res)
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			return false;
		}
		else if (VK_SUCCESS != vkResetCommandPool(device,presentCommandPool,VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT))
		{
			pErrMgr->vkGetError(res,__FILE__,__LINE__);
			return false;
		}
	}
	else
		presentCommandPool = graphicsCommandPool;

	return true;
}

bool CVulkanDevice::destroyDevice(void)
{
	if (VK_NULL_HANDLE == device)
		return false;

	vkDeviceWaitIdle(device);

	if (VK_NULL_HANDLE != renderPass)
	{
		vkDestroyRenderPass(device,renderPass,NULL);
		renderPass = VK_NULL_HANDLE;
	}

	if (NULL != frameBuffers)
	{
		for (uint32_t i=0;i<swapchainImageCount;i++)
		{
			vkDestroyFramebuffer(device,frameBuffers[i].frameBuffer,NULL);
			vkDestroyImageView(device,frameBuffers[i].view,NULL);
		}
		delete [] frameBuffers;
	}

	if (VK_NULL_HANDLE != graphicsCommandPool)
	{
		if (NULL != graphicsCommandBuffer)
			vkFreeCommandBuffers(device,graphicsCommandPool,swapchainImageCount,graphicsCommandBuffer);
		vkDestroyCommandPool(device,graphicsCommandPool,NULL);
	}

	if ((graphics_queueFamilyIndex != present_queueFamilyIndex) &&
		(graphicsCommandPool != presentCommandPool) &&
		(VK_NULL_HANDLE != presentCommandPool))
	{
		if (NULL != presentCommandBuffer)
			vkFreeCommandBuffers(device,presentCommandPool,1,presentCommandBuffer);
		vkDestroyCommandPool(device,presentCommandPool,NULL);
	}

	graphicsCommandPool = VK_NULL_HANDLE;
	presentCommandPool = VK_NULL_HANDLE;

#ifdef VK_KHR_swapchain
	if (VK_NULL_HANDLE != imageAvailableSemaphore)
		vkDestroySemaphore(device,imageAvailableSemaphore,NULL);
	if (VK_NULL_HANDLE != renderingCompleteSemaphore)
		vkDestroySemaphore(device,renderingCompleteSemaphore,NULL);
	if (VK_NULL_HANDLE != swapChain)
		vkDestroySwapchainKHR(device,swapChain,NULL);
		// TODO: Check swap chain images life time
	swapchainImageCount = 0;

	if (graphicsCommandBuffer != NULL)
		delete [] graphicsCommandBuffer;
	graphicsCommandBuffer = NULL;
	if ((graphics_queueFamilyIndex != present_queueFamilyIndex) &&
		(presentCommandBuffer != NULL))
		delete [] presentCommandBuffer;
	presentCommandBuffer = NULL;
#endif

	vkDestroyDevice(device,NULL);
	device = VK_NULL_HANDLE;

	return true;
}
