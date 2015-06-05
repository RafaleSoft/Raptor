// Memory.h: interface for the CMemory class.
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

//! This class is dedicated to specific memory management for:
//!	- aligned data, which is essential for fast SIMD processing
//!	- GPU memory for fast rendering of primitives, avoiding useless data copy from client to server.
//! Aligned data should be used as frequently as possible to benefit using simd.lib
class RAPTOR_API CMemory
{
public:
	class RAPTOR_API IBufferObject
    {
    public:
        typedef enum
        {
            VERTEX_BUFFER,
            INDEX_BUFFER,
            PIXEL_STORAGE,
            PIXEL_SOURCE,
            NB_BUFFER_KIND
        }   BUFFER_KIND;

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
		//! ( in the case of NULL it is also the offset from
		//! the currently bound VBO base address )
		virtual void* getBaseAddress(void) const = 0;

		//! Returns a valid buffer id of the buffer of 0
		virtual unsigned int getBufferId(void) const = 0;

	protected:
		IBufferObject() {};
		virtual ~IBufferObject() {};

	private:
        IBufferObject(const IBufferObject& ) {};
		IBufferObject& operator=(const IBufferObject& ) { return *this; };
	};


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
	virtual ~CMemory();

	//!	Returns the unique instance of the memory manager.
	//!	During creation, internal functions such as new handler
	//! or specific GPU AGP memory allocators are NOT initialized.
	//! This object is created by Raptor during glInitRaptor
	static CMemory *GetInstance(void);

	//!	Initialise memory management for Raptor.
	//!	User do not need to call this method, it is done by Raptor
	//! as soon as possible for internal management
	bool init(void);


    //
    // Conventional Memory Management
    //

	//!	Allocation method with aligned data
	//! allocate count chuncks of size bytes, aligned with alignment
	void *allocate(size_t size,unsigned int count,unsigned char alignment = 0) const;

	//!	Free aligned allocated memory only with Release method.
	//!	Do not Release memory not allocated with Allocate
	void release(void *data) const;

    //! Garbage for unsigned char [] blocs of pixels 
    //! Memory bloc must have been allocated with Allocate method
    void garbage(void *data) const;

	//!	Garbage maximum allowed size
	//!	( @see CRaptorConfig for initial value )
	void setGarbageMaxSize(unsigned int maxSize) const;




    //
    // Buffer Object Memory Management
    //

	//!	Memory transfer method that should be used when copying data to and from a vertex buffer object.
	//! This method might need a valid OpenGL context if VBO are supported.
	//! @param vb		 : the vertex buffer to which data are copied
	//! @param dstOffset : the data offset within the destintation buffer ( vb )
	//! @param src		 : a pointer to the data source
	//! @param sz		 : the size of the data to be copied
	void glSetBufferObjectData(IBufferObject &vb,unsigned int dstOffset,const void* src,size_t sz);

    //!	Memory transfer method that should be used when copying data to and from a vertex buffer object.
	//! This method might need a valid OpenGL context if VBO are supported.
	//! @param vb		 : the vertex buffer from which data are read
	//! @param srcOffset : the data offset within the source buffer ( vb )
	//! @param dst		 : a pointer to the data destination
	//! @param sz		 : the size of the data to be copied
	void glGetBufferObjectData(IBufferObject &vb,unsigned int srcOffset,void* dst,size_t sz);

    //! This method creates a new buffer object :
    //! @param kind : selects a kind of buffer buffer ( vertex, pixel, memory ... )
    //! @param size : sets the size of the buffer and allocates uninitialized memory
    //! @return the newly allocated buffer object or NULL if allocation failed.
    IBufferObject * glAllocateBufferObject( IBufferObject::BUFFER_KIND kind,
                                            IBufferObject::BUFFER_MODE mode,
                                            size_t size);


	//!	This method releases a buffer object allocated with glAllocateBufferObject
	//!	If buffer is valid and no error found during release, the buffer object is deleted 
	//!	and parameter vb is set to NULL.
	//!	@param vb : a valid buffer object
	//!	@return false if buffer invalid or any error, true otherwise
	bool glReleaseBufferObject(IBufferObject* &vb);

    //! Activates the buffer object : vb is now the currently selected buffer for
    //! all subsequent calls related to the kind of buffer
	bool glLockBufferObject(IBufferObject &vb);

    //! Deactivates the buffer object selected above.
	bool glUnlockBufferObject(IBufferObject &vb);



private:
    //! Forbidden members
	CMemory(void);
    CMemory(const CMemory& ) {};
    CMemory& operator=(const CMemory& ) { return *this; };

    //! The only available instance
	static CMemory	*s_pMemory;

    //! The heap managed
	CMemoryHeap*	m_pHeap;
};


template <class T,int a>
T* CMemory::Allocator<T,a>::allocate(unsigned int count)
{
    T* bloc = (T*)(CMemory::GetInstance()->allocate(sizeof(T),count,a));
    return bloc;
}

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)

