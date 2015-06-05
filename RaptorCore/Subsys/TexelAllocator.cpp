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
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

RAPTOR_NAMESPACE_BEGIN

CTexelAllocator	*CTexelAllocator::m_pInstance = NULL;

//  Add a constant offset to distinguish null ( unalocated ) pointers from actual memory blocs
static const int    RELOCATE_OFFSET = 16;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexelAllocator::CTexelAllocator()
	:m_bRelocated(false),m_bLocked(false),relocatedTexels(NULL)
{
	texels.address.uc_address = NULL;
	texels.size = 0;
}


CTexelAllocator::~CTexelAllocator()
{
	if (texels.address.uc_address != NULL)
		CMemory::GetInstance()->release(texels.address.uc_address);
	if (relocatedTexels != NULL)
		CMemory::GetInstance()->glReleaseBufferObject(relocatedTexels);
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



bool CTexelAllocator::glUseMemoryRelocation(void)
{
#if defined(GL_ARB_pixel_buffer_object)
	if (Raptor::glIsExtensionSupported("GL_ARB_pixel_buffer_object"))
	{
		m_bRelocated = texelBlocs.empty();

		return m_bRelocated;
	}
	else
		return false;
#else
	return false;
#endif
}


bool CTexelAllocator::glInitMemory(unsigned int texelSize)
{
    if (m_bLocked)
        return false;

	// Should there be several blocs ? No to be able to switch to relocatable blocs in AGP
	// ( it is much faster to have only one bufferobject bound for RT rendering )
	if ((texelSize == 0) || (!texelBlocs.empty()))
		return false;

    //  Allow user to allocate bloc size different from default.
    if (texels.address.uc_address != NULL)
    {
		CMemory::GetInstance()->release(texels.address.uc_address);
        texels.address.uc_address = NULL;
    }
    if (relocatedTexels != NULL)
        CMemory::GetInstance()->glReleaseBufferObject(relocatedTexels);

	if (m_bRelocated)
	{
		relocatedTexels = CMemory::GetInstance()->glAllocateBufferObject(CMemory::IBufferObject::PIXEL_SOURCE,
										                                 CMemory::IBufferObject::STREAM,
																		 texelSize+RELOCATE_OFFSET);

		texels.size = texelSize;

        CATCH_GL_ERROR

		return (relocatedTexels != NULL);
	}
	else
	{
        texels.address.uc_address = charAlloc.allocate(texelSize+RELOCATE_OFFSET);
		texels.size = texelSize;
		return (texels.address.uc_address != NULL);
	}
}


bool CTexelAllocator::glLockMemory(bool lock)
{
    bool res = true;

    if ((m_bRelocated) && (relocatedTexels != NULL))
    {
        if (lock && !m_bLocked)
        {
            CMemory::GetInstance()->glLockBufferObject(*relocatedTexels);
        }
        else if (!lock && m_bLocked)
        {
            CMemory::GetInstance()->glUnlockBufferObject(*relocatedTexels);
        }
        else
            res = false;
    }
 
    m_bLocked = lock;
    return res;
}


unsigned char*	const CTexelAllocator::allocateTexels(unsigned int size)
{
	if ((size == 0)  || (m_bLocked) || ((texels.address.uc_address == NULL) && (relocatedTexels == NULL)))
		return NULL;

	// be it relocated or not, texels can be the beginning or the memory block
	unsigned char *currentAddress = texels.address.uc_address;

	if (!texelBlocs.empty())
	{
		unsigned int maxSize = texels.size;
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
				unsigned char* addr = freeTexelBlocs[blocPos].address.uc_address;
				freeTexelBlocs.erase(freeTexelBlocs.begin() + blocPos);
				return addr;
			}
		}

		map<void*,data_bloc>::iterator it = texelBlocs.end();
		it--;
		currentAddress = (*it).second.address.uc_address + (*it).second.size;
	}

	if ( ((unsigned int)currentAddress - (unsigned int)texels.address.uc_address) + size > texels.size)
    {
		stringstream err;
		err << "Texel Allocator could not get enough memory:";
		err << " missing ";
		err << (((unsigned int)currentAddress - (unsigned int)texels.address.uc_address) + size) - texels.size;
		err << " bytes.";
		Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_FATAL,
														err.str().c_str());
		return NULL;
    }

    //  No NULL offset to distinguish nil pointers
    if ((m_bRelocated) && ( currentAddress == NULL))
        currentAddress = (unsigned char*)RELOCATE_OFFSET;

	//	Address should be aligned on a 16byte boundary
	data_bloc db;
	db.address.uc_address = (unsigned char*)(((unsigned int)(currentAddress) + 0x0f) & 0xfffffff0);
	db.size = size;
	texelBlocs[db.address.uc_address] = db;

	return db.address.uc_address;
}

float*	const CTexelAllocator::allocateFloatTexels(unsigned int size)
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
			db.address.uc_address = (unsigned char*)tex;
			db.size = (*blocPos).second.size;
			freeTexelBlocs.push_back(db);
		}
	}

	return res;
}


void CTexelAllocator::glCopyPointer(unsigned char *dst, unsigned char *src, unsigned int size)
{
	if ((!m_bRelocated) /*|| (m_bLocked)*/ || (src == NULL) || (dst == NULL))
        return;

	if (size == 0)
	{
		// find memory bloc and map a copy to local memory.
		map<void*,data_bloc>::const_iterator blocPos = texelBlocs.find(dst);
		if (blocPos != texelBlocs.end())
			CMemory::GetInstance()->glSetBufferObjectData(	*relocatedTexels,
															(unsigned int)dst,
															src,
															(*blocPos).second.size);
	}
	else
		CMemory::GetInstance()->glSetBufferObjectData(	*relocatedTexels,
														(unsigned int)dst,
														src,
														size);

	CATCH_GL_ERROR
}



void *CTexelAllocator::glMapPointer(void *pointer)
{
    if (!m_bRelocated || m_bLocked)
        return pointer;

    if ((relocatedTexels == NULL)  || (pointer == NULL))
        return NULL;

    // already mapped ?
    if (texelReMap.find(pointer) != texelReMap.end())
        return pointer;

    // find memory bloc and map a copy to local memory.
	map<void*,data_bloc>::const_iterator blocPos = texelBlocs.find(pointer);
	if (blocPos != texelBlocs.end())
    {
		unsigned int sz = (*blocPos).second.size;
		unsigned char* localData = charAlloc.allocate(sz);

        texelReMap[pointer] = localData;
        texelReMap[localData] = pointer;

        CMemory::GetInstance()->glGetBufferObjectData(	*relocatedTexels,
														(unsigned int)pointer,
														localData,
														sz);
        CATCH_GL_ERROR

        return localData;
    }
    else
        return NULL;
}

void *CTexelAllocator::glUnMapPointer(void *pointer)
{
    if (!m_bRelocated || m_bLocked)
        return pointer;

    if ((relocatedTexels == NULL) || (pointer == NULL))
        return NULL;

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

		// Here, serverData could be relocated to compress the data
		// and limit the number of holes or fragmentation.
		// As we have an array of free blocs, relocation could easily be done.
        CMemory::GetInstance()->glSetBufferObjectData(	*relocatedTexels,
														(unsigned int)serverData,
														pointer,
														(*blocPos).second.size);

        CMemory::GetInstance()->garbage(pointer);

        CATCH_GL_ERROR

        return serverData;
    }
    else
        return NULL;
}

