// RaptorToolBox.cpp : Defines the initialization routines for the DLL.
//

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

RAPTOR_NAMESPACE


extern "C" int APIENTRY
DllMain(HINSTANCE , DWORD dwReason)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr))
		{
			RAPTOR_ERROR(	CPersistence::CPersistenceClassID::GetClassId(), 
							"Failed to initialize COM layer.");
			return 0;
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		CoUninitialize();
	}
	return 1;   // ok
}
