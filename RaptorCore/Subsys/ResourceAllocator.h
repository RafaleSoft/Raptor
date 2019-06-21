/***************************************************************************/
/*                                                                         */
/*  ResourceAllocator.h                                                    */
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


#if !defined(AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_)
#define AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
	#include "System/RaptorDisplayConfig.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif



RAPTOR_NAMESPACE_BEGIN

class CResourceAllocator
{
public:
	class CResourceBinder
	{
	public:
		CResourceBinder(void);
		~CResourceBinder(void);

		bool setVertexArray(void *vertexPointer, int stride);

		bool glvkBindArrays(void);
		bool glvkUnbindArrays(void);

	private:
		CResourceBinder(const CResourceBinder&);
		CResourceBinder& operator=(const CResourceBinder&);

		static CRaptorDisplayConfig::GL_ARRAYS_STATE	bindingState;
	};

public:
	CResourceAllocator();
	virtual ~CResourceAllocator();

	//! Returns the lock state ( set with the method below ).
	bool    isMemoryLocked(void) const { return m_bLocked; };


protected:
	typedef struct data_bloc_t
	{
		uint8_t		*address;
		uint64_t	size;
	} data_bloc;

	//!	The memory state
	bool    m_bLocked;

	//!	A host allocator
	CHostMemoryManager::Allocator<uint8_t> charAlloc;

	//!	Memory manager for the device hosting the display holding this allocator.
	//! (Vulkan host memory is per device)
	IDeviceMemoryManager	*deviceMemoryManager;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_)
