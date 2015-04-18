// LightAttributes.cpp: implementation of the CLightAttributes class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"


#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "LightAttributes.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif



RAPTOR_NAMESPACE_BEGIN

CLight* CLightAttributes::s_activeLights[MAX_LIGHTS] =
{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

bool CLightAttributes::s_activeGLLights[MAX_LIGHTS] =
{ false, false, false, false, false, false, false, false };

vector<CLightObserver*>	CLightAttributes::m_pObservers;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightAttributes::CLightAttributes()
{
	m_position.Zero();
	m_viewPosition.Zero();
	m_direction.Zero();

	m_spotParams[0] = 0;
	m_spotParams[1] = 0;
	m_spotParams[2] = 1;
	m_spotParams[3] = 180;

	m_hwMapping = 0;
	m_spot = false;

    m_dMax = FLT_MAX;
    m_threshold = 256.0f;

    fGlowSize = 0.0f;
    m_uiGlow = 0;
    m_bRebuildGlow = false;
	m_uiFlare = 0;

	pProjector = NULL;

    m_fLightVolumeSize = 0.0f;
    m_volumeVisibility = 0;
    m_visibilityQuery = 0;
    m_fLightVolumeVisibility = 0.0f;
}

CLightAttributes::~CLightAttributes()
{
#if defined(GL_ARB_occlusion_query)
	if (m_visibilityQuery != 0)
		Raptor::glGetExtensions()->glDeleteQueriesARB(1,&m_visibilityQuery);
#endif
}

bool CLightAttributes::addObserver(CLightObserver* observer)
{
	bool res = false;
	bool found = false;
	unsigned int pos = 0;

	while ((pos < m_pObservers.size()) && (!found))
	{
		found = (m_pObservers[pos] == observer);
		pos++;
	}

	if (!found)
	{
		m_pObservers.push_back(observer);
		res = true;
	}

	return res;
}

bool CLightAttributes::removeObserver(CLightObserver* observer)
{
	bool res = false;
	bool found = false;

	vector<CLightObserver*>::iterator it = m_pObservers.begin();
	while ((!found) && (it != m_pObservers.end()))
	{
		found = ((*it) == observer);
		it++;
	}

	if (found)
	{
		it--;
		m_pObservers.erase(it);
		res = true;
	}

	return res;
}


void CLightAttributes::notify(CLight* owner,CLightObserver::UPDATE_KIND kind)
{
	if (m_hwMapping != 0)
	{
		if (CLightObserver::ACTIVATE == kind)
		{
			s_activeLights[m_hwMapping - GL_LIGHT0] = owner;
			s_activeGLLights[m_hwMapping - GL_LIGHT0] = true;
		}
		else if (CLightObserver::DEACTIVATE == kind)
		{
			s_activeLights[m_hwMapping - GL_LIGHT0] = NULL;
			s_activeGLLights[m_hwMapping - GL_LIGHT0] = false;
		}
	}

    vector<CLightObserver*>::const_iterator itr = m_pObservers.begin();
    while (itr != m_pObservers.end())
    {
        (*itr++)->update(owner,kind);
    }
}

