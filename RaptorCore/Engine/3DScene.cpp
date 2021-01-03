/***************************************************************************/
/*                                                                         */
/*  C3DScene.cpp                                                           */
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

#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_PROJECTOR_H__0AEE2092_215F_40FA_BBAE_7D8A2F5A482F__INCLUDED_)
    #include "GLHierarchy/Projector.h"
#endif
#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
    #include "Engine/3DEngineTaskManager.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "GLHierarchy/ObjectFactory.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif
#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
#endif
#if !defined(AFX_3DSCENEATTRIBUTES_H__8AA18F35_C0F4_40D6_A4EC_18DD32C20C87__INCLUDED_)
    #include "Subsys/3DSceneAttributes.h"
#endif
#if !defined(AFX_MIRROR_H__BA9C578A_40A8_451B_9EA3_C27CB04288FA__INCLUDED_)
    #include "Engine/Mirror.h"
#endif
#if !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
	#include "Subsys/Vulkan/VulkanDevice.h"
#endif
#if !defined(AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_)
	#include "Engine/Environment.h"
#endif



#include <set>
using std::set;


RAPTOR_NAMESPACE_BEGIN


static C3DScene::C3DSceneClassID sceneId;
const CPersistence::CPersistenceClassID& C3DScene::C3DSceneClassID::GetClassId(void)
{
	return sceneId;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
C3DScene::C3DScene(const std::string& sceneName)
	:CPersistence(sceneId,sceneName), m_currentPass(FULL_PASS), m_pEnvironment(NULL)
{
	m_pAttributes = new C3DSceneAttributes(this);
}

C3DScene::~C3DScene()
{
	if (NULL != m_pEnvironment)
        delete m_pEnvironment;

	m_pAttributes->glResetQueries();

	delete m_pAttributes;
}

void C3DScene::useZSort(bool use)
{
	m_pAttributes->m_bUseZSort = use;
}

bool C3DScene::useSceneGlobalAmbient(CColor::RGBA ambient)
{
    if ((ambient.r > 0.0f) && (ambient.r <= 1.0f) &&
        (ambient.g > 0.0f) && (ambient.g <= 1.0f) &&
        (ambient.b > 0.0f) && (ambient.b <= 1.0f))
    {
        m_pAttributes->m_bUseGlobalAmbient = true;
        m_pAttributes->m_ambient = ambient;
        return true;
    }
    else
    {
        m_pAttributes->m_bUseGlobalAmbient = false;
        m_pAttributes->m_ambient = CColor::RGBA(0.2f,0.2f,0.2f,1.0f);
        return false;
    }
}

bool C3DScene::addMirror(CMirror *mirror)
{
    if (mirror == NULL)
        return false;

    m_pAttributes->m_pMirrors.push_back(mirror);
    return true;
}

bool C3DScene::addLight(CLight* light)
{
    if (light == NULL)
        return false;

    m_pAttributes->m_pLights.push_back(light);
    return true;
}

bool C3DScene::addObject(CObject3D *object)
{
    if (object == NULL)
        return false;

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	const CBoundingBox * const box = object->boundingBox();
	if ((box->xMin() == box->xMax()) ||
		(box->yMin() == box->yMax()) ||
		(box->zMin() == box->zMax()))
	{
		RAPTOR_WARNING(	C3DScene::C3DSceneClassID::GetClassId(),
						CRaptorMessages::ID_NULL_OBJECT);
	}
#endif
	
	C3DSceneObject *sc = new C3DSceneObject(object);
	m_pAttributes->addObjet(sc);

	if (NULL != m_pEnvironment)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (m_pEnvironment->getKind() == CEnvironment::SHADOW_VOLUME)
		{
			RAPTOR_WARNING(	C3DScene::C3DSceneClassID::GetClassId(),
							CRaptorMessages::ID_UPDATE_FAILED);
		}
#endif
		m_pEnvironment->addObject(sc);
	}
	
    return true;
}

void C3DScene::glRenderObjects(const vector<C3DSceneObject*>& objects, PASS_KIND passKind)
{
	m_currentPass = passKind;

	//
	// Rendering 
	//  First step : prepare objects rendering sets.
	//
	if (!objects.empty())
		m_pAttributes->sortObjects(objects);

    //
	// Rendering 
    //  Second step : real objects of the scene
	//
    GLboolean proceedLights = true;
    glGetBooleanv(GL_LIGHTING,&proceedLights);
    proceedLights |= ((passKind == LIGHT_PASS) || (passKind == FULL_PASS));

	vector<C3DSceneObject*> occludedObjects;
	vector<C3DSceneObject*>::const_iterator itr = m_pAttributes->unsortedObjects.begin();
	while (itr != m_pAttributes->unsortedObjects.end())
	{
        C3DSceneObject* sc = *itr++;
        if (!sc->glRenderPass(	m_pAttributes->m_iCurrentPass,
								m_pAttributes->m_pLights,
								proceedLights))
			occludedObjects.push_back(sc);
	}

	if (!occludedObjects.empty())
		m_pAttributes->glComputeBBoxOcclusion(occludedObjects);

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	m_pAttributes->glRenderBBoxes(unsortedObjects);
#endif

	//
	// Rendering 
    //  Third step : render mirrors with real objects in the scene
	//
	m_pAttributes->glRenderMirrors(passKind);

    //
	// Rendering 
    //  Fourth step : render transparent objects last
    //  TODO : render from back to front
	//
    if (m_pAttributes->transparents.size() > 0)
    {
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        vector<CObject3D*>::const_iterator it = m_pAttributes->transparents.begin();
        while (it != m_pAttributes->transparents.end())
            (*it++)->glClipRender();
        
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
    }

    m_pAttributes->m_iCurrentPass++;

	CATCH_GL_ERROR
}

//!
//!	Main rendering
//!
void C3DScene::glRender(void)
{
    glPushMatrix();

#if defined(GL_ARB_occlusion_query)
    m_pAttributes->glMakeQueries();
#endif
   
	//!	Initialize rendering structures if not already available,
	//!	- clear intermdiate results, 
	//!	- reset all lights and lighting model.
	m_pAttributes->prepareData();

	//!	Extract the minimal set of objects to render.
    vector<C3DSceneObject*> viewableObjects = m_pAttributes->glGetObjects();
	//!	Determine required lights per object
    vector<CLight*>	requiredLights = m_pAttributes->glGetLights(viewableObjects);
	//!	Prepare light & material shading products
	m_pAttributes->glRenderLights(viewableObjects);

	//!	Render objects.
    if (NULL != m_pEnvironment)
    {
		m_pEnvironment->glRender(m_pAttributes->m_pCurrentLight,viewableObjects);
    }
    else
    {
        m_currentPass = FULL_PASS;
        glRenderObjects(viewableObjects, FULL_PASS);
    }

	glPopMatrix();
	
	//!
	//!	After effects:
	//!	- render lights glows & flares
	//!	- deactivate used lights
	int	blendSrc;
	int	blendDst;
	glGetIntegerv(GL_BLEND_SRC, &blendSrc);
	glGetIntegerv(GL_BLEND_DST, &blendDst);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT | GL_MULTISAMPLE_BIT_ARB);
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);
    for (unsigned int i=0;i<requiredLights.size();i++)
    {
		requiredLights[i]->glRenderEffects();
    }
	glDepthMask(GL_TRUE);
	glPopMatrix();
	glPopAttrib();
	glBlendFunc(blendSrc, blendDst);

	for (unsigned int i = 0; i<requiredLights.size(); i++)
		requiredLights[i]->glDeActivate();

	CATCH_GL_ERROR
}


void C3DScene::vkRender(CVulkanCommandBuffer& commandBuffer)
{
	const CVulkanDevice& rDevice = CVulkanDevice::getCurrentDevice();
	VkBuffer vertexBuffer = rDevice.getMemory()->getLockedBuffer(IDeviceMemoryManager::IBufferObject::VERTEX_BUFFER);
	VkBuffer indexBuffer = rDevice.getMemory()->getLockedBuffer(IDeviceMemoryManager::IBufferObject::INDEX_BUFFER);

	vector<C3DSceneObject*> viewableObjects = m_pAttributes->glGetObjects();
	for (unsigned int i = 0; i < viewableObjects.size(); i++)
		viewableObjects[i]->vkRender(commandBuffer, vertexBuffer, indexBuffer);
}


bool C3DScene::glManageEnvironment(CEnvironment *pEnv)
{
	if (NULL == pEnv)
		return false;

	if (NULL != m_pEnvironment)
	{
		if (pEnv->getKind() == m_pEnvironment->getKind())
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(	C3DScene::C3DSceneClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_OBJECT_DUPLICATE);
#endif
			return false;
		}
		else
			delete m_pEnvironment;
	}

	if (pEnv->glInitEnvironment(m_pAttributes->getAllObjects()))
	{
		m_pEnvironment = pEnv;
		return true;
	}
	else
	{
		m_pEnvironment = NULL;
		return false;
	}
}

bool C3DScene::exportObject(CRaptorIO& o)
{
	return true;
}

void C3DScene::importMirror(CRaptorIO& io)
{
    string name;
    io >> name;

    CMirror *mirror = new CMirror;
    GL_COORD_VERTEX planeNormal  = GL_COORD_VERTEX(0.0f, 1.0f, 0.0f, 1.0f);
    GL_COORD_VERTEX planePoint = GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 1.0f);

    string data = io.getValueName();
    while (io.hasMoreValues())
    {
        if (data == "Normal")
			io >> planeNormal ;
        else if (data == "Point")
            io >> planePoint;
        else if (data == "Object")
		{
			io >> name;
			data = io.getValueName();
			if (data == "name")
			{
				io >> name;
				CPersistence* object = CPersistence::FindObject(name);
				if (object != NULL)
				{
					const CPersistentObject &op = CObjectFactory::GetInstance()->getObject(object);
					mirror->addObject(op);
				}
			}
			io >> name;
		}
        else
			io >> name;

		data = io.getValueName();
    }
	io >> name;

    mirror->setMirrorPlane(planeNormal,planePoint);
    addMirror(mirror);
}

bool C3DScene::importObject(CRaptorIO& io)
{
    string name;
    io >> name;
    
    string data = io.getValueName();
    while (io.hasMoreValues())
    {
        if (data == "Object")
		{
			io >> name;
			data = io.getValueName();
			if (data == "name")
			{
				io >> name;
				CPersistence* object = CPersistence::FindObject(name);
				if (object != NULL)
				{
					const CPersistentObject &op = CObjectFactory::GetInstance()->getObject(object);
					addObject(op);
				}
			}
			io >> name;
		}
        else if (data == "Light")
		{
			io >> name;
			data = io.getValueName();
			if (data == "name")
			{
				io >> name;
				CPersistence* light = CPersistence::FindObject(name);
				if (light!= NULL)
					addLight((CLight*)light);
			}
			io >> name;
		}
        else if (data == "Mirror")
            importMirror(io);
		else if (data == "name")
			CPersistence::importObject(io);
		else
			io >> name;

		data = io.getValueName();
    }
	io >> name;

	return true;
}

