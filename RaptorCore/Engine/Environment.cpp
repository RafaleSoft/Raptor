// Environment.cpp: implementation of the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_)
    #include "Environment.h"
#endif
#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "3DScene.h"
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

CEnvironment::~CEnvironment()
{
	if (m_pObserver != NULL)
	{
		delete m_pObserver;
		m_pObserver = NULL;
	}
}

void CEnvironment::glRenderObjects(const vector<C3DSceneObject*>& objects) const
{
	m_rScene.glRenderObjects(objects);
}

void CEnvironment::unLink(const CPersistence* child)
{
	if (child == static_cast<CPersistence*>(m_pObserver))
	{
		m_pObserver = NULL;
		unLinkEnvironment();
	}
}
