#include "stdafx.h"
#include "Subsys/CodeGeneration.h"

#ifndef __PLUG_IN_H__
	#include "../PlugIn.h"
#endif

bool CPlugin::OpenPlugin(const std::string& name)
{
	moduleInstance = LoadLibrary(name.c_str());

	if (moduleInstance != NULL)
	{
		pGetModuleName = (GETMODULEINFO)(GetProcAddress(moduleInstance, "GetModuleName"));
		pGetModuleDescription = (GETMODULEINFO)(GetProcAddress(moduleInstance, "GetModuleDescription"));
		pGetModuleAuthor = (GETMODULEINFO)(GetProcAddress(moduleInstance, "GetModuleAuthor"));
		pGetModuleType = (GETMODULETYPE)(GetProcAddress(moduleInstance, "GetModuleType"));
		pGetModuleVersion = (GETMODULEVERSION)(GetProcAddress(moduleInstance, "GetModuleVersion"));
		pInitModule = (INITMODULE)(GetProcAddress(moduleInstance, "InitModule"));
		pGetParameters = (GETPARAMETERS)(GetProcAddress(moduleInstance, "GetParameters"));
		pSetParameters = (SETPARAMETERS)(GetProcAddress(moduleInstance, "SetParameters"));
		pAfterEffect = (AFTEREFFECT)(GetProcAddress(moduleInstance, "AfterEffect"));
		pNormal = (NORMAL)(GetProcAddress(moduleInstance, "Normal"));
		return true;
	}
	else
		return false;
}