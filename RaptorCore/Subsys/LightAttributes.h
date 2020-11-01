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


RAPTOR_NAMESPACE_BEGIN

class ITextureObject;
class CProjector;
class CLight;

class CLightAttributes  
{
public:
	//	Maximum number of physical simultaneous lights
	enum { MAX_LIGHTS = 8 };	// To Be Customized

	CLightAttributes();
	virtual ~CLightAttributes();

	//! Notifies observers using the event kind
    void notify(CLight* owner, CLightObserver::UPDATE_KIND kind);

	//!	Activates the light, assigning an available HW light id.
	void glActivate(CLight* owner, bool spot);

	//!	Deactivates the light, releasing the HW id.
	void glDeActivate(void);

	//!	ACtive and ordered light arrays
	typedef CLight* light_array[MAX_LIGHTS];
	typedef int light_order[MAX_LIGHTS];


	//!
	//!	Global lighting management:
	//!	maintain mapping between logical light and effective hardware light
	//!

    //!	Add a light observer to the global list of observers
    static bool addObserver(CLightObserver* observer);

    //! Remove an observer that was added with addObserver
	static bool removeObserver(CLightObserver* observer);

	//! Updates the order of active lights
	static void setLightOrder(CLight* lights[CLightAttributes::MAX_LIGHTS]);

	//! Returns the list of current active lights
	static CLight** const getActiveLights(void) { return s_activeLights; };

	//! Returns the packed ordered list of current active lights
	static CLight** const getOrderedLights(void) { return s_orderedLights; };

	//! Returns the packed list of active lights order
	static light_order const & getLightOrder(void);



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
	unsigned int	m_uiFlare;

    unsigned int		m_volumeVisibility;
    unsigned int		m_visibilityQuery;
    float				m_fLightVolumeVisibility;
	
    friend class CLight;

	static vector<CLightObserver*>	m_pObservers;
	static CLight* s_activeLights[MAX_LIGHTS];
	static CLight* s_orderedLights[MAX_LIGHTS];
	static int s_lightOrder[MAX_LIGHTS];
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)


