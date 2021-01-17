/***************************************************************************/
/*                                                                         */
/*  Environment.cpp                                                        */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"


#if !defined(AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_)
    #include "Engine/Environment.h"
#endif
#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
#endif
#if !defined(AFX_SHADOWMAP_H__996B1CFE_3445_4FB3_AE2B_D86E55BCE769__INCLUDED_)
	#include "Engine/ShadowMap.h"
#endif
#if !defined(AFX_OMNISHADOWMAP_H__FB391794_C7C1_404B_A146_061A62252C5D__INCLUDED_)
	#include "Engine/OmniShadowMap.h"
#endif
#if !defined(AFX_SHADOWVOLUME_H__D19B3347_87CF_48C0_87E5_3AF7925780C3__INCLUDED_)
	#include "Engine/ShadowVolume.h"
#endif
#if !defined(AFX_AMBIENTOCCLUSION_H__6BE8DC67_E93D_4BBB_B72E_A1A5F0263E5E__INCLUDED_)
	#include "Engine/AmbientOcclusion.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnvironment::CEnvironment(C3DScene& rScene)
    :m_enabled(true),m_rScene(rScene),m_pObserver(NULL)
{
	m_pObserver = new CEnvironmentObserver(*this);
}

//! Environment factory.
CEnvironment *CEnvironment::glCreateEnvironment(C3DScene&	rScene,
												CEnvironment::ENVIRONMENT_KIND kind,
												uint32_t width, uint32_t height)
{
	CEnvironment *pEnv = NULL;

	switch (kind)
	{
		case CEnvironment::SHADOW_MAP:
		{
			CShadowMap *env = new CShadowMap(rScene);
			env->glInitialize(width, height);
			pEnv = env;
			break;
		}
		case CEnvironment::OMNI_SHADOW_MAP:
		{
			COmniShadowMap *env = new COmniShadowMap(rScene);
			env->glInitialize(width, height);
			pEnv = env;
			break;
		}
		case CEnvironment::SHADOW_VOLUME:
		{
			CShadowVolume* pSV = new CShadowVolume(rScene);
			pSV->glInitialize(width, height);
			pEnv = pSV;
			break;
		}
		case CEnvironment::AMBIENT_OCCLUSION:
		{
			CAmbientOcclusion *pAO = new CAmbientOcclusion(rScene);
			pAO->glInitialize(width, height);
			pEnv = pAO;
			break;
		}
		default:
			break;
	}

	return pEnv;
}

CEnvironment::~CEnvironment()
{
	if (m_pObserver != NULL)
	{
		delete m_pObserver;
		m_pObserver = NULL;
	}
}

void CEnvironment::glRenderObjects(	const vector<C3DSceneObject*>& objects,
									C3DScene::PASS_KIND passKind) const
{
	m_rScene.glRenderObjects(objects, passKind);
}

void CEnvironment::unLink(const CPersistence* child)
{
	if (child == static_cast<CPersistence*>(m_pObserver))
	{
		m_pObserver = NULL;
		unLinkEnvironment();
	}
}
