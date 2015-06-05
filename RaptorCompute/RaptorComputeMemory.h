#if !defined(AFX_RAPTOR_COMPUTE_MEMORY_H__525371EA_0090_40AA_9889_333824045A7A__INCLUDED_)
#define AFX_RAPTOR_COMPUTE_MEMORY_H__525371EA_0090_40AA_9889_333824045A7A__INCLUDED_

#include <System/Memory.h>

RAPTOR_NAMESPACE


class RAPTORCOMPUTE_API CRaptorComputeMemory
{
public:
	//	TODO: Use / Inherit Raptor::CMemory::CBufferObject
	class RAPTORCOMPUTE_API CBufferObject
    {
    public:
        typedef enum
        {
			COMPUTE_BUFFER,
			LOCAL_BUFFER,
			IMAGE_BUFFER,
            NB_BUFFER_KIND
        }   BUFFER_KIND; 

		//!	Local buffer constructor
		CBufferObject(size_t sz)
			:m_size(sz),address(NULL),m_storage(LOCAL_BUFFER),m_isInterop(false) {};

		void *data(void) const 
		{ return address; };

		size_t getSize(void) const
		{ return m_size; };

		BUFFER_KIND getStorage(void) const
		{ return m_storage; }

		bool isInterop(void) const
		{ return m_isInterop; }

    private:
        friend class CRaptorComputeMemory;
        CBufferObject():m_size(0),address(NULL),m_storage(COMPUTE_BUFFER),m_isInterop(false) {};
        ~CBufferObject() {};
        CBufferObject(const CBufferObject& ) {};
		CBufferObject& operator=(const CBufferObject& ) {return *this; };

		//!	An opaque pointer to the data
		void		*address;

        //! The size in bytes of the buffer object
		size_t		m_size;

		//! Indicates the data storage usage: vertex, pixels, ...
        BUFFER_KIND m_storage;

		//! Indicates that the buffer is linked to a GL buffer
		bool		m_isInterop;
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
    CRaptorComputeMemory::CBufferObject* clCreateBuffer(size_t size,
														void *initialData = NULL,
														CRaptorComputeMemory::CBufferObject::BUFFER_KIND kind = CRaptorComputeMemory::CBufferObject::COMPUTE_BUFFER);

	//! This method creates a new buffer object wrapping a GL buffer.
	//! @param glBuffer : a Raptor Core GL buffer
    //! @return the newly allocated buffer object or NULL if allocation failed.
    CRaptorComputeMemory::CBufferObject* clCreateBuffer(CMemory::IBufferObject *glBuffer);


	//! This method deletes a buffer object allocate with cl
	//! @param bo : a buffer object created with the method above
	//! @return false if buffer invalid or device error, true otherwise
	//! Rq: the object pointed by bo is deleted, and bo set to NULL.
	bool clDestroyBuffer(CRaptorComputeMemory::CBufferObject *&bo);


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

