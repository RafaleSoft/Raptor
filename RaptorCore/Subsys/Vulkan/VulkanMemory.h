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

#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include <System/Memory.h>
#endif

RAPTOR_NAMESPACE_BEGIN


class CVulkanMemory
{
public:
#if defined(VK_VERSION_1_0)
	class IBufferObject : public CMemory::IBufferObject
    {
	public:
		//!	Returns the allocated memory pointer.
		virtual VkDeviceMemory	getAddress(void) const = 0;
		virtual VkBuffer getBuffer(void) const = 0;

    protected:
		IBufferObject() {};
        virtual ~IBufferObject() {};

	private:
        IBufferObject(const IBufferObject& );
		IBufferObject& operator=(const IBufferObject& );
	};

	class IMemoryWrapper : public IDeviceMemoryManager
	{
	public:
		virtual CVulkanMemory::IBufferObject* 
			vkCreateBufferObject(	VkDeviceSize size,
									CVulkanMemory::IBufferObject::BUFFER_KIND kind = CVulkanMemory::IBufferObject::VERTEX_BUFFER) const = 0;

		
		virtual void vkSetBufferObjectData(	const CVulkanMemory::IBufferObject &vb,
											VkDeviceSize dstOffset,
											const void* srcData,
											VkDeviceSize sz) const = 0;
	};


	//! This method creates a new buffer object :
    //! @param kind : selects a kind of buffer buffer ( vertex, pixel, memory ... )
    //! @param size : sets the size in bytes of the buffer and allocates uninitialized memory
	//! @param initialData : a pointer to data to initialize the buffer if necessary.
    //! @return the newly allocated buffer object or NULL if allocation failed.
    CVulkanMemory::IBufferObject* vkCreateBufferObject(	VkDevice device,
														VkDeviceSize size,
														CVulkanMemory::IBufferObject::BUFFER_KIND kind = CVulkanMemory::IBufferObject::VERTEX_BUFFER) const;

	//!	This method destroys a buffer objet
	bool vkDestroyBufferObject(VkDevice device, CVulkanMemory::IBufferObject* buffer);

	void vkSetBufferObjectData(	VkDevice device,
								const CVulkanMemory::IBufferObject &vb,
								VkDeviceSize dstOffset,
								const void* srcData,
								VkDeviceSize sz) const;


	static CVulkanMemory::IMemoryWrapper* GetInstance(VkDevice physicalDevice);

	static CVulkanMemory& GetInstance(VkPhysicalDevice physicalDevice,
									  const VkPhysicalDeviceMemoryProperties &memory_properties);

	static bool ManageDevice(	VkPhysicalDevice physicalDevice,
								VkDevice device);

	virtual ~CVulkanMemory(void);

	static const VkAllocationCallbacks* GetAllocator(void);


	DECLARE_VK_device_memory(STATIC_LINKAGE);
#endif


private:
	CVulkanMemory();
	CVulkanMemory(const CVulkanMemory& );
	CVulkanMemory& operator=(const CVulkanMemory&);

#if defined(VK_VERSION_1_0)
	//! The unique instances of the global manager (per physical device)
	static std::map<VkPhysicalDevice,CVulkanMemory*>	s_pMemories;
	static std::map<VkDevice,CVulkanMemory*>			s_pMemories2;
	static CVulkanMemory defaultMemory;

	VkPhysicalDeviceMemoryProperties	memory_properties;
#endif
};



RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
