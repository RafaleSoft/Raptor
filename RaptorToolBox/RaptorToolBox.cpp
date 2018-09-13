// RaptorToolBox.cpp : Defines the initialization routines for the DLL.
//

#include "Subsys/CodeGeneration.h"

extern "C" int APIENTRY
DllMain(HINSTANCE , DWORD dwReason)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{

	}
	return 1;   // ok
}
