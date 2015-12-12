// AmbientOcclusion.h: interface for the CAmbientOcclusion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMBIENTOCCLUSION_H__6BE8DC67_E93D_4BBB_B72E_A1A5F0263E5E__INCLUDED_)
#define AFX_AMBIENTOCCLUSION_H__6BE8DC67_E93D_4BBB_B72E_A1A5F0263E5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_)
#include "Environment.h"
#endif
#ifndef __SIMD_H__
	#include "Subsys/SimdLib/simd.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CAmbientOcclusionShader;
class CPersistence;
class CShader;
class CShadedGeometry;
class CObject3DInstance;

class CAmbientOcclusion : public CEnvironment  
{
public:
	CAmbientOcclusion(C3DScene& rScene);
	virtual ~CAmbientOcclusion();

	//! Implements base class
    virtual ENVIRONMENT_KIND    getKind(void) const { return AMBIENT_OCCLUSION; };

	//! Implements base class
    virtual bool glInitEnvironment(unsigned int width,unsigned int height);

	//! Implements base class
    virtual void glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects);

	//! Implements base class
    virtual void glRenderTexture(void);

	//! Initialise all attibutes for real time ambient occlusion computations
    void initOcclusions(const vector<C3DSceneObject*>& objects);

	//!	Handle notifier callbacks.
	void notifyFromChild(CShader* child);


private:
	//! Implements CEnvironment
	virtual void unLink(const CPersistence *object);

	//! Implements CEnvironment
	virtual void unLinkEnvironment(void);

	//! Implements CEnvironment
	virtual void addObject(C3DSceneObject* object);

	//!	A helper method to preform a registration of a ShadedGeometry
	bool registerForAmbientOcclusion(CShadedGeometry* shaded,
									 CObject3DInstance* tSource);

	//!	The list of all compute shaders from all scene objects passed at initialisation
	vector<CAmbientOcclusionShader*> m_AOShaders;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_AMBIENTOCCLUSION_H__6BE8DC67_E93D_4BBB_B72E_A1A5F0263E5E__INCLUDED_)

