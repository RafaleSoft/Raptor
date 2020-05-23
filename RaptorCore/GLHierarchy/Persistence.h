// Persistence.h: interface for the CPersistence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
#define AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000




//! file CodeGeneration
//!	This header is VERY IMPORTANT.
//!	It must not be undefined because a generated version
//!	of any application using Raptor will not run properly
//!	if the compiler options are not the same.
#include "Subsys/CodeGeneration.h"


RAPTOR_NAMESPACE_BEGIN

//!	I/O CPerssitence capabilities declaration.
//! Any CPersitence class can be imported or exported
//! through a CRaptorIO Service class. Derived classes
//! must implement Export/Import ( until automated code
//! generation is included in a future Raptor release )
class CRaptorIO;

#define DECLARE_IO\
	virtual bool exportObject(CRaptorIO& o); \
	virtual bool importObject(CRaptorIO& i);

#define DECLARE_CLASS_ID(classID,className,parentClass)\
	DECLARE_API_CLASS_ID(RAPTOR_API,classID,className,parentClass)
	

#define DECLARE_API_CLASS_ID(API,classID,className,parentClass)\
	class API classID : public parentClass::parentClass##ClassID\
	{\
	public:\
		virtual const char* ClassName(void) const\
		{ return className; };\
		virtual bool isSubClassOf(const CPersistence::CPersistenceClassID& id) const\
		{ return ((id.ID() == ID()) || parentClass::parentClass##ClassID::GetClassId().isSubClassOf(id)); };\
		static const CPersistenceClassID& GetClassId(void);\
	};

#define IMPLEMENT_CLASS_ID(className, factory) \
	static className::className##ClassID factory; \
	static CPersistentType<className> classFactory(factory); \
	const CPersistence::CPersistenceClassID& className::className##ClassID::GetClassId(void) \
	{ return factory; }



//!		This class is not intended to work standalone
//!		because it is the root af any Raptor
//!		object, be it internal or external. Persistence
//!		is usefull to find objects in a complex scene without
//!		any sets of pointers transfer burden.
class RAPTOR_API CPersistence
{
public:

	class RAPTOR_API CPersistenceClassID
	{
	public:
		CPersistenceClassID();
		CPersistenceClassID(const CPersistenceClassID& id)
		{ m_ID = id.m_ID; };
		virtual ~CPersistenceClassID() {};

		bool operator==(const CPersistenceClassID& id) const
		{ return id.m_ID == m_ID; };

		unsigned int ID(void) const
		{ return m_ID; };

		virtual const char* ClassName(void) const
		{ return "Persistence"; };

		virtual bool isSubClassOf(const CPersistenceClassID& id) const 
		{ return id.m_ID == m_ID; };

		static const CPersistenceClassID& GetClassId(void);

	private:
		unsigned int m_ID;
	};


public:
	//!	Public destructor but no constructor.
	//!	This object cannot be constructed manually
	//!	because it makes no sense ( and it would be
	//!	useless such a complexity for an object naming service )
	virtual ~CPersistence();

	//!	Returns the number of Raptor persistant objects 
	//!	currently in memory.
	static size_t NbInstance();

	//!	Returns a Raptor persistent object given it's position (map index).
	//!	DONT DELETE any of these objects manually.
	//!	Be sure that pos is a valid position because no extra check is performed.
	//!	Position may be updated in the following cases:
	//!	- if the position is valid it is updated to the next one.
	//!	- if position is not valid, NULL is returned, position is unchanged.
	//! - if position is NULL, the first object is returned and position
	//!	is updated to the next one.
	static CPersistence * Object(void *&it);

	//!	Finds and returns an object with it's name.
	//!	If the object is not found, NULL is returned,
	//! otherwise the first match is returned. 
	//!	The match found will always be the same, 
	//!	whatever the number of objects
	static CPersistence * FindObject(const std::string &name);

	//!	Finds and returns an object with it's name.
	//!	Same as above.
	static CPersistence * FindObject(const char *name);

	//!	Returns the class id of the object
	const CPersistenceClassID& getId() const { return m_ID; };

	//!	Returns the name of the object.
	//!	The name is the one given in constructor
	const std::string& getName() const { return m_name; };

	//!	Renames the object
	void setName(const std::string &name);

	DECLARE_IO

	//!	Add an observer for destruction.
	//!	Returns false if observer already registered, true otherwise.
	bool registerDestruction(CPersistence* p);

	//! Removes an observer.
	//! Returns false if observer was not registered, true otherwise.
	bool unregisterDestruction(CPersistence* p);



protected:
	//!	The only available constructor.
	//!	Each object must use this constructor with the
	//!	appropriate data
	CPersistence(const CPersistenceClassID &classID,const std::string& name = "");

	//!	Notifies observers that this persistence is about to be destroyed.
	virtual void unLink(const CPersistence*) {};



private:
	CPersistence();

	//! Persistence atributes.
	std::string				m_name;
	const CPersistenceClassID	&m_ID;
	
	//!	The list of registered objects listening destruction of this persistence.
	std::vector<CPersistence*>	m_pObservers;

	//!	This operator must not be available
	//! because persistence data MUST be unique
	const CPersistence& operator=(const CPersistence& ) { return *this;};
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)

