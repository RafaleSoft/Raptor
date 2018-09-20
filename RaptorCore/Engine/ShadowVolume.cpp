// ShadowVolume.cpp: implementation of the CShadowVolume class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_SHADOWVOLUME_H__D19B3347_87CF_48C0_87E5_3AF7925780C3__INCLUDED_)
    #include "ShadowVolume.h"
#endif
#if !defined(AFX_OBJECT3DSHADOW_H__A502A78F_0575_4F30_BCE2_0658B60EB490__INCLUDED_)
	#include "GLHierarchy/Object3DShadow.h"
#endif
#if !defined(AFX_IVIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "IViewPoint.h"
#endif
#if !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)
	#include "System/RaptorDisplay.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
    #include "Engine/3DEngineTaskManager.h"
#endif
#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif
#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "GLHierarchy/Object3DInstance.h"
#endif
#if !defined(AFX_OBJECT3DCONTAINERNOTIFIER_H__BF1EABCD_500E_4D7C_8561_2C535DF0640A__INCLUDED_)
    #include "Subsys/Object3DContainerNotifier.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShadowVolume::CShadowVolume(C3DScene& rScene)
	:CEnvironment(rScene),jobId(0)
{
	Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();
    C3DEngineTaskManager *taskManager = st.engineTaskMgr;
	jobId = taskManager->generateBatchId();

    m_shadowProperties.setCullFace(CRenderingProperties::ENABLE);
    m_shadowProperties.setTexturing(CRenderingProperties::DISABLE);
    m_shadowProperties.setLighting(CRenderingProperties::DISABLE);
    m_shadowProperties.clear(CGL_STENCIL);

    m_lightProperties.setTexturing(CRenderingProperties::DISABLE);
    m_lightProperties.setLighting(CRenderingProperties::DISABLE);
    m_lightProperties.setStencilTest(CRenderingProperties::ENABLE);
    m_lightProperties.clear(0);

	m_pObserver = new CObject3DContainerNotifier<CShadowVolume,CObject3D*>(*this,&CShadowVolume::notifyFromChild);
}

CShadowVolume::~CShadowVolume()
{
	if (m_pObserver != NULL)
		delete m_pObserver;
}

void CShadowVolume::notifyFromChild(CObject3D* child)
{
	if ((child != NULL) &&
        (m_pObserver->getEvent() == CContainerNotifier<CObject3D*>::DESTRUCTION))
	{
		vector<CObject3DShadow*>::iterator itr = m_pVolumes.begin();
		while (itr != m_pVolumes.end())
		{
			CObject3DShadow *pVolume = *itr;
			if (child == static_cast<CObject3DShadow*>(pVolume))
			{
				m_pVolumes.erase(itr);
				break;
			}
			itr++;
		}
	}
}

void CShadowVolume::clearShadowVolumes(void)
{
    if (!m_pVolumes.empty())
    {
#if defined(RAPTOR_SMP_CODE_GENERATION)
        Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();
        C3DEngineTaskManager *taskManager = st.engineTaskMgr;
        taskManager->cancelJobs();
#endif

        vector<CObject3DShadow*>::const_iterator itr = m_pVolumes.begin();
        CObject3DShadow *volume = NULL;
	    while (itr != m_pVolumes.end())
        {
            volume = (*itr++);
			volume->removeContainerNotifier(m_pObserver);
            delete volume;
        }
        m_pVolumes.clear();
    }
}



bool CShadowVolume::glInitEnvironment(unsigned int width,unsigned int height)
{
    return true;
}

void CShadowVolume::addObject(C3DSceneObject* object)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();
    C3DEngineTaskManager *taskManager = st.engineTaskMgr;
#endif

	// Compute all possible shadow volumes
    IViewPoint *vp = CRaptorDisplay::GetCurrentDisplay()->getViewPoint();
    float shadowExtrusion = C3DEngine::Get3DEngine()->getFarPlane();
    if (vp != NULL)
    {
        float vv[6];
        IViewPoint::VIEW_POINT_MODEL model;
        vp->getViewVolume(	vv[0],vv[1],vv[2],vv[3],vv[4],vv[5],model);
        shadowExtrusion = vv[5];
    }

	CObject3D *obj = object->getObject();
	if (obj->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()) ||
		obj->getId().isSubClassOf(C3DSet::C3DSetClassID::GetClassId()) ||
		obj->getId().isSubClassOf(CObject3DInstance::CObject3DInstanceClassID::GetClassId()))
	{
        if (obj->getProperties().isCastShadow())
        {
            CObject3DShadow *volume = new CObject3DShadow(obj,CObject3DShadow::SHADOW_VOLUME);
            volume->setShadowExtrusion(shadowExtrusion);
			volume->addContainerNotifier(m_pObserver);
            m_pVolumes.push_back(volume);

#if defined(RAPTOR_SMP_CODE_GENERATION)
            CEngineJob *job = volume->createJob(jobId);
            taskManager->registerJob(job);
#endif
        }
    }
}

void CShadowVolume::initVolumes(const vector<C3DSceneObject*>& objects)
{
    clearShadowVolumes();

#if defined(RAPTOR_SMP_CODE_GENERATION)
	Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();
    C3DEngineTaskManager *taskManager = st.engineTaskMgr;
#endif


    // Compute all possible shadow volumes
    IViewPoint *vp = CRaptorDisplay::GetCurrentDisplay()->getViewPoint();
    float shadowExtrusion = C3DEngine::Get3DEngine()->getFarPlane();
    if (vp != NULL)
    {
        float vv[6];
        IViewPoint::VIEW_POINT_MODEL model;
        vp->getViewVolume(	vv[0],vv[1],vv[2],vv[3],vv[4],vv[5],model);
        shadowExtrusion = vv[5];
    }

    vector<C3DSceneObject*>::const_iterator itr = objects.begin();
    CObject3DShadow *volume = NULL;
	while (itr != objects.end())
	{
		CObject3D *obj = (*itr++)->getObject();
		if (obj->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()) ||
			obj->getId().isSubClassOf(C3DSet::C3DSetClassID::GetClassId()) ||
			obj->getId().isSubClassOf(CObject3DInstance::CObject3DInstanceClassID::GetClassId()))
		{
            if (obj->getProperties().isCastShadow())
            {
                volume = new CObject3DShadow(obj,CObject3DShadow::SHADOW_VOLUME);
                volume->setShadowExtrusion(shadowExtrusion);
				volume->addContainerNotifier(m_pObserver);
                m_pVolumes.push_back(volume);

#if defined(RAPTOR_SMP_CODE_GENERATION)
                CEngineJob *job = volume->createJob(jobId);
                taskManager->registerJob(job);
#endif
            }
        }
    }
}


void CShadowVolume::glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects)
{
    if (!isEnabled())
    {
        C3DSceneObject::m_currentPass = C3DSceneObject::FULL_PASS;
        glRenderObjects(objects);
        return;
    }

    if (currentLight !=NULL) 
    {
		vector<CObject3DShadow*>::const_iterator itclip = m_pVolumes.begin();
		while (itclip != m_pVolumes.end())
		{
			CObject3DShadow* shadow = (*itclip++);
			shadow->setLight(const_cast<CLight*>(currentLight));
			shadow->glSelectContours();
			shadow->glClip();
		}
    }
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    else
    {
		Raptor::GetErrorManager()->generateRaptorError(	CObject3DShadow::CObject3DShadowClassID::GetClassId(),
													 	CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_WRONG_RENDERING);
    }
#endif

#if defined(RAPTOR_SMP_CODE_GENERATION)
    Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();
    C3DEngineTaskManager *taskManager = st.engineTaskMgr;
    taskManager->batchJobs(jobId);
#endif

    vector<C3DSceneObject*> receivers;
    vector<C3DSceneObject*> others;
    vector<C3DSceneObject*>::const_iterator itr = objects.begin();
    while (itr != objects.end())
    {
        C3DSceneObject* const h = *itr++;
		CObject3D* obj = h->getObject();

        bool cast = obj->getProperties().isCastShadow();
        bool receive = obj->getProperties().isReceiveShadow();

        if (receive)
            receivers.push_back(h);
        if (!receive && !cast)
            others.push_back(h);
    }

    //# glClearStencil(0);
    //  function lifted up to 128 to be able to render in OGL 1.0
    glClearStencil(128);
    m_shadowProperties.glPushProperties();

    //
    //	First pass : fill z-buffer
    //
	glDepthMask(GL_TRUE);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
    // Draw scene
    C3DSceneObject::m_currentPass = C3DSceneObject::DEPTH_PASS;
    glRenderObjects(receivers);

    //
    //	Second pass : fill stencil buffer
    //
	glEnable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

#if defined(RAPTOR_SMP_CODE_GENERATION)
    unsigned int remaining = taskManager->getNbJobsToProceed();
    //  If the GPU is much faster than the CPU, abort async jobs
    //  to continue in a synchronous way
    if (0 < remaining)
    {
		taskManager->cancelJobs();
    }
#endif

	//  Render volumes
    vector<CObject3DShadow*>::iterator it = m_pVolumes.begin();
    while (it != m_pVolumes.end())
    {
        CObject3DShadow* shadow = (*it++);
        shadow->glClipRender();
    }
    m_shadowProperties.glPopProperties();

    //
    //	Third pass : render shadowed objects
    //
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    m_lightProperties.setTexturing(CRenderingProperties::ENABLE);
    m_lightProperties.glPushProperties();
    //  function lifted up to 128 to be able to render in OGL 1.0
	//# glStencilFunc(GL_LESS,0,~0);
	glStencilFunc(GL_NOTEQUAL,128,~0);
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
	glDepthFunc(GL_EQUAL);
    //  Draw Scene
    C3DSceneObject::m_currentPass = C3DSceneObject::AMBIENT_PASS;
    glRenderObjects(receivers);
    m_lightProperties.glPopProperties();

    //
    //	Fourth pass : render lit objects
    //
    //  function lifted up to 128 to be able to render in OGL 1.0
	//# glStencilFunc(GL_EQUAL,0,~0);
	glStencilFunc(GL_EQUAL,128,~0);
    m_lightProperties.setLighting(CRenderingProperties::ENABLE);
    m_lightProperties.glPushProperties();
    // Draw Scene
    C3DSceneObject::m_currentPass = C3DSceneObject::LIGHT_PASS;
	glRenderObjects(receivers);

    glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	
    m_lightProperties.glPopProperties();
    m_lightProperties.setTexturing(CRenderingProperties::DISABLE);
    m_lightProperties.setLighting(CRenderingProperties::DISABLE);

    //
    //  Fifth pass : render other objects ( non shadowed & non caster )
    //
    if (others.size() > 0)
    {
        C3DSceneObject::m_currentPass = C3DSceneObject::FULL_PASS;
        glRenderObjects(others);
    }

#ifdef RAPTOR_DEBUG_MODE_GENERATION
    glColor4f(1.0f,0.0f,0.0f,0.4f);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	vector<CObject3DShadow*>::const_iterator itclip = m_pVolumes.begin();
    while (itclip != m_pVolumes.end())
    {
        CObject3DShadow* shadow = (*itclip++);
        shadow->glRender();
    }
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
#endif

}

void CShadowVolume::glRenderTexture(void)
{

}


