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
	m_bRelocated(false),m_bLocked(false),
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
		CMemory::GetInstance()->release(faceIndexes.address.us_address);
	if (vertices.address.f_address != NULL)
		CMemory::GetInstance()->release(vertices.address.f_address);
	if (relocatedFaceIndexes != NULL)
		CMemory::GetInstance()->glReleaseBufferObject(relocatedFaceIndexes);

	if (relocatedVertices != NULL)
		CMemory::GetInstance()->glReleaseBufferObject(relocatedVertices);
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

bool	CGeometryAllocator::glUseMemoryRelocation(void)
{
#if (defined(GL_ARB_vertex_buffer_object) || defined(GL_NV_vertex_array_range))
	if (Raptor::glIsExtensionSupported("GL_ARB_vertex_buffer_object") || 
		Raptor::glIsExtensionSupported("GL_NV_vertex_array_range"))
	{
		m_bRelocated = (indexBlocs.empty() && vertexBlocs.empty());

		// TODO : call again init memory to allocate relocation pointers

		return m_bRelocated;
	}
	else
		return false;
#else
	return false;
#endif
}

void CGeometryAllocator::vkGetBindPoint(float* pVertices, VkBuffer &binding, VkDeviceSize& offset)
{
	map<float*,unsigned int>::const_iterator blocPos = vertexBlocs.find(pVertices);
		
	if (blocPos != vertexBlocs.end())
	{
		CVulkanMemory::IBufferObject *pBuffer = static_cast<CVulkanMemory::IBufferObject*>(relocatedVertices);

		binding = pBuffer->getBuffer();
		offset = (VkDeviceSize)pVertices;
	}
	else
	{
		binding = VK_NULL_HANDLE;
		offset = 0;
	}
}

void CGeometryAllocator::vkCopyPointer(float *dst, float *src, unsigned int size)
{
	if ((!m_bRelocated) /*|| (m_bLocked)*/ || (src == NULL) || (dst == NULL))
        return;

	if (NULL == deviceMemoryManager)
		return;

	CVulkanMemory::IBufferObject *pBuffer = static_cast<CVulkanMemory::IBufferObject*>(relocatedVertices);
	CVulkanMemory::IMemoryWrapper *pDeviceMemory = static_cast<CVulkanMemory::IMemoryWrapper*>(deviceMemoryManager);

	if (size == 0)
	{
		// find memory bloc and map a copy to local memory.
		map<float*,unsigned int>::const_iterator blocPos = vertexBlocs.find(dst);
		if (blocPos != vertexBlocs.end())
			pDeviceMemory->vkSetBufferObjectData(	*pBuffer,
													(unsigned int)dst,
													src,
													(*blocPos).second);
	}
	else
		// No ckech is done to validate that dst is a bloc of size 'size'
		pDeviceMemory->vkSetBufferObjectData(	*pBuffer,
												(unsigned int)dst,
												src,
												sizeof(float)*size);

    CATCH_VK_ERROR
}

void CGeometryAllocator::glCopyPointer(float *dst, float *src, unsigned int size)
{
    if ((!m_bRelocated) /*|| (m_bLocked)*/ || (src == NULL) || (dst == NULL))
        return;

	if (size == 0)
	{
		// find memory bloc and map a copy to local memory.
		map<float*,unsigned int>::const_iterator blocPos = vertexBlocs.find(dst);
		if (blocPos != vertexBlocs.end())
			CMemory::GetInstance()->glSetBufferObjectData(	*relocatedVertices,
															(unsigned int)dst,
															src,
															(*blocPos).second);
	}
	else
		// No ckech is done to validate that dst is a bloc of size 'size'
		CMemory::GetInstance()->glSetBufferObjectData(	*relocatedVertices,
														(unsigned int)dst,
														src,
														sizeof(float)*size);

    CATCH_GL_ERROR
}

void CGeometryAllocator::glCopyPointer(unsigned short *dst, unsigned short *src, unsigned int size)
{
    if ((!m_bRelocated) /*|| (m_bLocked)*/ || (src == NULL) || (dst == NULL))
        return;

	if (size == 0)
	{
		// find memory bloc and map a copy to local memory.
		map<unsigned short*,unsigned int>::const_iterator blocPos = indexBlocs.find(dst);
		if (blocPos != indexBlocs.end())
			CMemory::GetInstance()->glSetBufferObjectData(	*relocatedFaceIndexes,
															(unsigned int)dst,
															src,
															(*blocPos).second);
	}
	else
		// No ckech is done to validate that dst is a bloc of size 'size'
		CMemory::GetInstance()->glSetBufferObjectData(	*relocatedFaceIndexes,
														(unsigned int)dst,
														src,
														sizeof(unsigned short)*size);

    CATCH_GL_ERROR
}

unsigned short *CGeometryAllocator::glMapPointer(unsigned short *pointer)
{
    if ((!m_bRelocated) || (m_bLocked))
        return pointer;

    if ((relocatedFaceIndexes == NULL) || (pointer == NULL))
        return NULL;

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

        CMemory::GetInstance()->glGetBufferObjectData(	*relocatedFaceIndexes,
														(unsigned int)pointer,
														localData,
														(*blocPos).second);
        CATCH_GL_ERROR

        return localData;
    }
    else
        return NULL;
}

unsigned short *CGeometryAllocator::glUnMapPointer(unsigned short *pointer)
{
    if (!m_bRelocated || m_bLocked)
        return pointer;

    if ((relocatedFaceIndexes == NULL)  || (pointer == NULL))
        return NULL;

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
        CMemory::GetInstance()->glSetBufferObjectData(	*relocatedFaceIndexes,
														(unsigned int)serverData,
														pointer,
														(*blocPos).second);

        CMemory::GetInstance()->garbage(pointer);

        CATCH_GL_ERROR

        return serverData;
    }
    else
        return NULL;
}

unsigned short *CGeometryAllocator::glDiscardPointer(unsigned short *pointer)
{
	if (!m_bRelocated || m_bLocked)
        return pointer;

    if ((relocatedFaceIndexes == NULL)  || (pointer == NULL))
        return NULL;

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
        
        CMemory::GetInstance()->garbage(pointer);

        CATCH_GL_ERROR

        return serverData;
    }
    else
        return NULL;
}

float *CGeometryAllocator::glMapPointer(float *pointer)
{
    if (!m_bRelocated || m_bLocked)
        return pointer;

    if ((relocatedVertices == NULL)  || (pointer == NULL))
        return NULL;

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

        CMemory::GetInstance()->glGetBufferObjectData(	*relocatedVertices,
														(unsigned int)pointer,
														localData,
														(*blocPos).second);
        CATCH_GL_ERROR

        return localData;
    }
    else
        return NULL;
}

float *CGeometryAllocator::glUnMapPointer(float *pointer)
{
    if (!m_bRelocated || m_bLocked)
        return pointer;

    if ((relocatedVertices == NULL) || (pointer == NULL))
        return NULL;

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
        CMemory::GetInstance()->glSetBufferObjectData(	*relocatedVertices,
														(unsigned int)serverData,
														pointer,
														(*blocPos).second);

        CMemory::GetInstance()->garbage(pointer);

        CATCH_GL_ERROR

        return serverData;
    }
    else
        return NULL;
}

float *CGeometryAllocator::glDiscardPointer(float *pointer)
{
	if (!m_bRelocated || m_bLocked)
        return pointer;

    if ((relocatedVertices == NULL) || (pointer == NULL))
        return NULL;

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
        
        CMemory::GetInstance()->garbage(pointer);

        CATCH_GL_ERROR

        return serverData;
    }
    else
        return NULL;
}

bool CGeometryAllocator::glInitMemory(IDeviceMemoryManager* pDeviceMemory,
									  unsigned int indexSize,unsigned int coordsSize)
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
		CMemory::GetInstance()->release(faceIndexes.address.us_address);
        faceIndexes.address.us_address = NULL;
    }
	if (vertices.address.f_address != NULL)
    {
		CMemory::GetInstance()->release(vertices.address.f_address);
        vertices.address.f_address = NULL;
    }
    if (relocatedVertices != NULL)
        CMemory::GetInstance()->glReleaseBufferObject(relocatedVertices);
    if (relocatedFaceIndexes != NULL)
        CMemory::GetInstance()->glReleaseBufferObject(relocatedFaceIndexes);

	if (m_bRelocated)
	{
		vertices.size = coordsSize * sizeof(float);
		relocatedVertices = CMemory::GetInstance()->glAllocateBufferObject(	CMemory::IBufferObject::VERTEX_BUFFER,
																			CMemory::IBufferObject::STATIC,
																			vertices.size+RELOCATE_OFFSET);
		
		faceIndexes.size = indexSize  * sizeof(unsigned short);
		relocatedFaceIndexes = CMemory::GetInstance()->glAllocateBufferObject(	CMemory::IBufferObject::INDEX_BUFFER,
																				CMemory::IBufferObject::STATIC,
																				faceIndexes.size+RELOCATE_OFFSET);
		

        CATCH_GL_ERROR

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

bool CGeometryAllocator::vkInitMemory(	IDeviceMemoryManager* pDeviceMemory,
										unsigned int indexSize,unsigned int coordsSize)
{
    if (m_bLocked)
        return false;

	// Should there be several blocs ? No to be able to switch to relocatable blocs in AGP
	// ( it is much faster to have only one bufferobject bound for RT rendering )
	if ((indexSize == 0) || (!indexBlocs.empty()))
		return false;
	if ((coordsSize == 0) || (!vertexBlocs.empty()))
		return false;

	if (faceIndexes.address.us_address != NULL)
    {
		CMemory::GetInstance()->release(faceIndexes.address.us_address);
        faceIndexes.address.us_address = NULL;
    }
	if (vertices.address.f_address != NULL)
    {
		CMemory::GetInstance()->release(vertices.address.f_address);
        vertices.address.f_address = NULL;
    }

	deviceMemoryManager = pDeviceMemory;

    //if (relocatedVertices != NULL)
    //    CMemory::GetInstance()->glReleaseBufferObject(relocatedVertices);
    //if (relocatedFaceIndexes != NULL)
    //    CMemory::GetInstance()->glReleaseBufferObject(relocatedFaceIndexes);

	m_bRelocated = true;
	CVulkanMemory::IMemoryWrapper *deviceMemory = static_cast<CVulkanMemory::IMemoryWrapper*>(deviceMemoryManager);

	vertices.size = coordsSize * sizeof(float);
	relocatedVertices = deviceMemory->vkCreateBufferObject(	vertices.size+RELOCATE_OFFSET,
															CVulkanMemory::IBufferObject::VERTEX_BUFFER);
	
	faceIndexes.size = indexSize  * sizeof(unsigned short);
	relocatedFaceIndexes = deviceMemory->vkCreateBufferObject(	faceIndexes.size+RELOCATE_OFFSET,
																CVulkanMemory::IBufferObject::INDEX_BUFFER);
	
	return ((relocatedFaceIndexes != NULL) && (relocatedVertices != NULL));
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
    if ((m_bRelocated) && ( currentAddress == NULL))
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


float	* const	CGeometryAllocator::allocateVertices(unsigned int size)
{
	if ((size == 0)  || (m_bLocked) || ((vertices.address.f_address == NULL) && (relocatedVertices == NULL)))
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
    if ((m_bRelocated) && ( currentAddress == NULL))
        currentAddress = (float*)RELOCATE_OFFSET;

	//	Address should be aligned on a 16byte boundary
	float* address = (float*)(((unsigned int)(currentAddress) + 0x0f) & 0xfffffff0);
	vertexBlocs[address] = sz;

	return address;
}


bool	CGeometryAllocator::releaseVertices(float *index)
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
bool CGeometryAllocator::glLockMemory(bool lock)
{
    bool res = true;

    if ((m_bRelocated) && (relocatedVertices != NULL) && (relocatedFaceIndexes != NULL))
    {
        if (lock && !m_bLocked)
        {
            CMemory::GetInstance()->glLockBufferObject(*relocatedVertices);
            CMemory::GetInstance()->glLockBufferObject(*relocatedFaceIndexes);
        }
        else if (!lock && m_bLocked)
        {
            CMemory::GetInstance()->glUnlockBufferObject(*relocatedVertices);
            CMemory::GetInstance()->glUnlockBufferObject(*relocatedFaceIndexes);
        }
        else
            res = false;
    }
 
    m_bLocked = lock;
    return res;
}

