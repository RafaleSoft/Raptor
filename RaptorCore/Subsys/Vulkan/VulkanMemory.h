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

class CVulkanBufferObject;

class CVulkanMemory
{
public:
#if defined(VK_VERSION_1_0)
	class CVulkanMemoryWrapper : public IDeviceMemoryManager
	{
	public:
		CVulkanMemoryWrapper(const CVulkanMemory& m,VkDevice d);

		virtual bool relocationAvailable(void) const;

		virtual IDeviceMemoryManager::IBufferObject *
			createBufferObject(	IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind, 
								IDeviceMemoryManager::IBufferObject::BUFFER_MODE mode, 
								uint64_t size);

		virtual bool setBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
											uint64_t dstOffset,
											const void* src,
											uint64_t sz);

		virtual bool getBufferObjectData(	IDeviceMemoryManager::IBufferObject &vb,
											uint64_t srcOffset,
											void* dst,
											uint64_t sz);

		virtual bool lockBufferObject(IDeviceMemoryManager::IBufferObject &bo);

		virtual bool unlockBufferObject(IDeviceMemoryManager::IBufferObject &bo);
		
		virtual bool releaseBufferObject(IDeviceMemoryManager::IBufferObject* &bo);

		VkBuffer getLockedBuffer(IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind) const;


	private:
		const CVulkanMemory& memory;
		VkDevice device;

		std::map<const IDeviceMemoryManager::IBufferObject*,const CVulkanBufferObject*> m_pBuffers;
		const CVulkanBufferObject* currentBuffers[IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND];
	};


	//! This method creates a new buffer object :
    //! @param kind : selects a kind of buffer buffer ( vertex, pixel, memory ... )
    //! @param size : sets the size in bytes of the buffer and allocates uninitialized memory
	//! @param initialData : a pointer to data to initialize the buffer if necessary.
    //! @return the newly allocated buffer object or NULL if allocation failed.
    CVulkanBufferObject* vkCreateBufferObject(	VkDevice device,
												VkDeviceSize size,
												IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind) const;

	//!	This method destroys a buffer objet
	bool vkDestroyBufferObject(VkDevice device, const CVulkanBufferObject* pBuffer) const;

	//!	This method work in 3 steps:
	//!	- map the buffer objet to host memory
	//!	- transfer data to a mapped buffer object
	//!	- flushes and unmap the buffer object.
	bool vkSetBufferObjectData(	VkDevice device,
								const CVulkanBufferObject &vb,
								VkDeviceSize dstOffset,
								const void* srcData,
								VkDeviceSize sz) const;

	//!	This method work in 3 steps:
	//!	- map the buffer objet to host memory
	//!	- transfer data from a mapped buffer object
	//!	- flushes and unmap the buffer object.
	bool vkGetBufferObjectData(	VkDevice device,
								const CVulkanBufferObject &vb,
								VkDeviceSize srcOffset,
								void* dst,
								VkDeviceSize sz) const;


	static CVulkanMemoryWrapper* CreateMemoryManager(VkDevice logicalDevice);

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
