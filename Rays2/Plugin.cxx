#ifndef __PLUGIN_CXX__
#define __PLUGIN_CXX__

#include "Plugin.h"

#ifndef PLUGIN_LOADER
#define PLUGIN_LOADER
	PLUGIN OpenPlugin(CString name)
	{
		PLUGIN p;
		memset(&p,0,sizeof(PLUGIN));

		p.moduleInstance = AfxLoadLibrary(name);

		if (p.moduleInstance != NULL)
		{
			p.pGetModuleName = (GETMODULEINFO)(GetProcAddress(p.moduleInstance,"GetModuleName"));
			p.pGetModuleDescription = (GETMODULEINFO)(GetProcAddress(p.moduleInstance,"GetModuleDescription"));
			p.pGetModuleAuthor = (GETMODULEINFO)(GetProcAddress(p.moduleInstance,"GetModuleAuthor"));
			p.pGetModuleType = (GETMODULETYPE)(GetProcAddress(p.moduleInstance,"GetModuleType"));
			p.pGetModuleVersion = (GETMODULEVERSION)(GetProcAddress(p.moduleInstance,"GetModuleVersion"));
			p.pInitModule = (INITMODULE)(GetProcAddress(p.moduleInstance,"InitModule"));
			p.pGetParameters = (GETPARAMETERS)(GetProcAddress(p.moduleInstance,"GetParameters"));
			p.pSetParameters = (SETPARAMETERS)(GetProcAddress(p.moduleInstance,"SetParameters"));
			p.pAfterEffect = (AFTEREFFECT)(GetProcAddress(p.moduleInstance,"AfterEffect"));
			p.pNormal = (NORMAL)(GetProcAddress(p.moduleInstance,"Normal"));
		}

		return p;
	}
#else
	extern PLUGIN OpenPlugin(CString name);
#endif

#endif