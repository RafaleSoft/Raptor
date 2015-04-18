// Persistence.cpp: implementation of the CPersistence class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif

#include <stdio.h>	//	sprintf
#include <stdlib.h>	//	atoi

RAPTOR_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// Objects Roots
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4711)	// automatic inline expansion warning

static bool lockRegistration = false;
static MapStringToPtr	objects;
static MapStringToPtr::iterator last_itr;
static CRaptorMutex     persistenceMutex;
static CPersistence::CPersistenceClassID persistenceID;
static unsigned int uniqueID = 0;

CPersistence::CPersistenceClassID::CPersistenceClassID()
{
	m_ID = uniqueID++;
}

const CPersistence::CPersistenceClassID& CPersistence::CPersistenceClassID::GetClassId(void)
{
	return persistenceID;
}

template <class T>
T* CPersistence::CPersistenceClassID::narrow(const CPersistence *P) const
{
	if (P == NULL)
		return NULL;
	else if (T::TClassId::GetClassId().isSubClassOf(P->getId().GetClassId()))
		return (T*)(P);
	else
		return NULL;
}

#pragma warning (default : 4711)	// automatic inline expansion warning

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPersistence::CPersistence():m_ID(persistenceID) 
{
}

CPersistence::CPersistence(const CPersistence::CPersistenceClassID &classID,
						   const std::string& name)
	:m_ID(classID)
{
	if (name.empty())
		m_name="RAPTOR_OBJECT";
	else
		m_name = name;

    //! After this point, we are in critical section
    CRaptorLock lock(persistenceMutex);

	MapStringToPtr::iterator itr = objects.find(m_name);
	
    bool firstfind = true;
	while (itr != objects.end())
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        if (firstfind)
        {
            vector<CRaptorMessages::MessageArgument> args;
            CRaptorMessages::MessageArgument msgArg;
            msgArg.arg_sz = name.data();
            args.push_back(msgArg);

			Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
														 	CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_OBJECT_DUPLICATE,args);
            firstfind = false;
        }
#endif

        string::size_type pos = m_name.find('#');

		if (pos > m_name.size())
			m_name += "#2";
		else
		{
            char nb[32];
			sprintf(nb,"%d",atoi(m_name.data() + pos+1)+1);
			m_name = m_name.substr(0,pos) + "#" + nb;
		}
		itr = objects.find(m_name);
	}

	MapStringToPtr::value_type val(m_name,this);
	objects.insert(val);
}

CPersistence::~CPersistence()
{
    CRaptorLock lock(persistenceMutex);
    lockRegistration = true;
    lock.release();

	vector<CPersistence*>::iterator it = m_pObservers.begin();
	while (it != m_pObservers.end())
		(*it++)->unLink(this);

    //! After this point, we are in critical section
    lockRegistration = false;
    lock.acquire();

	MapStringToPtr::iterator itr = objects.find(m_name);
	
	if (itr != objects.end())
		objects.erase(itr);

#ifdef RAPTOR_DEBUG_MODE_GENERATION
    else
		Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_DESTROY_FAILED);
#endif
}

bool CPersistence::registerDestruction(CPersistence* p)
{
    //! After this point, we are in critical section
    CRaptorLock lock(persistenceMutex);

	if ((p == NULL) || lockRegistration)
		return false;

	vector<CPersistence*>::iterator it = m_pObservers.begin();
	while ((it != m_pObservers.end()) && (*it != p))
		it++;

	if (it != m_pObservers.end())
		return false;
	else
	{
		m_pObservers.push_back(p);
		return true;
	}
}


bool CPersistence::unregisterDestruction(CPersistence* p)
{
    //! After this point, we are in critical section
    CRaptorLock lock(persistenceMutex);

	if ((p == NULL) || lockRegistration)
		return false;

	vector<CPersistence*>::iterator it = m_pObservers.begin();
	while ((it != m_pObservers.end()) && ((*it) != p))
		it++;

	if (it == m_pObservers.end())
		return false;
	else
	{
		m_pObservers.erase(it);
		return true;
	}
}


void CPersistence::setName(const std::string &name)
{
    //! After this point, we are in critical section
    CRaptorLock lock(persistenceMutex);

	MapStringToPtr::iterator itr = objects.find(m_name);
	
	if (itr == objects.end())
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_UPDATE_FAILED);

#endif
    }
	else
	{
		CPersistence *p = (CPersistence*)((*itr).second);

        // object is erased before reinserted.
        // clever algorithm would avoid rebalancing the map...
		objects.erase(itr);

		p->m_name = name;
		itr = objects.find(p->m_name);

		while (itr != objects.end())
		{
            string::size_type pos = p->m_name.find('#');

			if (pos > p->m_name.size())
				p->m_name += "#2";
			else
			{
                char nb[32];
			    sprintf(nb,"%d",atoi(p->m_name.data() + pos)+1);
                p->m_name[pos+1] = 0;
			    p->m_name += nb;
			}
			itr = objects.find(p->m_name);
		}

		MapStringToPtr::value_type val(m_name,this);
		objects.insert(val);
	}
}

int CPersistence::NbInstance()
{ 
	return objects.size(); 
}


CPersistence *CPersistence::Object(void*& it)
{
    //! After this point, we are in critical section
    CRaptorLock lock(persistenceMutex);

	void *obj = NULL;
	CPersistence *res = NULL;

	MapStringToPtr::iterator *itr = (MapStringToPtr::iterator *)it;
	
	if (itr == NULL)
	{
		last_itr = objects.begin();
		itr = &last_itr;
		it = itr;
	}

	if ((*itr) != objects.end())
	{
		res = (CPersistence*)((*(*itr)).second);
		(*itr)++;
	}
	else
	{
		res = NULL;
		it = NULL;
	}

	return res;
}


CPersistence *CPersistence::FindObject(const char *name)
{
	return FindObject(string(name));
}

CPersistence *CPersistence::FindObject(const std::string &name)
{
    //! After this point, we are in critical section
    CRaptorLock lock(persistenceMutex);

	CPersistence *res = NULL;

	MapStringToPtr::iterator itr = objects.find(name);

	if (itr != objects.end())
		res = (CPersistence*)((*itr).second);

	return res;
}


bool CPersistence::exportObject(CRaptorIO& o)
{
	o << m_ID.ID();
	o << m_name.size();
	o << m_name.data();

	return true;
}

bool CPersistence::importObject(CRaptorIO& io)
{
    string name;
	io >> name;
	
	setName(name);

	return true;
}

