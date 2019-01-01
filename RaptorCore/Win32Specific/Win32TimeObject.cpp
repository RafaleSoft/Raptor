// Win32AnimatorStream.cpp: implementation of the CWin32AnimatorStream class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_WIN32TIMEOBJECT_H__81BA3EBB_33AF_411A_80D9_9E83894B0D30__INCLUDED_)
	#include "Win32TimeObject.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWin32TimeObject::CWin32TimeObject()
{
	if (!QueryPerformanceFrequency(&frequency))
	{
		frequency.QuadPart=0;
		oldtime.QuadPart = GetTickCount64();
	}
	else
		QueryPerformanceCounter(&oldtime);
}

CWin32TimeObject::~CWin32TimeObject()
{

}


float CWin32TimeObject::deltaTime(void)
{
	float dt = 1.0f;

	if (frequency.QuadPart > 0)
	{
		QueryPerformanceCounter(&newtime);
		dt = ((float)(newtime.QuadPart - oldtime.QuadPart)) / ((float)frequency.QuadPart);
	}
	else
	{
		newtime.QuadPart = GetTickCount64();
		dt = 0.001f * (newtime.QuadPart - oldtime.QuadPart);
	}
	oldtime = newtime;

	return dt;
}

void CWin32TimeObject::markTime(void* mark)
{
	map<void*, LARGE_INTEGER>::iterator it = _markers.find(mark);

	LARGE_INTEGER markTime;
	if (frequency.QuadPart > 0)
		QueryPerformanceCounter(&markTime);
	else
		markTime.QuadPart = GetTickCount64();
	
	if (it != _markers.end())
		(*it).second = markTime;
	else
		_markers[mark] = markTime;
}

float CWin32TimeObject::deltaMarkTime(void* mark)
{
	LARGE_INTEGER markTime;
	if (frequency.QuadPart > 0)
		QueryPerformanceCounter(&markTime);
	else
		markTime.QuadPart = GetTickCount64();

	float dt = 0.0f;

	map<void*,LARGE_INTEGER>::iterator it = _markers.find(mark);
	if (it != _markers.end())
	{	
		LARGE_INTEGER &oldMarkTime = (*it).second;
		if (frequency.QuadPart > 0)
			dt = ((float)(markTime.QuadPart - oldMarkTime.QuadPart)) / ((float)frequency.QuadPart);
		else
			dt = 0.001f * (markTime.QuadPart - oldMarkTime.QuadPart);
		oldMarkTime = markTime;
	}

	return dt;
}

