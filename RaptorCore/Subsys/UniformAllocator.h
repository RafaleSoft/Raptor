/***************************************************************************/
/*                                                                         */
/*  UniformAllocator.h                                                     */
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


#if !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)
#define AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_)
	#include "ResourceAllocator.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CUniformAllocator : public CResourceAllocator
{
public:
	//!	Singleton access : returns the current instance or create a new instance if there is no current.
	static CUniformAllocator*	GetInstance(void);

	//! Set the current instance.
	//!	@param uniformAllocator : the instance that becomes current.
	//! @return : the previous current instance, or NULL if none.
	static CUniformAllocator *SetCurrentInstance(CUniformAllocator* uniformAllocator);

	//! Destructor destroy all memory blocs created by this instance.
	virtual ~CUniformAllocator();

	//!	Initialize memory blocks
	bool	glvkInitMemory(	IDeviceMemoryManager* pDeviceMemory,
							uint64_t uniformSize);
	
	void glvkCopyPointer(uint8_t *dst, uint8_t *src, uint64_t size);

	//! Lock memory data and relocation so that no change can be made.
	//! If data is relocated, High Performance blocks are activated on server
	bool    glvkLockMemory(bool lock);

	//! Binds a uniform memory data block and relocation so that no change can be made.
	//! @param uniform : the uniform buffer object pervously allocated with allocateUniforms.
	//! @index uniform : the uniform binding location to bind uniform buffer to.
	//! @return true if binding is effective, false otherwise (ResourceAllocator must be locked)
	bool    glvkBindUniform(uint8_t *uniform, int32_t index);

	//!	This method returns the address of a free block of the requested size, ( nb of indexes )
	//!	or NULL if not enough space or other error.
	uint8_t * const allocateUniforms(uint64_t size);
	
	//!	Release the block allocated here above.
	//! Returns false if block not found or if error.
	bool releaseUniforms(uint8_t *uniform);


private:
	//!	Singleton constructor is not allowed
	CUniformAllocator();
	CUniformAllocator(const CUniformAllocator&);
	CUniformAllocator& operator=(const CUniformAllocator&);

	//!	The unique allocator instance
	static CUniformAllocator	*m_pInstance;
	
	//!	Global array for uniforms allocation when GPU relocation is not available
	data_bloc	uniforms;

	//!	If relocated, High Performance buffer object
	IDeviceMemoryManager::IBufferObject *relocatedUniforms;

	//! Actual memory structure : bloc fragments of global allocated space
	//!	IMPORTANT: The structure implementation requires a binary tree for template class map<>
	map<uint8_t*, uint64_t>	uniformBlocs;

	//! Free blocs for faster reallocation ( blocs are contained in actual memory structure )
	vector<data_bloc>	freeUniformBlocs;

	//!	A helper to find mapped memory blocs
	map<uint8_t*, uint8_t*>  uniformReMap;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)

