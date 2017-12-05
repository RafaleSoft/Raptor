/////////////////////////////////////////////////////////////////////////////
//
//	Precompiled headers
//

// Raptordll.cpp : source file that includes just the standard includes
//	RaptorCore.pch will be the pre-compiled header
//	Raptordll.obj will contain the pre-compiled type information
#ifdef WIN32
	#pragma warning(disable:4244)
#endif

#include "Subsys/CodeGeneration.h"

extern "C" int RAPTOR_APICALL
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID )
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
	}
	else if (dwReason == DLL_THREAD_ATTACH)
	{
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
	}

	return 1;   // ok
}


