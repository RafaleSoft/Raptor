// TimeObject.cpp: implementation of the CTimeObject class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "TimeObject.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if defined(_WIN32)
	#if !defined(AFX_WIN32TIMEOBJECT_H__81BA3EBB_33AF_411A_80D9_9E83894B0D30__INCLUDED_)
		#include "Win32Specific/Win32TimeObject.h"
	#endif
#endif
#if defined(LINUX)
	#if !defined(AFX_GLXTIMEOBJECT_H__3079A145_D92D_45B8_BF7A_19FD1261159D__INCLUDED_)
		#include "GLXSpecific/GLXTimeObject.h"
	#endif
#endif

RAPTOR_NAMESPACE_BEGIN

static CRaptorMutex     *tmMutex = NULL;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

CRaptorMutex& CTimeObject::getLock(void)
{
	if (NULL == tmMutex)
	{
		tmMutex = new CRaptorMutex();
	}

	return *tmMutex;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeObject::CTimeObject()
	:m_owner(CRaptorInstance::GetInstance()),
	 m_animate(true), m_synchronized(false)
{
	m_owner.m_rootTimeObjects.push_back(this);

	getLock();
}

CTimeObject::~CTimeObject()
{
	CRaptorLock lock(*tmMutex);

	vector<CTimeObject*>::iterator itr = m_owner.m_rootTimeObjects.begin();

	while ((*itr) != this)
		itr++;

	if (itr != m_owner.m_rootTimeObjects.end())
		m_owner.m_rootTimeObjects.erase(itr);
}

const std::vector<CTimeObject*>& CTimeObject::getTimeObjects(void)
{
	CRaptorLock lock(*tmMutex);
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	return instance.m_rootTimeObjects;
}

void RAPTOR_FASTCALL CTimeObject::setTimeFactor(float factor)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	instance.m_deltat = factor;
}

float CTimeObject::deltaTime(void)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	float delta = instance.m_timeImplementation->deltaTime();

	instance.m_time += (instance.m_deltat * delta);
	instance.m_globalTime += delta;

	return delta;
}

void CTimeObject::markTime(void* mark)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	instance.m_timeImplementation->markTime(mark);
}

float CTimeObject::deltaMarkTime(void *mark)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	return instance.m_timeImplementation->deltaMarkTime(mark);
}

float RAPTOR_FASTCALL CTimeObject::GetGlobalTime(void) 
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	return instance.m_globalTime; 
}

float RAPTOR_FASTCALL CTimeObject::GetTime(void) 
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	return instance.m_time; 
}

void CTimeObject::synchronize(float dt, bool synchro)
{
	if (!m_synchronized && synchro)
	{
		if (m_animate)
			deltaTime(dt);
	}

	m_synchronized = synchro;
}

bool CTimeObject::prioritize(void)
{
	CRaptorLock lock(*tmMutex);

	std::vector<CTimeObject*>::iterator itr = m_owner.m_rootTimeObjects.begin();

	while (((*itr) != this) && (itr != m_owner.m_rootTimeObjects.end()))
		itr++;

	if (itr != m_owner.m_rootTimeObjects.end())
	{
		m_owner.m_rootTimeObjects.erase(itr);
		m_owner.m_rootTimeObjects.insert(m_owner.m_rootTimeObjects.begin(), this);
		return true;
	}
	else
		return false;
}
