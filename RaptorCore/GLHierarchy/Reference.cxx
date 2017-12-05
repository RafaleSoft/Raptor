#if !defined(AFX_REFERENCE_H__D29BE5EA_DA55_4BCA_A700_73E007EFE5F9__INCLUDED_)
#define AFX_REFERENCE_H__D29BE5EA_DA55_4BCA_A700_73E007EFE5F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
	#include "GLHierarchy/ObjectReference.h"
#endif

//! The implementation in a separate file is necessary
//!	for CReference<T> to be declared and used with predeclaration
//! of T (T may be fully declared only forward)

RAPTOR_NAMESPACE_BEGIN

template <class T>
__inline CReference<T>::CReference(T* ref)
	:m_pReference(ref)
{	
	if (m_pReference != NULL)
		m_pReference->addReference();
}

template <class T>
__inline const CReference<T>& CReference<T>::operator=(const CReference<T>& ref)
{
	if ((m_pReference != NULL) && (ref.m_pReference != m_pReference))
		m_pReference->releaseReference();

	m_pReference = ref.m_pReference;
	if (m_pReference != NULL)
		m_pReference->addReference();

	return *this;
}

template <class T>
__inline CReference<T>::~CReference(void)
{ 
	if (m_pReference != NULL)
		m_pReference->releaseReference();
}

template <class T>
__inline const CReference<T>& CReference<T>::operator=(T* ref)
{
	if (m_pReference != NULL)
		m_pReference->releaseReference();
	m_pReference = ref;
	if (m_pReference != NULL)
		m_pReference->addReference();

	return *this;
}

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_REFERENCE_H__D29BE5EA_DA55_4BCA_A700_73E007EFE5F9__INCLUDED_)

