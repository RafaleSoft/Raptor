// Memory.h: interface for the CHostMemoryManager class and IDeviceMemoryManager interface
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
#define AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CMemoryHeap;

class RAPTOR_API IDeviceMemoryManager
{
public:
	class RAPTOR_API IBufferObject
    {
    public:
		typedef unsigned int BUFFER_KIND;
        static const BUFFER_KIND VERTEX_BUFFER = 0;
        static const BUFFER_KIND INDEX_BUFFER = 1;
        static const BUFFER_KIND PIXEL_STORAGE = 2;
        static const BUFFER_KIND PIXEL_SOURCE = 3;
        static const BUFFER_KIND NB_BUFFER_KIND = 4;


        typedef enum
        {
            STATIC,
            STREAM,
            DYNAMIC
        } BUFFER_MODE;

		typedef union
		{
			void		*address;
			unsigned int id;
        } BUFFER_DATA;

		//! Returns the size of the buffer.
		virtual size_t getSize(void) const = 0;

		//! Returns the buffer kind.
		virtual BUFFER_KIND getStorage(void) const = 0;

		//!	Returns a valid address of the buffer or NULL
		virtual void* getBaseAddress(void) const = 0;

		//! Returns a valid buffer id of the buffer or 0
		virtual unsigned int getBufferId(void) const = 0;

	protected:
		IBufferObject() {};
		virtual ~IBufferObject() {};

	private:
        IBufferObject(const IBufferObject& );
		IBufferObject& operator=(const IBufferObject& );
	};

	virtual bool relocationAvailable(void) const = 0;

    //
    // Buffer Object Memory Management
    //

	//! This method creates a new buffer object :
    //! @param kind : selects a kind of buffer buffer ( vertex, pixel, memory ... )
    //! @param size : sets the size of the buffer and allocates uninitialized memory
    //! @return the newly allocated buffer object or NULL if allocation failed.
	virtual IDeviceMemoryManager::IBufferObject *
			createBufferObject(	IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind, 
								IDeviceMemoryManager::IBufferObject::BUFFER_MODE mode, 
								uint64_t size) = 0;

	//! Activates the buffer object : bo is now the currently selected buffer for
    //! all subsequent calls related to the kind of buffer
	virtual bool lockBufferObject(IBufferObject &bo) = 0;

	//! Deactivates the buffer object selected above.
	virtual bool unlockBufferObject(IDeviceMemoryManager::IBufferObject &bo) = 0;


	//!	Memory transfer method that should be used when copying data to and from a vertex buffer object.
	//! This method might need a valid OpenGL context if VBO are supported.
	//! @param bo		 : the vertex buffer to which data are copied
	//! @param dstOffset : the data offset within the destintation buffer ( vb )
	//! @param src		 : a pointer to the data source
	//! @param sz		 : the size of the data to be copied
	//! @return true if data correctly set to buffer.
	virtual bool setBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
										uint64_t dstOffset,
										const void* src,
										uint64_t sz) = 0;

	//!	Memory transfer method that should be used when copying data to and from a vertex buffer object.
	//! This method might need a valid OpenGL context if VBO are supported.
	//! @param bo		 : the vertex buffer from which data are read
	//! @param srcOffset : the data offset within the source buffer ( vb )
	//! @param dst		 : a pointer to the data destination
	//! @param sz		 : the size of the data to be copied
	//! @return true if data correctly retrieved from buffer.
	virtual bool getBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
										uint64_t srcOffset,
										void* dst,
										uint64_t sz) = 0;

	//!	This method releases a buffer object allocated with createBufferObject
	//!	If buffer is valid and no error found during release, the buffer object is deleted 
	//!	and parameter vb is set to NULL.
	//!	@param bo : a valid buffer object
	//!	@return false if buffer invalid or any error, true otherwise
	virtual bool releaseBufferObject(	IDeviceMemoryManager::IBufferObject* &bo) = 0;



protected:
	IDeviceMemoryManager() {};

private:
	IDeviceMemoryManager(const IDeviceMemoryManager&);
	IDeviceMemoryManager& operator=(const IDeviceMemoryManager&);
};





//! This class is dedicated to specific memory management for:
//!	- aligned data, which is essential for fast SIMD processing
//!	- GPU memory for fast rendering of primitives, avoiding useless data copy from client to server.
//! Aligned data should be used as frequently as possible to benefit using simd.lib
class RAPTOR_API CHostMemoryManager
{
public:
    //! A helper class to handle typed allocation in heap and reuse garbaged blocs.
    //! This helper is used because old platforms would not compile template member functions.
    //! The retreived block is finally cleared.
    template<class T,int a=16> class Allocator
    {
    public:
        Allocator() {};
        virtual ~Allocator() {};

        T* allocate(unsigned int count);
    };


public:
	virtual ~CHostMemoryManager();

	//!	Returns the unique instance of the memory manager.
	//!	During creation, internal functions such as new handler
	//! or specific GPU AGP memory allocators are NOT initialized.
	//! This object is created by Raptor during glInitRaptor
	static CHostMemoryManager *GetInstance(void);

	//!	Initialise memory management for Raptor.
	//!	User do not need to call this method, it is done by Raptor
	//! as soon as possible for internal management
	bool init(void);


    //
    // Conventional Memory Management
    //

	//!	Allocation method with aligned data
	//! allocate count chuncks of size bytes, aligned with alignment
	void *allocate(size_t size,unsigned int count,size_t alignment = 0) const;

	//!	Free aligned allocated memory only with Release method.
	//!	Do not Release memory not allocated with Allocate
	void release(void *data) const;

    //! Garbage for unsigned char [] blocs of pixels 
    //! Memory bloc must have been allocated with Allocate method
    void garbage(void *data) const;

	//!	Garbage maximum allowed size
	//!	( @see CRaptorConfig for initial value )
	void setGarbageMaxSize(unsigned int maxSize) const;


private:
    //! Forbidden members
	CHostMemoryManager(void);
    CHostMemoryManager(const CHostMemoryManager& ) {};
    CHostMemoryManager& operator=(const CHostMemoryManager& ) { return *this; };

    //! The only available instance
	static CHostMemoryManager	*s_pMemory;

    //! The heap managed
	CMemoryHeap*	m_pHeap;
};


template <class T,int a>
T* CHostMemoryManager::Allocator<T,a>::allocate(unsigned int count)
{
    void* bloc = CHostMemoryManager::GetInstance()->allocate(sizeof(T),count,a);
    return new(bloc) T;
}

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)

