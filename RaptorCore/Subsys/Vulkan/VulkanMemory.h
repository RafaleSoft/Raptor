// VulkanMemory.h: interface for the CVulkanMemory class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
#define AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(__RAPTOR_VKEXT_H__)
	#include "System/vkext.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CVulkanMemory
{
public:
	CVulkanMemory& getInstance(void);
	virtual ~CVulkanMemory(void);

	VkAllocationCallbacks getAllocator(void) const;

private:
	CVulkanMemory(void);
	CVulkanMemory(const CVulkanMemory& );
	CVulkanMemory& operator=(const CVulkanMemory&);

	static CVulkanMemory	*pVulkanMemory;
};



RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
