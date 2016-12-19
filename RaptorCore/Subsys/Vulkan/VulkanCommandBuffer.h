// VulkanCommandBuffer.h: interface for the VulkanCommandBuffer class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_)
#define AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(__RAPTOR_VKEXT_H__)
	#include "System/vkext.h"
#endif
#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CVulkanCommandBuffer
{
public:
	//!	Constructor for a viewport update
	CVulkanCommandBuffer(	VkCommandBuffer cmdBuffer,
							const VkRect2D& scissor);
	//!	Constructor for a command buffer.
	//!	Command buffer is finalized upon destruction of self.
	//!	If a berrier has been inserted, it is reused before destruction.
	CVulkanCommandBuffer(	VkCommandBuffer cmdBuffer);

	//! Insert a pipeline barrier for the parameter image
	//!	- change queue ownership from presentation to graphics
	//!	- the reverse barrier is inserted at command buffer completion
	void imageBarrier(	uint32_t present_queueFamilyIndex,
						uint32_t graphics_queueFamilyIndex,
						VkImage image);

	//!	Initialise the render pass in the command buffer.
	//!	Render pass is finalized upon command buffer destruction.
	void renderPass(VkRenderPass renderPass,
					VkFramebuffer framebuffer,
					const CColor::RGBA& clearColor);

	//!	Insert a viewport and scissor (same size!) in the commnd buffer.
	void resize();


	~CVulkanCommandBuffer(void);


	DECLARE_VK_command_buffer(STATIC_LINKAGE)

	VkCommandBuffer	commandBuffer;

private:
	CVulkanCommandBuffer(void);
	CVulkanCommandBuffer(const CVulkanCommandBuffer&);

	//!	Remember to insert a pipeline barrier when terminating the command buffer
	bool					retore_barrier;
	bool					render_pass;
	VkImageMemoryBarrier	image_barrier;
	VkRect2D				view_scissor;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_)
