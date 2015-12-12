// ShadowVolume.h: interface for the CShadowVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWVOLUME_H__D19B3347_87CF_48C0_87E5_3AF7925780C3__INCLUDED_)
#define AFX_SHADOWVOLUME_H__D19B3347_87CF_48C0_87E5_3AF7925780C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_)
    #include "Environment.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CObject3DShadow;
class CShadowVolumePersistence;


//!
//!    This class implements a shadow volume environment
//!
class CShadowVolume : public CEnvironment
{
public:
	CShadowVolume(C3DScene&	rScene);
	virtual ~CShadowVolume();

	//! Implements base class
    virtual ENVIRONMENT_KIND    getKind(void) const { return SHADOW_VOLUME; };

	//! Implements base class
    virtual bool glInitEnvironment(unsigned int width,unsigned int height);

	//! Implements base class
    virtual void glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects);

	//! Implements base class
    virtual void glRenderTexture(void);

	//! Initialise all shadow volumes corresponding to the
	//! contours of each object passed in the list.
    void initVolumes(const vector<C3DSceneObject*>& objects);


private:
	//! Forbidden operators
    CShadowVolume(const CShadowVolume&);
    CShadowVolume& operator=(const CShadowVolume&);

	//!	Helper for destruction/initialisation: remove all volumes
    void clearShadowVolumes(void);

	//! The callback passed to the notifier
	void notifyFromChild(CObject3D* child);

	//! Implements CEnvironment
	virtual void addObject(C3DSceneObject* object);

	//! The job identifier wher using SMP
	unsigned int	jobId;

	//! The list of volumes and their sources
    vector<CObject3DShadow*>    m_pVolumes;

	//! A notifier observer to update the set when items are updated.
    CContainerNotifier<CObject3D*> *m_pObserver;

	//! Rendering propoerties for shadowed and lighted passes
    CRenderingProperties            m_shadowProperties;
    CRenderingProperties            m_lightProperties;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADOWVOLUME_H__D19B3347_87CF_48C0_87E5_3AF7925780C3__INCLUDED_)

