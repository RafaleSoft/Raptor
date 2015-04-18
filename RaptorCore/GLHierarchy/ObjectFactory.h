// ObjectFactory.h: interface for the CObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
#define AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CObject3D;

class CPersistentObject
{
public:
	virtual operator CObject3D*(void) const = 0;

	virtual operator CPersistence*(void) const = 0;

protected:
	CPersistentObject() {};
	virtual ~CPersistentObject() {};

	friend class CObjectFactory;
	virtual void createType(void) = 0;

private:
	virtual void setType(CPersistence *) = 0;
};



//! This class creates object by names or Ids using registered persistent object's 
//! construction method.
class RAPTOR_API  CObjectFactory  
{
public:
	//! Singleton access
	static CObjectFactory* GetInstance(void);

	//! Destructor
	virtual ~CObjectFactory();

	//! Creates a persistent object by Id
	const CPersistentObject & createObject(const CPersistence::CPersistenceClassID& id);

	//! Creates a persistent object by name
	const CPersistentObject & createObject(const std::string& className);

	//! Register a persistent object type so that this factory
	//! can create some instances later. 
	//! Returns true if object is properly registered, false otherwise ( e.g. already registered )
	bool registerPersistentObject(	CPersistentObject* objectType, 
									const CPersistence::CPersistenceClassID& id);

	//! Returns true if object is registered, false otherwise
	bool isRegistered(const CPersistence::CPersistenceClassID& id) const;

	//! Get a persistent object. This method is a way to 'cast' objects quite safely
	const CPersistentObject & getObject(CPersistence* object);


private:
	CObjectFactory();

    //! A helper to format class names triming off white spaces and tabs
	string trim(const std::string &className);

    //! The only one instance
	static CObjectFactory* m_pInstance;

	map<std::string,CPersistentObject*>	m_objectTypes;
	map<unsigned int,CPersistentObject*>	m_objectIDs;
};




//! Self-register specialized object type for both CObject3D & CPersistence
template <class T> class CPersistentObjectType : public CPersistentObject
{
public:
	CPersistentObjectType(	const CPersistence::CPersistenceClassID& id)
							//const std::string& className,
							//CPersistence::PERSISTENCE_CLASS_ID id)
	 :m_pType(NULL)
	{ CObjectFactory::GetInstance()->registerPersistentObject(this,id); }

	virtual ~CPersistentObjectType() {};

	virtual operator CObject3D*(void) const
	{ return static_cast<CObject3D*>(m_pType); };

	virtual operator CPersistence*(void) const
	{ return static_cast<CPersistence*>(m_pType); };

private:
	void createType(void) { m_pType = new T; };

	void setType(CPersistence *p) { m_pType = static_cast<T*>(p); };

	T *m_pType;
};


//! Self-register specialized object type for CPersistence 
template <class T,class U=CObject3D> class CPersistentType : public CPersistentObject
{
public:
    typedef U* (T::*GETOBJECT_t)(void) const;

public:
	CPersistentType(const CPersistence::CPersistenceClassID& id,
					GETOBJECT_t getObject = NULL)
		:m_pType(NULL),m_pGetObject(getObject)
	{ CObjectFactory::GetInstance()->registerPersistentObject(this,id);	}

	virtual ~CPersistentType() {};

	virtual operator CObject3D*(void) const
	{	
        if (m_pGetObject == NULL)
		    return NULL;
		else
		    return static_cast<CObject3D*>((m_pType->*m_pGetObject)()); 
	};

	virtual operator CPersistence*(void) const
	{ return static_cast<CPersistence*>(m_pType); };

private:
	void createType(void) { m_pType = new T; };

	void setType(CPersistence *p) { m_pType = static_cast<T*>(p); };

	T			*m_pType;
	GETOBJECT_t	m_pGetObject;
};
    

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)

