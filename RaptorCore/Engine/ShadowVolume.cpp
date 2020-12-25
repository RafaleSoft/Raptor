/***************************************************************************/
/*                                                                         */
/*  ShadowVolume.cpp                                                       */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
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
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
    #include "Engine/3DEngineTaskManager.h"
#endif
#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
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
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_OBJECT3DCONTOUR_H__C0C2B562_ABBC_4B04_A1E7_E0727FAC66AB__INCLUDED_)
	#include "GLHierarchy/Object3DContour.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShadowVolume::CShadowVolume(C3DScene& rScene)
	:CEnvironment(rScene),jobId(0), m_boxes(NULL), min_bound(SIZE_MAX), max_bound(0)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
    C3DEngineTaskManager *taskManager = instance.engineTaskMgr;
	jobId = taskManager->generateBatchId();

	m_shadowProperties.setCullFace(IRenderingProperties::ENABLE);
	m_shadowProperties.setTexturing(IRenderingProperties::DISABLE);
	m_shadowProperties.setLighting(IRenderingProperties::DISABLE);
    m_shadowProperties.clear(CGL_STENCIL);

	m_lightProperties.setTexturing(IRenderingProperties::DISABLE);
	m_lightProperties.setLighting(IRenderingProperties::DISABLE);
	m_lightProperties.setStencilTest(IRenderingProperties::ENABLE);
    m_lightProperties.clear(0);

	m_pObserver = new CObject3DContainerNotifier<CShadowVolume,CObject3D*>(*this,&CShadowVolume::notifyFromChild);
}

CShadowVolume::~CShadowVolume()
{
	if (m_pObserver != NULL)
		delete m_pObserver;
	if (NULL != m_boxes)
		delete[] m_boxes;
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
		CRaptorInstance &instance = CRaptorInstance::GetInstance();
        C3DEngineTaskManager *taskManager = instance.engineTaskMgr;
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



bool CShadowVolume::glInitialize(uint32_t width, uint32_t height)
{
    return true;
}

void CShadowVolume::addObject(C3DSceneObject* object)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
    C3DEngineTaskManager *taskManager = instance.engineTaskMgr;
#endif

	// Compute all possible shadow volumes
    IViewPoint *vp = CRaptorDisplay::GetCurrentDisplay()->getViewPoint();
    float shadowExtrusion = C3DEngine::Get3DEngine()->getFarPlane();
    if (vp != NULL)
    {
        float vv[6];
        IViewPoint::VIEW_POINT_MODEL model;
        vp->getViewVolume(vv[0],vv[1],vv[2],vv[3],vv[4],vv[5],model);
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

bool CShadowVolume::glInitEnvironment(const vector<C3DSceneObject*>& objects)
{
    clearShadowVolumes();
	
#if defined(RAPTOR_SMP_CODE_GENERATION)
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
    C3DEngineTaskManager *taskManager = instance.engineTaskMgr;
#endif

    // Compute all possible shadow volumes
    IViewPoint *vp = CRaptorDisplay::GetCurrentDisplay()->getViewPoint();
    float shadowExtrusion = C3DEngine::Get3DEngine()->getFarPlane();

	if (NULL != m_boxes)
		delete[] m_boxes;
	m_boxes = new GL_COORD_VERTEX[2 * objects.size()];

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

				uint64_t bbox = volume->getBoundingBoxIndex();
				min_bound = min(bbox, min_bound);
				max_bound = max(bbox, max_bound);
				
#if defined(RAPTOR_SMP_CODE_GENERATION)
                CEngineJob *job = volume->createJob(jobId);
                taskManager->registerJob(job);
#endif
            }
        }
    }

	return true;
}


void CShadowVolume::glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects)
{
    if (!isEnabled())
    {
        glRenderObjects(objects, C3DScene::FULL_PASS);
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
		RAPTOR_WARNING(	CObject3DShadow::CObject3DShadowClassID::GetClassId(),
						CRaptorMessages::ID_WRONG_RENDERING);
    }
#endif

#if defined(RAPTOR_SMP_CODE_GENERATION)
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
    C3DEngineTaskManager *taskManager = instance.engineTaskMgr;
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
    glRenderObjects(receivers, C3DScene::DEPTH_PASS);	// first render of set 'shadow receivers'

    //
    //	Second pass : fill stencil buffer with shadow volumes
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

	//!  Render volumes:
	//!  - first step = try a full render of the shadow
    vector<CObject3DShadow*>::iterator it = m_pVolumes.begin();
    while (it != m_pVolumes.end())
    {
        CObject3DShadow* shadow = (*it++);
        shadow->glClipRender();
    }
	//!  - second step = render the shadow bboxes in occlusion queries.
	glRenderBoxOcclusion();

    m_shadowProperties.glPopProperties();

    //
    //	Third pass : render shadowed objects
    //
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	m_lightProperties.setTexturing(IRenderingProperties::ENABLE);
    m_lightProperties.glPushProperties();
    //  function lifted up to 128 to be able to render in OGL 1.0
	//# glStencilFunc(GL_LESS,0,~0);
	glStencilFunc(GL_NOTEQUAL,128,~0);
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
	glDepthFunc(GL_EQUAL);
    //  Draw Scene : second render of set 'receivers', pass a empty list to reuse previous draw
	glRenderObjects(vector<C3DSceneObject*>(), C3DScene::AMBIENT_PASS);
    m_lightProperties.glPopProperties();

    //
    //	Fourth pass : render lit objects
    //
    //  function lifted up to 128 to be able to render in OGL 1.0
	//# glStencilFunc(GL_EQUAL,0,~0);
	glStencilFunc(GL_EQUAL,128,~0);
	m_lightProperties.setLighting(IRenderingProperties::ENABLE);
    m_lightProperties.glPushProperties();
    // Draw Scene : third render of set 'receivers', pass a empty list to reuse previous draw
	glRenderObjects(vector<C3DSceneObject*>(), C3DScene::LIGHT_PASS);

    glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	
    m_lightProperties.glPopProperties();
	m_lightProperties.setTexturing(IRenderingProperties::DISABLE);
	m_lightProperties.setLighting(IRenderingProperties::DISABLE);

    //
    //  Fifth pass : render other objects ( non shadowed & non caster )
    //
    if (others.size() > 0)
        glRenderObjects(others, C3DScene::FULL_PASS);

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

void CShadowVolume::glRenderBoxOcclusion(void)
{
	GLint dFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &dFunc);
	GLboolean cFace;
	glGetBooleanv(GL_CULL_FACE, &cFace);
	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);

	vector<CObject3DShadow*>::iterator it = m_pVolumes.begin();

#ifdef SHADOW_SHADERS
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	CShader *pShader = instance.m_pFilledBboxShader;
	
	while (it != m_pVolumes.end())
	{
		CObject3DShadow* shadow = (*it++);
		const std::vector<CObject3DContour*> &contours = shadow->getSelectedContours();
		const GL_COORD_VERTEX &min = contours[0]->getContourVolume().boxMin;
		const GL_COORD_VERTEX &max = contours[0]->getContourVolume().boxMax;

		//!	This method returns the bounding box index is current instance resources.
		uint64_t bbox = shadow->getBoundingBoxIndex();
		m_boxes[bbox] = min;
		m_boxes[bbox + 1] = max;
	}

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	float *dst = instance.boxes[min_bound];
	size_t sz = 2 * GL_COORD_VERTEX_STRIDE * m_pVolumes.size();
	pAllocator->glvkSetPointerData(dst, (float*)m_boxes, sz);

	CResourceAllocator::CResourceBinder *binder = instance.m_pBoxBinder;
	binder->glvkBindArrays();
	pShader->glRender();

	it = m_pVolumes.begin();
#endif
	 
	while (it != m_pVolumes.end())
	{
		CObject3DShadow* shadow = (*it++);
		shadow->glRenderBoxOcclusion();
	}

#ifdef SHADOW_SHADERS
	pShader->glStop();
	binder->glvkUnbindArrays();
#endif

	glEnable(GL_STENCIL_TEST);
	glDepthFunc(dFunc);
	if (cFace)
		glEnable(GL_CULL_FACE);
}

void CShadowVolume::glRenderTexture(void)
{

}


