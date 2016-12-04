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
#if !defined(AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_)
	#include "Subsys/Vulkan/VulkanCommandBuffer.h"
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
		//memory->release(pMemory);
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
	if (size > 0)
		return vkAllocationFunction(pUserData,size,alignment,allocationScope);
	else
		return pOriginal;
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
		return pBuffer->m_deviceBuffer;
	else
		return VK_NULL_HANDLE;
}

bool CVulkanMemory::CVulkanMemoryWrapper::needBufferObjectDataSynchro(void) const
{
	bool needSynchro = false;

	for (unsigned int i=0;i<IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND;i++)
	{
		const CVulkanBufferObject* pBuffer = currentBuffers[i];
		if (NULL != pBuffer)
		{
			const std::vector<VkBufferCopy> &sync = pBuffer->m_unsynchronizedData;
			if (sync.size() > 0)
			{
				needSynchro = true;
				break;
			}
		}
	}

	return needSynchro;
}

bool CVulkanMemory::CVulkanMemoryWrapper::synchroniseBufferObjectData(const CVulkanCommandBuffer &commandBuffer)
{
	bool res = false;

	for (unsigned int i=0;i<IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND;i++)
	{
		CVulkanBufferObject* pBuffer = const_cast<CVulkanBufferObject*>(currentBuffers[i]);
		if (NULL != pBuffer)
		{
			std::vector<VkBufferCopy> &sync = pBuffer->m_unsynchronizedData;
			if (sync.size() > 0)
			{
				CVulkanCommandBuffer::vkCmdCopyBuffer(	commandBuffer.commandBuffer, 
														pBuffer->m_buffer,
														pBuffer->m_deviceBuffer,
														sync.size(),
														sync.data());

				sync.clear();
				res = true;
			}
		}
	}

	return res;
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


bool CVulkanMemory::CVulkanMemoryWrapper::setBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
																uint64_t dstOffset,
																const void* src,
																uint64_t sz)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::map<const IDeviceMemoryManager::IBufferObject*,CVulkanBufferObject*>::iterator it = m_pBuffers.find(pIBuffer);
	if (m_pBuffers.end() == it)
		return false;
	return memory.vkSetBufferObjectData(device,*((*it).second),dstOffset,src,sz);
}

bool CVulkanMemory::CVulkanMemoryWrapper::getBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
																uint64_t srcOffset,
																void* dst,
																uint64_t sz)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::map<const IDeviceMemoryManager::IBufferObject*,CVulkanBufferObject*>::const_iterator it = m_pBuffers.find(pIBuffer);
	if (m_pBuffers.end() == it)
		return false;
	return memory.vkGetBufferObjectData(device,*((*it).second),srcOffset,dst,sz);
}

bool CVulkanMemory::CVulkanMemoryWrapper::discardBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
																	uint64_t dstOffset,
																	uint64_t sz)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::map<const IDeviceMemoryManager::IBufferObject*,CVulkanBufferObject*>::const_iterator it = m_pBuffers.find(pIBuffer);
	if (m_pBuffers.end() == it)
		return false;

	CVulkanBufferObject &vb = *((*it).second);

	bool erased = false;
	std::vector<VkBufferCopy>::iterator it2 = vb.m_unsynchronizedData.begin();
	while (it2 != vb.m_unsynchronizedData.end())
	{
		VkBufferCopy &outOfDate = *it2;
		erased = ((dstOffset == outOfDate.dstOffset) && (sz == outOfDate.size));
		if (erased)
		{
			vb.m_unsynchronizedData.erase(it2);
			break;
		}
		else
			it2++;
	}

	return erased;
}

bool CVulkanMemory::CVulkanMemoryWrapper::lockBufferObject(IDeviceMemoryManager::IBufferObject &bo)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::map<const IDeviceMemoryManager::IBufferObject*,CVulkanBufferObject*>::const_iterator it = m_pBuffers.find(pIBuffer);
	if (m_pBuffers.end() == it)
		return false;

	const CVulkanBufferObject * const pBuffer = (*it).second;
	currentBuffers[pBuffer->getStorage()] = pBuffer;
	return true;
}

bool CVulkanMemory::CVulkanMemoryWrapper::unlockBufferObject(IDeviceMemoryManager::IBufferObject &bo)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::map<const IDeviceMemoryManager::IBufferObject*,CVulkanBufferObject*>::const_iterator it = m_pBuffers.find(pIBuffer);
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
	std::map<const IDeviceMemoryManager::IBufferObject*,CVulkanBufferObject*>::const_iterator it = m_pBuffers.find(pIBuffer);
	if (m_pBuffers.end() == it)
		return false;

	CVulkanBufferObject * pBuffer = (*it).second;
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

bool CVulkanMemory::vkGetBufferObjectData(	VkDevice device,
											const CVulkanBufferObject &vb,
											VkDeviceSize srcOffset,
											void* dst,
											VkDeviceSize sz) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((NULL == dst) || (0 == sz))
		return false;
	if (vb.getSize() < (srcOffset + sz))
		return false;
#endif
	void *vertex_buffer_memory_pointer = NULL;
	VkResult res = vkMapMemory(	device,
								vb.m_address,
								srcOffset,
								sz,
								0, 
								&vertex_buffer_memory_pointer);
	CATCH_VK_ERROR(res);
	if (VK_SUCCESS != res)
		return false;

	memcpy(dst,vertex_buffer_memory_pointer, sz);

	vkUnmapMemory(device, vb.m_address);

	return true;
}

bool CVulkanMemory::vkSetBufferObjectData(VkDevice device,
										  CVulkanBufferObject &vb,
										  VkDeviceSize dstOffset,
										  const void* srcData,
										  VkDeviceSize sz) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((NULL == srcData) || (0 == sz))
		return false;
	if (vb.getSize() < (dstOffset + sz))
		return false;
#endif
	void *vertex_buffer_memory_pointer = NULL;
	VkResult res = vkMapMemory(	device,
								vb.m_address,
								dstOffset,
								sz,
								0, 
								&vertex_buffer_memory_pointer);
	CATCH_VK_ERROR(res);
	if (VK_SUCCESS != res)
		return false;

	memcpy(vertex_buffer_memory_pointer, srcData, sz);

	if (!vb.m_coherent)
	{
		VkMappedMemoryRange flush_range = {	VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
											NULL,
											vb.m_address,
											dstOffset,
											sz }; // VK_WHOLE_SIZE
		res = vkFlushMappedMemoryRanges(device, 1, &flush_range );
		CATCH_VK_ERROR(res);
		if (VK_SUCCESS != res)
			return false;
	}

	vkUnmapMemory(device, vb.m_address);

	VkBufferCopy outOfDate;
	outOfDate.srcOffset = dstOffset;
	outOfDate.dstOffset = dstOffset;
	outOfDate.size = sz;
	vb.m_unsynchronizedData.push_back(outOfDate);

	return true;
}

bool CVulkanMemory::vkDestroyBufferObject(VkDevice device,
										  CVulkanBufferObject* pBuffer) const
{
	if (VK_NULL_HANDLE != pBuffer->m_deviceAddress)
	{
		vkFreeMemory(device, pBuffer->m_deviceAddress, &s_vulkanAllocator);
		pBuffer->m_deviceAddress = NULL;
	}
	else
		return false;

	if (VK_NULL_HANDLE != pBuffer->m_deviceBuffer)
	{
		vkDestroyBuffer(device,pBuffer->m_deviceBuffer,&s_vulkanAllocator);
		pBuffer->m_deviceBuffer = NULL;
	}
	else
		return false;

	if (VK_NULL_HANDLE != pBuffer->m_address)
	{
		vkFreeMemory(device, pBuffer->m_address, &s_vulkanAllocator);
		pBuffer->m_address = NULL;
	}
	else
		return false;

	if (VK_NULL_HANDLE != pBuffer->m_buffer)
	{
		vkDestroyBuffer(device,pBuffer->m_buffer,&s_vulkanAllocator);
		pBuffer->m_buffer = NULL;
	}
	else
		return false;

	return true;
}

bool needFlush = true;
CVulkanBufferObject* CVulkanMemory::vkCreateBufferObject(	VkDevice device,
															VkDeviceSize size,
															IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind) const
{
	//!
	//!	Create host visible buffer
	//!
	VkBufferCreateInfo buffer_create_info = {	VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
												NULL,
												0,
												size,
												VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
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

	//!
	//!	Select buffer usage
	//!
	VkBufferUsageFlagBits usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	switch (kind)
	{
		case IDeviceMemoryManager::IBufferObject::VERTEX_BUFFER:
			usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			break;
		case IDeviceMemoryManager::IBufferObject::INDEX_BUFFER:
			usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			break;
		case IDeviceMemoryManager::IBufferObject::PIXEL_STORAGE:
			usage = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
			break;
		case IDeviceMemoryManager::IBufferObject::PIXEL_SOURCE:
			usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
			break;
		default:
			usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			break;
	};

	//!
	//!	Create device local buffer for requested usage
	//!
	VkBuffer device_buffer = VK_NULL_HANDLE;
	buffer_create_info.usage = (VkBufferUsageFlagBits)(usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	res = vkCreateBuffer(device, &buffer_create_info, &s_vulkanAllocator, &device_buffer);
	if (VK_SUCCESS != res)
	{
		CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
		pErrMgr->vkGetError(res,__FILE__,__LINE__);
		vkDestroyBuffer(device, buffer, &s_vulkanAllocator);
		return NULL;
	}

	//!
	//!	Allocate host visible memory
	//!
	needFlush = true;
	VkDeviceMemory pMemory = allocateMemory(device, buffer,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	if (VK_NULL_HANDLE == pMemory)
		vkDestroyBuffer(device, buffer, &s_vulkanAllocator);

	//!
	//!	Allocate host visible memory
	//!
	VkDeviceMemory pDeviceMemory = allocateMemory(device, device_buffer,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (VK_NULL_HANDLE == pDeviceMemory)
		vkDestroyBuffer(device, device_buffer, &s_vulkanAllocator);

	if ((VK_NULL_HANDLE != pMemory) && (VK_NULL_HANDLE != pDeviceMemory))
	{
		CVulkanBufferObject *pBuffer = new CVulkanBufferObject();
		pBuffer->m_buffer = buffer;
		pBuffer->m_address = pMemory;
		pBuffer->m_deviceBuffer = device_buffer;
		pBuffer->m_deviceAddress = pDeviceMemory;
		pBuffer->m_size = size;
		pBuffer->m_storage = kind;
		pBuffer->m_coherent = !needFlush;
		return pBuffer;
	}
	else
		return NULL;
}

VkDeviceMemory CVulkanMemory::allocateMemory(VkDevice device, VkBuffer buffer, VkMemoryPropertyFlagBits memory_type) const
{
	VkResult res = VK_NOT_READY;
	VkMemoryRequirements buffer_memory_requirements;
	vkGetBufferMemoryRequirements( device, buffer, &buffer_memory_requirements );

	VkDeviceMemory pMemory = VK_NULL_HANDLE;
	for( uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i )
	{
		if ((buffer_memory_requirements.memoryTypeBits & (1 << i)) && 
			(memory_properties.memoryTypes[i].propertyFlags & memory_type) )
		{
			if (memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
				needFlush = false;
			 VkMemoryAllocateInfo memory_allocate_info = {	VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType 
															NULL,                                    // const void                            *pNext 
															buffer_memory_requirements.size,            // VkDeviceSize                           allocationSize 
															i };// uint32_t                               memoryTypeIndex 

			VkResult res = vkAllocateMemory( device, &memory_allocate_info, &s_vulkanAllocator, &pMemory );
			if( VK_SUCCESS == res )
				break;
			else 
			{
				CATCH_VK_ERROR(res)
				pMemory = VK_NULL_HANDLE;
			}
		}
	}

	if (VK_NULL_HANDLE != pMemory)
	{
		res = vkBindBufferMemory(device, buffer,pMemory,0);
		if (VK_SUCCESS != res)
		{
			RAPTOR_ERROR(	Global::CVulkanClassID::GetClassId(),
							"Allocated memory cannot be mapped to buffer!");
			vkFreeMemory(device,pMemory,NULL);
		}
	}

	return pMemory;
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
