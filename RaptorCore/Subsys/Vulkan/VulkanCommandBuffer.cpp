// VulkanCommandBuffer.cpp: implementation of the CVulkanCommandBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys\CodeGeneration.h"

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
	:commandBuffer(cmdBuffer)
{
	VkCommandBufferBeginInfo graphics_command_buffer_begin_info = {	VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
																	NULL,
																	VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
																	NULL };
	
	
	VkResult res = vkBeginCommandBuffer(commandBuffer,
										&graphics_command_buffer_begin_info);
	CATCH_VK_ERROR(res);

	VkViewport viewport = {0, 0, scissor.extent.width, scissor.extent.height, 0.0f, 1.0f };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

CVulkanCommandBuffer::CVulkanCommandBuffer(VkCommandBuffer cmdBuffer)
	:commandBuffer(cmdBuffer)
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
	VkResult res = vkEndCommandBuffer(commandBuffer);
	CATCH_VK_ERROR(res);
}
