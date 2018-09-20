// Win32AnimatorStream.h: interface for the CWin32AnimatorStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITIMEOBJECTIMPL_H__E82D22CD_C1A3_4DA9_8D85_8015E6C8A00A__INCLUDED_)
#define AFX_ITIMEOBJECTIMPL_H__E82D22CD_C1A3_4DA9_8D85_8015E6C8A00A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN

RAPTOR_INTERFACE ITImeObjectImpl
{
public:
	//!	Implements @see CTimeObject
	virtual float deltaTime(void) = 0;

	//!	Implements @see CTimeObject
	virtual void markTime(void* mark) = 0;

	//!	Implements @see CTimeObject
	virtual float deltaMarkTime(void *mark) = 0;
};

RAPTOR_NAMESPACE_END


#endif // !defined(AFX_ITIMEOBJECTIMPL_H__E82D22CD_C1A3_4DA9_8D85_8015E6C8A00A__INCLUDED_)
