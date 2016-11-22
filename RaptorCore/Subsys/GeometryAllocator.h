// GeometryAllocator.h: interface for the CGeometryAllocator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
#define AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CGeometryAllocator  
{
public:
	//!	Singleton access : returns the current instance or create a new instance if there is no current.
	static CGeometryAllocator	*GetInstance(void);

	//! Set the current instance.
	//! @return : the previous value.
	static CGeometryAllocator *SetCurrentInstance(CGeometryAllocator* geometryAllocator);

	//! Destructor destroy all memory blocs created by this instance.
	virtual ~CGeometryAllocator();

	
	//!	Initialize memory blocks
	bool	glvkInitMemory(	IDeviceMemoryManager* pDeviceMemory,
							uint64_t indexSize,uint64_t coordsSize);

	//! Returns the relocate state ( set with the method here above ).
	bool    isMemoryRelocated(void) const
	{ return ((NULL != deviceMemoryManager) && (NULL != relocatedFaceIndexes) && (NULL != relocatedVertices)); };

	//! Returns the lock state ( set with the method below ).
	bool    isMemoryLocked(void) const { return m_bLocked; };

	//! Lock memory data and relocation so that no change can be made.
	//! If data is relocated, High Performance memory blocks are activated on server
	bool    glvkLockMemory(bool lock);

	//! These methods are used to map pointers from GPU memory to client classic memory.
	//! Returns: 
	//!     - NULL if mapping fails.
	//!     - pointer if already mapped ( or already unmapped )
	//!     - a new pointer otherwise, which should replace pointer for subsequent calls
	//!		- discarding a pointer retrive the mapped pointer, but the data pointed 
	//!			is discarded without copy.
	unsigned short *glvkMapPointer(unsigned short *pointer);
	unsigned short *glvkUnMapPointer(unsigned short *pointer);
	unsigned short *glDiscardPointer(unsigned short *pointer);
	float *glvkMapPointer(float *pointer);
	float *glvkUnMapPointer(float *pointer);
	float *glDiscardPointer(float *pointer);
	

	//! These methods allow data transfer to a relocated block when mapping/unmapping 
	//! is not necessary ( i.e. only data copy )
	//!	@param dst : the pointer returned by a previous call to allocateIndexes
	//!	@param src : the conventional (host) memory pointer to data to copy to GPU memory
	//!	@param size : the size of data to be copied that should always be less than the allocated size.
	//!	If size is 0, the dst memory size is recomputed, otherwise, size floats are copied.
	//!	Rq: No other testing are performed ! ( no ckech is done to validate that dst is a bloc of size 'size' )
	void glvkCopyPointer(float *dst, float *src, uint64_t size = 0);
	void glvkCopyPointer(unsigned short *dst, unsigned short *src, uint64_t size = 0);
	

	//!	This method returns the address of a free block of the requested size, ( nb of indexes )
	//!	or NULL if not enough space or other error.
	unsigned short	* const	allocateIndexes(unsigned short size);

	//!	Release the block allocated here above.
	//! Returns false if block not found or if error.
	bool	releaseIndexes(unsigned short *index);

	//!	This method returns the address of a free block of the requested size, ( nb of indexes )
	//!	or NULL if not enough space or other error.
	float	* const	allocateVertices(uint64_t size);

	//!	Release the block allocated here above.
	//! Returns false if block not found or if error.
	bool	releaseVertices(float *index);



private:
	//!	Singleton constructor is not allowed
	CGeometryAllocator();
    CGeometryAllocator(const CGeometryAllocator&);
    CGeometryAllocator& operator=(const CGeometryAllocator&);

	//!	The unique allocator instance
	static CGeometryAllocator	*m_pInstance;

    CHostMemoryManager::Allocator<unsigned short> shortAlloc;
    CHostMemoryManager::Allocator<float> floatAlloc;

	//!	The memory state
    bool    m_bLocked;

	//!	Structure for memory blocs
	typedef struct data_bloc_t
	{
		//	bloc address
		union
		{
			unsigned short *us_address;
			float *f_address;
		} address;
		//	bloc size in bytes
		unsigned int size;
	} data_bloc;


	//!	Global array for geometry face indexes allocation.
	data_bloc	faceIndexes;

	//!	Global array for geometry coordinates allocation.
	data_bloc	vertices;

	//!	If relocated, High Performance buffer object
	IDeviceMemoryManager::IBufferObject *relocatedFaceIndexes;
	IDeviceMemoryManager::IBufferObject *relocatedVertices;

	//!	Memory manager for the device hosting the display holding this allocator.
	//! (Vulkan host memory is per device)
	IDeviceMemoryManager	*deviceMemoryManager;

	//! Actual memory structure : bloc fragments of global allocated space
	//!	IMPORTANT: The structure implementation requires a binary tree for template class map<>
	map<unsigned short*,unsigned int>	indexBlocs;
	map<float*,unsigned int>			vertexBlocs;

	//! Free blocs for faster reallocation ( blocs are contained in actual memory structure )
	vector<data_bloc>	freeIndexBlocs;
	vector<data_bloc>	freeVertexBlocs;

    //!  memory mappers from local memory to server memory
    map<unsigned short*,unsigned short*>    indexReMap;
    map<float*,float*>  vertexReMap;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)

