// ShadowMap.h: interface for the CShadowMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWMAP_H__996B1CFE_3445_4FB3_AE2B_D86E55BCE769__INCLUDED_)
#define AFX_SHADOWMAP_H__996B1CFE_3445_4FB3_AE2B_D86E55BCE769__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_)
    #include "Environment.h"
#endif
#ifndef __SIMD_H__
	#include "Subsys/SimdLib/simd.h"
#endif
#if !defined(AFX_OPENGLRENDERINGPROPERTIES_H__1F0F1E67_FC84_4772_A6EE_923BD81F91D3__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLRenderingProperties.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class IViewPoint;
class CRaptorDisplay;
class CTextureObject;
class CVertexShader ;
class CFragmentShader;
class CShadowMapPersistence;
class CPersistence;


//!
//!    This class implements a shadow mapping environment
//!
class CShadowMap :	public CEnvironment
{
public:
	CShadowMap(C3DScene& rScene);
	virtual ~CShadowMap();

	//!	Implements base class @see CEnvironment
    virtual ENVIRONMENT_KIND    getKind(void) const { return SHADOW_MAP; };

	//!	Implements base class @see CEnvironment
    virtual bool glInitEnvironment(unsigned int width,unsigned int height);

	//!	Implements base class @see CEnvironment
    virtual void glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects);

	//!	Implements base class @see CEnvironment
    virtual void glRenderTexture(void);


private:
	//! Implements CEnvironment
	virtual void unLink(const CPersistence *object);

	//! Implements CEnvironment
	virtual void unLinkEnvironment(void);


    void glRenderMap(const CLight* currentLight,const vector<C3DSceneObject*>& objects);

    void glRenderShadow(const vector<C3DSceneObject*>& objects);

	void glInitPixelBuffer(unsigned int width,unsigned int height);

	void glInitRenderBuffer(unsigned int width,unsigned int height);

	//! Implements CEnvironment
	virtual void addObject(C3DSceneObject* object);


	CGenericMatrix<float>		m_lightProjection;
	COpenGLRenderingProperties	m_lightProperties;

    IViewPoint		*m_pViewPoint;
    CRaptorDisplay	*m_pShadowMap;
    CTextureObject	*m_pShadowTexture;
    CVertexShader	*m_pVSShadowMap;
    CFragmentShader	*m_pFSShadowMap;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADOWMAP_H__996B1CFE_3445_4FB3_AE2B_D86E55BCE769__INCLUDED_)

