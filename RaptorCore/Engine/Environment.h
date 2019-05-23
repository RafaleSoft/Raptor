// Environment.h: interface for the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_)
#define AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __CGLTYPES_HPP__
    #include <System/CGLTypes.h>
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CLight;
class C3DSceneObject;
class C3DScene;


class RAPTOR_API CEnvironment
{
public:
    //! Available predefined Raptor environments
    //! More kinds are to come.
    typedef enum ENVIRONMENT_KIND_t
    {
        SHADOW_MAP,
        OMNI_SHADOW_MAP,
        SHADOW_VOLUME,
		AMBIENT_OCCLUSION
    } ENVIRONMENT_KIND;


public:
	CEnvironment(C3DScene&	rScene);
	virtual ~CEnvironment();

    virtual ENVIRONMENT_KIND    getKind(void) const = 0;

    //!    Enables environment rendering.
    //!     If enabled is true, environment is applyied, otherwise objects are simply rendered.
    void    enable(bool enabled) { m_enabled = enabled; };

    //!     Returns the enabled state of the environment.
    bool    isEnabled(void) const { return m_enabled; }

    //! Initialises environment data. 
    //! Specific to derived classes.
    virtual bool glInitEnvironment(unsigned int width,unsigned int height) = 0;

    //! Rendering entry point:
    //! - currentLight : the current active light for the current pass.
    //! - objects : the list of objects requested to use this environment
    virtual void glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects) = 0;

    //! TEMPORARY
    //! Renders the environment generated texture to the current TMU
    virtual void glRenderTexture(void) = 0;

	//! If the environnement is initialised with objects, 
	//!	a additional object can be passed to updade the environment.
    virtual void addObject(C3DSceneObject* object) = 0;


protected:
	//!	Environment sub classes shall use this rendering method
	//! to forward rendering to the current scene.
    void glRenderObjects(const vector<C3DSceneObject*>& objects) const;

	class CEnvironmentObserver : public CPersistence
	{
	public:
		CEnvironmentObserver(CEnvironment& owner)
			:CPersistence(CPersistence::CPersistenceClassID::GetClassId(), "EnvironmentObserver"),
			m_owner(owner)
		{};
		virtual ~CEnvironmentObserver()
		{
			m_owner.unLink(this);
		};

		//!	Notifies observers that this persistence is about to be destroyed.
		virtual void unLink(const CPersistence* child)
		{
			if (child != NULL)
				m_owner.unLink(child);
		};

	private:
		CEnvironment& m_owner;
	};

	//! Called from the observer (after unLink) to
	//! provoke a clean & destroy of all references of the environment.
	virtual void unLinkEnvironment(void) {};

	//! Environment sub classes shall handle unlink calls
	//!	from environmentObserver.
	virtual void unLink(const CPersistence* child);

	bool		m_enabled;
	C3DScene&	m_rScene;
	//! the observer cannot be a reference because
	//! when Raptor cleans out all persistences,
	//! it will become invalid.
	CEnvironmentObserver *m_pObserver;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_)

