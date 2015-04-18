// Win32AnimatorStream.h: interface for the CWin32AnimatorStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIN32TIMEOBJECT_H__81BA3EBB_33AF_411A_80D9_9E83894B0D30__INCLUDED_)
#define AFX_WIN32TIMEOBJECT_H__81BA3EBB_33AF_411A_80D9_9E83894B0D30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ITIMEOBJECTIMPL_H__E82D22CD_C1A3_4DA9_8D85_8015E6C8A00A__INCLUDED_)
	#include "Subsys/TimeObjectImpl.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CWin32TimeObject : public ITImeObjectImpl
{
public:
	CWin32TimeObject();
	virtual ~CWin32TimeObject();

	//!	Implements @see CTimeObject
	virtual float deltaTime(void);

	//!	Implements @see CTimeObject
	virtual void markTime(void* mark);

	//!	Implements @see CTimeObject
	virtual float deltaMarkTime(void *mark);


private:
	LARGE_INTEGER	frequency;
	LARGE_INTEGER	oldtime;
	LARGE_INTEGER	newtime;
	LARGE_INTEGER	lastfreq;

	map<void*,LARGE_INTEGER> _markers;
};

RAPTOR_NAMESPACE_END


#endif // !defined(AFX_WIN32TIMEOBJECT_H__81BA3EBB_33AF_411A_80D9_9E83894B0D30__INCLUDED_)
