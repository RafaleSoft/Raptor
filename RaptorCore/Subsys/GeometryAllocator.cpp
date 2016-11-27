// GeometryAllocator.cpp: implementation of the CGeometryAllocator class.
//
//////////////////////////////////////////////////////////////////////

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
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif


RAPTOR_NAMESPACE_BEGIN

CGeometryAllocator	*CGeometryAllocator::m_pInstance = NULL;

//  Add a constant offset to distinguish null ( unalocated ) pointers from actual memory blocs
static const int    RELOCATE_OFFSET = 16;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeometryAllocator::CGeometryAllocator():
	m_bLocked(false),
	relocatedFaceIndexes(NULL),relocatedVertices(NULL),
	deviceMemoryManager(NULL)
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

void CGeometryAllocator::glvkCopyPointer(float *dst, float *src, uint64_t size)
{
    if ((NULL == deviceMemoryManager) || (NULL == relocatedVertices) || (NULL == src) || (NULL == dst))
        return;

	if (0 == size)
	{
		// find memory bloc and map a copy to local memory.
		map<float*,unsigned int>::const_iterator blocPos = vertexBlocs.find(dst);
		if (blocPos != vertexBlocs.end())
		{
			deviceMemoryManager->setBufferObjectData(	*relocatedVertices,
														(unsigned int)dst,
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
													(unsigned int)dst,
													src,
													sizeof(float)*size);
}

void CGeometryAllocator::glvkCopyPointer(unsigned short *dst, unsigned short *src, uint64_t size)
{
    if ((NULL == deviceMemoryManager) || (NULL == relocatedFaceIndexes) || (NULL == src) || (NULL == dst))
        return;

	if (0 == size)
	{
		// find memory bloc and map a copy to local memory.
		map<unsigned short*,unsigned int>::const_iterator blocPos = indexBlocs.find(dst);
		if (blocPos != indexBlocs.end())
		{
			deviceMemoryManager->setBufferObjectData(	*relocatedFaceIndexes,
														(unsigned int)dst,
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
													(unsigned int)dst,
													src,
													sizeof(unsigned short)*size);
}

unsigned short *CGeometryAllocator::glvkMapPointer(unsigned short *pointer)
{
    if ((NULL == relocatedFaceIndexes) || (m_bLocked) || (NULL == pointer))
        return pointer;

    // already mapped ?
    if (indexReMap.find(pointer) != indexReMap.end())
        return pointer;

    // find memory bloc and map a copy to local memory.
	map<unsigned short*,unsigned int>::const_iterator blocPos = indexBlocs.find(pointer);
	if (blocPos != indexBlocs.end())
    {
		unsigned short *localData = shortAlloc.allocate((*blocPos).second/sizeof(unsigned short));
        indexReMap[pointer] = localData;
        indexReMap[localData] = pointer;

        deviceMemoryManager->getBufferObjectData(	*relocatedFaceIndexes,
													(unsigned int)pointer,
													localData,
													(*blocPos).second);
        return localData;
    }
    else
        return NULL;
}

unsigned short *CGeometryAllocator::glvkUnMapPointer(unsigned short *pointer)
{
	if ((NULL == relocatedFaceIndexes) || (m_bLocked) || (NULL == pointer))
        return pointer;

    // pointer has been mapped ?
    map<unsigned short*,unsigned short*>::iterator it = indexReMap.find(pointer);
    if (it == indexReMap.end())
        //  I shouldn't return NULL as it can be buffer index 0.
        //  To keep a meaning to buffer offset 0, I could keep the fist data in buffer
        //  ( offset 0 ) to be invalid, and reserved at allocation, garantying thus that any
        //  other data bloc will have an offset > 0
        return pointer;


    // find memory bloc and copy local memory to server address space.
    unsigned short *serverData = (*it).second;

	map<unsigned short*,unsigned int>::const_iterator blocPos = indexBlocs.find(serverData);
	if (blocPos != indexBlocs.end())
    {
        indexReMap.erase(it);
        map<unsigned short*,unsigned short*>::iterator it2 = indexReMap.find(serverData);
        //  Should check for errors.
        indexReMap.erase(it2);
        
		// Here, serverData could be relocated to compress the data
		// and limit the number of holes or fragmentation.
		// As we have an array of free blocs, relocation could easily be done.
        deviceMemoryManager->setBufferObjectData(	*relocatedFaceIndexes,
													(unsigned int)serverData,
													pointer,
													(*blocPos).second);

        CHostMemoryManager::GetInstance()->garbage(pointer);

        return serverData;
    }
    else
        return NULL;
}

unsigned short *CGeometryAllocator::glDiscardPointer(unsigned short *pointer)
{
	if ((relocatedFaceIndexes == NULL) || (m_bLocked) || (NULL == pointer))
        return pointer;

    // pointer has been mapped ?
    map<unsigned short*,unsigned short*>::iterator it = indexReMap.find(pointer);
    if (it == indexReMap.end())
        //  I shouldn't return NULL as it can be buffer index 0.
        //  To keep a meaning to buffer offset 0, I could keep the fist data in buffer
        //  ( offset 0 ) to be invalid, and reserved at allocation, garantying thus that any
        //  other data bloc will have an offset > 0
        return pointer;

    // find mapped memory bloc.
    unsigned short *serverData = (*it).second;

	map<unsigned short*,unsigned int>::const_iterator blocPos = indexBlocs.find(serverData);
	if (blocPos != indexBlocs.end())
    {
        indexReMap.erase(it);
        map<unsigned short*,unsigned short*>::iterator it2 = indexReMap.find(serverData);
        //  Should check for errors.
        indexReMap.erase(it2);
        
        CHostMemoryManager::GetInstance()->garbage(pointer);

        return serverData;
    }
    else
        return NULL;
}

float *CGeometryAllocator::glvkMapPointer(float *pointer)
{
	if ((NULL == relocatedVertices) || (m_bLocked) || (NULL == pointer))
        return pointer;

    // already mapped ?
    if (vertexReMap.find(pointer) != vertexReMap.end())
        return pointer;

    // find memory bloc and map a copy to local memory.
    map<float*,unsigned int>::const_iterator blocPos = vertexBlocs.find(pointer);
	if (blocPos != vertexBlocs.end())
    {
        float *localData = floatAlloc.allocate((*blocPos).second/sizeof(float));
        vertexReMap[pointer] = localData;
        vertexReMap[localData] = pointer;

        deviceMemoryManager->getBufferObjectData(	*relocatedVertices,
													(unsigned int)pointer,
													localData,
													(*blocPos).second);
        return localData;
    }
    else
        return NULL;
}

float *CGeometryAllocator::glvkUnMapPointer(float *pointer)
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

    map<float*,unsigned int>::const_iterator blocPos = vertexBlocs.find(serverData);
	if (blocPos != vertexBlocs.end())
    {
        vertexReMap.erase(it);
        map<float*,float*>::iterator it2 = vertexReMap.find(serverData);
        //  Should check for errors.
        vertexReMap.erase(it2);
        
		// Here, serverData could be relocated to compress the data
		// and limit the number of holes or fragmentation.
		// As we have an array of free blocs, relocation could easily be done.
        deviceMemoryManager->setBufferObjectData(	*relocatedVertices,
													(unsigned int)serverData,
													pointer,
													(*blocPos).second);
        CHostMemoryManager::GetInstance()->garbage(pointer);

        return serverData;
    }
    else
        return NULL;
}

float *CGeometryAllocator::glDiscardPointer(float *pointer)
{
	if ((relocatedVertices == NULL) || (m_bLocked) || (NULL == pointer))
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

    map<float*,unsigned int>::const_iterator blocPos = vertexBlocs.find(serverData);
	if (blocPos != vertexBlocs.end())
    {
        vertexReMap.erase(it);
        map<float*,float*>::iterator it2 = vertexReMap.find(serverData);
        //  Should check for errors.
        vertexReMap.erase(it2);
        
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
																	vertices.size+RELOCATE_OFFSET);
		
		faceIndexes.size = indexSize  * sizeof(unsigned short);
		relocatedFaceIndexes = deviceMemoryManager->createBufferObject(	IDeviceMemoryManager::IBufferObject::INDEX_BUFFER,
																		IDeviceMemoryManager::IBufferObject::STATIC,
																		faceIndexes.size+RELOCATE_OFFSET);

		return ((relocatedFaceIndexes != NULL) && (relocatedVertices != NULL));
	}
	else
	{
		faceIndexes.address.us_address = shortAlloc.allocate(indexSize+RELOCATE_OFFSET);
		faceIndexes.size = indexSize * sizeof(unsigned short);
        vertices.address.f_address = floatAlloc.allocate(coordsSize+RELOCATE_OFFSET);
		vertices.size = coordsSize * sizeof(float);
		return ((faceIndexes.address.us_address != NULL) && (vertices.address.f_address != NULL));
	}
}

unsigned short	* const	CGeometryAllocator::allocateIndexes(unsigned short size)
{
	if ((size == 0) || (m_bLocked) || ((faceIndexes.address.us_address == NULL) && (relocatedFaceIndexes == NULL)))
		return NULL;

	// be it relocated or not, faceIndexes can be the beginning of the memory block
	unsigned short *currentAddress = faceIndexes.address.us_address;
	unsigned int sz = size * sizeof(unsigned short);
    
	if (!indexBlocs.empty())
	{
		unsigned int maxSize = faceIndexes.size;
		if (!freeIndexBlocs.empty())
		{
			bool reuse = false;
			unsigned int blocPos = 0;
			for (size_t i=0;i<freeIndexBlocs.size();i++)
			{
				data_bloc &db = freeIndexBlocs[i];
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

		map<unsigned short*,unsigned int>::iterator it = indexBlocs.end();
		it--;
		currentAddress = (*it).first + (*it).second/sizeof(unsigned short);
	}

	if ( ((unsigned int)currentAddress - (unsigned int)faceIndexes.address.us_address) + sz > faceIndexes.size)
    {
        Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_FATAL,
														"Geometry Allocator could not get enough memory");
		return NULL;
    }

    //  No NULL offset to distinguish nil pointers
    if ((NULL !=relocatedFaceIndexes) && (currentAddress == NULL))
        currentAddress = (unsigned short*)RELOCATE_OFFSET;

	//	Address should be aligned on a 16byte boundary
	unsigned short* address = (unsigned short*)(((unsigned int)(currentAddress) + 0x0f) & 0xfffffff0);
	indexBlocs[address] = sz;

	return address;
}


bool	CGeometryAllocator::releaseIndexes(unsigned short *index)
{
    if  (m_bLocked)
        return false;

	bool res = false;

	map<unsigned short*,unsigned int>::iterator blocPos = indexBlocs.find(index);
	if (blocPos != indexBlocs.end())
	{
		map<unsigned short*,unsigned int>::iterator last = indexBlocs.end();
		last--;
		if ((*blocPos).first == (*last).first)
		{
			indexBlocs.erase(blocPos);
		}
		else
		{
			res = true;
			data_bloc db;
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

	// be it relocated or not, vertices can be the beginning or the memory block
	float *currentAddress = vertices.address.f_address;
	unsigned int sz = size * sizeof(float);
    
	if (!vertexBlocs.empty())
	{
		unsigned int maxSize = vertices.size;
		if (!freeVertexBlocs.empty())
		{
			bool reuse = false;
			unsigned int blocPos = 0;
			for (size_t i=0;i<freeVertexBlocs.size();i++)
			{
				data_bloc &db = freeVertexBlocs[i];
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

		map<float*,unsigned int>::iterator it = vertexBlocs.end();
		it--;
		currentAddress = (*it).first + (*it).second/sizeof(float);
	}


	if ( ((unsigned int)currentAddress - (unsigned int)vertices.address.f_address) + sz > vertices.size)
    {
        Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_FATAL,
														"Geometry Allocator could not get enough memory");
		return NULL;
    }

    //  No NULL offset to distinguish nil pointers
    if ((NULL != relocatedVertices) && (currentAddress == NULL))
        currentAddress = (float*)RELOCATE_OFFSET;

	//	Address should be aligned on a 16byte boundary
	float* address = (float*)(((unsigned int)(currentAddress) + 0x0f) & 0xfffffff0);
	vertexBlocs[address] = sz;

	return address;
}


bool CGeometryAllocator::releaseVertices(float *index)
{
    if (m_bLocked)
        return false;

	bool res = false;

	map<float*,unsigned int>::iterator blocPos = vertexBlocs.find(index);
	if (blocPos != vertexBlocs.end())
	{
		map<float*,unsigned int>::iterator last = vertexBlocs.end();
		last--;
		if ((*blocPos).first == (*last).first)
		{
			vertexBlocs.erase(blocPos);
		}
		else
		{
			res = true;
			data_bloc db;
			db.address.f_address = index;
			db.size = (*blocPos).second;
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
            deviceMemoryManager->lockBufferObject(*relocatedVertices);
            deviceMemoryManager->lockBufferObject(*relocatedFaceIndexes);
        }
        else if (!lock && m_bLocked)
        {
            deviceMemoryManager->unlockBufferObject(*relocatedVertices);
            deviceMemoryManager->unlockBufferObject(*relocatedFaceIndexes);
        }
        else
            res = false;
    }
 
    m_bLocked = lock;
    return res;
}

