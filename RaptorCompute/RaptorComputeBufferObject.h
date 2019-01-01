#if !defined(AFX_RAPTOR_COMPUTE_BUFFER_OBJECT_H__6AB1B061_4A89_4F5C_BCFE_01C26304A434__INCLUDED_)
#define AFX_RAPTOR_COMPUTE_BUFFER_OBJECT_H__6AB1B061_4A89_4F5C_BCFE_01C26304A434__INCLUDED_


RAPTOR_NAMESPACE


class CRaptorComputeBufferObject : public CRaptorComputeMemory::IBufferObject
{
public:
	//!	Compute buffer constructor
	CRaptorComputeBufferObject() :m_size(0), address(NULL), m_storage(COMPUTE_BUFFER), m_granularity(0)
	{};

	//!	Local buffer constructor
	CRaptorComputeBufferObject(uint64_t sz)
		:m_size(sz),address(NULL),m_storage(LOCAL_BUFFER) {};

	//!	Destructor
	virtual ~CRaptorComputeBufferObject() {};

	//! Implements IDeviceMemoryManager::IBufferObject
	virtual uint64_t getSize(void) const
	{ return m_size; };

	//! Implements IDeviceMemoryManager::IBufferObject
	BUFFER_KIND getStorage(void) const
	{ return m_storage; }

	//! Implements IDeviceMemoryManager::IBufferObject
	uint32_t getBufferId(void) const
	{ return 0; }

	//! Implements IDeviceMemoryManager::IBufferObject
	virtual uint64_t getRelocationOffset(void) const
	{ return m_granularity;	}


	//!	Retrieve an existing buffer.
	//! @return NULL if buffer not found
	static const CRaptorComputeBufferObject* getBuffer(const CRaptorComputeMemory::IBufferObject *bo);

	//!	Add a compute buffer
	//! @return false if buffer already exists
	static bool addBuffer(const CRaptorComputeBufferObject* cb);

	//!	Remove a compute buffer
	//! @return false if buffer not found
	static bool removeBuffer(const CRaptorComputeMemory::IBufferObject *bo);

	//!	An opaque pointer to the data
	cl_mem		address;

    //! The size in bytes of the buffer object
	uint64_t	m_size;

	//! Indicates the data storage usage: vertex, pixels, ...
    BUFFER_KIND m_storage;

	//!	Memory granularity
	uint64_t	m_granularity;


private:
    CRaptorComputeBufferObject(const CRaptorComputeBufferObject& ) {};
	CRaptorComputeBufferObject& operator=(const CRaptorComputeBufferObject& ) {return *this; };

	//! Store a reverse mapping of buffer interfaces
	static std::map<const CRaptorComputeMemory::IBufferObject*,const CRaptorComputeBufferObject*> s_pBuffers;
};

#endif