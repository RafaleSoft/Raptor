// Win32AnimatorStream.cpp: implementation of the CWin32AnimatorStream class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GLXTIMEOBJECT_H__3079A145_D92D_45B8_BF7A_19FD1261159D__INCLUDED_)
	#include "GLXTimeObject.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLXTimeObject::CGLXTimeObject()
{
	int res = clock_getres(CLOCK_REALTIME, &m_resolution);
	if (0 == res)
		res = clock_gettime(CLOCK_REALTIME, &m_oldtime);
	else
	{
	}
}

CGLXTimeObject::~CGLXTimeObject()
{

}


float CGLXTimeObject::deltaTime(void)
{
	float dt = 1.0f;

	if ((m_resolution.tv_sec > 0) || (m_resolution.tv_nsec > 0))
	{
		clock_gettime(CLOCK_REALTIME, &m_newtime);
		dt = ((float)(newtime.QuadPart - oldtime.QuadPart)) / ((float)frequency.QuadPart);
	}

	m_oldtime = m_newtime;

	return dt;
}

void CGLXTimeObject::markTime(void* mark)
{
	map<void*, uint64_t>::iterator it = _markers.find(mark);

	struct timespec markTime;
	clock_gettime(CLOCK_REALTIME, &markTime);
	
	if (it != _markers.end())
		(*it).second = markTime;
	else
		_markers[mark] = markTime;
}

float CGLXTimeObject::deltaMarkTime(void* mark)
{
	struct timespec markTime;
	clock_gettime(CLOCK_REALTIME, &markTime);

	float dt = 0.0f;

	map<void*, uint64_t>::iterator it = _markers.find(mark);
	if (it != _markers.end())
	{	
		uint64_t &oldMarkTime = (*it).second;
		if ((m_resolution.tv_sec > 0) || (m_resolution.tv_nsec > 0))
			dt = ((float)(markTime.QuadPart - oldMarkTime.QuadPart)) / ((float)frequency.QuadPart);
		else
			dt = 0.001f * (markTime.QuadPart - oldMarkTime.QuadPart);
		oldMarkTime = markTime;
	}

	return dt;
}

