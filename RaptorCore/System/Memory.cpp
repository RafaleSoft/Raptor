// Memory.cpp: implementation of the CHostMemoryManager class.
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
//#define INITIALISE_MEMORY 1					//	Enable allocated memory initialisation

CHostMemoryManager	*CHostMemoryManager::s_pMemory = NULL;

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
	typedef struct DATA_BLOC_t
	{
        unsigned char   *address;
		size_t	        size;
		size_t			usedBytes;

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
	//!	A heap map by addresses
	map<void*,DATA_BLOC>	heap;
	//map<void*,DATA_BLOC>	deviceHeap;

    //! A storage for garbaged blocs. A set is used to be able
    //! to find the most appropriate bloc when needed
    //! ( the first having a size greater than the requested )
    set<DATA_BLOC,DATA_BLOC>	garbage;

	unsigned int	garbageSize;
	unsigned int	garbageMaxSize;

    CRaptorMutex     memoryMutex;
};


RAPTOR_NAMESPACE_END
//
///////////////////////////////////////////////////////////////////////////////////////////////////


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHostMemoryManager::CHostMemoryManager(void)
{
	m_pHeap = new CMemoryHeap;

	m_pHeap->garbageSize = 0;
	m_pHeap->garbageMaxSize = Global::GetInstance().getConfig().m_uiGarbageSize;
}

CHostMemoryManager::~CHostMemoryManager()
{
    //! Gather garbage blocs:
    set<CMemoryHeap::DATA_BLOC,CMemoryHeap::DATA_BLOC>::const_iterator itr = m_pHeap->garbage.begin();
    while (m_pHeap->garbage.end() != itr)
	{
		const CMemoryHeap::DATA_BLOC& db = (*itr++);
		release(db.address);
	}
	m_pHeap->garbage.clear();

	map<void*,CMemoryHeap::DATA_BLOC>::const_iterator itr2 = m_pHeap->heap.begin();
	while (m_pHeap->heap.end() != itr2)
	{
		const CMemoryHeap::DATA_BLOC& db = (*itr2++).second;
		release(db.address);
	}
	m_pHeap->heap.clear();

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

CHostMemoryManager *CHostMemoryManager::GetInstance(void)
{
	if (s_pMemory == NULL)
	{
		s_pMemory = new CHostMemoryManager;
	}

	return s_pMemory;
}

bool CHostMemoryManager::init(void)
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

void CHostMemoryManager::setGarbageMaxSize(unsigned int maxSize) const
{
	m_pHeap->garbageMaxSize = maxSize;
}

void *CHostMemoryManager::allocate(size_t size,unsigned int count,size_t alignment) const
{
    //! After this point, we are in critical section
    CRaptorLock lock(m_pHeap->memoryMutex);

    bool found = false;
	size_t requestedSize = size*count;
	
	set<CMemoryHeap::DATA_BLOC,CMemoryHeap::DATA_BLOC>::iterator itr = m_pHeap->garbage.begin();
	while ((!found) && (itr != m_pHeap->garbage.end()))
	{
        const CMemoryHeap::DATA_BLOC &db = (*itr);

		if (db.size >= requestedSize)
		{
			size_t treshold = requestedSize + WASTE_SPACE_TRESHOLD*db.size;
			if (db.size >= treshold)
				break;
			else
				found = true;
		}
		else
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
#ifdef INITIALISE_MEMORY
        memset(pT,0,size * count + align);
#endif
        *((unsigned char*)data-1) = 0xff & ((unsigned int)(data) - (unsigned int)(pT));

	    CMemoryHeap::DATA_BLOC db;
	    db.address = (unsigned char*)data;
	    db.size = requestedSize;
		db.usedBytes = requestedSize;

		m_pHeap->heap[db.address] = db;
        return data;
    }
    else
    {
        CMemoryHeap::DATA_BLOC db = (*itr);
		db.usedBytes = requestedSize;
		m_pHeap->heap[db.address] = db;
        m_pHeap->garbage.erase(itr);
		m_pHeap->garbageSize -= db.size;
#ifdef INITIALISE_MEMORY
        memset(db.address,0,db.size);
#endif
		
        return db.address;
    }
}


void CHostMemoryManager::release(void *data) const
{
    //! After this point, we are in critical section
    CRaptorLock lock(m_pHeap->memoryMutex);

    //! Search to validate the bloc is allocated with alignment and exists
	map<void*,CMemoryHeap::DATA_BLOC>::iterator itr = m_pHeap->heap.find(data);
	if (m_pHeap->heap.end() != itr)
		m_pHeap->heap.erase(itr);
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_WARNING,
                                                       "You are trying to free a block that was not properly allocated");
#endif
		return;
	}

	unsigned char *pT = (unsigned char*)data;

	// get allocation offset
	pT -= *(pT-1);

	// proceed to memory release
	delete [] pT;
}

void CHostMemoryManager::garbage(void *data) const
{
    //! After this point, we are in critical section
    CRaptorLock lock(m_pHeap->memoryMutex);

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	bool found = false;
#endif
    //! Search to validate the bloc is allocated with alignment and exists
	map<void*,CMemoryHeap::DATA_BLOC>::iterator itr = m_pHeap->heap.find(data);
	if (m_pHeap->heap.end() != itr)
	{
        CMemoryHeap::DATA_BLOC &db = (*itr).second;
        m_pHeap->garbage.insert(db);
		m_pHeap->garbageSize += db.size;
        m_pHeap->heap.erase(itr);
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		found = true;
#endif
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
