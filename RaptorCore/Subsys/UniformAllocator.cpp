/***************************************************************************/
/*                                                                         */
/*  UniformAllocator.cpp                                                   */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
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

#if !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)
	#include "Subsys/UniformAllocator.h"
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
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif


RAPTOR_NAMESPACE_BEGIN

CUniformAllocator	*CUniformAllocator::m_pInstance = NULL;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUniformAllocator::CUniformAllocator()
	:relocatedUniforms(NULL)
{
	uniforms.address = NULL;
	uniforms.size = 0;
}


CUniformAllocator::~CUniformAllocator()
{
	if (uniforms.address != NULL)
		CHostMemoryManager::GetInstance()->release(uniforms.address);
	if (relocatedUniforms != NULL)
		deviceMemoryManager->releaseBufferObject(relocatedUniforms);

	if (this == m_pInstance)
		m_pInstance = NULL;
}

CUniformAllocator* CUniformAllocator::GetInstance(void)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CUniformAllocator();
	}

	return m_pInstance;
}


CUniformAllocator	*CUniformAllocator::SetCurrentInstance(CUniformAllocator* uniformAllocator)
{
	CUniformAllocator *pInstance = m_pInstance;

	m_pInstance = uniformAllocator;

	return pInstance;
}

bool CUniformAllocator::glvkInitMemory(	IDeviceMemoryManager* pDeviceMemory,
										uint64_t uniformSize)
{
	if (m_bLocked)
		return false;

	// Should there be several blocs ? No to be able to switch to relocatable blocs in AGP
	// ( it is much faster to have only one bufferobject bound for RT rendering )
	if ((uniformSize == 0) || (!uniformBlocs.empty()))
		return false;

	deviceMemoryManager = pDeviceMemory;

	//  Allow user to allocate bloc size different from default.
	if (uniforms.address != NULL)
	{
		CHostMemoryManager::GetInstance()->release(uniforms.address);
		uniforms.address = NULL;
	}
	if (relocatedUniforms != NULL)
		deviceMemoryManager->releaseBufferObject(relocatedUniforms);

	if ((NULL != deviceMemoryManager) && (deviceMemoryManager->relocationAvailable()))
	{
		uniforms.size = uniformSize;
		relocatedUniforms = deviceMemoryManager->createBufferObject(IDeviceMemoryManager::IBufferObject::UNIFORM_BUFFER,
																	IDeviceMemoryManager::IBufferObject::STREAM,
																	uniformSize);
		CATCH_GL_ERROR

		return (relocatedUniforms != NULL);
	}
	else
	{
		uniforms.address = charAlloc.allocate(uniformSize);
		uniforms.size = uniformSize;
		return (uniforms.address != NULL);
	}
}


void CUniformAllocator::glvkSetPointerData(uint8_t *dst, uint8_t *src, uint64_t size)
{
	if ((NULL == deviceMemoryManager) || (NULL == relocatedUniforms) || (NULL == src) || (NULL == dst))
		return;

	if (0 == size)
	{
		// find memory bloc and map a copy to local memory.
		map<uint8_t*, uint64_t>::const_iterator blocPos = uniformBlocs.find(dst);
		if (blocPos != uniformBlocs.end())
		{
			deviceMemoryManager->setBufferObjectData(	*relocatedUniforms,
														(uint64_t)dst,
														src,
														(*blocPos).second);
		}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		else
		{
			Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
				CRaptorErrorManager::RAPTOR_WARNING,
				"The destination device buffer does not exist");
		}
#endif
	}
	else
		// No ckech is done to validate that dst is a bloc of size 'size'
		deviceMemoryManager->setBufferObjectData(	*relocatedUniforms,
													(uint64_t)dst,
													src,
													size);
}

uint8_t * const CUniformAllocator::allocateUniforms(uint64_t size)
{
	if ((0 == size) || (m_bLocked) || ((NULL == uniforms.address) && (NULL == relocatedUniforms)))
		return NULL;

	// be it relocated or not, vertices can be the beginning of the memory block
	void *currentAddress = uniforms.address;

	if (!uniformBlocs.empty())
	{
		uint64_t maxSize = uniforms.size;
		if (!freeUniformBlocs.empty())
		{
			bool reuse = false;
			size_t blocPos = 0;
			for (size_t i = 0; i<freeUniformBlocs.size(); i++)
			{
				data_bloc &db = freeUniformBlocs[i];
				if ((db.size >= size) && (db.size < maxSize))
				{
					blocPos = i;
					maxSize = db.size;
					reuse = true;
				}
			}
			if (reuse)
			{
				uint8_t* addr = freeUniformBlocs[blocPos].address;
				freeUniformBlocs.erase(freeUniformBlocs.begin() + blocPos);
				return addr;
			}
		}

		map<uint8_t*, uint64_t>::iterator it = uniformBlocs.end();
		it--;
		currentAddress = (*it).first + (*it).second;
	}


	if (((uint64_t)currentAddress - (uint64_t)uniforms.address) + size > uniforms.size)
	{
		RAPTOR_FATAL(	COpenGL::COpenGLClassID::GetClassId(),
						"Uniform Allocator could not get enough memory");
		return NULL;
	}

	//	Align on quad dword
	uint64_t granularity = 16;
	if (NULL != relocatedUniforms)
		granularity = relocatedUniforms->getRelocationOffset();

	//	Address should be aligned on a boundary multiple of buffer granularity
	uint64_t base = ((uint64_t)currentAddress) / granularity;
	//  and no NULL offset to distinguish nil pointers
	base = (base + 1) * granularity;

	uint8_t* address = (uint8_t*)(base);
	uniformBlocs[address] = size;

	return address;
}

bool CUniformAllocator::releaseUniforms(uint8_t *uniform)
{
	if (m_bLocked)
		return false;

	bool res = false;

	map<uint8_t*, uint64_t>::iterator blocPos = uniformBlocs.find(uniform);
	if (blocPos != uniformBlocs.end())
	{
		map<uint8_t*, uint64_t>::iterator last = uniformBlocs.end();
		last--;
		if ((*blocPos).first == (*last).first)
		{
			uniformBlocs.erase(blocPos);
		}
		else
		{
			res = true;
			data_bloc db;
			db.address = uniform;
			db.size = (*blocPos).second;
			freeUniformBlocs.push_back(db);
		}
	}

	return res;
}

bool CUniformAllocator::glvkLockMemory(bool lock)
{
	bool res = true;

	if ((NULL != deviceMemoryManager) && (relocatedUniforms != NULL))
	{
		if (lock && !m_bLocked)
			res = deviceMemoryManager->lockBufferObject(*relocatedUniforms);
		else if (!lock && m_bLocked)
			res = deviceMemoryManager->unlockBufferObject(*relocatedUniforms);
		else
			res = false;
	}

	m_bLocked = lock;
	return res;
}

bool CUniformAllocator::glvkBindUniform(uint8_t *uniform, int32_t index, uint64_t offset, uint64_t size)
{
	//! In locked state, the buffer is bound, we are in rendering.
	if (!m_bLocked)
		return false;

	map<uint8_t*, uint64_t>::iterator blocPos = uniformBlocs.find(uniform);
	if (blocPos != uniformBlocs.end())
	{
		uint64_t sz = size;
		if (0 == sz)
			sz = (*blocPos).second;
		if (0 == sz)
			return false;

		uint8_t *base = (*blocPos).first;
		if (offset + sz < (*blocPos).second)	// do not bind more that existing data.
			base = base + offset;
		uint32_t buffer = relocatedUniforms->getBufferId();

#ifdef RAPTOR_DEBUG_MODE_GENERATION
		uint64_t granularity = 16;
		if (NULL != relocatedUniforms)
			granularity = relocatedUniforms->getRelocationOffset();
		if (((uint64_t)base % granularity) > 0)
		{
			RAPTOR_ERROR(	COpenGL::COpenGLClassID::GetClassId(),
							"Uniform allocator buffer base is not aligned on granularity" );
		}
#endif

		const CRaptorGLExtensions *const pExtensions = IRaptor::glGetExtensions();
		if (pExtensions->glBindBufferRangeARB != NULL)
		{
			pExtensions->glBindBufferRangeARB(GL_UNIFORM_BUFFER_ARB, index, buffer, base, sz);
			
			CATCH_GL_ERROR
		}

		return true;
	}
	else
		return false;
}


void *CUniformAllocator::glvkMapPointer(uint8_t *pointer, bool syncData)
{
	if ((NULL == relocatedUniforms) || (m_bLocked) || (NULL == pointer))
		return pointer;

	// already mapped ?
	if (uniformReMap.find(pointer) != uniformReMap.end())
		return pointer;

	// find memory bloc and map a copy to local memory.
	map<uint8_t*, uint64_t>::const_iterator blocPos = uniformBlocs.find(pointer);
	if (blocPos != uniformBlocs.end())
	{
		uint64_t sz = (*blocPos).second;
		unsigned char* localData = charAlloc.allocate(sz);

		uniformReMap[pointer] = localData;
		uniformReMap[localData] = pointer;

		if (syncData)
		{
			deviceMemoryManager->getBufferObjectData(	*relocatedUniforms,
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

void *CUniformAllocator::glvkUnMapPointer(uint8_t *pointer, bool syncData)
{
	if ((NULL == relocatedUniforms) || (m_bLocked) || (NULL == pointer))
		return pointer;

	// pointer has been mapped ?
	map<uint8_t*, uint8_t*>::iterator it = uniformReMap.find(pointer);
	if (it == uniformReMap.end())
		// I shouldn't return NULL as it can be buffer index 0.
		//  To keep a meaning to buffer offset 0, I could keep the fist data in buffer
		//  ( offset 0 ) to be invalid, and reserved at allocation, garantying thus that any
		//  other data bloc will have an offset > 0
		return pointer;


	// find memory bloc and copy local memory to server address space.
	uint8_t *serverData = (*it).second;

	map<uint8_t*, uint64_t>::const_iterator blocPos = uniformBlocs.find(serverData);
	if (blocPos != uniformBlocs.end())
	{
		uniformReMap.erase(it);
		map<uint8_t*, uint8_t*>::iterator it2 = uniformReMap.find(serverData);
		//  Should check for errors.
		uniformReMap.erase(it2);

		if (syncData)
		{
			// Here, serverData could be relocated to compress the data
			// and limit the number of holes or fragmentation.
			// As we have an array of free blocs, relocation could easily be done.
			deviceMemoryManager->setBufferObjectData(	*relocatedUniforms,
														(uint64_t)serverData,
														pointer,
														(*blocPos).second);

		}

		CHostMemoryManager::GetInstance()->garbage(pointer);

		CATCH_GL_ERROR

		return serverData;
	}
	else
		return NULL;
}

