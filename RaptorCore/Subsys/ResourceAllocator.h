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
#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
	#include "GLHierarchy/ProgramParameters.h"
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

		//!	Defines array binding data.
		bool useVertexArrayObjects(void);

		bool setArray(CProgramParameters::GL_VERTEX_ATTRIB attribute, 
					  void *vertexPointer, 
					  size_t size = 4,
					  size_t stride = 0);

		//!	Bind arrays to current state for rendering.
		bool glvkBindArrays(void);
		//!	Revert to unbound state (default array binding).
		bool glvkUnbindArrays(void);

		//! Init arrays binding data and state with current state.
		bool glScanBindings(void);


	private:
		CResourceBinder(const CResourceBinder&);
		CResourceBinder& operator=(const CResourceBinder&);

		//! Bind a single array.
		bool bindArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state,
					   CRaptorDisplayConfig::GL_ARRAY_STATE &global_state);
		bool bindAttribArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state);

		//! Unbind a single array.
		bool unbindArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state,
						 CRaptorDisplayConfig::GL_ARRAY_STATE &global_state);
		bool unbindAttribArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state);

		//!	This resource binder arrays bindings.
		CRaptorDisplayConfig::GL_ARRAYS_STATE			bindings;

		//!	Vertex Array Object.
		GLuint	array;
		bool	updateArray;
		bool	legacy;
		bool	vao;
	};


public:
	virtual ~CResourceAllocator();

	//! Returns the lock state ( set with the method below ).
	bool    isMemoryLocked(void) const { return m_bLocked; };


protected:
	typedef struct data_bloc_t
	{
		uint8_t		*address;
		uint64_t	size;
	} data_bloc;


	//!	The constructor, for subclasses (Geometry, Texels, Uniforms).
	CResourceAllocator();

	//!	The memory state
	bool    m_bLocked;

	//!	A host allocator
	CHostMemoryManager::Allocator<uint8_t> charAlloc;

	//!	Memory manager for the device hosting the display holding this allocator.
	//! (Vulkan host memory is per device)
	IDeviceMemoryManager	*deviceMemoryManager;


private:
	CResourceAllocator(const CResourceAllocator &);
	CResourceAllocator& operator = (const CResourceAllocator&);
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_)
