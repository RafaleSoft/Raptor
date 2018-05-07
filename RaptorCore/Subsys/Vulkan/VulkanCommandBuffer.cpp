// VulkanCommandBuffer.cpp: implementation of the CVulkanCommandBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_)
	#include "VulkanCommandBuffer.h"
#endif
#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#ifndef __vkext_macros_h_
	#include "System/VKEXTMacros.h"
#endif


RAPTOR_NAMESPACE

IMPLEMENT_RAPTOR_VK_command_buffer(CVulkanCommandBuffer)

/*
 * Graphics pipeline order:
 *
	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
	VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
	VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
	VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
	VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
	VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
	VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
	VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
	VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
 *
 */


CVulkanCommandBuffer::CVulkanCommandBuffer(VkCommandBuffer cmdBuffer,const VkRect2D& scissor)
	:commandBuffer(cmdBuffer), retore_barrier(false), view_scissor(scissor), render_pass(false)
{
	image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_barrier.pNext = NULL;
	image_barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	image_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT; //VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // VkAccessFlags
	image_barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;	// VkImageLayout
	image_barrier.newLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;	// VkImageLayout
	image_barrier.srcQueueFamilyIndex = 0;
	image_barrier.dstQueueFamilyIndex = 0;
	image_barrier.image = VK_NULL_HANDLE;
	VkImageSubresourceRange range = { 0, 0, 0, 0, 0 };
	image_barrier.subresourceRange = range;

	VkCommandBufferBeginInfo graphics_command_buffer_begin_info = {	VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
																	NULL,
																	VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
																	NULL };
	
	
	VkResult res = vkBeginCommandBuffer(commandBuffer,
										&graphics_command_buffer_begin_info);
	CATCH_VK_ERROR(res);
}

CVulkanCommandBuffer::CVulkanCommandBuffer(VkCommandBuffer cmdBuffer)
	:commandBuffer(cmdBuffer), retore_barrier(false), render_pass(false)
{
	VkCommandBufferBeginInfo copy_command_buffer_begin_info = {	VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
																NULL,
																VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
																NULL };
	
	
	VkResult res = vkBeginCommandBuffer(commandBuffer,
										&copy_command_buffer_begin_info);
	CATCH_VK_ERROR(res);
}

CVulkanCommandBuffer::~CVulkanCommandBuffer(void)
{
	if (retore_barrier)
	{
		VkImageSubresourceRange image_subresource_range = { VK_IMAGE_ASPECT_COLOR_BIT,
															0, // baseMipLevel;
															1, // levelCount;
															0, // baseArrayLayer;
															1 };  // layerCount };

		uint32_t oldQueue = image_barrier.srcQueueFamilyIndex;
		image_barrier.srcQueueFamilyIndex = image_barrier.dstQueueFamilyIndex;
		image_barrier.dstQueueFamilyIndex = oldQueue;

		CVulkanCommandBuffer::vkCmdPipelineBarrier(	commandBuffer,
													VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
													VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
													0, // VkDependencyFlags
													0,
													NULL, // const VkMemoryBarrier*
													0,
													NULL, // const VkBufferMemoryBarrier*
													1, &image_barrier);

		retore_barrier = false;
	}

	if (render_pass)
		vkCmdEndRenderPass(commandBuffer);
	
	VkResult res = vkEndCommandBuffer(commandBuffer);
	CATCH_VK_ERROR(res);
}

void CVulkanCommandBuffer::resize()
{
	//!	zMin and zFar MUST be 0.0f and 1.0f
	//! Otherwise, it has no effect or worse, not supported and renders anything
	//!	This implies z clip coordinates of any vertex belongs to [0.0f..1.0f]
	VkViewport viewport = { 0, 0, view_scissor.extent.width, view_scissor.extent.height, 0.0f, 1.0f };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &view_scissor);
}

void CVulkanCommandBuffer::imageBarrier(uint32_t present_queueFamilyIndex,
										uint32_t graphics_queueFamilyIndex,
										VkImage image)
{
	if ((graphics_queueFamilyIndex != present_queueFamilyIndex) &&
		(VK_NULL_HANDLE != image))
	{
		VkImageSubresourceRange image_subresource_range = { VK_IMAGE_ASPECT_COLOR_BIT,
															0, // baseMipLevel;
															1, // levelCount;
															0, // baseArrayLayer;
															1 };  // layerCount };

		//!	barrier_from_present_to_draw
		image_barrier.srcQueueFamilyIndex = present_queueFamilyIndex;
		image_barrier.dstQueueFamilyIndex = graphics_queueFamilyIndex;
		image_barrier.image = image;
		image_barrier.subresourceRange = image_subresource_range;

		CVulkanCommandBuffer::vkCmdPipelineBarrier(	commandBuffer,
													VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
													VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
													0, // VkDependencyFlags
													0, NULL, // const VkMemoryBarrier*
													0, NULL, // const VkBufferMemoryBarrier*
													1, &image_barrier);
		retore_barrier = true;
	}
}


void CVulkanCommandBuffer::imageBarrier(VkImageLayout oldLayout,
										VkImageLayout newLayout,
										VkImage image,
										VkImageAspectFlags aspect) const
{
	if ((oldLayout != newLayout) && (VK_NULL_HANDLE != image))
	{
		VkImageSubresourceRange image_subresource_range = { aspect,
															0, // baseMipLevel;
															1, // levelCount;
															0, // baseArrayLayer;
															1 };  // layerCount };

		//!	barrier_from_present_to_draw
		VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
										NULL,
										VK_ACCESS_TRANSFER_WRITE_BIT,			// VkAccessFlagBits
										VK_ACCESS_TRANSFER_WRITE_BIT,			// VkAccessFlagsBits
										oldLayout,								// VkImageLayout
										newLayout,								// VkImageLayout
										VK_QUEUE_FAMILY_IGNORED,
										VK_QUEUE_FAMILY_IGNORED,
										image,
										image_subresource_range };

		VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}

		CVulkanCommandBuffer::vkCmdPipelineBarrier(commandBuffer,
												   sourceStage,
												   destinationStage,
												   0, // VkDependencyFlags
												   0, NULL, // const VkMemoryBarrier*
												   0, NULL, // const VkBufferMemoryBarrier*
												   1, &barrier);
	}
}

void CVulkanCommandBuffer::renderPass(	VkRenderPass renderPass,
										VkFramebuffer framebuffer,
										const CColor::RGBA& clearColor,
										float clearDepth,
										int clearStencil)
{
	VkClearValue clear_value[2];
	clear_value[0].color.float32[0] = clearColor.r;
	clear_value[0].color.float32[1] = clearColor.g;
	clear_value[0].color.float32[2] = clearColor.b;
	clear_value[0].color.float32[3] = clearColor.a;
	clear_value[1].depthStencil.depth = clearDepth;
	clear_value[1].depthStencil.stencil = clearStencil;
	VkRenderPassBeginInfo render_pass_begin_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
													NULL,
													renderPass,
													framebuffer,
													view_scissor,
													2, clear_value };

	vkCmdBeginRenderPass(	commandBuffer,
							&render_pass_begin_info,
							VK_SUBPASS_CONTENTS_INLINE);
	render_pass = true;
}

void CVulkanCommandBuffer::copyBuffer(VkBuffer buffer,
									  VkImage image,
									  VkBufferImageCopy outOfDate) const
{
	vkCmdCopyBufferToImage(commandBuffer,
						   buffer,
						   image,
						   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						   1, &outOfDate);
}

void CVulkanCommandBuffer::copyBuffer(VkBuffer hostBuffer,
									  VkBuffer deviceBuffer,
									  VkBufferCopy outOfDate) const
{
	vkCmdCopyBuffer(commandBuffer,
					hostBuffer,
					deviceBuffer,
					1,
					&outOfDate);
}