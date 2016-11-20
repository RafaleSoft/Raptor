// VulkanMemory.cpp: implementation of the CVulkanMemory class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys\CodeGeneration.h"

#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif
#if !defined(AFX_VULKANBUFFEROBJECT_H__A159E677_723F_4EED_84C0_D9446234A50B__INCLUDED_)
	#include "Subsys/Vulkan/VulkanBufferObject.h"
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



static void* VKAPI_PTR vkAllocationFunction(void* pUserData,
											size_t size,
											size_t alignment,
											VkSystemAllocationScope allocationScope)
{
	CHostMemoryManager *memory = CHostMemoryManager::GetInstance();
	void* pointer = memory->allocate(size,1,alignment);
	return pointer;
}

static void VKAPI_PTR vkFreeFunction(	void* pUserData,
										void* pMemory)
{
	if (pMemory != NULL)
	{
		CHostMemoryManager *memory = CHostMemoryManager::GetInstance();
		memory->garbage(pMemory);	// free strategy could use allocationScope
	}
	return;
}

static void* VKAPI_PTR vkReallocationFunction(	void* pUserData,
												void* pOriginal,
												size_t size,
												size_t alignment,
												VkSystemAllocationScope allocationScope)
{
	vkFreeFunction(pUserData,pOriginal);
	return vkAllocationFunction(pUserData,size,alignment,allocationScope);
}

static void VKAPI_PTR vkInternalAllocationNotification(	void* pUserData,
														size_t size,
														VkInternalAllocationType allocationType,
														VkSystemAllocationScope allocationScope)
{
	CHostMemoryManager *memory = CHostMemoryManager::GetInstance();
	return;
}

static void VKAPI_PTR vkInternalFreeNotification(	void* pUserData,
													size_t size,
													VkInternalAllocationType allocationType,
													VkSystemAllocationScope allocationScope)
{
	CHostMemoryManager *memory = CHostMemoryManager::GetInstance();
	return;
}

static VkAllocationCallbacks s_vulkanAllocator = 
{
	NULL,
	&vkAllocationFunction,	// pfnAllocation;
	&vkReallocationFunction,	// pfnReallocation;
	&vkFreeFunction,	// pfnFree;
	&vkInternalAllocationNotification,	// pfnInternalAllocation;
	&vkInternalFreeNotification	// pfnInternalFree;
};




CVulkanMemory::CVulkanMemoryWrapper::CVulkanMemoryWrapper(const CVulkanMemory& m,VkDevice d)
:memory(m),device(d)
{
	for (unsigned int i=0;i<IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND;i++)
		currentBuffers[i] = NULL;
}

bool CVulkanMemory::CVulkanMemoryWrapper::relocationAvailable(void) const
{
	return (VK_NULL_HANDLE != device);
}

VkBuffer CVulkanMemory::CVulkanMemoryWrapper::getLockedBuffer(IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind) const
{
	const CVulkanBufferObject* pBuffer = currentBuffers[kind];
	if (NULL != pBuffer)
		return pBuffer->getBuffer();
	else
		return VK_NULL_HANDLE;
}

IDeviceMemoryManager::IBufferObject *
CVulkanMemory::CVulkanMemoryWrapper::createBufferObject(IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind, 
														IDeviceMemoryManager::IBufferObject::BUFFER_MODE mode, 
														uint64_t size)
{
	CVulkanBufferObject* pBuffer = memory.vkCreateBufferObject(device,size,kind);
	m_pBuffers[pBuffer] = pBuffer;
	return pBuffer;
}


void CVulkanMemory::CVulkanMemoryWrapper::setBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
																uint64_t dstOffset,
																const void* src,
																uint64_t sz)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::map<const IDeviceMemoryManager::IBufferObject*,const CVulkanBufferObject*>::const_iterator it = m_pBuffers.find(pIBuffer);
	if (m_pBuffers.end() == it)
		return;
	memory.vkSetBufferObjectData(device,*((*it).second),dstOffset,src,sz);
}

void CVulkanMemory::CVulkanMemoryWrapper::getBufferObjectData(	IDeviceMemoryManager::IBufferObject &vb,
																uint64_t srcOffset,
																void* dst,
																uint64_t sz)
{
}


bool CVulkanMemory::CVulkanMemoryWrapper::lockBufferObject(IDeviceMemoryManager::IBufferObject &bo)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::map<const IDeviceMemoryManager::IBufferObject*,const CVulkanBufferObject*>::const_iterator it = m_pBuffers.find(pIBuffer);
	if (m_pBuffers.end() == it)
		return false;

	const CVulkanBufferObject * const pBuffer = (*it).second;
	currentBuffers[pBuffer->getStorage()] = pBuffer;
	return true;
}

bool CVulkanMemory::CVulkanMemoryWrapper::unlockBufferObject(IDeviceMemoryManager::IBufferObject &bo)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::map<const IDeviceMemoryManager::IBufferObject*,const CVulkanBufferObject*>::const_iterator it = m_pBuffers.find(pIBuffer);
	if (m_pBuffers.end() == it)
		return false;

	const CVulkanBufferObject * const pBuffer = (*it).second;
	if (currentBuffers[pBuffer->getStorage()] != pBuffer)
		return false;
	else
		currentBuffers[pBuffer->getStorage()] = NULL;

	return true;
}

bool CVulkanMemory::CVulkanMemoryWrapper::releaseBufferObject(IDeviceMemoryManager::IBufferObject* &bo)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = bo;
	std::map<const IDeviceMemoryManager::IBufferObject*,const CVulkanBufferObject*>::const_iterator it = m_pBuffers.find(pIBuffer);
	if (m_pBuffers.end() == it)
		return false;

	const CVulkanBufferObject * const pBuffer = (*it).second;
	if (memory.vkDestroyBufferObject(device,pBuffer))
	{
		m_pBuffers.erase(it);
		delete pBuffer;
		bo = NULL;
		return true;
	}
	else
		return false;
}



const VkAllocationCallbacks* CVulkanMemory::GetAllocator(void)
{
	return &s_vulkanAllocator;
}

void CVulkanMemory::vkSetBufferObjectData(VkDevice device,
										  const CVulkanBufferObject &vb,
										  VkDeviceSize dstOffset,
										  const void* srcData,
										  VkDeviceSize sz) const
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
										  const CVulkanBufferObject* pBuffer) const
{
	VkDeviceMemory pMemory = pBuffer->getAddress();
	if (VK_NULL_HANDLE != pMemory)
		vkFreeMemory(device, pMemory, &s_vulkanAllocator);
	else
		return false;

	VkBuffer vkuffer = pBuffer->getBuffer();
	if (VK_NULL_HANDLE != vkuffer)
		vkDestroyBuffer(device,vkuffer,&s_vulkanAllocator);
	else
		return false;

	return true;
}


CVulkanBufferObject* CVulkanMemory::vkCreateBufferObject(	VkDevice device,
															VkDeviceSize size,
															IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind) const
{
	VkBufferUsageFlagBits flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	switch (kind)
	{
		case IDeviceMemoryManager::IBufferObject::VERTEX_BUFFER:
			flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			break;
		case IDeviceMemoryManager::IBufferObject::INDEX_BUFFER:
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
	VkResult res = vkCreateBuffer(device, &buffer_create_info, &s_vulkanAllocator, &buffer);
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

			res = vkAllocateMemory( device, &memory_allocate_info, &s_vulkanAllocator, &pMemory );
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
		vkDestroyBuffer(device, buffer, &s_vulkanAllocator);
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

	return pBuffer;
}

CVulkanMemory::CVulkanMemory()
{
}

CVulkanMemory::~CVulkanMemory(void)
{
}


bool CVulkanMemory::ManageDevice(	VkPhysicalDevice physicalDevice,
									VkDevice device)
{
	if ((VK_NULL_HANDLE == device) ||
		(VK_NULL_HANDLE == physicalDevice))
		return false;

	map<VkPhysicalDevice,CVulkanMemory*>::iterator it = s_pMemories.find(physicalDevice);
	if (s_pMemories.end() == it)
		return false;

	map<VkDevice,CVulkanMemory*>::iterator it2 = s_pMemories2.find(device);
	if (s_pMemories2.end() != it2)
	{
		//	Thie device is managed by another Memory object. Is this case possible ?
		if ((*it).second != (*it2).second)
			return false;
		//	A Memory object exists for this device of this physical device.
		else
			return true;
	}

	//	Associate this device with Memory object of this physical device.
	CVulkanMemory *pMem = (*it).second;
	s_pMemories2[device] = pMem;

	return true;
}

CVulkanMemory::CVulkanMemoryWrapper* CVulkanMemory::CreateMemoryManager(VkDevice device)
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

	CVulkanMemoryWrapper *memory = new CVulkanMemoryWrapper(*pMemory,device);
	return memory;
}

CVulkanMemory& CVulkanMemory::GetInstance(VkPhysicalDevice physicalDevice,
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
