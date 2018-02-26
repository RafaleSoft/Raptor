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
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif


RAPTOR_NAMESPACE_BEGIN

CLight* CLightAttributes::s_activeLights[MAX_LIGHTS] =
{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

int CLightAttributes::s_lightOrder[MAX_LIGHTS] =
{ 0, 1, 2, 3, 4, 5, 6, 7 };

CLight* CLightAttributes::s_orderedLights[MAX_LIGHTS] =
{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };


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


void CLightAttributes::glActivate(CLight* owner, bool spot)
{
	m_spot = spot;

	if (m_hwMapping == 0)
	{
		m_hwMapping = GL_LIGHT0;

		while ((m_hwMapping <= GL_LIGHT7) && (glIsEnabled(m_hwMapping)))
			m_hwMapping++;

		if (m_hwMapping <= GL_LIGHT7)
			glEnable(m_hwMapping);
		else
			m_hwMapping = 0;
	}

	if (m_hwMapping != 0)
		s_activeLights[m_hwMapping - GL_LIGHT0] = owner;

	CATCH_GL_ERROR
}

void CLightAttributes::glDeActivate(void)
{
	if (0 != m_hwMapping)
	{
		s_activeLights[m_hwMapping - GL_LIGHT0] = NULL;
		glDisable(m_hwMapping);

		m_hwMapping = 0;
	}

	m_spot = false;

	CATCH_GL_ERROR
}

void CLightAttributes::notify(CLight* owner,CLightObserver::UPDATE_KIND kind)
{
    vector<CLightObserver*>::const_iterator itr = m_pObservers.begin();
    while (itr != m_pObservers.end())
    {
        (*itr++)->update(owner,kind);
    }
}

int* const CLightAttributes::getLightOrder(void)
{
	unsigned int lpos = 0;
	for (unsigned int i = 0; i < CLightAttributes::MAX_LIGHTS; i++)
	{
		s_lightOrder[i] = -1;

		// s_orderedLights is packed, but ensure the array is empty.
		if (NULL == s_orderedLights[i])
			continue;

		for (unsigned int j = 0; j < CLightAttributes::MAX_LIGHTS; j++)
		{
			if (s_activeLights[j] == s_orderedLights[i])
				s_lightOrder[lpos++] = j;
		}
	}

	return s_lightOrder;
};

void CLightAttributes::setLightOrder(CLight* lights[CLightAttributes::MAX_LIGHTS])
{
	unsigned int lpos = 0;
	for (unsigned int i = 0; i < CLightAttributes::MAX_LIGHTS; i++)
	{
		s_orderedLights[i] = NULL;
		CLight* l = lights[i];
		if ((NULL != l) && (l->isActive()))
			s_orderedLights[lpos++] = l;
	}
}
