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



RAPTOR_NAMESPACE_BEGIN


class CVulkanCommandBuffer
{
public:
	CVulkanCommandBuffer(	VkCommandBuffer cmdBuffer,
							const VkRect2D& scissor);
	CVulkanCommandBuffer(	VkCommandBuffer cmdBuffer);


	~CVulkanCommandBuffer(void);

	DECLARE_VK_command_buffer(STATIC_LINKAGE)

	VkCommandBuffer	commandBuffer;

private:
	CVulkanCommandBuffer(void);
	CVulkanCommandBuffer(const CVulkanCommandBuffer&);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_)
