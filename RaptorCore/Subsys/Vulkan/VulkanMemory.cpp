// VulkanMemory.cpp: implementation of the CVulkanMemory class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys\CodeGeneration.h"

#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE

static VkAllocationCallbacks s_vulkanAllocator = 
{
	NULL,
	NULL,	// pfnAllocation;
	NULL,	// pfnReallocation;
	NULL,	// pfnFree;
	NULL,	// pfnInternalAllocation;
	NULL	// pfnInternalFree;
};

CVulkanMemory	*CVulkanMemory::pVulkanMemory = NULL;

CVulkanMemory::CVulkanMemory(void)
{
}

CVulkanMemory::~CVulkanMemory(void)
{
}

CVulkanMemory& CVulkanMemory::getInstance(void)
{
	if (NULL == pVulkanMemory)
	{
		pVulkanMemory = new CVulkanMemory();
	}

	return *pVulkanMemory;
}

VkAllocationCallbacks CVulkanMemory::getAllocator(void) const
{
	return s_vulkanAllocator;
}
