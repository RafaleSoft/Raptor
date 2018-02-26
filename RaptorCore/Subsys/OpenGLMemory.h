// OpenGLMemory.h: interface for the COpenGLMemory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLMEMORY_H__C344567B_877F_4F43_8961_C4E59E3BBF7E__INCLUDED_)
#define AFX_OPENGLMEMORY_H__C344567B_877F_4F43_8961_C4E59E3BBF7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CBufferObject;
class COpenGLMemory : public IDeviceMemoryManager
{
public:
	COpenGLMemory(void);
	virtual ~COpenGLMemory(void);

	virtual bool relocationAvailable(void) const;

	virtual IDeviceMemoryManager::IBufferObject *
			createBufferObject(	IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind, 
								IDeviceMemoryManager::IBufferObject::BUFFER_MODE mode, 
								uint64_t size);
	virtual bool lockBufferObject(IDeviceMemoryManager::IBufferObject &bo);
	virtual bool unlockBufferObject(IDeviceMemoryManager::IBufferObject &bo);
	virtual bool setBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
										uint64_t dstOffset,
										const void* src,
										uint64_t sz);
	virtual bool getBufferObjectData(	IDeviceMemoryManager::IBufferObject &vb,
										uint64_t srcOffset,
										void* dst,
										uint64_t sz);
	virtual bool discardBufferObjectData(	IDeviceMemoryManager::IBufferObject &bo,
											uint64_t dstOffset,
											uint64_t sz);
	virtual bool releaseBufferObject(IDeviceMemoryManager::IBufferObject* &vb);


private:
	COpenGLMemory(const COpenGLMemory&);
	COpenGLMemory& operator=(const COpenGLMemory&);

	GLenum  BufferKindToGL(IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind) const;
	GLenum  BufferModeToGL(	IDeviceMemoryManager::IBufferObject::BUFFER_KIND kind,
							IDeviceMemoryManager::IBufferObject::BUFFER_MODE mode) const;

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	bool isBufferObjectValid(unsigned int buffer) const;
#endif

	unsigned int currentBuffers[IDeviceMemoryManager::IBufferObject::NB_BUFFER_KIND];
};



RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OPENGLMEMORY_H__C344567B_877F_4F43_8961_C4E59E3BBF7E__INCLUDED_)

