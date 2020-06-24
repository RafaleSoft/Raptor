/***************************************************************************/
/*                                                                         */
/*  GeometryAllocator.cpp                                                  */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif



RAPTOR_NAMESPACE_BEGIN

CGeometryAllocator	*CGeometryAllocator::m_pInstance = NULL;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeometryAllocator::CGeometryAllocator()
	:relocatedFaceIndexes(NULL),relocatedVertices(NULL)
{
	faceIndexes.address.us_address = NULL;
	faceIndexes.size = 0;

	vertices.address.f_address = NULL;
	vertices.size = 0;
}

CGeometryAllocator::~CGeometryAllocator()
{
	if (faceIndexes.address.us_address != NULL)
		CHostMemoryManager::GetInstance()->release(faceIndexes.address.us_address);
	if (vertices.address.f_address != NULL)
		CHostMemoryManager::GetInstance()->release(vertices.address.f_address);
	if (relocatedFaceIndexes != NULL)
		deviceMemoryManager->releaseBufferObject(relocatedFaceIndexes);
	if (relocatedVertices != NULL)
		deviceMemoryManager->releaseBufferObject(relocatedVertices);

	if (this == m_pInstance)
		m_pInstance = NULL;
}

CGeometryAllocator	*CGeometryAllocator::GetInstance(void)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CGeometryAllocator();
	}

	return m_pInstance;
}

CGeometryAllocator	*CGeometryAllocator::SetCurrentInstance(CGeometryAllocator* geometryAllocator)
{
	CGeometryAllocator *pInstance = m_pInstance;

    m_pInstance = geometryAllocator;

	return pInstance;
}

bool CGeometryAllocator::glvkCopyPointer(float *dst, float *src, uint64_t size)
{
	map<float*, uint64_t>::const_iterator dstbloc = vertexBlocs.find(dst);
	if (dstbloc == vertexBlocs.end())
		return false;
	map<float*, uint64_t>::const_iterator srcbloc = vertexBlocs.find(src);
	if (srcbloc == vertexBlocs.end())
		return false;

	//! Copy conventional memory.
	if (NULL == relocatedVertices)
	{
		if ((NULL == src) || (NULL == dst))
			return false;

		memcpy(dst, src, size);
		return true;
	}
	else
	{
		uint64_t dstOffset = (uint64_t)dst;
		uint64_t srcOffset = (uint64_t)src;

		uint64_t sz = size * sizeof(float);
		if ((*dstbloc).second < sz)	// destination must fit copy size.
			return false;
		if ((*srcbloc).second < sz) // source must provide copy size.
			return false;

		return CResourceAllocator::glvkCopyPointer(	relocatedVertices, dstOffset,
													relocatedVertices, srcOffset,
													sz);
	}
}


bool CGeometryAllocator::glvkCopyPointer(uint16_t *dst, uint16_t *src, uint64_t size)
{
	map<uint16_t*, uint64_t>::const_iterator dstbloc = indexBlocs.find(dst);
	if (dstbloc == indexBlocs.end())
		return false;
	map<uint16_t*, uint64_t>::const_iterator srcbloc = indexBlocs.find(src);
	if (srcbloc == indexBlocs.end())
		return false;

	//! Copy conventional memory.
	if (NULL == relocatedVertices)
	{
		if ((NULL == src) || (NULL == dst))
			return false;

		memcpy(dst, src, size);
		return true;
	}
	else
	{
		uint64_t dstOffset = (uint64_t)dst;
		uint64_t srcOffset = (uint64_t)src;

		uint64_t sz = size * sizeof(uint16_t);
		if ((*dstbloc).second < sz) // destination must fit copy size.
			return false;
		if ((*srcbloc).second < sz) // source must provide copy size.
			return false;

		return CResourceAllocator::glvkCopyPointer(	relocatedFaceIndexes, dstOffset,
													relocatedFaceIndexes, srcOffset,
													sz);
	}
}

void CGeometryAllocator::glvkSetPointerData(float *dst, float *src, uint64_t size)
{
    if ((NULL == deviceMemoryManager) || (NULL == relocatedVertices) || (NULL == src) || (NULL == dst))
        return;

	if (0 == size)
	{
		// find memory bloc and map a copy to local memory.
		map<float*,uint64_t>::const_iterator blocPos = vertexBlocs.find(dst);
		if (blocPos != vertexBlocs.end())
		{
			deviceMemoryManager->setBufferObjectData(	*relocatedVertices,
														(uint64_t)dst,
														src,
														(*blocPos).second);
		}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		else
		{
			Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
				                                            "The destination device buffer does not exist");
		}
#endif
	}
	else
		// No ckech is done to validate that dst is a bloc of size 'size'
		deviceMemoryManager->setBufferObjectData(	*relocatedVertices,
													(uint64_t)dst,
													src,
													sizeof(float)*size);
}

void CGeometryAllocator::glvkSetPointerData(uint16_t *dst, uint16_t *src, uint64_t size)
{
    if ((NULL == deviceMemoryManager) || (NULL == relocatedFaceIndexes) || (NULL == src) || (NULL == dst))
        return;

	if (0 == size)
	{
		// find memory bloc and map a copy to local memory.
		map<uint16_t*, uint64_t>::const_iterator blocPos = indexBlocs.find(dst);
		if (blocPos != indexBlocs.end())
		{
			deviceMemoryManager->setBufferObjectData(	*relocatedFaceIndexes,
														(uint64_t)dst,
														src,
														(*blocPos).second);
		}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		else
		{
			Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
				                                            "The destination device buffer does not exist");
		}
#endif
	}
	else
		// No ckech is done to validate that dst is a bloc of size 'size'
		deviceMemoryManager->setBufferObjectData(	*relocatedFaceIndexes,
													(uint64_t)dst,
													src,
													sizeof(uint16_t)*size);
}

unsigned short *CGeometryAllocator::glvkMapPointer(uint16_t *pointer, bool syncData)
{
    if ((NULL == relocatedFaceIndexes) || (m_bLocked) || (NULL == pointer))
        return pointer;

    // already mapped ?
    if (indexReMap.find(pointer) != indexReMap.end())
        return pointer;

    // find memory bloc and map a copy to local memory.
	map<uint16_t*, uint64_t>::const_iterator blocPos = indexBlocs.find(pointer);
	if (blocPos != indexBlocs.end())
    {
		uint16_t *localData = shortAlloc.allocate((*blocPos).second / sizeof(uint16_t));
        indexReMap[pointer] = localData;
        indexReMap[localData] = pointer;

		if (syncData)
		{
			deviceMemoryManager->getBufferObjectData(	*relocatedFaceIndexes,
														(uint64_t)pointer,
														localData,
														(*blocPos).second);
		}
        return localData;
    }
    else
        return NULL;
}

unsigned short *CGeometryAllocator::glvkUnMapPointer(uint16_t *pointer, bool syncData)
{
	if ((NULL == relocatedFaceIndexes) || (m_bLocked) || (NULL == pointer))
        return pointer;

    // pointer has been mapped ?
	map<uint16_t*, uint16_t*>::iterator it = indexReMap.find(pointer);
    if (it == indexReMap.end())
        //  I shouldn't return NULL as it can be buffer index 0.
        //  To keep a meaning to buffer offset 0, I could keep the fist data in buffer
        //  ( offset 0 ) to be invalid, and reserved at allocation, garantying thus that any
        //  other data bloc will have an offset > 0
        return pointer;


    // find memory bloc and copy local memory to server address space.
	uint16_t *serverData = (*it).second;

	map<uint16_t*, uint64_t>::const_iterator blocPos = indexBlocs.find(serverData);
	if (blocPos != indexBlocs.end())
    {
        indexReMap.erase(it);
		map<uint16_t*, uint16_t*>::iterator it2 = indexReMap.find(serverData);
        //  Should check for errors.
        indexReMap.erase(it2);
        
		// Here, serverData could be relocated to compress the data
		// and limit the number of holes or fragmentation.
		// As we have an array of free blocs, relocation could easily be done.
		if (syncData)
		{
			deviceMemoryManager->setBufferObjectData(	*relocatedFaceIndexes,
														(uint64_t)serverData,
														pointer,
														(*blocPos).second);
		}

        CHostMemoryManager::GetInstance()->garbage(pointer);
        return serverData;
    }
    else
        return NULL;
}

float *CGeometryAllocator::glvkMapPointer(float *pointer, bool syncData)
{
	if ((NULL == relocatedVertices) || (m_bLocked) || (NULL == pointer))
        return pointer;

    // already mapped ?
    if (vertexReMap.find(pointer) != vertexReMap.end())
        return pointer;

    // find memory bloc and map a copy to local memory.
    map<float*,uint64_t>::const_iterator blocPos = vertexBlocs.find(pointer);
	if (blocPos != vertexBlocs.end())
    {
        float *localData = floatAlloc.allocate((*blocPos).second/sizeof(float));
        vertexReMap[pointer] = localData;
        vertexReMap[localData] = pointer;

		if (syncData)
		{
			deviceMemoryManager->getBufferObjectData(	*relocatedVertices,
														(uint64_t)pointer,
														localData,
														(*blocPos).second);
		}
        return localData;
    }
    else
        return NULL;
}

float *CGeometryAllocator::glvkUnMapPointer(float *pointer, bool syncData)
{
	if ((NULL == relocatedVertices) || (m_bLocked) || (NULL == pointer))
        return pointer;

    // pointer has been mapped ?
    map<float*,float*>::iterator it = vertexReMap.find(pointer);
    if (it == vertexReMap.end())
        // I shouldn't return NULL as it can be buffer index 0.
        //  To keep a meaning to buffer offset 0, I could keep the fist data in buffer
        //  ( offset 0 ) to be invalid, and reserved at allocation, garantying thus that any
        //  other data bloc will have an offset > 0
        return pointer;


    // find memory bloc and copy local memory to server address space.
    float *serverData = (*it).second;

    map<float*,uint64_t>::const_iterator blocPos = vertexBlocs.find(serverData);
	if (blocPos != vertexBlocs.end())
    {
        vertexReMap.erase(it);
        map<float*,float*>::iterator it2 = vertexReMap.find(serverData);
        //  Should check for errors.
        vertexReMap.erase(it2);
        
		if (syncData)
		{
			// Here, serverData could be relocated to compress the data
			// and limit the number of holes or fragmentation.
			// As we have an array of free blocs, relocation could easily be done.
			deviceMemoryManager->setBufferObjectData(	*relocatedVertices,
														(uint64_t)serverData,
														pointer,
														(*blocPos).second);
		}

        CHostMemoryManager::GetInstance()->garbage(pointer);
        return serverData;
    }
    else
        return NULL;
}

bool CGeometryAllocator::glvkInitMemory(IDeviceMemoryManager* pDeviceMemory,
										uint64_t indexSize,uint64_t coordsSize)
{
    if (m_bLocked)
        return false;

	// Should there be several blocs ? No to be able to switch to relocatable blocs in AGP
	// ( it is much faster to have only one bufferobject bound for RT rendering )
	if ((indexSize == 0) || (!indexBlocs.empty()))
		return false;
	if ((coordsSize == 0) || (!vertexBlocs.empty()))
		return false;

	deviceMemoryManager = pDeviceMemory;

    //  Allow user to allocate bloc size different from default.
	if (faceIndexes.address.us_address != NULL)
    {
		CHostMemoryManager::GetInstance()->release(faceIndexes.address.us_address);
        faceIndexes.address.us_address = NULL;
    }
	if (vertices.address.f_address != NULL)
    {
		CHostMemoryManager::GetInstance()->release(vertices.address.f_address);
        vertices.address.f_address = NULL;
    }
    if (relocatedVertices != NULL)
        deviceMemoryManager->releaseBufferObject(relocatedVertices);
    if (relocatedFaceIndexes != NULL)
        deviceMemoryManager->releaseBufferObject(relocatedFaceIndexes);

	if ((NULL != deviceMemoryManager) && (deviceMemoryManager->relocationAvailable()))
	{
		vertices.size = coordsSize * sizeof(float);
		relocatedVertices = deviceMemoryManager->createBufferObject(IDeviceMemoryManager::IBufferObject::VERTEX_BUFFER,
																	IDeviceMemoryManager::IBufferObject::STATIC,
																	vertices.size);
		
		faceIndexes.size = indexSize  * sizeof(unsigned short);
		relocatedFaceIndexes = deviceMemoryManager->createBufferObject(	IDeviceMemoryManager::IBufferObject::INDEX_BUFFER,
																		IDeviceMemoryManager::IBufferObject::STATIC,
																		faceIndexes.size);

		return ((relocatedFaceIndexes != NULL) && (relocatedVertices != NULL));
	}
	else
	{
		faceIndexes.address.us_address = shortAlloc.allocate(indexSize);
		faceIndexes.size = indexSize * sizeof(unsigned short);
        vertices.address.f_address = floatAlloc.allocate(coordsSize);
		vertices.size = coordsSize * sizeof(float);
		return ((faceIndexes.address.us_address != NULL) && (vertices.address.f_address != NULL));
	}
}

unsigned short	* const	CGeometryAllocator::allocateIndexes(uint64_t size)
{
	if ((size == 0) || (m_bLocked) || ((faceIndexes.address.us_address == NULL) && (relocatedFaceIndexes == NULL)))
		return NULL;

	// be it relocated or not, faceIndexes can be the beginning of the memory block
	uint16_t *currentAddress = faceIndexes.address.us_address;
	uint64_t sz = size * sizeof(uint16_t);
    
	if (!indexBlocs.empty())
	{
		uint64_t maxSize = faceIndexes.size;
		if (!freeIndexBlocs.empty())
		{
			bool reuse = false;
			unsigned int blocPos = 0;
			for (size_t i=0;i<freeIndexBlocs.size();i++)
			{
				data_bloc2 &db = freeIndexBlocs[i];
				if ((db.size >= sz) && (db.size < maxSize))
				{
					blocPos = i;
					maxSize = db.size;
					reuse = true;
				}
			}
			if (reuse)
			{
				unsigned short* addr = freeIndexBlocs[blocPos].address.us_address;
				freeIndexBlocs.erase(freeIndexBlocs.begin() + blocPos);
				return addr;
			}
		}

		map<uint16_t*,uint64_t>::iterator it = indexBlocs.end();
		it--;
		currentAddress = (*it).first + (*it).second/sizeof(uint16_t);
	}

	if ( ((uint64_t)currentAddress - (uint64_t)faceIndexes.address.us_address) + sz > faceIndexes.size)
    {
        RAPTOR_FATAL(	COpenGL::COpenGLClassID::GetClassId(),
						"Geometry Allocator could not get enough memory");
		return NULL;
    }

    //  No NULL offset to distinguish nil pointers
    if ((NULL != relocatedFaceIndexes) && (NULL == currentAddress))
		currentAddress = (uint16_t*)relocatedFaceIndexes->getRelocationOffset();

	//	Address should be aligned on a 16byte boundary
	uint16_t* address = (uint16_t*)(((uint64_t)(currentAddress)+0x0f) & ~0x0f);
	indexBlocs[address] = sz;

	return address;
}


bool	CGeometryAllocator::releaseIndexes(uint16_t *index)
{
    if (m_bLocked)
        return false;

	bool res = false;

	map<unsigned short*,uint64_t>::iterator blocPos = indexBlocs.find(index);
	if (blocPos != indexBlocs.end())
	{
		uint64_t sz = (*blocPos).second;
		if (NULL != relocatedFaceIndexes)
			deviceMemoryManager->discardBufferObjectData(*relocatedFaceIndexes,(uint64_t)index,sz);

		map<unsigned short*,uint64_t>::iterator last = indexBlocs.end();
		last--;
		if ((*blocPos).first == (*last).first)
		{
			indexBlocs.erase(blocPos);
		}
		else
		{
			res = true;
			data_bloc2 db;
			db.address.us_address = index;
			db.size = (*blocPos).second;
			freeIndexBlocs.push_back(db);
		}
	}

	return res;
}


float * const CGeometryAllocator::allocateVertices(uint64_t size)
{
	if ((0 == size)  || (m_bLocked) || ((NULL == vertices.address.f_address) && (NULL == relocatedVertices)))
		return NULL;

	// be it relocated or not, vertices can be the beginning of the memory block
	float *currentAddress = vertices.address.f_address;
	uint64_t sz = size * sizeof(float);
    
	if (!vertexBlocs.empty())
	{
		uint64_t maxSize = vertices.size;
		if (!freeVertexBlocs.empty())
		{
			bool reuse = false;
			unsigned int blocPos = 0;
			for (size_t i=0;i<freeVertexBlocs.size();i++)
			{
				data_bloc2 &db = freeVertexBlocs[i];
				if ((db.size >= sz) && (db.size < maxSize))
				{
					blocPos = i;
					maxSize = db.size;
					reuse = true;
				}
			}
			if (reuse)
			{
				float* addr = freeVertexBlocs[blocPos].address.f_address;
				freeVertexBlocs.erase(freeVertexBlocs.begin() + blocPos);
				return addr;
			}
		}

		map<float*,uint64_t>::iterator it = vertexBlocs.end();
		it--;
		currentAddress = (*it).first + (*it).second/sizeof(float);
	}


	if ( ((uint64_t)currentAddress - (uint64_t)vertices.address.f_address) + sz > vertices.size)
    {
        RAPTOR_FATAL(	COpenGL::COpenGLClassID::GetClassId(),
						"Geometry Allocator could not get enough memory");
		return NULL;
    }

    //  No NULL offset to distinguish nil pointers
    if ((NULL != relocatedVertices) && (NULL == currentAddress))
		currentAddress = (float*)relocatedVertices->getRelocationOffset();

	//	Address should be aligned on a 16byte boundary
	float* address = (float*)(((uint64_t)(currentAddress)+0x0f) & ~0x0f);
	vertexBlocs[address] = sz;

	return address;
}

bool CGeometryAllocator::releaseVertices(float *index)
{
    if ((m_bLocked) || (NULL == index))
        return false;

	bool res = false;

	map<float*,uint64_t>::iterator blocPos = vertexBlocs.find(index);
	if (blocPos != vertexBlocs.end())
	{
		uint64_t sz = (*blocPos).second;
		if (NULL != relocatedVertices)
			deviceMemoryManager->discardBufferObjectData(*relocatedVertices,(uint64_t)index,sz);

		map<float*,uint64_t>::iterator last = vertexBlocs.end();
		last--;
		if ((*blocPos).first == (*last).first)
		{
			vertexBlocs.erase(blocPos);
		}
		else
		{
			res = true;
			data_bloc2 db;
			db.address.f_address = index;
			db.size = sz;
			freeVertexBlocs.push_back(db);
		}
	}

	return res;
}

// Is the boolean 'm_bLocked' really necessary ?
//	a 're-lock' can be harmless and could allow reentry...
bool CGeometryAllocator::glvkLockMemory(bool lock)
{
    bool res = true;

    if ((NULL != deviceMemoryManager) && (relocatedVertices != NULL) && (relocatedFaceIndexes != NULL))
    {
        if (lock && !m_bLocked)
        {
            res = deviceMemoryManager->lockBufferObject(*relocatedVertices);
            res = res && deviceMemoryManager->lockBufferObject(*relocatedFaceIndexes);
        }
        else if (!lock && m_bLocked)
        {
            res = deviceMemoryManager->unlockBufferObject(*relocatedVertices);
            res = res && deviceMemoryManager->unlockBufferObject(*relocatedFaceIndexes);
        }
        else
            res = false;
    }
 
    m_bLocked = lock;
    return res;
}

