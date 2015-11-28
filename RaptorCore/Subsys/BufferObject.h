#if !defined(AFX_BUFFEROBJECT_H__FFF8C159_E959_496B_9962_0AF829E7FB4F__INCLUDED_)
#define AFX_BUFFEROBJECT_H__FFF8C159_E959_496B_9962_0AF829E7FB4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

RAPTOR_NAMESPACE_BEGIN


class CBufferObject : public CMemory::IBufferObject
{
public:
	CBufferObject():m_size(0),m_storage(NB_BUFFER_KIND)
	{ m_buffer.id = 0; };

	virtual size_t getSize(void) const;

	virtual BUFFER_KIND getStorage(void) const;

	virtual void* getBaseAddress(void) const;

	virtual unsigned int getBufferId(void) const;

	virtual ~CBufferObject() {};

	//! The size of the buffer object
	unsigned int	m_size;

	//! Indicates the data storage usage: vertex, pixels, ...
    BUFFER_KIND		m_storage;

	//!	Actual data
	BUFFER_DATA		m_buffer;
};

size_t CBufferObject::getSize(void) const
{
	return m_size;
}

inline CMemory::IBufferObject::BUFFER_KIND CBufferObject::getStorage(void) const
{
	return m_storage;
}

inline void* CBufferObject::getBaseAddress(void) const
{
	if (m_buffer.id & 1) return NULL;
	else return m_buffer.address;
}

inline unsigned int CBufferObject::getBufferId(void) const
{
	if (m_buffer.id & 1) return (m_buffer.id >> 16);
	else return 0;
}


RAPTOR_NAMESPACE_END


#endif // !defined(AFX_BUFFEROBJECT_H__FFF8C159_E959_496B_9962_0AF829E7FB4F__INCLUDED_)

