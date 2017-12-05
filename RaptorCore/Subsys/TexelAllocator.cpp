// TexelAllocator.cpp: implementation of the CTexelAllocator class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
	#include "Subsys/Vulkan/VulkanDevice.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

RAPTOR_NAMESPACE_BEGIN

CTexelAllocator	*CTexelAllocator::m_pInstance = NULL;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexelAllocator::CTexelAllocator()
	:relocatedTexels(NULL)
{
	texels.address = NULL;
	texels.size = 0;
}


CTexelAllocator::~CTexelAllocator()
{
	if (texels.address != NULL)
		CHostMemoryManager::GetInstance()->release(texels.address);
	if (relocatedTexels != NULL)
		deviceMemoryManager->releaseBufferObject(relocatedTexels);
}

CTexelAllocator* CTexelAllocator::GetInstance(void)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CTexelAllocator();
	}

	return m_pInstance;
}


CTexelAllocator	*CTexelAllocator::SetCurrentInstance(CTexelAllocator* texelAllocator)
{
	CTexelAllocator *pInstance = m_pInstance;

    m_pInstance = texelAllocator;

	return pInstance;
}


bool CTexelAllocator::glvkInitMemory(	IDeviceMemoryManager* pDeviceMemory,
										uint64_t texelSize)
{
    if (m_bLocked)
        return false;

	// Should there be several blocs ? No to be able to switch to relocatable blocs in AGP
	// ( it is much faster to have only one bufferobject bound for RT rendering )
	if ((texelSize == 0) || (!texelBlocs.empty()))
		return false;

	deviceMemoryManager = pDeviceMemory;

    //  Allow user to allocate bloc size different from default.
    if (texels.address != NULL)
    {
		CHostMemoryManager::GetInstance()->release(texels.address);
        texels.address = NULL;
    }
    if (relocatedTexels != NULL)
        deviceMemoryManager->releaseBufferObject(relocatedTexels);

	if ((NULL != deviceMemoryManager) && (deviceMemoryManager->relocationAvailable()))
	{
		texels.size = texelSize;
		relocatedTexels = deviceMemoryManager->createBufferObject(	IDeviceMemoryManager::IBufferObject::PIXEL_SOURCE,
																	IDeviceMemoryManager::IBufferObject::STREAM,
																	texelSize);
        CATCH_GL_ERROR

		return (relocatedTexels != NULL);
	}
	else
	{
        texels.address = charAlloc.allocate(texelSize);
		texels.size = texelSize;
		return (texels.address != NULL);
	}
}


bool CTexelAllocator::glvkLockMemory(bool lock)
{
    bool res = true;

    if ((NULL != deviceMemoryManager) && (relocatedTexels != NULL))
    {
        if (lock && !m_bLocked)
        {
            deviceMemoryManager->lockBufferObject(*relocatedTexels);
        }
        else if (!lock && m_bLocked)
        {
            deviceMemoryManager->unlockBufferObject(*relocatedTexels);
        }
        else
            res = false;
    }
 
    m_bLocked = lock;
    return res;
}

VkImage CTexelAllocator::vkAllocateTextureImage(uint64_t width,
												uint64_t height,
												uint64_t depth,
												CTextureObject::TEXEL_TYPE format,
												unsigned char* texels)
{
	VkImage image = VK_NULL_HANDLE;
	if ((width == 0) || (height == 0) || (depth == 0))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "CTextureObject wrong size update");
#endif
		return image;
	}

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = NULL;
	imageInfo.flags = 0;

	uint64_t size = width * height * depth;
	//!
	//! Select Image format
	//!
	switch (format)
	{
		case CTextureObject::CGL_COLOR24_ALPHA:
			imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			size = size * 4;
			break;
		case CTextureObject::CGL_COLOR24:
			imageInfo.format = VK_FORMAT_R8G8B8_UNORM;
			size = size * 3;
			break;
		default:
			imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			size = size * 4;
			break;
	}

	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = depth;

	//!
	//! Select Image type
	//!
	if (2 > depth)
	{
		if (2 > height)
			imageInfo.imageType = VK_IMAGE_TYPE_1D;
		else
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
	}
	else
		imageInfo.imageType = VK_IMAGE_TYPE_3D;

	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

	CVulkanDevice *pDevice = CVulkanDevice::getCurrentDevice();
	if (NULL != pDevice)
	{
		unsigned char *texpointer = this->allocateTexels(size);
		VkDeviceSize offset = (VkDeviceSize)texpointer;

		CVulkanMemory::CVulkanMemoryWrapper* memory = pDevice->getMemory();
		image = memory->createImage(imageInfo, *relocatedTexels, offset);

		if ((VK_NULL_HANDLE != image) && (NULL != texels))
			glvkCopyPointer(texpointer, texels, size);
	}

	return image;
}

unsigned char*	const CTexelAllocator::allocateTexels(uint64_t size)
{
	if ((0 == size)  || (m_bLocked) || ((NULL == texels.address) && (NULL == relocatedTexels)))
		return NULL;

	// be it relocated or not, texels can be the beginning or the memory block
	unsigned char *currentAddress = texels.address;

	if (!texelBlocs.empty())
	{
		uint64_t maxSize = texels.size;
		if (!freeTexelBlocs.empty())
		{
			bool reuse = false;
			unsigned int blocPos = 0;
			for (size_t i=0;i<freeTexelBlocs.size();i++)
			{
				data_bloc &db = freeTexelBlocs[i];
				if ((db.size >= size) && (db.size < maxSize))
				{
					blocPos = i;
					maxSize = size;
					reuse = true;
				}
			}
			if (reuse)
			{
				unsigned char* addr = freeTexelBlocs[blocPos].address;
				freeTexelBlocs.erase(freeTexelBlocs.begin() + blocPos);
				return addr;
			}
		}

		map<void*,data_bloc>::iterator it = texelBlocs.end();
		it--;
		currentAddress = (*it).second.address + (*it).second.size;
	}

	if ( ((uint64_t)currentAddress - (uint64_t)texels.address) + size > texels.size)
    {
		stringstream err;
		err << "Texel Allocator could not get enough memory:";
		err << " missing ";
		err << (((uint64_t)currentAddress - (uint64_t)texels.address) + size) - texels.size;
		err << " bytes.";
		Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_FATAL,
														err.str().c_str());
		return NULL;
    }

    //  No NULL offset to distinguish nil pointers
    if ((NULL != relocatedTexels) && (NULL == currentAddress))
		currentAddress = (unsigned char*)relocatedTexels->getRelocationOffset();

	//	Address should be aligned on a 16byte boundary
	data_bloc db;
	db.address = (unsigned char*)(((unsigned int)(currentAddress) + 0x0f) & 0xfffffff0);
	db.size = size;
	texelBlocs[db.address] = db;

	return db.address;
}

float*	const CTexelAllocator::allocateFloatTexels(uint64_t size)
{
	float *fTexels = (float*)allocateTexels(size*sizeof(float));
	return fTexels;
}

bool CTexelAllocator::releaseTexels(void *tex)
{
    if (m_bLocked)
        return false;

	bool res = false;

	map<void*,data_bloc>::iterator blocPos = texelBlocs.find(tex);
	if (blocPos != texelBlocs.end())
	{
		map<void*,data_bloc>::iterator last = texelBlocs.end();
		last--;
		if ((*blocPos).first == (*last).first)
		{
			texelBlocs.erase(blocPos);
		}
		else
		{
			res = true;
			data_bloc db;
			db.address = (unsigned char*)tex;
			db.size = (*blocPos).second.size;
			freeTexelBlocs.push_back(db);
		}
	}

	return res;
}


void CTexelAllocator::glvkCopyPointer(unsigned char *dst, unsigned char *src, uint64_t size)
{
	if ((NULL == deviceMemoryManager) || (NULL == relocatedTexels) || (NULL == src) || (NULL == dst))
        return;

	if (0 == size)
	{
		// find memory bloc and map a copy to local memory.
		map<void*,data_bloc>::const_iterator blocPos = texelBlocs.find(dst);
		if (blocPos != texelBlocs.end())
		{
			deviceMemoryManager->setBufferObjectData(	*relocatedTexels,
														(uint64_t)dst,
														src,
														(*blocPos).second.size);
		}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		else
		{
			Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
				                                            "The destination device buffer does not exist");
		}
#endif
	}
	else
		// No ckech is done to validate that dst is a bloc of size 'size'
		deviceMemoryManager->setBufferObjectData(	*relocatedTexels,
													(uint64_t)dst,
													src,
													size);

	CATCH_GL_ERROR
}



void *CTexelAllocator::glvkMapPointer(void *pointer,bool syncData)
{
    if ((NULL == relocatedTexels) || (m_bLocked) || (NULL == pointer))
        return pointer;

    // already mapped ?
    if (texelReMap.find(pointer) != texelReMap.end())
        return pointer;

    // find memory bloc and map a copy to local memory.
	map<void*,data_bloc>::const_iterator blocPos = texelBlocs.find(pointer);
	if (blocPos != texelBlocs.end())
    {
		uint64_t sz = (*blocPos).second.size;
		unsigned char* localData = charAlloc.allocate(sz);

        texelReMap[pointer] = localData;
        texelReMap[localData] = pointer;

		if (syncData)
		{
			deviceMemoryManager->getBufferObjectData(	*relocatedTexels,
														(uint64_t)pointer,
														localData,
														sz);
		}

        CATCH_GL_ERROR

        return localData;
    }
    else
        return NULL;
}

void *CTexelAllocator::glvkUnMapPointer(void *pointer,bool syncData)
{
    if ((NULL == relocatedTexels) || (m_bLocked) || (NULL == pointer))
        return pointer;

    // pointer has been mapped ?
    map<void*,void*>::iterator it = texelReMap.find(pointer);
    if (it == texelReMap.end())
        // I shouldn't return NULL as it can be buffer index 0.
        //  To keep a meaning to buffer offset 0, I could keep the fist data in buffer
        //  ( offset 0 ) to be invalid, and reserved at allocation, garantying thus that any
        //  other data bloc will have an offset > 0
        return pointer;


    // find memory bloc and copy local memory to server address space.
    void *serverData = (*it).second;

	map<void*,data_bloc>::const_iterator blocPos = texelBlocs.find(serverData);
	if (blocPos != texelBlocs.end())
    {
        texelReMap.erase(it);
        map<void*,void*>::iterator it2 = texelReMap.find(serverData);
        //  Should check for errors.
        texelReMap.erase(it2);

		if (syncData)
		{
			// Here, serverData could be relocated to compress the data
			// and limit the number of holes or fragmentation.
			// As we have an array of free blocs, relocation could easily be done.
			deviceMemoryManager->setBufferObjectData(	*relocatedTexels,
														(uint64_t)serverData,
														pointer,
														(*blocPos).second.size);

		}

        CHostMemoryManager::GetInstance()->garbage(pointer);

        CATCH_GL_ERROR

        return serverData;
    }
    else
        return NULL;
}

