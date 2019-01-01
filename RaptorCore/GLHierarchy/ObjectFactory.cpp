// ObjectFactory.cpp: implementation of the CObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Object3D.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif

RAPTOR_NAMESPACE_BEGIN

CObjectFactory* CObjectFactory::m_pInstance = NULL;

class NilObject : public CPersistentObject
{
public:
	NilObject() {};
	virtual ~NilObject() {};

	virtual operator CObject3D*(void) const { return NULL; }
	virtual operator CPersistence*(void) const { return NULL; }
	virtual void createType(void) {};
	virtual void setType(CPersistence*) {};
};

static NilObject nilObject;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectFactory::CObjectFactory()
{

}

CObjectFactory::~CObjectFactory()
{

}

CObjectFactory* CObjectFactory::GetInstance(void)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CObjectFactory();
	}

	return m_pInstance;
}

bool CObjectFactory::isRegistered(const CPersistence::CPersistenceClassID& id) const
{
	return (m_objectIDs.find(id.ID()) != m_objectIDs.end());
}

const CPersistentObject & CObjectFactory::createObject(const CPersistence::CPersistenceClassID& id)
{
	map<unsigned int,CPersistentObject*>::const_iterator itr = m_objectIDs.find(id.ID());
	if (itr != m_objectIDs.end())
	{
		CPersistentObject* object = (*itr).second;
		object->createType();
		return (*object);
	}
	else
		return nilObject;
}


const CPersistentObject & CObjectFactory::createObject(const std::string& className)
{
    string realClassName = trim(className);
    if (realClassName.empty())
        return nilObject;

	map<string,CPersistentObject*>::const_iterator itr = m_objectTypes.find(realClassName);
	if (itr != m_objectTypes.end())
	{
		CPersistentObject* object = (*itr).second;
		object->createType();
		return (*object);
	}
	else
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		RAPTOR_ERROR(	CPersistence::CPersistenceClassID::GetClassId(),
						CRaptorMessages::ID_CREATE_FAILED);

#endif
		return nilObject;
    }
}


bool CObjectFactory::registerPersistentObject(CPersistentObject* objectType, 
											  const CPersistence::CPersistenceClassID& id)
{
	string realClassName = trim(id.ClassName());
    if (realClassName.empty())
        return false;

	if ((m_objectTypes.find(realClassName) != m_objectTypes.end()) ||
		(m_objectIDs.find(id.ID()) != m_objectIDs.end()))
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		RAPTOR_ERROR(	CPersistence::CPersistenceClassID::GetClassId(),
						CRaptorMessages::ID_OBJECT_DUPLICATE);

#endif
		return false;
    }
	else
	{
		m_objectTypes[realClassName] = objectType;
		m_objectIDs[id.ID()] = objectType;
		return true;
	}
}

const CPersistentObject & CObjectFactory::getObject(CPersistence* object)
{
	map<unsigned int,CPersistentObject*>::const_iterator itr = m_objectIDs.find(object->getId().ID());
	if (itr != m_objectIDs.end())
	{
		CPersistentObject* persistence = (*itr).second;
		persistence->setType(object);
		return (*persistence);
	}
	else
		return nilObject;
}

std::string CObjectFactory::trim(const std::string &className)
{
    string spaces = "\t\n ";
    string res = "";

    string::size_type first = className.find_first_not_of(spaces);
    if (first == string::npos)
        return res;

    string::size_type last = className.find_last_not_of(spaces);
    if (last == string::npos)
        return res;

    res = className.substr(first,last-first+1);
    return res;
}
