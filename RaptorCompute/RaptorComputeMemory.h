/***************************************************************************/
/*                                                                         */
/*  RaptorComputeMemory.h                                                  */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_RAPTOR_COMPUTE_MEMORY_H__525371EA_0090_40AA_9889_333824045A7A__INCLUDED_)
#define AFX_RAPTOR_COMPUTE_MEMORY_H__525371EA_0090_40AA_9889_333824045A7A__INCLUDED_

#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include <System/Memory.h>
#endif

RAPTOR_NAMESPACE


class RAPTOR_API CRaptorComputeMemory
{
public:
	class RAPTOR_API IBufferObject : public IDeviceMemoryManager::IBufferObject
    {
    public:
		static const BUFFER_KIND COMPUTE_BUFFER = IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND;				// 5
		static const BUFFER_KIND INTEROP_COMPUTE_BUFFER = IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND + 1;	// 6
		static const BUFFER_KIND LOCAL_BUFFER = IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND + 2;			// 7
		static const BUFFER_KIND IMAGE_BUFFER = IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND + 3;			// 8
		static const BUFFER_KIND NB_BUFFER_KIND = IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND + 4;			// 9

    protected:
        IBufferObject() {};
        virtual ~IBufferObject() {};

	private:
        IBufferObject(const IBufferObject& );
		IBufferObject& operator=(const IBufferObject& );
	};

	//!	Single instance accessor.
	//!	@param numPlatform : the platform onto which the instance will allocate a buffer
	//!	@param numDevice : the device onto which the instance will allocate a buffer
	static CRaptorComputeMemory	&GetInstance(	unsigned int numPlatform,
												unsigned int numDevice);

	//! This method creates a new buffer object :
    //! @param kind : selects a kind of buffer buffer ( vertex, pixel, memory ... )
    //! @param size : sets the size in bytes of the buffer and allocates uninitialized memory
	//! @param initialData : a pointer to data to initialize the buffer if necessary.
    //! @return the newly allocated buffer object or NULL if allocation failed.
    CRaptorComputeMemory::IBufferObject* clCreateBuffer(size_t size,
														void *initialData = NULL,
														CRaptorComputeMemory::IBufferObject::BUFFER_KIND kind = CRaptorComputeMemory::IBufferObject::COMPUTE_BUFFER);

	//! This method creates a new buffer object wrapping a GL buffer.
	//! @param glBuffer : a Raptor Core GL buffer
    //! @return the newly allocated buffer object or NULL if allocation failed.
    CRaptorComputeMemory::IBufferObject* clCreateBuffer(IDeviceMemoryManager::IBufferObject *glBuffer);


	//! This method deletes a buffer object allocate with cl
	//! @param bo : a buffer object created with the method above
	//! @return false if buffer invalid or device error, true otherwise
	//! Rq: the object pointed by bo is deleted, and bo set to NULL.
	bool clDestroyBuffer(CRaptorComputeMemory::IBufferObject *&bo);


private:
	CRaptorComputeMemory(unsigned int numPlatform,unsigned int numDevice);
	~CRaptorComputeMemory(void);

	//! The unique instances of the global manager (per device)
	static map<unsigned int,CRaptorComputeMemory*>	s_pMemories;

	//!	The unique ID.
	unsigned int mID;

	//!	Max memory sizes
	uint64_t	maxSizes;
	int64_t		maxAllocatable;
};

#endif

