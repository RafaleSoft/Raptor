// VulkanPipeline.h: interface for the CVulkanPipeline class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_)
#define AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(__RAPTOR_VKEXT_H__)
	#include "System/vkext.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CVulkanShader;

class CVulkanPipeline
{
public:
	CVulkanPipeline(VkDevice device,
					VkRenderPass renderPass);
	virtual ~CVulkanPipeline(void);

	bool initPipeline(void);
	bool destroyPipeline(void);

	VkPipeline getPipeline(void) const { return pipeline; };

	bool addShader(CVulkanShader* pShader);

#if defined(VK_VERSION_1_0)
	DECLARE_VK_device(DEFAULT_LINKAGE)
#endif

private:
	std::vector<CVulkanShader*> m_shaders;

#if defined(VK_VERSION_1_0)
	VkDevice		device;
	VkRenderPass	renderPass;
	VkPipeline		pipeline;
	VkPipelineLayout layout;
#endif
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_)
