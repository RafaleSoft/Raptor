// TimeObject.cpp: implementation of the CTimeObject class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "TimeObject.h"
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

float CTimeObject::m_time = 0.0f;
float CTimeObject::m_globalTime = 0.0f;
float CTimeObject::m_deltat = 0.05f;	// Should this value be changed ?
ITImeObjectImpl	*CTimeObject::m_impl = NULL;

vector<CTimeObject*>	CTimeObject::m_rootTimeObjects;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeObject::CTimeObject() : 
		m_animate(true)
		,m_synchronized(false)
{
	m_rootTimeObjects.push_back(this);
	m_time = 0;

	if (0 == m_impl)
	{
#if defined(_WIN32)
		m_impl = new CWin32TimeObject();
#elif defined(LINUX)
		m_impl = new CGLXTimeObject();
#endif
	}
}

CTimeObject::~CTimeObject()
{
	vector<CTimeObject*>::iterator itr = m_rootTimeObjects.begin();

	while ((*itr) != this)
		itr++;

	if (itr != m_rootTimeObjects.end())
		m_rootTimeObjects.erase(itr);
}

const vector<CTimeObject*>& CTimeObject::getTimeObjects(void)
{
	return m_rootTimeObjects;
}

void CTimeObject::setTimeFactor(float factor)
{
	m_deltat = factor;
}

float CTimeObject::deltaTime(void)
{
	float delta = m_impl->deltaTime();

	m_time += (m_deltat * delta);
	m_globalTime += delta;

	return delta;
}

void CTimeObject::markTime(void* mark)
{
	m_impl->markTime(mark);
}

float CTimeObject::deltaMarkTime(void *mark)
{
	return m_impl->deltaMarkTime(mark);
}

float RAPTOR_FASTCALL CTimeObject::GetGlobalTime(void) 
{ 
	return m_globalTime; 
}

float RAPTOR_FASTCALL CTimeObject::GetTime(void) 
{ 
	return m_time; 
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
	vector<CTimeObject*>::iterator itr = m_rootTimeObjects.begin();

	while ((*itr) != this)
		itr++;

	if (itr != m_rootTimeObjects.end())
	{
		m_rootTimeObjects.erase(itr);
		m_rootTimeObjects.insert(m_rootTimeObjects.begin(),this);
		return true;
	}
	else
		return false;
}
