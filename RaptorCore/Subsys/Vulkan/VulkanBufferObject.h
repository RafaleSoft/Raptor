#if !defined(AFX_VULKANBUFFEROBJECT_H__A159E677_723F_4EED_84C0_D9446234A50B__INCLUDED_)
#define AFX_VULKANBUFFEROBJECT_H__A159E677_723F_4EED_84C0_D9446234A50B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

RAPTOR_NAMESPACE_BEGIN


class CVulkanBufferObject : public IDeviceMemoryManager::IBufferObject
{
public:
	//!	Vulkan buffer objet constructor
	CVulkanBufferObject()
		:m_size(0),m_storage(VERTEX_BUFFER),m_coherent(false),
		m_buffer(0),m_address(0),m_deviceBuffer(0),m_deviceAddress(0)
	{};

	//!	Destructor
	virtual ~CVulkanBufferObject() {};

	//! Implements IDeviceMemoryManager::IBufferObject
	virtual uint64_t getSize(void) const;

	//! Implements IDeviceMemoryManager::IBufferObject
	virtual BUFFER_KIND getStorage(void) const;

	//! Implements IDeviceMemoryManager::IBufferObject
	unsigned int getBufferId(void) const;


    //! The size in bytes of the buffer object
	uint64_t	m_size;

	//! Indicates the data storage usage: vertex, pixels, ...
    BUFFER_KIND m_storage;

	//!	Type of host visible memory (do not need flush)
	bool		m_coherent;

	//!	Buffer and memory handles
	VkBuffer		m_buffer;
	VkDeviceMemory	m_address;
	VkBuffer		m_deviceBuffer;
	VkDeviceMemory	m_deviceAddress;

	//! The array of unsynchronized host_visible vs. device local memory data chunks
	std::vector<VkBufferCopy>	m_unsynchronizedData;


private:
    CVulkanBufferObject(const CVulkanBufferObject& );
	CVulkanBufferObject& operator=(const CVulkanBufferObject& );
};


inline uint64_t CVulkanBufferObject::getSize(void) const
{
	return m_size;
}

inline IDeviceMemoryManager::IBufferObject::BUFFER_KIND CVulkanBufferObject::getStorage(void) const
{
	return m_storage;
}

inline unsigned int CVulkanBufferObject::getBufferId(void) const
{
	return 0;
}

RAPTOR_NAMESPACE_END


#endif // !defined(AFX_VULKANBUFFEROBJECT_H__A159E677_723F_4EED_84C0_D9446234A50B__INCLUDED_)

