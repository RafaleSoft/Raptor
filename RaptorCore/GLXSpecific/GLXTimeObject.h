// Win32AnimatorStream.h: interface for the CWin32AnimatorStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLXTIMEOBJECT_H__3079A145_D92D_45B8_BF7A_19FD1261159D__INCLUDED_)
#define AFX_GLXTIMEOBJECT_H__3079A145_D92D_45B8_BF7A_19FD1261159D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ITIMEOBJECTIMPL_H__E82D22CD_C1A3_4DA9_8D85_8015E6C8A00A__INCLUDED_)
	#include "Subsys/TimeObjectImpl.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CGLXTimeObject : public ITImeObjectImpl
{
public:
	CGLXTimeObject();
	virtual ~CGLXTimeObject();

	//!	Implements @see CTimeObject
	virtual float deltaTime(void);

	//!	Implements @see CTimeObject
	virtual void markTime(void* mark);

	//!	Implements @see CTimeObject
	virtual float deltaMarkTime(void *mark);


private:
	struct timespec	m_resolution;
	struct timespec	m_oldtime;
	struct timespec	m_newtime;
	struct timespec	m_lastfreq;

	map<void*, uint64_t> m_markers;
};

RAPTOR_NAMESPACE_END


#endif // !defined(AFX_GLXTIMEOBJECT_H__3079A145_D92D_45B8_BF7A_19FD1261159D__INCLUDED_)
