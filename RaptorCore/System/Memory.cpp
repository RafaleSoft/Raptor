// Memory.cpp: implementation of the CMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#ifndef __GLOBAL_H__
    #include "Global.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif

#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif

#if !defined(AFX_RAPTORMESSAGES_H__55776166_2943_4D08_BFC8_65DFB74FD780__INCLUDED_)
    #include "System/RaptorMessages.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif


#ifdef WIN32
    #include <new.h>    // to support old platform prototypes, but not for a long time.
#else
    #include <new>
#endif

#include <set>


///////////////////////////////////////////////////////////////////////////////////////////////////
//  Statics and constants
//
RAPTOR_NAMESPACE_BEGIN

static const float WASTE_SPACE_TRESHOLD = 0.1f;	//	Allow 10% of wasted space in reallocated blocs

CMemory	*CMemory::s_pMemory = NULL;

#ifndef WIN32
    typedef new_handler _PNH;
#endif
static _PNH __oldNewHandler = NULL;


//	A specific handler to detect large memory allocations that failed
#ifdef WIN32
    int RAPTOR_NEW_HANDLER(size_t)
#else
    void RAPTOR_NEW_HANDLER(void)
#endif
{
	Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
													CRaptorErrorManager::RAPTOR_FATAL,
													CRaptorMessages::ID_NO_RESOURCE);

    Raptor::GetMessages()->displayMessage("Failed to allocate more memory");
#ifdef WIN32
	return 0;
#endif
}


//!
//! This class hodls the Memory datas
//!
class CMemoryHeap
{
public:
	typedef enum MEMORY_TYPE_t
	{
		MAIN,
		MANAGED,
		AGP,
		NB_MEMORY_TYPE
	} MEMORY_TYPE;

	typedef struct DATA_BLOC_t
	{
        unsigned char   *address;
		size_t	        size;
		size_t			usedBytes;
		MEMORY_TYPE		type;

        bool operator()(const DATA_BLOC_t& lsh,const DATA_BLOC_t& rsh)
        {
            if (lsh.size == rsh.size)
                return (lsh.address < rsh.address);
            else
                return (lsh.size < rsh.size);
        }
	} DATA_BLOC;

public:
	CMemoryHeap() {};
	~CMemoryHeap() {};

    //! Global memory allocations
	vector<DATA_BLOC>	heap;

    //! A storage for garbaged blocs. A set is used to be able
    //! to find the most appropriate bloc when needed
    //! ( the first having a size greater than the requested )
    set<DATA_BLOC,DATA_BLOC>	garbage;

	unsigned int	garbageSize;
	unsigned int	garbageMaxSize;

    CRaptorMutex     memoryMutex;

    unsigned int currentBuffers[CMemory::CBufferObject::NB_BUFFER_KIND];
};



void* CMemory::CBufferObject::getBaseAddress(void) const
{
	if (buffer.id & 1) return NULL;
	else return buffer.address;
}

static GLenum  BufferKindToGL(CMemory::CBufferObject::BUFFER_KIND kind)
{
#if defined(GL_ARB_vertex_buffer_object)
    GLenum res = GL_ARRAY_BUFFER_ARB;

    switch(kind)
    {
        case CMemory::CBufferObject::VERTEX_BUFFER:
            res = GL_ARRAY_BUFFER_ARB;
            break;
        case CMemory::CBufferObject::INDEX_BUFFER:
            res = GL_ELEMENT_ARRAY_BUFFER_ARB;
            break;
#if defined(GL_ARB_pixel_buffer_object)
        case CMemory::CBufferObject::PIXEL_STORAGE:
            res = GL_PIXEL_PACK_BUFFER_ARB;
            break;
        case CMemory::CBufferObject::PIXEL_SOURCE:
            res = GL_PIXEL_UNPACK_BUFFER_ARB;
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

static GLenum  BufferModeToGL(CMemory::CBufferObject::BUFFER_KIND kind,CMemory::CBufferObject::BUFFER_MODE mode)
{
#if defined(GL_ARB_vertex_buffer_object)
    GLenum res = GL_STATIC_DRAW_ARB;

    switch(kind)
    {
        case CMemory::CBufferObject::VERTEX_BUFFER:
            res = GL_STATIC_DRAW_ARB;
            break;
        case CMemory::CBufferObject::INDEX_BUFFER:
            res = GL_STATIC_DRAW_ARB;
            break;
#if defined(GL_ARB_pixel_buffer_object)
        case CMemory::CBufferObject::PIXEL_STORAGE:
            if (mode == CMemory::CBufferObject::STATIC) res = GL_STATIC_READ_ARB;
            else if (mode == CMemory::CBufferObject::STREAM) res = GL_STREAM_READ_ARB;
            else if (mode == CMemory::CBufferObject::DYNAMIC) res = GL_DYNAMIC_READ_ARB;
            else res = GL_STATIC_DRAW_ARB;
            break;
        case CMemory::CBufferObject::PIXEL_SOURCE:
            if (mode == CMemory::CBufferObject::STATIC) res = GL_STATIC_DRAW_ARB;
            else if (mode == CMemory::CBufferObject::STREAM) res = GL_STREAM_DRAW_ARB;
            else if (mode == CMemory::CBufferObject::DYNAMIC) res = GL_DYNAMIC_DRAW_ARB;
            else res = GL_DYNAMIC_DRAW_ARB;
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


static bool IsBufferObjectValid(unsigned int buffer)
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
#endif
}

RAPTOR_NAMESPACE_END
//
///////////////////////////////////////////////////////////////////////////////////////////////////


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemory::CMemory(void)
{
	m_pHeap = new CMemoryHeap;

	m_pHeap->garbageSize = 0;
	m_pHeap->garbageMaxSize = Global::GetInstance().getConfig().m_uiGarbageSize;


    for (int i=0;i<CBufferObject::NB_BUFFER_KIND;i++)
        m_pHeap->currentBuffers[i] = 0;
}

CMemory::~CMemory()
{
    //! Gather garbage blocs:
    set<CMemoryHeap::DATA_BLOC,CMemoryHeap::DATA_BLOC>::const_iterator itr = m_pHeap->garbage.begin();
    while (itr != m_pHeap->garbage.end())
        m_pHeap->heap.push_back((*itr++));

	while (m_pHeap->heap.size() > 0)
	{
		CMemoryHeap::DATA_BLOC& db = m_pHeap->heap[0];

		if (db.type == CMemoryHeap::MAIN)
			release(db.address);
		else if (db.type == CMemoryHeap::AGP)
		{
			CBufferObject *vb = new CBufferObject();
			vb->buffer.address = db.address;
			glReleaseBufferObject(vb);
		}
        else if (db.type == CMemoryHeap::MANAGED)
        {
            // reverse a garbage bloc to normal heap bloc and deallocate it.
            db.type = CMemoryHeap::MAIN;
			release(db.address);
        }
	}

	delete m_pHeap;

    //  restore default allocator handler
	s_pMemory = NULL;

	if (__oldNewHandler != NULL)
#ifdef WIN32
		_set_new_handler(__oldNewHandler);
#else
        set_new_handler(__oldNewHandler);
#endif
}

CMemory *CMemory::GetInstance(void)
{
	if (s_pMemory == NULL)
	{
		s_pMemory = new CMemory;
	}

	return s_pMemory;
}

bool CMemory::init(void)
{
	if (__oldNewHandler == NULL)
	{
#ifdef WIN32
		__oldNewHandler = _set_new_handler(RAPTOR_NEW_HANDLER);
#else
        set_new_handler(__oldNewHandler);
#endif

		return true;
	}
	else
		return false;
}

void CMemory::setGarbageMaxSize(unsigned int maxSize) const
{
	m_pHeap->garbageMaxSize = maxSize;
}

void CMemory::glSetBufferObjectData(CBufferObject &vb,unsigned int dstOffset,const void* src,size_t sz)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((src == NULL) || (sz == 0))
		return;
	if (vb.m_size < (dstOffset + sz))
		return;
#endif

    //! This method could be called very often per  frame, lock/unlock 
    //! could be very expensive, so I will try to lock at a higher level
    //! CThreadLock lock(m_pHeap->memoryMutex);

#ifdef GL_ARB_vertex_buffer_object
	// Is it a VBO ?
	if ((vb.buffer.id & 1) != 0)
	{
		unsigned int buffer = vb.buffer.id >> 16;
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (IsBufferObjectValid(buffer))
#else
		if (buffer > 0)
#endif
		{
            GLenum glStorage = BufferKindToGL(vb.m_storage);

            if (m_pHeap->currentBuffers[vb.m_storage] != buffer)
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
            if (m_pHeap->currentBuffers[vb.m_storage] != buffer)
            {
		        pExtensions->glBindBufferARB(glStorage,0);
                m_pHeap->currentBuffers[vb.m_storage] = 0;
            }
		}
	}
	else
#endif
	{
		memcpy((char*)vb.buffer.address + dstOffset,src,sz);
	}

    CATCH_GL_ERROR
}


void CMemory::glGetBufferObjectData(CBufferObject &vb,unsigned int srcOffset,void* dst,size_t sz)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((dst == NULL) || (sz == 0))
		return;
	if (vb.m_size < (srcOffset + sz))
		return;
#endif

    //! This method could be called very often, lock/unlock 
    //! could be very expensive, so I will try to lock at a higher level
    //! CThreadLock lock(m_pHeap->memoryMutex);

#ifdef GL_ARB_vertex_buffer_object
	// Is it a VBO ?
	if ((vb.buffer.id & 1) != 0)
	{
		unsigned int buffer = vb.buffer.id >> 16;
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (IsBufferObjectValid(buffer))
#else
		if (buffer > 0)
#endif
		{
            GLenum glStorage = BufferKindToGL(vb.m_storage);

			if (m_pHeap->currentBuffers[vb.m_storage] != buffer)
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
		    if (m_pHeap->currentBuffers[vb.m_storage] != buffer)
            {
		        pExtensions->glBindBufferARB(glStorage,0);
                m_pHeap->currentBuffers[vb.m_storage] = 0;
            }
		}
	}
	else
#endif
	{
		memcpy(dst,(char*)vb.buffer.address + srcOffset,sz);
	}

    CATCH_GL_ERROR
}

CMemory::CBufferObject * CMemory::glAllocateBufferObject(	CMemory::CBufferObject::BUFFER_KIND kind, 
															CBufferObject::BUFFER_MODE mode, 
															size_t size)
{
	if (size == 0)
	{
		return NULL;
	}

    CMemory::CBufferObject * res = NULL;
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

#ifdef GL_ARB_vertex_buffer_object
	//	This should be the fastest memory bloc for
	//	OpenGL 1.4.1 drivers
	if (pExtensions->glGenBuffersARB != NULL)
	{
        //! After this point, we are in critical section
        CRaptorLock lock(m_pHeap->memoryMutex);

		GLenum glStorage = BufferKindToGL(kind);
        GLenum glMode = BufferModeToGL(kind,mode);

		unsigned int buffer = 0;
		pExtensions->glGenBuffersARB(1,&buffer);
		pExtensions->glBindBufferARB(glStorage,buffer);

		CATCH_GL_ERROR

#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (IsBufferObjectValid(buffer))
#else
		if (buffer > 0)
#endif
		{
        	//	Store the buffer in the high part
			//	and store the memory type in the low part.
			//	This format is odd enough to be sure it cannot
			//	be an address.
			res = new CMemory::CBufferObject;
			res->buffer.id = ((buffer & 0xffff) << 16) + 1;
            res->m_size = size;
            res->m_storage = kind;

			//	Allocate uninitialised data space
			pExtensions->glBufferDataARB(glStorage,size,NULL,glMode);

            //	0 should by to the "GL default" array model.
		    pExtensions->glBindBufferARB(glStorage,0);

			CMemoryHeap::DATA_BLOC db;
			db.type = CMemoryHeap::AGP;
            // Store the id instead of the base address because we need to recreate
            // BufferObjects when CMemory is destroyed to correctly call glReleaseBufferObject.
            // Besides, this id will uniquely identify an AGP block wheareas base addresses can be identical.
			//db.address = (unsigned char*)res->getBaseAddress();
            db.address = (unsigned char*)res->buffer.id;
			db.size = size;
			db.usedBytes = size;

			m_pHeap->heap.push_back(db);

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
	// This should be the fastest possible chunck of memory for nVidia AGP
	else 
#endif
#ifdef GL_NV_vertex_array_range
	if (pExtensions->wglAllocateMemoryNV != NULL)
	{
        //! After this point, we are in critical section
        CRaptorLock lock(m_pHeap->memoryMutex);

        res = new CMemory::CBufferObject;

		//	Allocate memory on AGP
        res->m_size = size;
        res->m_storage = kind;
		res->buffer.address = pExtensions->wglAllocateMemoryNV(size,0.0f,0.0f,0.5f);
		if (res->buffer.address == NULL)
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
				                                            "The requested Buffer Object could not be allocated");
#endif
            delete res;
			return NULL;
		}
		else
		{

			CMemoryHeap::DATA_BLOC db;
			db.type = CMemoryHeap::AGP;
			db.address = (unsigned char*)res->buffer.address;
			db.size = size;
			db.usedBytes = size;

			m_pHeap->heap.push_back(db);

			return res;
		}
	}
	else
#endif
	{
		return NULL;
	}
}

bool CMemory::glReleaseBufferObject(CBufferObject* &vb)
{
	bool found = false;

    //! After this point, we are in critical section
    CRaptorLock lock(m_pHeap->memoryMutex);

	vector<CMemoryHeap::DATA_BLOC>::iterator itr = m_pHeap->heap.begin();
	while ((!found) && (itr != m_pHeap->heap.end()))
	{
        // The buffer address holds the unique id when it is an AGP bloc ( a BufferObject is always an AGP bloc )
		//found = (((*itr).address == vb.getBaseAddress()) && ((*itr).type == CMemoryHeap::AGP));
        found = ((*itr).address == vb->buffer.address);
		if (!found) itr++;
	}

	if (!found)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
                                                        "You are trying to free a block that was not properly allocated");
#endif
		return false;
	}
	else
	{
		m_pHeap->heap.erase(itr);
	}

	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
    if (pExtensions == NULL)
        return false;

#if defined(GL_ARB_vertex_buffer_object)
	if ((pExtensions->glDeleteBuffersARB != NULL) && ((vb->buffer.id & 1) != 0))
	{
		unsigned int buffer = (vb->buffer.id >> 16);
		pExtensions->glDeleteBuffersARB(1,&buffer);
		delete vb;
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

bool CMemory::glLockBufferObject(CBufferObject &vb)
{
	if ((vb.buffer.address == NULL) || (vb.m_size == 0))
		return false;

	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

    //! This method could be called very often, lock/unlock 
    //! could be very expensive, so I will try to lock at a higher level
    //! CThreadLock lock(m_pHeap->memoryMutex);

#ifdef GL_ARB_vertex_buffer_object
	if ((pExtensions->glBindBufferARB != NULL) && ((vb.buffer.id & 1) != 0))
	{
		unsigned int buffer = (vb.buffer.id >> 16);

		GLenum glStorage = BufferKindToGL(vb.m_storage);

		pExtensions->glBindBufferARB(glStorage,buffer);

        m_pHeap->currentBuffers[vb.m_storage] = buffer;

		return true;
	}
	else 
#endif
#ifdef GL_NV_vertex_array_range
	if (pExtensions->glVertexArrayRangeNV != NULL)
	{
		pExtensions->glVertexArrayRangeNV(vb.m_size,vb.getBaseAddress());
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
		return true;
	}
	else
#endif
	{
		return false;
	}
}

bool CMemory::glUnlockBufferObject(CBufferObject &vb)
{
	if ((vb.buffer.address == NULL) || (vb.m_size == 0))
		return false;

	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

    //! This method could be called very often, lock/unlock 
    //! could be very expensive, so I will try to lock at a higher level
    //! CThreadLock lock(m_pHeap->memoryMutex);

#ifdef GL_ARB_vertex_buffer_object
	if ((pExtensions->glBindBufferARB != NULL) && ((vb.buffer.id & 1) != 0))
	{
        GLenum glStorage = BufferKindToGL(vb.m_storage);

		//	0 should by to the "GL default" array model.
		pExtensions->glBindBufferARB(glStorage,0);

        m_pHeap->currentBuffers[vb.m_storage] = 0;

		return true;
	}
	else 
#endif
	{
		return false;
	}
}

void *CMemory::allocate(size_t size,unsigned int count,unsigned char alignment) const
{
    //! After this point, we are in critical section
    CRaptorLock lock(m_pHeap->memoryMutex);

    bool found = false;
	size_t requestedSize = size*count;
	set<CMemoryHeap::DATA_BLOC,CMemoryHeap::DATA_BLOC>::iterator itr = m_pHeap->garbage.begin();

	while ((!found) && (itr != m_pHeap->garbage.end()))
	{
        const CMemoryHeap::DATA_BLOC &db = (*itr);

		found = (	(db.size >= requestedSize) && 
					(db.type == CMemoryHeap::MANAGED) &&
					((db.size - requestedSize) < WASTE_SPACE_TRESHOLD*db.size)	);

		if (!found) 
            itr++;
	}

    if (!found)
    {
	    //	minimum non nul alignment is 4
	    unsigned int align = (unsigned int)(alignment & 0xfc);
        if (align == 0)
            align = 4;
	    
	    //	allocate data
	    char *pT = new char[size * count + align];

	    //	align data
	    void *data = (void*)( (int(pT) + align) & ~((int)(align-1)));

	    // store allocation offset
        memset(pT,0,size * count + align);
        *((unsigned char*)data-1) = 0xff & ((unsigned int)(data) - (unsigned int)(pT));

	    CMemoryHeap::DATA_BLOC db;
	    db.type = CMemoryHeap::MAIN;
	    db.address = (unsigned char*)data;
	    db.size = requestedSize;
		db.usedBytes = requestedSize;

	    m_pHeap->heap.push_back(db);
        return data;
    }
    else
    {
        CMemoryHeap::DATA_BLOC db = (*itr);
        db.type = CMemoryHeap::MAIN;
		db.usedBytes = requestedSize;
        m_pHeap->heap.push_back(db);
        m_pHeap->garbage.erase(itr);
		m_pHeap->garbageSize -= db.size;
        memset(db.address,0,db.size);
		
        return db.address;
    }
}


void CMemory::release(void *data) const
{
    //! After this point, we are in critical section
    CRaptorLock lock(m_pHeap->memoryMutex);

	bool found = false;

    //! Search to validate the bloc is allocated with alignment and exists
	vector<CMemoryHeap::DATA_BLOC>::iterator itr = m_pHeap->heap.begin();
	while ((!found) && (itr != m_pHeap->heap.end()))
	{
        CMemoryHeap::DATA_BLOC &db = (*itr);
		found = ((db.address == data) && (db.type == CMemoryHeap::MAIN));

		if (!found) 
            itr++;
	}

	if (!found)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_WARNING,
                                                       "You are trying to free a block that was not properly allocated");
#endif
		return;
	}
	else
	{
		m_pHeap->heap.erase(itr);
	}

	unsigned char *pT = (unsigned char*)data;

	// get allocation offset
	pT -= *(pT-1);

	// proceed to memory release
	delete [] pT;
}

void CMemory::garbage(void *data) const
{
    //! After this point, we are in critical section
    CRaptorLock lock(m_pHeap->memoryMutex);
    bool found = false;

    //! Search to validate the bloc is allocated with alignment and exists
	vector<CMemoryHeap::DATA_BLOC>::iterator itr = m_pHeap->heap.begin();
	while (!found && (itr != m_pHeap->heap.end()))
	{
        CMemoryHeap::DATA_BLOC &db = (*itr);
		if ((db.address == data) && (db.type == CMemoryHeap::MAIN))
        {
            db.type = CMemoryHeap::MANAGED;
            found = true;
            m_pHeap->garbage.insert(db);
			m_pHeap->garbageSize += db.size;
            m_pHeap->heap.erase(itr);
        }

        if (!found)
            itr++;
	}

	if (m_pHeap->garbageSize > m_pHeap->garbageMaxSize)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_NO_ERROR,
                                                       "Garbage reached the limit, releasing memory");
#endif

		set<CMemoryHeap::DATA_BLOC,CMemoryHeap::DATA_BLOC>::iterator it = m_pHeap->garbage.begin();
		while (it != m_pHeap->garbage.end())
		{
			const CMemoryHeap::DATA_BLOC &db = (*it++);
			unsigned char *pT = (unsigned char*)(db.address);

			// get allocation offset
			pT -= *(pT-1);

			// proceed to memory release
			delete [] pT;
		}
		m_pHeap->garbage.clear();
		m_pHeap->garbageSize = 0;
	}

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (!found)
	{
		Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_WARNING,
                                                       "You are trying to garbage a block that was not properly allocated");
	}
#endif
}




