#if !defined(AFX_BUFFEROBJECT_H__FFF8C159_E959_496B_9962_0AF829E7FB4F__INCLUDED_)
#define AFX_BUFFEROBJECT_H__FFF8C159_E959_496B_9962_0AF829E7FB4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

RAPTOR_NAMESPACE_BEGIN


class CBufferObject : public IDeviceMemoryManager::IBufferObject
{
public:
	//!	OpenGL buffer objet constructor
	CBufferObject()
		:m_size(0),m_storage(NB_BUFFER_KIND),m_buffer(0) { };

	//!	Destructor
	virtual ~CBufferObject() {};


	virtual uint64_t getSize(void) const;

	virtual BUFFER_KIND getStorage(void) const;

	virtual unsigned int getBufferId(void) const;

	
	//! The size of the buffer object
	uint64_t		m_size;

	//! Indicates the data storage usage: vertex, pixels, ...
    BUFFER_KIND		m_storage;

	//!	Actual data
	unsigned int	m_buffer;
};

uint64_t CBufferObject::getSize(void) const
{
	return m_size;
}

inline IDeviceMemoryManager::IBufferObject::BUFFER_KIND CBufferObject::getStorage(void) const
{
	return m_storage;
}

inline unsigned int CBufferObject::getBufferId(void) const
{
	if (m_buffer & 1) return (m_buffer >> 16);
	else return 0;
}


RAPTOR_NAMESPACE_END


#endif // !defined(AFX_BUFFEROBJECT_H__FFF8C159_E959_496B_9962_0AF829E7FB4F__INCLUDED_)

