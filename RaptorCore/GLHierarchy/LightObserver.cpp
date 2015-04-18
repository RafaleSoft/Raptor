// LightObserver.cpp: implementation of the CLightObserver class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"


#if !defined(AFX_LIGHTOBSERVER_H__D6E2195C_21B4_42FB_ABA5_4219EECFF076__INCLUDED_)
	#include "LightObserver.h"
#endif

#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "Subsys/LightAttributes.h"
#endif

#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif


#include <set>


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightObserver::CLightObserver()
{
	CLightAttributes::addObserver(this);
}

CLightObserver::~CLightObserver()
{
	CLightAttributes::removeObserver(this);
}	

CLight* CLightObserver::getLight(CGenericVector<float> location, unsigned int numLight)
{
	CLight **ppLights = CLightAttributes::getActiveLights();

	multiset<lightCompare,lightCompare>	sortedLights;
	for (unsigned int nLight = 0;nLight < CLightAttributes::MAX_LIGHTS;nLight++)
	{
		lightCompare lc;
		lc.light = ppLights[nLight];
		if (lc.light != NULL)
		{
			lc.intensity = lc.light->getLightIntensity(location);
			sortedLights.insert(lc);
		}
	}

	vector<CLight*>	activeLights;
	multiset<lightCompare,lightCompare>::const_iterator it = sortedLights.begin();
	while (it != sortedLights.end())
		activeLights.push_back((*it++).light);


    if (numLight >= activeLights.size())
        return NULL;
    else
		return activeLights[numLight];
}
