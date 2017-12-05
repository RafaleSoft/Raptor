// Surfacer.cpp: implementation of the CSurfacer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Raytracer.h"
#include "Surfacer.h"

#include <math.h>
#include <memory.h>

#include "..\Plugin.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSurfacer::CSurfacer()
{
	generateNormal = false;
}

CSurfacer::~CSurfacer()
{

}

bool CSurfacer::InitNormalGenerator(rays_plugin_t& gen)
{
	memset(normal.name,0,MAX_STR_LEN);
	normal = gen;
	
	char *pdot = strrchr(normal.name,'.');
	if (pdot != NULL)
		*pdot = 0;

	return true;
}

bool CSurfacer::InitPlugins(const CRaytracerData& raytracer_data)
{
	bool ready = false;
/*	POSITION pos = raytracer_data.plugins.GetStartPosition();
	while (pos != NULL)
	{
		CString name;
		PLUGIN *plugin = NULL;
		raytracer_data.plugins.GetNextAssoc(pos,name,(void*&)plugin);

		if ((plugin != NULL) && (plugin->moduleInstance != NULL))
		{
			CString pname = plugin->pGetModuleName();
			if (!strcmp(normal.name,LPCTSTR(pname)))
			{
				PARAMETER_DATA* pData = plugin->pGetParameters();
				pNormal = plugin->pNormal;

				if ((pData != NULL) && (pNormal != NULL))
				{
					plugin->pInitModule();
					generateNormal = true;
					ready = true;
				}
			}
		}
	}
*/
	return ready;
}

bool CSurfacer::GenerateNormal(CGenericVector<float> point,
							   CGenericVector<float> &normal)
{
	if (generateNormal)
	{
		((NORMAL)(pNormal))(point.vector(),normal.vector());
		return true;
	}
	else 
		return false;
}

bool CSurfacer::PerturbNormal(	CGenericVector<float> point,
								CGenericVector<float> &normal)
{
	if (generateNormal)
	{
		((NORMAL)(pNormal))(point.vector(),turb.vector());
		turb.Normalize();

		//if ((turb % normal) > 0)
		{
			normal += turb * this->normal.params[0];
			normal.Normalize();
		}

		return true;
	}
	else 
		return false;
}