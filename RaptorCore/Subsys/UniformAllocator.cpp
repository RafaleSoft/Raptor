// UniformAllocator.cpp: implementation of the CUniformAllocator class.
//
//////////////////////////////////////////////////////////////////////

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


void CUniformAllocator::glvkCopyPointer(uint8_t *dst, uint8_t *src, uint64_t size)
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
			unsigned int blocPos = 0;
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
						"Geometry Allocator could not get enough memory");
		return NULL;
	}

	
	//	Address should be aligned on a boundary multiple of buffer granularity
	uint64_t base = ((uint64_t)currentAddress) / relocatedUniforms->getRelocationOffset();
	//  and no NULL offset to distinguish nil pointers
	base = (base + 1) * relocatedUniforms->getRelocationOffset();

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

bool CUniformAllocator::glvkBindUniform(uint8_t *uniform, int32_t index)
{
	if (m_bLocked)
		return false;

	map<uint8_t*, uint64_t>::iterator blocPos = uniformBlocs.find(uniform);
	if (blocPos != uniformBlocs.end())
	{
		uint64_t size = (*blocPos).second;
		if (0 == size)
			return false;

		uint8_t *base = (*blocPos).first;
		uint32_t buffer = relocatedUniforms->getBufferId();

		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		if (pExtensions->glBindBufferRangeARB != NULL)
		{
			pExtensions->glBindBufferRangeARB(GL_UNIFORM_BUFFER_ARB, index, buffer, (GLintptrARB)base, size);
			CATCH_GL_ERROR
		}

		return true;
	}
	else
		return false;
}

