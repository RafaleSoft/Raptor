// TexelAllocator.h: interface for the CTexelAllocator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
#define AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CTexelAllocator
{
public:
	//!	Singleton access : returns the current instance or create a new instance if there is no current.
	static CTexelAllocator*	GetInstance(void);

    //! Set the current instance.
    //! @return : the previous value.
    static CTexelAllocator *SetCurrentInstance(CTexelAllocator* texelAllocator);

    //! Destructor destroy all memory blocs created by this instance.
	virtual ~CTexelAllocator();

	//!	Initialize memory blocks
	bool	glInitMemory(unsigned int texelSize);

	//!	Indicates if relocation is possible
	bool	glUseMemoryRelocation(void);

    //! Returns the relocate state ( set with the method here above ).
    bool    isMemoryRelocated(void) const { return m_bRelocated; };

    //! Lock memory data and relocation so that no change can be made.
    //! If data is relocated, High Performance blocks are activated on server
    bool    glLockMemory(bool lock);

	//!	Returns the locking status of the data.
	bool	isMemoryLocked(void) const { return m_bLocked; };


	//!	This method returns the address of a free block of the requested size, ( nb of texels )
	//!	or NULL if not enough space or other error.
	unsigned char*	const allocateTexels(unsigned int size);

	//!	This method returns the address of a free block of the requested size, ( nb of texels )
	//!	or NULL if not enough space or other error.
	float*	const allocateFloatTexels(unsigned int size);

	//!	Release the block allocated here above.
	//! @return: false if block not found or if error, true otherwise
	bool releaseTexels(void *tex);

	//! These methods allow data transfer to a relocated block when mapping/unmapping 
    //! is not necessary ( i.e. only data copy )
	//!	@param dst : the pointer returned by a previous call to allocateIndexes
	//!	@param src : the conventional memory pointer to data to copy to GPU memory
	//!	@param size : the size of data to be copied that should always be less than the allocated size.
	//!	If size is 0, the dst memory size is recomputed, otherwise, size floats are copied.
	//!	Rq: No other testing are performed !
    void glCopyPointer(unsigned char *dst, unsigned char *src, unsigned int size = 0);

	void *glMapPointer(void *pointer);
	void *glUnMapPointer(void *pointer);


private:
	//!	Singleton constructor is not allowed
	CTexelAllocator();
    CTexelAllocator(const CTexelAllocator&);
    CTexelAllocator& operator=(const CTexelAllocator&);

	//!	The unique allocator instance
	static CTexelAllocator	*m_pInstance;

	//!	The memory state
	bool	m_bRelocated;
    bool    m_bLocked;

	CMemory::Allocator<unsigned char> charAlloc;

	typedef struct data_bloc_t
	{
		union
		{
			unsigned char	*uc_address;
			float			*f_address;
		} address;
		unsigned int	size;
	} data_bloc;


	//!	Global array for texel allocation when GPU relocation is not available
	data_bloc	texels;

	//!	If relocated, High Performance buffer object
	CMemory::CBufferObject *relocatedTexels;

	//! Actual memory structure : bloc fragments of global allocated space
	//!	IMPORTANT: The structure implementation requires a binary tree for template class map<>
	map<void*,data_bloc>	texelBlocs;

	//! Free blocs for faster reallocation ( blocs are contained in actual memory structure )
	vector<data_bloc>	freeTexelBlocs;

	//!	A helper to find mapped memory blocs
	map<void*,void*>  texelReMap;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)

