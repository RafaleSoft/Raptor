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
PFN_vkGetImageMemoryRequirements CVulkanMemory::vkGetImageMemoryRequirements = VK_NULL_HANDLE;
PFN_vkGetImageSparseMemoryRequirements CVulkanMemory::vkGetImageSparseMemoryRequirements = VK_NULL_HANDLE;
PFN_vkCreateBuffer CVulkanMemory::vkCreateBuffer = VK_NULL_HANDLE;
PFN_vkDestroyBuffer CVulkanMemory::vkDestroyBuffer = VK_NULL_HANDLE;
PFN_vkAllocateMemory CVulkanMemory::vkAllocateMemory = VK_NULL_HANDLE;
PFN_vkBindBufferMemory CVulkanMemory::vkBindBufferMemory = VK_NULL_HANDLE;
PFN_vkCreateImage CVulkanMemory::vkCreateImage = VK_NULL_HANDLE;
PFN_vkDestroyImage CVulkanMemory::vkDestroyImage = VK_NULL_HANDLE;
PFN_vkBindImageMemory CVulkanMemory::vkBindImageMemory = VK_NULL_HANDLE;
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
	if (NULL != pMemory)
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
	if (NULL == pOriginal)
		return vkAllocationFunction(pUserData, size, alignment, allocationScope);
	else if (0 == size)
	{
		vkFreeFunction(pUserData, pOriginal);
		return NULL;
	}
	else
	{
		CHostMemoryManager *memory = CHostMemoryManager::GetInstance();
		return memory->reallocate(pOriginal, size, 1, alignment);
	}
}

static void VKAPI_PTR vkInternalAllocationNotification(	void* pUserData,
														size_t size,
														VkInternalAllocationType allocationType,
														VkSystemAllocationScope allocationScope)
{
	//CHostMemoryManager *memory = CHostMemoryManager::GetInstance();
	return;
}

static void VKAPI_PTR vkInternalFreeNotification(	void* pUserData,
													size_t size,
													VkInternalAllocationType allocationType,
													VkSystemAllocationScope allocationScope)
{
	//CHostMemoryManager *memory = CHostMemoryManager::GetInstance();
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
			const std::vector<VkBufferCopy> &sync = pBuffer->m_unsynchronizedBuffers;
			if (sync.size() > 0)
			{
				needSynchro = true;
				break;
			}

			const std::vector<CVulkanBufferObject::unsynchronizedImage> &sync2 = pBuffer->m_unsynchronizedImages;
			if (sync2.size() > 0)
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

	//!	Step 1 : transition image layouts
	for (unsigned int i = 0; i < IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND; i++)
	{
		CVulkanBufferObject* pBuffer = const_cast<CVulkanBufferObject*>(currentBuffers[i]);
		if (NULL != pBuffer)
		{
			std::vector<CVulkanBufferObject::unsynchronizedImage> &sync2 = pBuffer->m_unsynchronizedImages;
			for (size_t i = 0; i < sync2.size(); i++)
			{
				CVulkanBufferObject::unsynchronizedImage& synchro = sync2[i];
				commandBuffer.imageBarrier(VK_IMAGE_LAYOUT_UNDEFINED,
										   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
										   synchro.image);
			}
		}
	}

	//!	Step 2 : copy buffers and images
	for (unsigned int i=0;i<IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND;i++)
	{
		CVulkanBufferObject* pBuffer = const_cast<CVulkanBufferObject*>(currentBuffers[i]);
		if (NULL != pBuffer)
		{
			std::vector<VkBufferCopy> &sync = pBuffer->m_unsynchronizedBuffers;
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

			std::vector<CVulkanBufferObject::unsynchronizedImage> &sync2 = pBuffer->m_unsynchronizedImages;
			for (size_t i = 0; i < sync2.size(); i++)
			{
				CVulkanBufferObject::unsynchronizedImage& synchro = sync2[i];
				CVulkanCommandBuffer::vkCmdCopyBufferToImage( commandBuffer.commandBuffer,
															  pBuffer->m_buffer,
															  synchro.image,
															  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
															  1, &synchro.bufferCopy);
				res = true;
			}
			//	Do not clear unsynchronised data here : we need additional transitions.
			//sync2.clear();
		}
	}

	//!	Step 3 : transition image layouts
	for (unsigned int i = 0; i < IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND; i++)
	{
		CVulkanBufferObject* pBuffer = const_cast<CVulkanBufferObject*>(currentBuffers[i]);
		if (NULL != pBuffer)
		{
			std::vector<CVulkanBufferObject::unsynchronizedImage> &sync2 = pBuffer->m_unsynchronizedImages;
			for (size_t i = 0; i < sync2.size(); i++)
			{
				CVulkanBufferObject::unsynchronizedImage& synchro = sync2[i];
				commandBuffer.imageBarrier(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
										   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
										   synchro.image);
			}

			sync2.clear();
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
	pair<const IDeviceMemoryManager::IBufferObject*, CVulkanBufferObject*> p(pBuffer, pBuffer);
	m_pBuffers.push_back(p);
	return pBuffer;
}


bool CVulkanMemory::CVulkanMemoryWrapper::setBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
																uint64_t dstOffset,
																const void* src,
																uint64_t sz)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::list<pair<const IDeviceMemoryManager::IBufferObject*, CVulkanBufferObject*>>::iterator it = m_pBuffers.begin();
	while (it != m_pBuffers.end())
	{
		if ((*it).first == pIBuffer)
			break;
		else
			it++;
	}
	if (m_pBuffers.end() == it)
		return false;
	CVulkanBufferObject& vb = *((*it).second);
	bool set_data = memory.vkSetBufferObjectData(device,vb,dstOffset,src,sz);

	if (set_data)
	{
		if ((IDeviceMemoryManager::IBufferObject::PIXEL_SOURCE == vb.getStorage()) &&
			(0 == vb.m_deviceBuffer))
		{
			std::map<VkDeviceSize, data_bloc>::iterator it = m_images.find(dstOffset);
			data_bloc &db = (*it).second;

			VkBufferImageCopy outOfDate;
			outOfDate.bufferOffset = dstOffset;
			outOfDate.bufferImageHeight = 0;	// image data tight packing in buffer -> image extent
			outOfDate.bufferRowLength = 0;		// image data tight packing in buffer -> image extent

			VkImageSubresourceLayers layers = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
			outOfDate.imageSubresource = layers;

			VkOffset3D offset = { 0, 0, 0 };
			outOfDate.imageOffset = offset;

			outOfDate.imageExtent = db.imageExtent;

			CVulkanBufferObject::unsynchronizedImage synchro;
			synchro.image = db.image;
			synchro.bufferCopy = outOfDate;
			vb.m_unsynchronizedImages.push_back(synchro);
		}
		else
		{
			VkBufferCopy outOfDate;
			outOfDate.srcOffset = dstOffset;
			outOfDate.dstOffset = dstOffset;
			outOfDate.size = sz;
			vb.m_unsynchronizedBuffers.push_back(outOfDate);
		}
	}

	return set_data;
}

bool CVulkanMemory::CVulkanMemoryWrapper::getBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
																uint64_t srcOffset,
																void* dst,
																uint64_t sz)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::list<pair<const IDeviceMemoryManager::IBufferObject*, CVulkanBufferObject*>>::iterator it = m_pBuffers.begin();
	while (it != m_pBuffers.end())
	{
		if ((*it).first == pIBuffer)
			break;
		else
			it++;
	}
	if (m_pBuffers.end() == it)
		return false;
	return memory.vkGetBufferObjectData(device,*((*it).second),srcOffset,dst,sz);
}

bool CVulkanMemory::CVulkanMemoryWrapper::discardBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
																	uint64_t dstOffset,
																	uint64_t sz)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::list<pair<const IDeviceMemoryManager::IBufferObject*, CVulkanBufferObject*>>::iterator it = m_pBuffers.begin();
	while (it != m_pBuffers.end())
	{
		if ((*it).first == pIBuffer)
			break;
		else
			it++;
	}
	if (m_pBuffers.end() == it)
		return false;

	CVulkanBufferObject &vb = *((*it).second);

	bool erased = false;
	std::vector<VkBufferCopy>::iterator it2 = vb.m_unsynchronizedBuffers.begin();
	while (it2 != vb.m_unsynchronizedBuffers.end())
	{
		VkBufferCopy &outOfDate = *it2;
		erased = ((dstOffset == outOfDate.dstOffset) && (sz == outOfDate.size));
		if (erased)
		{
			vb.m_unsynchronizedBuffers.erase(it2);
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
	std::list<pair<const IDeviceMemoryManager::IBufferObject*, CVulkanBufferObject*>>::iterator it = m_pBuffers.begin();
	while (it != m_pBuffers.end())
	{
		if ((*it).first == pIBuffer)
			break;
		else
			it++;
	}
	if (m_pBuffers.end() == it)
		return false;

	const CVulkanBufferObject * const pBuffer = (*it).second;
	currentBuffers[pBuffer->getStorage()] = pBuffer;
	return true;
}

bool CVulkanMemory::CVulkanMemoryWrapper::unlockBufferObject(IDeviceMemoryManager::IBufferObject &bo)
{
	const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
	std::list<pair<const IDeviceMemoryManager::IBufferObject*, CVulkanBufferObject*>>::iterator it = m_pBuffers.begin();
	while (it != m_pBuffers.end())
	{
		if ((*it).first == pIBuffer)
			break;
		else
			it++;
	}
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
	std::list<pair<const IDeviceMemoryManager::IBufferObject*, CVulkanBufferObject*>>::iterator it = m_pBuffers.begin();
	while (it != m_pBuffers.end())
	{
		if ((*it).first == pIBuffer)
			break;
		else
			it++;
	}
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



VkImage CVulkanMemory::CVulkanMemoryWrapper::createImage(const VkImageCreateInfo &imageInfo,
														 IDeviceMemoryManager::IBufferObject &bo,
														 uint64_t srcOffset)
{
	if (VK_NULL_HANDLE != device)
	{
		const IDeviceMemoryManager::IBufferObject *pIBuffer = &bo;
		std::list<pair<const IDeviceMemoryManager::IBufferObject*, CVulkanBufferObject*>>::iterator it = m_pBuffers.begin();
		while (it != m_pBuffers.end())
		{
			if ((*it).first == pIBuffer)
				break;
			else
				it++;
		}
		if (m_pBuffers.end() == it)
			return VK_NULL_HANDLE;

		CVulkanBufferObject * pBuffer = (*it).second;
		if (VK_NULL_HANDLE == pBuffer->m_deviceAddress)
		{
			RAPTOR_ERROR(Global::CVulkanClassID::GetClassId(), "There is no allocated memory on given buffer object!");
			return VK_NULL_HANDLE;
		}


		VkImage image = VK_NULL_HANDLE;

		VkImageCreateInfo image_info = imageInfo;
		image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.queueFamilyIndexCount = 0;
		image_info.pQueueFamilyIndices = NULL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

		VkResult res = vkCreateImage(device, &image_info, &s_vulkanAllocator, &image);
		if (VK_SUCCESS != res)
		{
			CATCH_VK_ERROR(res);
			return VK_NULL_HANDLE;
		}
		else
		{
			VkMemoryRequirements image_memory_requirements;
			vkGetImageMemoryRequirements(device, image, &image_memory_requirements);

			VkDeviceSize offset = 0;
			if (!m_images.empty())
			{
				std::map<VkImage, data_bloc>::iterator it = m_images.end();
				it--;
				data_bloc &db = (*it).second;

				offset = db.memory_offset + db.size;
				offset = (offset / image_memory_requirements.alignment) * image_memory_requirements.alignment;
				offset = offset + image_memory_requirements.alignment;
			}

			//!
			//!	Allocation is done on a per image basis.
			//!
			res = vkBindImageMemory(device, image, pBuffer->m_deviceAddress, offset);
			if (VK_SUCCESS != res)
			{
				CATCH_VK_ERROR(res);
				RAPTOR_ERROR(Global::CVulkanClassID::GetClassId(),"Allocated memory cannot be mapped to image!");
				vkDestroyImage(device, image, &s_vulkanAllocator);
				return VK_NULL_HANDLE;
			}
			else
			{
				//	Register image mapping into device memory.
				data_bloc db;
				db.image = image;
				db.imageExtent = image_info.extent;
				db.memory_offset = offset;
				db.size = image_memory_requirements.size;
				m_images[srcOffset] = db;
			}
		}
		return image;
	}
	else
		return VK_NULL_HANDLE;
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
		CATCH_VK_ERROR(res);
		return NULL;
	}

	//!
	//!	Select buffer usage
	//!
	bool allocateBuffer = true;
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
			allocateBuffer = false;
			break;
		case IDeviceMemoryManager::IBufferObject::UNIFORM_BUFFER:
			usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			break;
		default:
			usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			break;
	};

	//!
	//!	Create device local buffer for requested usage
	//!
	VkBuffer device_buffer = VK_NULL_HANDLE;
	if (allocateBuffer)
	{
		buffer_create_info.usage = (VkBufferUsageFlagBits)(usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		res = vkCreateBuffer(device, &buffer_create_info, &s_vulkanAllocator, &device_buffer);
		if (VK_SUCCESS != res)
		{
			CATCH_VK_ERROR(res);
			vkDestroyBuffer(device, buffer, &s_vulkanAllocator);
			return NULL;
		}
	}

	//!
	//!	Allocate host visible memory & device local memory
	//!
	m_bNeedFlush = true;
	VkDeviceSize alignment = 0;
	VkDeviceSize device_alignment = 0;
	VkDeviceMemory pMemory = allocateBufferMemory(device, buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,alignment);
	VkDeviceMemory pDeviceMemory = VK_NULL_HANDLE;
	if (allocateBuffer)
		pDeviceMemory = allocateBufferMemory(device, device_buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_alignment);
	else
		pDeviceMemory = allocateImageMemory(device, size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_alignment);
	
	if ((VK_NULL_HANDLE != pMemory) && (VK_NULL_HANDLE != pDeviceMemory))
	{
		CVulkanBufferObject *pBuffer = new CVulkanBufferObject();
		pBuffer->m_buffer = buffer;
		pBuffer->m_address = pMemory;
		pBuffer->m_deviceBuffer = device_buffer;
		pBuffer->m_deviceAddress = pDeviceMemory;
		pBuffer->m_size = size + alignment;
		pBuffer->m_storage = kind;
		pBuffer->m_coherent = !m_bNeedFlush;
		pBuffer->m_granularity = alignment;
		return pBuffer;
	}
	else
	{
		if (VK_NULL_HANDLE == pMemory)
			vkDestroyBuffer(device, buffer, &s_vulkanAllocator);
		if (VK_NULL_HANDLE == pDeviceMemory)
			vkDestroyBuffer(device, device_buffer, &s_vulkanAllocator);

		return NULL;
	}
}

VkDeviceMemory CVulkanMemory::allocateBufferMemory(VkDevice device,
												   VkBuffer buffer,
												   VkMemoryPropertyFlagBits memory_type,
												   VkDeviceSize &alignment) const
{
	VkResult res = VK_NOT_READY;
	VkMemoryRequirements buffer_memory_requirements;
	vkGetBufferMemoryRequirements(device, buffer, &buffer_memory_requirements );
	alignment = buffer_memory_requirements.alignment;

	VkDeviceMemory pMemory = VK_NULL_HANDLE;
	for( uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i )
	{
		if ((buffer_memory_requirements.memoryTypeBits & (1 << i)) && 
			(memory_properties.memoryTypes[i].propertyFlags & memory_type) )
		{
			if (memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
				m_bNeedFlush = false;
			 VkMemoryAllocateInfo memory_allocate_info = {	VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,			// VkStructureType sType 
															NULL,											// const void *pNext 
															buffer_memory_requirements.size + alignment,	// VkDeviceSize allocationSize 
															i };											// uint32_t memoryTypeIndex 

			VkResult res = vkAllocateMemory(device, &memory_allocate_info, &s_vulkanAllocator, &pMemory );
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


VkDeviceMemory CVulkanMemory::allocateImageMemory(VkDevice device,
												  VkDeviceSize size,
												  VkMemoryPropertyFlagBits memory_type,
												  VkDeviceSize &alignment) const
{
	//!
	//!	Use a 2D RGBA Image for initial allocation
	//!	TODO: check memory_type is the same faot all image formats with the same usage (texture source for sampler)
	//!
	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
									NULL,					// const void*
									0,						// VkImageCreateFlags
									VK_IMAGE_TYPE_2D,		// VkImageType
									VK_FORMAT_R8G8B8A8_UNORM, // VkFormat
									{ 1, 1, 1 },			// VkExtent3D
									1,						//uint32_t mipLevels
									1,						// uint32_t arrayLayers
									VK_SAMPLE_COUNT_1_BIT,	// VkSampleCountFlagBits
									VK_IMAGE_TILING_OPTIMAL,	// VkImageTiling
									VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,		// VkImageUsageFlags
									VK_SHARING_MODE_EXCLUSIVE,	// VkSharingMode
									0,						// uint32_t  queueFamilyIndexCount
									NULL,					// const uint32_t* pQueueFamilyIndices
									VK_IMAGE_LAYOUT_PREINITIALIZED };	// VkImageLayout

	VkResult res = VK_NOT_READY;
	VkImage device_image = VK_NULL_HANDLE;
	res = vkCreateImage(device, &imageInfo, &s_vulkanAllocator, &device_image);
	if (VK_SUCCESS != res)
	{
		CATCH_VK_ERROR(res);
		return NULL;
	}
		
	VkMemoryRequirements image_memory_requirements;
	vkGetImageMemoryRequirements(device, device_image, &image_memory_requirements);
	alignment = image_memory_requirements.alignment;

	vkDestroyImage(device, device_image, &s_vulkanAllocator);

	VkDeviceMemory pMemory = VK_NULL_HANDLE;
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
	{
		if ((image_memory_requirements.memoryTypeBits & (1 << i)) &&
			(memory_properties.memoryTypes[i].propertyFlags & memory_type))
		{
			if (memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
				m_bNeedFlush = false;
			VkMemoryAllocateInfo memory_allocate_info = {	VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType sType 
															NULL,										// const void *pNext 
															size + alignment,							// VkDeviceSize allocationSize 
															i };										// uint32_t memoryTypeIndex 

			res = vkAllocateMemory(device, &memory_allocate_info, &s_vulkanAllocator, &pMemory);
			if (VK_SUCCESS == res)
				break;
			else
			{
				CATCH_VK_ERROR(res);
				pMemory = VK_NULL_HANDLE;
			}
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
