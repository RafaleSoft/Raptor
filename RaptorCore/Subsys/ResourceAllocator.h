// ResourceAllocator.h: interface for the CResourceAllocator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_)
#define AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CResourceAllocator
{
public:
	CResourceAllocator();
	virtual ~CResourceAllocator();

	//! Returns the lock state ( set with the method below ).
	bool    isMemoryLocked(void) const { return m_bLocked; };


protected:
	typedef struct data_bloc_t
	{
		unsigned char	*address;
		uint64_t		size;
	} data_bloc;

	//!	The memory state
	bool    m_bLocked;

	//!	A host allocator
	CHostMemoryManager::Allocator<unsigned char> charAlloc;

	//!	Memory manager for the device hosting the display holding this allocator.
	//! (Vulkan host memory is per device)
	IDeviceMemoryManager	*deviceMemoryManager;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_)
