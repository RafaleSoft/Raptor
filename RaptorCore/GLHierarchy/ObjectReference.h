// ObjectReference.h: interface for the CObjectReference class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
#define AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN

//! This class is a simple object reference counter. It is used to manage 
//! shared properties such as texture names, materials, shaders among objects
class RAPTOR_API CObjectReference  
{
public:
    //! Constructor
	//! At contruction, objet cannot be referenced
    CObjectReference():referenceCount(0) {};

    //! Increment counter
    void addReference(void) 
    { referenceCount++; };

    //! Decrement counter.
    void releaseReference(void)
    {
        if (referenceCount == 0)
            delete this;
		else
			referenceCount--;
    }

	//! Reference count value.
	//! @return : the current reference count of the object.
	unsigned int getRefCount(void) const 
	{ return referenceCount; };


protected:
    virtual ~CObjectReference() {};

private:
    //! The actual number of instance
    unsigned int    referenceCount;
};

//!	This class defines a reference to CObjectReference objects,
//! and provides direct reference management for classes attributes
template <class T>
class CReference
{
public:
	//!	Default constructor.
	CReference(void):m_pReference(NULL) { };

	//!	Constructor with a handled referenced object
	CReference(T* ref);

	//!	Copy oerator, handles current & new reference.
	const CReference& operator=(const CReference& ref);

	//!	Destructor: releases referenced object.
	virtual ~CReference(void);

	//!	Assign a new reference, handles current & new reference.
	const CReference& operator=(T* ref);

	//!	Access to referenced object.
	T* const operator->(void) const
	{ return m_pReference;}

	//!	Compares handled reference.
	bool operator==(const T* ref) const
	{ return (m_pReference == ref); };

	//!	Compares handled reference.
	bool operator!=(const T* ref) const
	{ return (m_pReference != ref); };

	operator T*(void) const
	{ return m_pReference; };


private:
	T*	m_pReference;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)

