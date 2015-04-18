/////////////////////////////////////////////////////////////////////////////
//
//	Precompiled headers
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


