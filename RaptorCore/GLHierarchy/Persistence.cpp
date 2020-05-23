// Persistence.cpp: implementation of the CPersistence class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
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
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


#include <stdio.h>	//	sprintf
#include <stdlib.h>	//	atoi

RAPTOR_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// Objects Roots
//////////////////////////////////////////////////////////////////////
static bool lockRegistration = false;
static MapStringToPtr::iterator last_itr;
static CRaptorMutex     persistenceMutex;
static CPersistence::CPersistenceClassID persistenceID;
static uint32_t uniqueID = 0;

CPersistence::CPersistenceClassID::CPersistenceClassID()
{
	m_ID = uniqueID++;
}

const CPersistence::CPersistenceClassID& CPersistence::CPersistenceClassID::GetClassId(void)
{
	return persistenceID;
}

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

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	MapStringToPtr::iterator itr = instance.objects.find(m_name);
	
    bool firstfind = true;
	while (itr != instance.objects.end())
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        if (firstfind)
        {
            vector<CRaptorMessages::MessageArgument> args;
            CRaptorMessages::MessageArgument msgArg;
            msgArg.arg_sz = name.data();
            args.push_back(msgArg);

			CRaptorErrorManager* mgr = Raptor::GetErrorManager();
			if (NULL != mgr)
			{
				mgr->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
										 CRaptorErrorManager::RAPTOR_WARNING,
										 CRaptorMessages::ID_OBJECT_DUPLICATE,
										 __FILE__, __LINE__, args);
			}
            firstfind = false;
        }
#endif

        string::size_type pos = m_name.find('#');

		if (pos > m_name.size())
			m_name += "#2";
		else
		{
			stringstream nb;
			nb << m_name.substr(0, pos);
			nb << "#";
			nb << atoi(m_name.data() + pos + 1) + 1;
			nb << std::ends;
			m_name = nb.str();
		}
		itr = instance.objects.find(m_name);
	}

	MapStringToPtr::value_type val(m_name,this);
	instance.objects.insert(val);
}

CPersistence::~CPersistence()
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	stringstream msg;
	msg << " deleting " << this << " - " << this->m_name << " - " << this->getId().ClassName();
	RAPTOR_NO_ERROR(CPersistence::CPersistenceClassID::GetClassId(), msg.str());
#endif

    CRaptorLock lock(persistenceMutex);
    lockRegistration = true;
    lock.release();

	vector<CPersistence*>::iterator it = m_pObservers.begin();
	while (it != m_pObservers.end())
		(*it++)->unLink(this);

    //! After this point, we are in critical section
    lockRegistration = false;
    lock.acquire();

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	MapStringToPtr::iterator itr = instance.objects.find(m_name);
	
	if (itr != instance.objects.end())
		instance.objects.erase(itr);

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	else
	{
		RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(), CRaptorMessages::ID_DESTROY_FAILED);
	}

	RAPTOR_NO_ERROR(CPersistence::CPersistenceClassID::GetClassId(), " done !");
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

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	MapStringToPtr::iterator itr = instance.objects.find(m_name);
	
	if (itr == instance.objects.end())
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		RAPTOR_ERROR(	CPersistence::CPersistenceClassID::GetClassId(),
						CRaptorMessages::ID_UPDATE_FAILED);

#endif
    }
	else
	{
		CPersistence *p = (CPersistence*)((*itr).second);

        // object is erased before reinserted.
        // clever algorithm would avoid rebalancing the map...
		instance.objects.erase(itr);

		p->m_name = name;
		itr = instance.objects.find(p->m_name);

		while (itr != instance.objects.end())
		{
            string::size_type pos = p->m_name.find('#');

			if (pos > p->m_name.size())
				p->m_name += "#2";
			else
			{
				stringstream nb;
				nb << m_name.substr(0, pos);
				nb << "#";
				nb << atoi(m_name.data() + pos + 1) + 1;
				nb << std::ends;
				p->m_name = nb.str();
			}
			itr = instance.objects.find(p->m_name);
		}

		MapStringToPtr::value_type val(m_name,this);
		instance.objects.insert(val);
	}
}

size_t CPersistence::NbInstance()
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	return instance.objects.size(); 
}


CPersistence *CPersistence::Object(void*& it)
{
    //! After this point, we are in critical section
    CRaptorLock lock(persistenceMutex);

	void *obj = NULL;
	CPersistence *res = NULL;

	MapStringToPtr::iterator *itr = (MapStringToPtr::iterator *)it;
	
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if (itr == NULL)
	{
		last_itr = instance.objects.begin();
		itr = &last_itr;
		it = itr;
	}

	if ((*itr) != instance.objects.end())
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

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	MapStringToPtr::iterator itr = instance.objects.find(name);

	if (itr != instance.objects.end())
		res = (CPersistence*)((*itr).second);

	return res;
}


bool CPersistence::exportObject(CRaptorIO& o)
{
	o << m_ID.ID();
	o << (uint32_t)(m_name.size());
	o << m_name.data();

	return true;
}

bool CPersistence::importObject(CRaptorIO& io)
{
	//uint32_t id = 0;
	//io >> id;

	//uint32_t sz = 0;
	//io >> sz;

    string name;
	io >> name;
	
	setName(name);

	return true;
}

