// LightAttributes.h: interface for the CLightAttributes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
#define AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_LIGHTOBSERVER_H__D6E2195C_21B4_42FB_ABA5_4219EECFF076__INCLUDED_)
	#include "GLHierarchy/LightObserver.h"
#endif
#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
	#include "GLHierarchy/ObjectReference.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureObject;
class CProjector;

class CLightAttributes  
{
public:
	//	Maximum number of physical simultaneous lights
	enum { MAX_LIGHTS = 8 };	// To Be Customized

	CLightAttributes();
	virtual ~CLightAttributes();

	//! Notifies observers using the event kind
    void notify(CLight* owner, CLightObserver::UPDATE_KIND kind);


    //!	Add a light observer to the global list of observers
    static bool addObserver(CLightObserver* observer);

    //! Remove an observer that was added with addObserver
	static bool removeObserver(CLightObserver* observer);

	//! Returns the list of current active lights
	static CLight** const getActiveLights(void) { return s_activeLights; };

	//! Returns the list of current active lights
	static bool* const getActiveGLLights(void) { return s_activeGLLights; };


private:
	CGenericVector<float>	m_position;
    CGenericVector<float>	m_viewPosition;
    CGenericVector<float>	m_eyePosition;
	CGenericVector<float>	m_direction;

	GLenum			m_hwMapping;
	bool			m_spot;
	GL_COORD_VERTEX	m_spotParams;
    float			m_dMax;
    float			m_threshold;

    typedef struct flare_item_t
    {
        CTextureObject	*pFlare;
        float			fSize;
        float			fDistance;
    } flare_item;
	vector<flare_item>   mFlares;
	unsigned int		m_uiFlare;

    CReference<CTextureObject>	pGlow;
    float				fGlowSize;
    unsigned int		m_uiGlow;
    bool				m_bRebuildGlow;

	CProjector			*pProjector;

    float				m_fLightVolumeSize;
    unsigned int		m_volumeVisibility;
    unsigned int		m_visibilityQuery;
    float				m_fLightVolumeVisibility;
	
    friend class CLight;

	static vector<CLightObserver*>	m_pObservers;
	static CLight* s_activeLights[MAX_LIGHTS];
	static bool s_activeGLLights[MAX_LIGHTS];
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)


