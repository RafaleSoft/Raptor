// LightObserver.h: interface for the CLightObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTOBSERVER_H__D6E2195C_21B4_42FB_ABA5_4219EECFF076__INCLUDED_)
#define AFX_LIGHTOBSERVER_H__D6E2195C_21B4_42FB_ABA5_4219EECFF076__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CLight;

//! This class defines an observer for light updates.
//! When an observer is registered to a light, a modification will call-back the update method.
//! This class ban be used manually to control more precisely objects' status update.
//! Raptor engine ( C3DScene ) use this class to manage local objet lighting, be beware not
//! to use too many light observers, bacause each light will trigger all it's observers.
class RAPTOR_API CLightObserver  
{
public:
    typedef enum
    {
        ACTIVATE,
        DEACTIVATE,
        POSITION,
        DIRECTION,
        COLOR,
        SPOT
    } UPDATE_KIND;

	virtual ~CLightObserver();

	//!	Must be implemented to handle light notification.
	//!	Whenever a lighting parameter change, this method 
	//!	is called.
	virtual void update(CLight *light,CLightObserver::UPDATE_KIND kind) = 0;

	//! Returns a light specified by numLight where numLight is the ordered light number.
    //! The order is defined by the relative intensity at location.
    CLight* getLight(CGenericVector<float> location, unsigned int numLight=0);


protected:
    CLightObserver();

private:
	//! This structure is used to sort the lights with their intensity:
    //!	provided the light's intensity has been computed for the same position
	//!	for all lights compared ( m_activeLights ), this structure is a simple 
	//!	helper to sort items.
    struct lightCompare
    {
        CLight	*light;
        float	intensity;

        bool operator()(const lightCompare &l, const lightCompare &r) const
        {
            return (l.intensity > r.intensity);
        };
    };
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_LIGHTOBSERVER_H__D6E2195C_21B4_42FB_ABA5_4219EECFF076__INCLUDED_)

