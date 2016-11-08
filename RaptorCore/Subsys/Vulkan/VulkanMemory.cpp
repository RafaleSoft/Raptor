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
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif



RAPTOR_NAMESPACE

	class CVulkanBufferObject : public CVulkanMemory::IBufferObject
    {
    public:
		//!	Compute buffer constructor
		CVulkanBufferObject(VkBuffer buffer,VkDeviceMemory memory)
			:m_size(0),address(NULL),m_storage(VERTEX_BUFFER),m_buffer(buffer),m_memory(memory)
		{};

		//!	Destructor
		virtual ~CVulkanBufferObject() {};

		//void *data(void) const 
		//{ return address; };

		void* getBaseAddress(void) const
		{ return address; };

		size_t getSize(void) const
		{ return m_size; };

		BUFFER_KIND getStorage(void) const
		{ return m_storage; }

		unsigned int getBufferId(void) const
		{ return 0; }

		VkDeviceMemory	getAddress(void) const
		{ return m_memory; }

		VkBuffer getBuffer(void) const
		{ return m_buffer; }

		//!	An opaque pointer to the data
		void		*address;

        //! The size in bytes of the buffer object
		size_t		m_size;

		//! Indicates the data storage usage: vertex, pixels, ...
        BUFFER_KIND m_storage;

    private:
        CVulkanBufferObject(const CVulkanBufferObject& ) {};
		CVulkanBufferObject& operator=(const CVulkanBufferObject& ) {return *this; };

		VkBuffer		m_buffer;
		VkDeviceMemory	m_memory;
	};


//!
//!	CVulkanMemory static variables and data
//!
std::map<VkPhysicalDevice,CVulkanMemory*>	CVulkanMemory::s_pMemories;
std::map<VkDevice,CVulkanMemory*>			CVulkanMemory::s_pMemories2;
CVulkanMemory CVulkanMemory::defaultMemory;
PFN_vkGetBufferMemoryRequirements CVulkanMemory::vkGetBufferMemoryRequirements = VK_NULL_HANDLE;
PFN_vkCreateBuffer CVulkanMemory::vkCreateBuffer = VK_NULL_HANDLE;
PFN_vkDestroyBuffer CVulkanMemory::vkDestroyBuffer = VK_NULL_HANDLE;
PFN_vkAllocateMemory CVulkanMemory::vkAllocateMemory = VK_NULL_HANDLE;
PFN_vkBindBufferMemory CVulkanMemory::vkBindBufferMemory = VK_NULL_HANDLE;
PFN_vkFreeMemory CVulkanMemory::vkFreeMemory = VK_NULL_HANDLE;
PFN_vkMapMemory CVulkanMemory::vkMapMemory = VK_NULL_HANDLE;
PFN_vkFlushMappedMemoryRanges CVulkanMemory::vkFlushMappedMemoryRanges = VK_NULL_HANDLE;
PFN_vkUnmapMemory CVulkanMemory::vkUnmapMemory = VK_NULL_HANDLE;


static VkAllocationCallbacks s_vulkanAllocator = 
{
	NULL,
	NULL,	// pfnAllocation;
	NULL,	// pfnReallocation;
	NULL,	// pfnFree;
	NULL,	// pfnInternalAllocation;
	NULL	// pfnInternalFree;
};


void CVulkanMemory::vkSetBufferObjectData(VkDevice device,
										  const CVulkanMemory::IBufferObject &vb,
										  VkDeviceSize dstOffset,
										  const void* srcData,
										  VkDeviceSize sz)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((NULL == srcData) || (0 == sz))
		return;
	if (vb.getSize() < (dstOffset + sz))
		return;
#endif
	void *vertex_buffer_memory_pointer = NULL;
	VkResult res = vkMapMemory(	device,
								vb.getAddress(),
								dstOffset,
								sz,
								0, 
								&vertex_buffer_memory_pointer);
	if (VK_ERROR_MEMORY_MAP_FAILED == res)
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Failed to map Buffer Objects to host memory for this device !");
		return;
	}
	else if (VK_ERROR_OUT_OF_DEVICE_MEMORY == res)
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Out of memory on this device !");
		return;
	}
	else if (VK_ERROR_OUT_OF_HOST_MEMORY == res)
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Out of memory on host !");
		return;
	}

	memcpy(vertex_buffer_memory_pointer, srcData, sz);

	VkMappedMemoryRange flush_range = {	VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
										NULL,
										vb.getAddress(),
										dstOffset,
										sz }; // VK_WHOLE_SIZE
	res = vkFlushMappedMemoryRanges(device, 1, &flush_range );
	if (VK_ERROR_OUT_OF_DEVICE_MEMORY == res)
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Out of memory on this device !");
		return;
	}
	else if (VK_ERROR_OUT_OF_HOST_MEMORY == res)
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Out of memory on host !");
		return;
	}

	vkUnmapMemory(device, vb.getAddress());
}

bool CVulkanMemory::vkDestroyBufferObject(VkDevice device,
										  CVulkanMemory::IBufferObject* buffer)
{
	if (NULL == buffer)
		return false;

	VkDeviceMemory pMemory = buffer->getAddress();
	if (VK_NULL_HANDLE != pMemory)
		vkFreeMemory(device, pMemory, NULL);
	else
		return false;

	VkBuffer pBuffer = VK_NULL_HANDLE;
	vkDestroyBuffer(device,pBuffer,NULL);

	return true;
}

CVulkanMemory::IBufferObject* CVulkanMemory::vkCreateBufferObject(	VkDevice device,
																	VkDeviceSize size,
																	CVulkanMemory::IBufferObject::BUFFER_KIND kind)
{
	VkBufferUsageFlagBits flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	switch (kind)
	{
		case CVulkanMemory::IBufferObject::VERTEX_BUFFER:
			flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			break;
		case CVulkanMemory::IBufferObject::INDEX_BUFFER:
			flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			break;
		default:
			flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			break;
	};

	VkBufferCreateInfo buffer_create_info = {	VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
												NULL,
												0,
												size,
												flags,
												VK_SHARING_MODE_EXCLUSIVE,
												0, NULL };
	VkBuffer buffer = VK_NULL_HANDLE;
	VkResult res = vkCreateBuffer(device, &buffer_create_info, NULL, &buffer);
	if (VK_SUCCESS != res)
	{
		CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		return NULL;
	}

	VkMemoryRequirements buffer_memory_requirements;
	vkGetBufferMemoryRequirements( device, buffer, &buffer_memory_requirements );

	VkDeviceMemory pMemory = VK_NULL_HANDLE;
	for( uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i )
	{
		if ((buffer_memory_requirements.memoryTypeBits & (1 << i)) && 
			(memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) )
		{ 
			 VkMemoryAllocateInfo memory_allocate_info = {	VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType 
															NULL,                                    // const void                            *pNext 
															buffer_memory_requirements.size,            // VkDeviceSize                           allocationSize 
															i };// uint32_t                               memoryTypeIndex 

			res = vkAllocateMemory( device, &memory_allocate_info, NULL, &pMemory );
			if( VK_SUCCESS == res )
				break;
			else 
			{
				if (VK_ERROR_TOO_MANY_OBJECTS == res)
				{
					RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
									"Too many Buffer Objects for this device !");
				}
				else if (VK_ERROR_OUT_OF_DEVICE_MEMORY == res)
				{
					RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
									"Out of memory on this device !");
				}
				else if (VK_ERROR_OUT_OF_HOST_MEMORY == res)
				{
					RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
									"Out of memory on host !");
				}

				pMemory = VK_NULL_HANDLE;
			}
		}
	}

	if (VK_NULL_HANDLE == pMemory)
	{
		vkDestroyBuffer(device, buffer, NULL);
		return NULL;
	}

	res = vkBindBufferMemory(device, buffer,pMemory,0);
	if (VK_SUCCESS != res)
	{
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"Allocated memory cannot be mapped to buffer!");
		vkFreeMemory(device,pMemory,NULL);
		vkDestroyBuffer(device, buffer, NULL);
		return NULL;
	}

	CVulkanBufferObject *pBuffer = new CVulkanBufferObject(buffer,pMemory);
	pBuffer->m_size = size;
	pBuffer->m_storage = kind;
	pBuffer->address = 0;

	return pBuffer;
}

CVulkanMemory::CVulkanMemory()
{
}

CVulkanMemory::~CVulkanMemory(void)
{
}


bool CVulkanMemory::manageDevice(	VkPhysicalDevice physicalDevice,
									VkDevice device)
{
	if ((VK_NULL_HANDLE == device) ||
		(VK_NULL_HANDLE == physicalDevice))
		return false;

	map<VkPhysicalDevice,CVulkanMemory*>::iterator it = s_pMemories.find(physicalDevice);
	if (it == s_pMemories.end())
		return false;

	
	if (s_pMemories2.end() != s_pMemories2.find(device))
		return false;

	CVulkanMemory *pMem = (*it).second;
	s_pMemories2[device] = pMem;

	return true;
}

CVulkanMemory& CVulkanMemory::getInstance(VkDevice device)
{
	CVulkanMemory* pMemory = NULL;
	map<VkDevice,CVulkanMemory*>::iterator it = s_pMemories2.find(device);
	if (it != s_pMemories2.end())
		pMemory = (*it).second;
	else
	{
		pMemory = &defaultMemory;
		RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
						"No memory object allocated to this device");
	}

	return *pMemory;
}

CVulkanMemory& CVulkanMemory::getInstance(VkPhysicalDevice physicalDevice,
										  const VkPhysicalDeviceMemoryProperties &memory_properties)
{
	if (VK_NULL_HANDLE == physicalDevice)
		return defaultMemory;

	map<VkPhysicalDevice,CVulkanMemory*>::iterator it = s_pMemories.find(physicalDevice);

	CVulkanMemory *pMem = NULL;
	if (it == s_pMemories.end())
	{
		pMem = new CVulkanMemory();
		pMem->memory_properties = memory_properties;
		s_pMemories[physicalDevice] = pMem;
	}
	else
	{
		pMem = (*it).second;
	}

	return *pMem;
}

VkAllocationCallbacks CVulkanMemory::getAllocator(void) const
{
	return s_vulkanAllocator;
}
