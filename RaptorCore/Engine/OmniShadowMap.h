// OmniShadowMap.h: interface for the COmniShadowMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OMNISHADOWMAP_H__FB391794_C7C1_404B_A146_061A62252C5D__INCLUDED_)
#define AFX_OMNISHADOWMAP_H__FB391794_C7C1_404B_A146_061A62252C5D__INCLUDED_

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

class CViewPoint;
class CRaptorDisplay;
class CTextureObject;
class CVertexShader ;
class CFragmentShader;


class COmniShadowMap : public CEnvironment  
{
public:
	COmniShadowMap(C3DScene& rScene);
	virtual ~COmniShadowMap();

    virtual ENVIRONMENT_KIND    getKind(void) const { return OMNI_SHADOW_MAP; };

    virtual bool glInitEnvironment(unsigned int width,unsigned int height);

    virtual void glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects);

    virtual void glRenderTexture(void);

private:
	//! Implements CEnvironment
	virtual void unLink(const CPersistence *object);

	//! Implements CEnvironment
	virtual void unLinkEnvironment(void);


	void glRenderMap(const CLight* currentLight,const vector<C3DSceneObject*>& objects);

    void glRenderShadow(const vector<C3DSceneObject*>& objects);


	CGenericMatrix<float>	m_lightProjection;

    CViewPoint*         m_pViewPoint;
    CRaptorDisplay*   m_pShadowCubeMap;
    CTextureObject*	  m_pShadowTexture;
    CVertexShader*     m_pVSShadowMap;
    CFragmentShader* m_pFSShadowMap;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OMNISHADOWMAP_H__FB391794_C7C1_404B_A146_061A62252C5D__INCLUDED_)

