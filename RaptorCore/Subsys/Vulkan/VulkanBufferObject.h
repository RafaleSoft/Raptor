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
	CVulkanBufferObject(VkBuffer buffer,VkDeviceMemory memory)
		:m_size(0),m_storage(VERTEX_BUFFER),m_buffer(buffer),m_address(memory),m_coherent(false)
	{};

	//!	Destructor
	virtual ~CVulkanBufferObject() {};


	virtual uint64_t getSize(void) const;

	virtual BUFFER_KIND getStorage(void) const;

	unsigned int getBufferId(void) const;


	VkDeviceMemory	getAddress(void) const
	{ return m_address; }

	VkBuffer getBuffer(void) const
	{ return m_buffer; }

    //! The size in bytes of the buffer object
	uint64_t	m_size;

	//! Indicates the data storage usage: vertex, pixels, ...
    BUFFER_KIND m_storage;

	//!	Type of host visible memory (do not need flush)
	bool		m_coherent;

private:
    CVulkanBufferObject(const CVulkanBufferObject& );
	CVulkanBufferObject& operator=(const CVulkanBufferObject& );

	VkBuffer		m_buffer;
	VkDeviceMemory	m_address;
};


uint64_t CVulkanBufferObject::getSize(void) const
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

