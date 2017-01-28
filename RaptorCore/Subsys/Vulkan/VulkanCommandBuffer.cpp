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


RAPTOR_NAMESPACE


PFN_vkCmdPipelineBarrier	CVulkanCommandBuffer::vkCmdPipelineBarrier = NULL;
PFN_vkCmdBeginRenderPass	CVulkanCommandBuffer::vkCmdBeginRenderPass = NULL;
PFN_vkCmdEndRenderPass		CVulkanCommandBuffer::vkCmdEndRenderPass = NULL;
PFN_vkCmdBindPipeline		CVulkanCommandBuffer::vkCmdBindPipeline = NULL;
PFN_vkBeginCommandBuffer	CVulkanCommandBuffer::vkBeginCommandBuffer = NULL;
PFN_vkEndCommandBuffer		CVulkanCommandBuffer::vkEndCommandBuffer = NULL;
PFN_vkCmdBindVertexBuffers	CVulkanCommandBuffer::vkCmdBindVertexBuffers = NULL;
PFN_vkCmdBindIndexBuffer	CVulkanCommandBuffer::vkCmdBindIndexBuffer = NULL;
PFN_vkCmdBindDescriptorSets	CVulkanCommandBuffer::vkCmdBindDescriptorSets = NULL;
PFN_vkCmdSetViewport		CVulkanCommandBuffer::vkCmdSetViewport = NULL;
PFN_vkCmdSetScissor			CVulkanCommandBuffer::vkCmdSetScissor = NULL;
PFN_vkCmdCopyBuffer			CVulkanCommandBuffer::vkCmdCopyBuffer;
PFN_vkCmdCopyImage			CVulkanCommandBuffer::vkCmdCopyImage;
PFN_vkCmdBlitImage			CVulkanCommandBuffer::vkCmdBlitImage;
PFN_vkCmdCopyBufferToImage	CVulkanCommandBuffer::vkCmdCopyBufferToImage;
PFN_vkCmdCopyImageToBuffer	CVulkanCommandBuffer::vkCmdCopyImageToBuffer;
PFN_vkCmdDraw				CVulkanCommandBuffer::vkCmdDraw = NULL;
PFN_vkCmdDrawIndexed		CVulkanCommandBuffer::vkCmdDrawIndexed = NULL;


CVulkanCommandBuffer::CVulkanCommandBuffer(VkCommandBuffer cmdBuffer,const VkRect2D& scissor)
	:commandBuffer(cmdBuffer), retore_barrier(false), view_scissor(scissor), render_pass(false)
{
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
	VkViewport viewport = { 0, 0, view_scissor.extent.width, view_scissor.extent.height, 0.0f, 1.0f };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &view_scissor);
}

void CVulkanCommandBuffer::imageBarrier(uint32_t present_queueFamilyIndex,
										uint32_t graphics_queueFamilyIndex,
										VkImage image)
{
	if (graphics_queueFamilyIndex != present_queueFamilyIndex)
	{
		VkImageSubresourceRange image_subresource_range = { VK_IMAGE_ASPECT_COLOR_BIT,
															0, // baseMipLevel;
															1, // levelCount;
															0, // baseArrayLayer;
															1 };  // layerCount };

		//!	barrier_from_present_to_draw
		image_barrier  = {	VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
							NULL,
							VK_ACCESS_MEMORY_READ_BIT, // VkAccessFlagBits
							VK_ACCESS_MEMORY_READ_BIT, //VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // VkAccessFlags
							VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,	// VkImageLayout
							VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,	// VkImageLayout
							present_queueFamilyIndex,
							graphics_queueFamilyIndex,
							image,
							image_subresource_range };

		CVulkanCommandBuffer::vkCmdPipelineBarrier(	commandBuffer,
													VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
													VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
													0, // VkDependencyFlags
													0,
													NULL, // const VkMemoryBarrier*
													0,
													NULL, // const VkBufferMemoryBarrier*
													1, &image_barrier);
		retore_barrier = true;
	}
}

void CVulkanCommandBuffer::renderPass(	VkRenderPass renderPass,
										VkFramebuffer framebuffer,
										const CColor::RGBA& clearColor)
{
	VkClearValue clear_value;
	clear_value.color.float32[0] = clearColor.r;
	clear_value.color.float32[1] = clearColor.g;
	clear_value.color.float32[2] = clearColor.b;
	clear_value.color.float32[3] = clearColor.a;
	VkRenderPassBeginInfo render_pass_begin_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
													NULL,
													renderPass,
													framebuffer,
													view_scissor,
													1, &clear_value };

	vkCmdBeginRenderPass(	commandBuffer,
							&render_pass_begin_info,
							VK_SUBPASS_CONTENTS_INLINE);
	render_pass = true;
}

