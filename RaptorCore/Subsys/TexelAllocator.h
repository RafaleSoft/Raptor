/***************************************************************************/
/*                                                                         */
/*  TexelAllocator.h                                                       */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
#define AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(__RAPTOR_VKEXT_H__)
	#include "System/vkext.h"
#endif
#if !defined(AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_)
	#include "ResourceAllocator.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTexelAllocator : public CResourceAllocator
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
	bool	glvkInitMemory(	IDeviceMemoryManager* pDeviceMemory,
							uint64_t texelSize);

    //! Returns the relocate state ( set with the method here above ).
    bool    isMemoryRelocated(void) const
	{ return ((NULL != deviceMemoryManager) && (relocatedTexels != NULL)); };

    //! Lock memory data and relocation so that no change can be made.
    //! If data is relocated, High Performance blocks are activated on server
    bool    glvkLockMemory(bool lock);

	//!	This method returns the address of a free block of the requested size, ( nb of texels )
	//!	or NULL if not enough space or other error.
	unsigned char*	const allocateTexels(uint64_t size);

	//!	This method returns the address of a free block of the requested size, ( nb of texels )
	//!	or NULL if not enough space or other error.
	float*	const allocateFloatTexels(uint64_t size);

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
    void glvkSetPointerData(unsigned char *dst, unsigned char *src, uint64_t size = 0);

	void *glvkMapPointer(void *pointer,bool syncData = true);
	void *glvkUnMapPointer(void *pointer,bool syncData = true);

	//!	Create a Vulkan image and allocate device memory
	VkImage vkAllocateTextureImage(VkImageCreateInfo imageInfo,
								   VkDeviceSize offset = 0);

private:
	//!	Singleton constructor is not allowed
	CTexelAllocator();
    CTexelAllocator(const CTexelAllocator&);
    CTexelAllocator& operator=(const CTexelAllocator&);

	//!	The unique allocator instance
	static CTexelAllocator	*m_pInstance;

	//!	Global array for texel allocation when GPU relocation is not available
	data_bloc	texels;

	//!	If relocated, High Performance buffer object
	IDeviceMemoryManager::IBufferObject *relocatedTexels;

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

