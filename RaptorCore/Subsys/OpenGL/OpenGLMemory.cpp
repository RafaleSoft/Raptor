// OpenGLMemory.cpp: implementation of the COpenGLMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys\CodeGeneration.h"

#if !defined(AFX_OPENGLMEMORY_H__C344567B_877F_4F43_8961_C4E59E3BBF7E__INCLUDED_)
	#include "OpenGLMemory.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_BUFFEROBJECT_H__FFF8C159_E959_496B_9962_0AF829E7FB4F__INCLUDED_)
	#include "Subsys/BufferObject.h"
#endif


RAPTOR_NAMESPACE

//  Add a constant offset to distinguish null ( unalocated ) pointers from actual memory blocs
static const int    RELOCATE_OFFSET = 16;

static 	std::map<IDeviceMemoryManager::IBufferObject *,CBufferObject*> m_deviceHeap;


COpenGLMemory::COpenGLMemory(void)
	:IDeviceMemoryManager()
{
	for (unsigned int i=0; i<IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND; i++)
		currentBuffers[i] = 0;
}

COpenGLMemory::~COpenGLMemory(void)
{
}

bool COpenGLMemory::relocationAvailable(void) const
{
#if (defined(GL_ARB_vertex_buffer_object) || defined(GL_NV_vertex_array_range))
	if (Raptor::glIsExtensionSupported(GL_ARB_VERTEX_BUFFER_OBJECT_EXTENSION_NAME) ||
		Raptor::glIsExtensionSupported(GL_NV_VERTEX_ARRAY_RANGE_EXTENSION_NAME) ||
		Raptor::glIsExtensionSupported(GL_ARB_PIXEL_BUFFER_OBJECT_EXTENSION_NAME) ||
		Raptor::glIsExtensionSupported(GL_ARB_UNIFORM_BUFFER_OBJECT_EXTENSION_NAME))
	{
		// TODO: scan device memory and check available space
		return true;
	}
	else
		return false;
#else
	return false;
#endif
}

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	bool COpenGLMemory::isBufferObjectValid(unsigned int buffer) const
	{
	#if defined(GL_ARB_vertex_buffer_object) || defined(GL_ARB_pixel_buffer_object)
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

		if (pExtensions->glIsBufferARB(buffer))
			return true;
		else
		{
	#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															"Buffer Object extension is not properly supported by your driver !");
	#endif
			//	Here, we could also check all other glIsXXX to strengthen the check
			if (buffer > 0)
				return true;
			else
				return false;
		}
		CATCH_GL_ERROR
	#endif
	}
#endif

IDeviceMemoryManager::IBufferObject *
COpenGLMemory::createBufferObject(	IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind, 
									IDeviceMemoryManager::IBufferObject::BUFFER_MODE mode, 
									uint64_t size)
{
	if (0 == size)
		return NULL;

    IDeviceMemoryManager::IBufferObject * res = NULL;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#ifdef GL_ARB_vertex_buffer_object
	//	This should be the fastest memory bloc for
	//	OpenGL 1.4.1 drivers
	if (pExtensions->glGenBuffersARB != NULL)
	{
		GLenum glStorage = BufferKindToGL(kind);
        GLenum glMode = BufferModeToGL(kind,mode);

		unsigned int buffer = 0;
		pExtensions->glGenBuffersARB(1,&buffer);
		pExtensions->glBindBufferARB(glStorage,buffer);

		CATCH_GL_ERROR

#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (isBufferObjectValid(buffer))
#else
		if (buffer > 0)
#endif
		{
        	//	Store the buffer in the high part
			//	and store the memory type in the low part.
			//	This format is odd enough to be sure it cannot
			//	be an address.
			CBufferObject* pbuffer = new CBufferObject;
			pbuffer->m_buffer = ((buffer & 0xffff) << 16) + 1;
			pbuffer->m_size = size + RELOCATE_OFFSET;
            pbuffer->m_storage = kind;
			pbuffer->m_granularity = RELOCATE_OFFSET;

			//	Allocate uninitialised data space
			pExtensions->glBufferDataARB(glStorage, size + RELOCATE_OFFSET, NULL, glMode);

            //	0 should by to the "GL default" array model.
		    pExtensions->glBindBufferARB(glStorage,0);

			res = pbuffer;
			m_deviceHeap[res] = pbuffer;
			CATCH_GL_ERROR
			return res;
		}
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
				                                            "The requested Buffer Object could not be allocated");
#endif
			return NULL;
		}
	}
	else
#endif
		return NULL;
}

bool COpenGLMemory::setBufferObjectData(IDeviceMemoryManager::IBufferObject &bo,
										uint64_t dstOffset,
										const void* src,
										uint64_t sz)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((src == NULL) || (sz == 0))
		return false;
	if (bo.getSize() < (dstOffset + sz))
		return false;
#endif

    //! This method could be called very often per  frame, lock/unlock 
    //! could be very expensive, so I will try to lock at a higher level
    //! CThreadLock lock(m_pHeap->memoryMutex);

#ifdef GL_ARB_vertex_buffer_object
	// Is it a VBO ?
	uint32_t buffer = bo.getBufferId();
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (isBufferObjectValid(buffer))
#else
	if (buffer > 0)
#endif
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		IDeviceMemoryManager::IBufferObject::BUFFER_KIND storage = bo.getStorage();
		if (storage > IDeviceMemoryManager::IBufferObject::PIXEL_SOURCE)
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															"Buffer Object storage is not supported by RaptorCore COpenGLMemory");
#endif
			return false;
		}

        GLenum glStorage = BufferKindToGL(storage);

        if (currentBuffers[storage] != buffer)
        {
		    pExtensions->glBindBufferARB(glStorage,buffer);
        }
		
		//	This call may be faster in the future.
		pExtensions->glBufferSubDataARB(glStorage, dstOffset, sz, src);
		/*
		char *data = (char*)pExtensions->glMapBufferARB(glStorage,GL_WRITE_ONLY_ARB);
        
        if (data != NULL)
        {
		    memcpy(data + dstOffset,src,sz);
        }
		
		pExtensions->glUnmapBufferARB(glStorage);
		*/
        //	0 should by to the "GL default" array model.
        if (currentBuffers[storage] != buffer)
        {
	        pExtensions->glBindBufferARB(glStorage,0);
            currentBuffers[storage] = 0;
        }
	}
	else
#endif
		return false;

    CATCH_GL_ERROR;
	return true;
}


bool COpenGLMemory::getBufferObjectData(IDeviceMemoryManager::IBufferObject &vb,
										uint64_t srcOffset,
										void* dst,
										uint64_t sz)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((dst == NULL) || (sz == 0))
		return false;
	if (vb.getSize() < (srcOffset + sz))
		return false;
#endif

    //! This method could be called very often, lock/unlock 
    //! could be very expensive, so I will try to lock at a higher level
    //! CThreadLock lock(m_pHeap->memoryMutex);

#ifdef GL_ARB_vertex_buffer_object
	// Is it a VBO ?
	uint32_t buffer = vb.getBufferId();
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (isBufferObjectValid(buffer))
#else
	if (buffer > 0)
#endif
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		
		IDeviceMemoryManager::IBufferObject::BUFFER_KIND storage = vb.getStorage();
        GLenum glStorage = BufferKindToGL(storage);
		if (storage > IDeviceMemoryManager::IBufferObject::PIXEL_SOURCE)
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															"Buffer Object storage is not supported by RaptorCore COpenGLMemory");
#endif
			return false;
		}

		if (currentBuffers[storage] != buffer)
        {
		    pExtensions->glBindBufferARB(glStorage,buffer);
        }


		//	This call may be faster in the future.
		//	I have no explanation on the fact that everything is
		//	veeeery slow after a call to that function !!!
		//pExtensions->glGetBufferSubDataARB(glStorage, srcOffset, sz, dst);
		char *data = (char*)pExtensions->glMapBufferARB(glStorage,GL_READ_ONLY_ARB);
        
        if (data != NULL)
        {
		    memcpy(dst,data + srcOffset,sz);
        }

		pExtensions->glUnmapBufferARB(glStorage);

        //	0 should by to the "GL default" array model.
	    if (currentBuffers[storage] != buffer)
        {
	        pExtensions->glBindBufferARB(glStorage,0);
            currentBuffers[storage] = 0;
        }
	}
	else
#endif
		return false;

    CATCH_GL_ERROR;
	return true;
}

bool COpenGLMemory::discardBufferObjectData(IDeviceMemoryManager::IBufferObject &bo,
											uint64_t dstOffset,
											uint64_t sz)
{
	return true;
}

bool COpenGLMemory::releaseBufferObject(IDeviceMemoryManager::IBufferObject* &vb)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (vb == NULL)
		return false;
#endif
	CBufferObject *cb = NULL;
	std::map<IDeviceMemoryManager::IBufferObject *,CBufferObject*>::const_iterator itr = m_deviceHeap.find(vb);
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (m_deviceHeap.end() == itr)
	{
		Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														"You are trying to free a block that was not properly allocated");
		return false;
	}
	else
#endif
	{
		cb = (*itr).second;
		m_deviceHeap.erase(itr);
	}

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
    if (pExtensions == NULL)
        return false;

#if defined(GL_ARB_vertex_buffer_object)
	uint32_t buffer = vb->getBufferId();
	if ((pExtensions->glDeleteBuffersARB != NULL) &&
		(buffer > 0))
	{
		pExtensions->glDeleteBuffersARB(1,&buffer);
		delete cb;
		vb = NULL;
		return true;
	}
#elif defined(GL_NV_vertex_array_range)
	if ((pExtensions->wglFreeMemoryNV != NULL) && (vb->buffer.address == NULL))
	{
		pExtensions->wglFreeMemoryNV(vb->buffer.address);
		delete vb;
		vb = NULL;
		return true;
	}
#endif
	else
	{
		return false;
	}
}

bool COpenGLMemory::lockBufferObject(IDeviceMemoryManager::IBufferObject &bo)
{
	if (bo.getStorage() > IDeviceMemoryManager::IBufferObject::PIXEL_SOURCE)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														"Buffer Object storage is not supported by RaptorCore COpenGLMemory");
#endif
		return false;
	}

	if (0 == bo.getSize())
		return false;
	uint32_t buffer = bo.getBufferId();

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

    //! This method could be called very often, lock/unlock 
    //! could be very expensive, so I will try to lock at a higher level
    //! CThreadLock lock(m_pHeap->memoryMutex);

#ifdef GL_ARB_vertex_buffer_object
	if ((pExtensions->glBindBufferARB != NULL) &&
		(buffer > 0))
	{
		IDeviceMemoryManager::IBufferObject::BUFFER_KIND storage = bo.getStorage();
		GLenum glStorage = BufferKindToGL(storage);

		pExtensions->glBindBufferARB(glStorage,buffer);

        currentBuffers[storage] = buffer;

	//!	compiled vertex array locking is subject to deprecation
#if defined(GL_EXT_compiled_vertex_array)
		if (pExtensions->glLockArraysEXT != NULL)
		{
			//!	locked arrays are limited to 32bits size/index:
			//!	- check size is not exceeded
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			if (0x7fffffff < bo.getSize())
			{
				Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
															   CRaptorErrorManager::RAPTOR_WARNING,
															   "Buffer Object size over 2Gb is not supported by RaptorCore COpenGLMemory");
			}
#endif
			GLsizei size = (GLsizei)(0x7fffffff & bo.getSize());
			if (IDeviceMemoryManager::IBufferObject::VERTEX_BUFFER == bo.getStorage())
				pExtensions->glLockArraysEXT(0, size / 4);	// floats
			else if (IDeviceMemoryManager::IBufferObject::INDEX_BUFFER == bo.getStorage())
				pExtensions->glLockArraysEXT(0, size / 2);	// unsigned short, big geometry will need attention.
		}
#endif

		return true;
	}
	else 
#endif
	{
		return false;
	}
}

bool COpenGLMemory::unlockBufferObject(IDeviceMemoryManager::IBufferObject &bo)
{
	if (bo.getStorage() > IDeviceMemoryManager::IBufferObject::PIXEL_SOURCE)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														"Buffer Object storage is not supported by RaptorCore COpenGLMemory");
#endif
		return false;
	}

	if (0 == bo.getSize())
		return false;

	uint32_t buffer = bo.getBufferId();
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

    //! This method could be called very often, lock/unlock 
    //! could be very expensive, so I will try to lock at a higher level
    //! CThreadLock lock(m_pHeap->memoryMutex);

#ifdef GL_ARB_vertex_buffer_object
	if ((pExtensions->glBindBufferARB != NULL) &&
		(buffer > 0))
	{
		IDeviceMemoryManager::IBufferObject::BUFFER_KIND storage = bo.getStorage();
        GLenum glStorage = BufferKindToGL(storage);

		//	0 should by to the "GL default" array model.
		pExtensions->glBindBufferARB(glStorage,0);

        currentBuffers[storage] = 0;

//!	compiled vertex array locking is subject to deprecation
#if defined(GL_EXT_compiled_vertex_array)
		if (pExtensions->glUnlockArraysEXT != NULL)
			pExtensions->glUnlockArraysEXT();
#endif

		return true;
	}
	else 
#endif
	{
		return false;
	}
}


GLenum  COpenGLMemory::BufferKindToGL(IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind) const
{
#if defined(GL_ARB_vertex_buffer_object)
    GLenum res = GL_ARRAY_BUFFER_ARB;

    switch(kind)
    {
        case IDeviceMemoryManager::IBufferObject::VERTEX_BUFFER:
            res = GL_ARRAY_BUFFER_ARB;
            break;
        case IDeviceMemoryManager::IBufferObject::INDEX_BUFFER:
            res = GL_ELEMENT_ARRAY_BUFFER_ARB;
            break;
#if defined(GL_ARB_pixel_buffer_object)
        case IDeviceMemoryManager::IBufferObject::PIXEL_STORAGE:
            res = GL_PIXEL_PACK_BUFFER_ARB;
            break;
        case IDeviceMemoryManager::IBufferObject::PIXEL_SOURCE:
            res = GL_PIXEL_UNPACK_BUFFER_ARB;
            break;
#endif
#if defined(GL_ARB_uniform_buffer_object)
		case IDeviceMemoryManager::IBufferObject::UNIFORM_BUFFER:
			res = GL_UNIFORM_BUFFER_ARB;
			break;
#endif
        default:
            res = GL_ARRAY_BUFFER_ARB;
            break;
    }

    return res;
#else
    return CGL_NULL;
#endif
}

GLenum  COpenGLMemory::BufferModeToGL(	IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind,
										IDeviceMemoryManager::IBufferObject::BUFFER_MODE mode) const
{
#if defined(GL_ARB_vertex_buffer_object)
    GLenum res = GL_STATIC_DRAW_ARB;

    switch(kind)
    {
        case IDeviceMemoryManager::IBufferObject::VERTEX_BUFFER:
            res = GL_STATIC_DRAW_ARB;
            break;
        case IDeviceMemoryManager::IBufferObject::INDEX_BUFFER:
            res = GL_STATIC_DRAW_ARB;
            break;
#if defined(GL_ARB_pixel_buffer_object)
        case IDeviceMemoryManager::IBufferObject::PIXEL_STORAGE:
            if (mode == IDeviceMemoryManager::IBufferObject::STATIC) res = GL_STATIC_READ_ARB;
            else if (mode == IDeviceMemoryManager::IBufferObject::STREAM) res = GL_STREAM_READ_ARB;
            else if (mode == IDeviceMemoryManager::IBufferObject::DYNAMIC) res = GL_DYNAMIC_READ_ARB;
            else res = GL_STATIC_DRAW_ARB;
            break;
        case IDeviceMemoryManager::IBufferObject::PIXEL_SOURCE:
            if (mode == IDeviceMemoryManager::IBufferObject::STATIC) res = GL_STATIC_DRAW_ARB;
            else if (mode == IDeviceMemoryManager::IBufferObject::STREAM) res = GL_STREAM_DRAW_ARB;
            else if (mode == IDeviceMemoryManager::IBufferObject::DYNAMIC) res = GL_DYNAMIC_DRAW_ARB;
            else res = GL_DYNAMIC_DRAW_ARB;
            break;
#endif
#if defined(GL_ARB_uniform_buffer_object)
		case IDeviceMemoryManager::IBufferObject::UNIFORM_BUFFER:
			res = GL_DYNAMIC_DRAW_ARB;
			break;
#endif

        default:
            res = GL_STATIC_DRAW_ARB;
            break;
    }

    return res;
#else
    return CGL_NULL;
#endif
}
