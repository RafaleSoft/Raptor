#if !defined(AFX_RAPTOR_COMPUTE_MEMORY_H__525371EA_0090_40AA_9889_333824045A7A__INCLUDED_)
#define AFX_RAPTOR_COMPUTE_MEMORY_H__525371EA_0090_40AA_9889_333824045A7A__INCLUDED_

#include <System/Memory.h>

RAPTOR_NAMESPACE


class RAPTORCOMPUTE_API CRaptorComputeMemory
{
public:
	//	TODO: Use / Inherit Raptor::CMemory::CBufferObject
	class RAPTORCOMPUTE_API IBufferObject : public CMemory::IBufferObject
    {
    public:
        static const BUFFER_KIND COMPUTE_BUFFER = 4;
		static const BUFFER_KIND INTEROP_COMPUTE_BUFFER = 5;
		static const BUFFER_KIND LOCAL_BUFFER = 6;
		static const BUFFER_KIND IMAGE_BUFFER = 7;
        static const BUFFER_KIND NB_BUFFER_KIND = 8;

    protected:
        IBufferObject() {};
        virtual ~IBufferObject() {};

	private:
        IBufferObject(const IBufferObject& ) {};
		IBufferObject& operator=(const IBufferObject& ) {return *this; };
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
    CRaptorComputeMemory::IBufferObject* clCreateBuffer(CMemory::IBufferObject *glBuffer);


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
	unsigned __int64	maxSizes;
	__int64				maxAllocatable;
};

#endif

