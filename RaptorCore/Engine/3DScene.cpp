// 3DScene.cpp: implementation of the C3DScene class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "3DScene.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_PROJECTOR_H__0AEE2092_215F_40FA_BBAE_7D8A2F5A482F__INCLUDED_)
    #include "GLHierarchy/Projector.h"
#endif

#ifndef __GLOBAL_H__
	#include "System/Global.h"
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
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif

#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
#endif
#if !defined(AFX_3DSCENEATTRIBUTES_H__8AA18F35_C0F4_40D6_A4EC_18DD32C20C87__INCLUDED_)
    #include "Subsys/3DSceneAttributes.h"
#endif
#if !defined(AFX_SHADOWMAP_H__996B1CFE_3445_4FB3_AE2B_D86E55BCE769__INCLUDED_)
    #include "ShadowMap.h"
#endif
#if !defined(AFX_OMNISHADOWMAP_H__FB391794_C7C1_404B_A146_061A62252C5D__INCLUDED_)
    #include "OmniShadowMap.h"
#endif
#if !defined(AFX_SHADOWVOLUME_H__D19B3347_87CF_48C0_87E5_3AF7925780C3__INCLUDED_)
    #include "ShadowVolume.h"
#endif
#if !defined(AFX_AMBIENTOCCLUSION_H__6BE8DC67_E93D_4BBB_B72E_A1A5F0263E5E__INCLUDED_)
    #include "AmbientOcclusion.h"
#endif
#if !defined(AFX_MIRROR_H__BA9C578A_40A8_451B_9EA3_C27CB04288FA__INCLUDED_)
    #include "Mirror.h"
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
	:CPersistence(sceneId,sceneName)
{
	m_pAttributes = new C3DSceneAttributes(this);
}

C3DScene::~C3DScene()
{
    vector<CEnvironment*>::const_iterator it = m_pAttributes->m_pEnvironments.begin();
    while (it != m_pAttributes->m_pEnvironments.end())
        delete (*it++);

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

    if (getEnvironment(CEnvironment::SHADOW_VOLUME) != NULL)
    {
        RAPTOR_WARNING(	C3DScene::C3DSceneClassID::GetClassId(),
						CRaptorMessages::ID_UPDATE_FAILED);
    }
#endif

	C3DSceneObject *sc = new C3DSceneObject(object);
	m_pAttributes->addObjet(sc);

    return true;
}

bool C3DScene::glAddObject(RAPTOR_HANDLE handle)
{
    if (handle.handle == 0)
        return false;

	if (handle.hClass == Global::COpenGLClassID::GetClassId().ID())
    {
        if (glIsList(handle.handle))
        {
            // This is the only valid case: a correct handle
            m_pAttributes->m_pHandles.push_back(handle);

            CATCH_GL_ERROR

            return true;
        }
        else
            return false;
    }
    else
    {
        // Assume the handle is correct.
        // Should raise a warning.
        m_pAttributes->m_pHandles.push_back(handle);
        return true;
    }
}
 

void C3DScene::glRenderObjects(	const vector<C3DSceneObject*>& objects)
{
    vector<CObject3D*>  transparents;
    vector<CObject3D*>  mirrors;
    vector<C3DSceneObject*>  unsortedObjects;
    set<C3DSceneObject*,C3DSceneObject::zorder>	sortedObjects;

    C3DSceneObject::PASS_KIND passKind = C3DSceneObject::m_currentPass;

    //! View point must be recomputed because engine modelview will be
    //! recomputed only on next viewpoint rendering.
    GL_MATRIX m;
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
        
    //
	// Sort objects from viewPoint
	//
	for (unsigned int i = 0; i<objects.size(); i++)
	{
		C3DSceneObject* const h = objects[i];
		CObject3D* obj = h->getObject();

        if (obj->getProperties().isTransparent())
            transparents.push_back(obj);
        //!    store mirrors apart to avoid a double render :
        //!    mirrors should only be added with a C3DScene::CMirror object.
        else if (obj->getProperties().isMirror())
            mirrors.push_back(obj);
        else 
        {
            if (m_pAttributes->m_bUseZSort)
            {
				const CBoundingBox * const bbox = obj->boundingBox();

				GL_COORD_VERTEX r_min;
				GL_COORD_VERTEX r_max;
				bbox->get(r_min,r_max,m);

				h->z_order = r_max.z;
                h->z_span = r_min.z;
				sortedObjects.insert(h);
            }
            else
                unsortedObjects.push_back(h);
        }
	}

    if (m_pAttributes->m_bUseZSort)
    {
        set<C3DSceneObject*,C3DSceneObject::zorder>::reverse_iterator ritr = sortedObjects.rbegin();
		while (ritr != sortedObjects.rend())
		{
			C3DSceneObject* sc = *ritr++;
            unsortedObjects.push_back(sc);
        }
    }

    //
	// Rendering 
    //  First step : undefined OGL display lists / Raptor handles
	//
    for (unsigned int i=0;i<m_pAttributes->m_pHandles.size();i++)
		glCallList(m_pAttributes->m_pHandles.at(i).handle);


    //
	// Rendering 
    //  Second step : real objects of the scene
	//
    GLboolean proceedLights = true;
    glGetBooleanv(GL_LIGHTING,&proceedLights);
    proceedLights |= ((passKind == C3DSceneObject::LIGHT_PASS) || (passKind == C3DSceneObject::FULL_PASS));

	vector<C3DSceneObject*> occludedObjects;
	vector<C3DSceneObject*>::const_iterator itr = unsortedObjects.begin();
	while (itr != unsortedObjects.end())
	{
        C3DSceneObject* sc = *itr++;
        if (!sc->glRenderPass(	m_pAttributes->m_iCurrentPass,
								m_pAttributes->m_pLights,
								proceedLights))
			occludedObjects.push_back(sc);
	}

	if (!occludedObjects.empty())
		C3DSceneObject::glComputeBBoxOcclusion(	m_pAttributes->m_iCurrentPass,
												occludedObjects);


	//
	// Rendering 
    //  Third step : render mirrors with real objects in the scene
	//
    bool proceedMirrors = ((passKind == C3DSceneObject::DEPTH_PASS) || (passKind == C3DSceneObject::FULL_PASS))
                        && (!m_pAttributes->m_bMirrorsRendered);
    for (unsigned int i=0;i<m_pAttributes->m_pMirrors.size();i++)
    {
        CMirror *pMirror = m_pAttributes->m_pMirrors[i];

        if (proceedMirrors)
        {
            m_pAttributes->m_iCurrentPass++;
            pMirror->glApplyMirror(true);

            GLboolean cMask[4];

            if (passKind != C3DSceneObject::FULL_PASS)
            {
                glGetBooleanv(GL_COLOR_WRITEMASK ,cMask);
                glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
            }

			occludedObjects.clear();
            itr = unsortedObjects.begin();
		    while (itr != unsortedObjects.end())
		    {
                C3DSceneObject* sc = *itr++;
		      
                if (!sc->glRenderPass(	m_pAttributes->m_iCurrentPass,
										m_pAttributes->m_pLights,
										true))
					occludedObjects.push_back(sc);
            }
			if (!occludedObjects.empty())
				C3DSceneObject::glComputeBBoxOcclusion(	m_pAttributes->m_iCurrentPass,
														occludedObjects);

            //  Transparent objects must also be rendered here if they are mirrored ?
            pMirror->glApplyMirror(false);

            if (passKind != C3DSceneObject::FULL_PASS)
                glColorMask(cMask[0],cMask[1],cMask[2],cMask[3]);
            
			pMirror->glClipRender();
            m_pAttributes->m_bMirrorsRendered = true;

			//	Need to deactivate lights because Objects above activated them
			for (unsigned int j = 0; j < m_pAttributes->m_pLights.size(); j++)
				m_pAttributes->m_pLights[j]->glDeActivate();
        }
        else if (((passKind == C3DSceneObject::AMBIENT_PASS) || (passKind == C3DSceneObject::LIGHT_PASS))
                    && (m_pAttributes->m_bMirrorsRendered))
        {
            pMirror->glClipRender();
        }
    }

    //
	// Rendering 
    //  Fourth step : render transparent objects last
    //  TODO : render from back to front
	//
    if (transparents.size() > 0)
    {
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        vector<CObject3D*>::const_iterator it = transparents.begin();
        while (it != transparents.end())
            (*it++)->glClipRender();
        
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
    }

    m_pAttributes->m_iCurrentPass++;

	CATCH_GL_ERROR
}


void C3DScene::glRender(void)
{
    glPushMatrix();

#if defined(GL_ARB_occlusion_query)
    m_pAttributes->glMakeQueries();
#endif
    m_pAttributes->prepareData();

    vector<C3DSceneObject*> viewableObjects = m_pAttributes->glGetObjects();
    vector<CLight*>	requiredLights = m_pAttributes->glGetLights(viewableObjects);

    unsigned int nbEnvs = m_pAttributes->m_pEnvironments.size();
    if (nbEnvs > 0)
    {
        for (unsigned int i=0;i<nbEnvs;i++)
        {
            CEnvironment *pEnv = m_pAttributes->m_pEnvironments[i];
            pEnv->glRender(m_pAttributes->m_pCurrentLight,viewableObjects);
        }
    }
    else
    {
        C3DSceneObject::m_currentPass = C3DSceneObject::FULL_PASS;
        glRenderObjects(viewableObjects);
    }

	glPopMatrix();
	
    for (unsigned int i=0;i<requiredLights.size();i++)
    {
        m_pAttributes->m_pLights[i]->glRenderGlow();
        m_pAttributes->m_pLights[i]->glRenderFlare();
        m_pAttributes->m_pLights[i]->glDeActivate();
    }

	CATCH_GL_ERROR
}


void C3DScene::vkRender(CVulkanCommandBuffer& commandBuffer,
						VkBuffer vertexBinding,
						VkBuffer indexBinding,
						VkBuffer uniformBinding)
{
	vector<C3DSceneObject*> viewableObjects = m_pAttributes->glGetObjects();
	for (unsigned int i = 0; i < viewableObjects.size(); i++)
		viewableObjects[i]->vkRender(commandBuffer, vertexBinding, indexBinding, uniformBinding);
}


CEnvironment * const C3DScene::getEnvironment(CEnvironment::ENVIRONMENT_KIND kind)
{
    CEnvironment * res = NULL;

    unsigned int nbEnvs = m_pAttributes->m_pEnvironments.size();

    if (nbEnvs > 0)
    {
        unsigned int pos = 0;
        while ((res == NULL) && (pos < nbEnvs))
        {
            CEnvironment * const pEnv = m_pAttributes->m_pEnvironments[pos++];
			if (NULL != pEnv)
				if (pEnv->getKind() == kind)
					res = pEnv;
        }
    }

    return res;
}

bool C3DScene::glManageEnvironment(CEnvironment::ENVIRONMENT_KIND kind,unsigned int width,unsigned int height)
{
    if (getEnvironment(kind) != NULL)
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	C3DScene::C3DSceneClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_OBJECT_DUPLICATE);
#endif
        return false;
    }

	CEnvironment* pEnv = NULL;

    switch(kind)
    {
        case CEnvironment::SHADOW_MAP:
        {
            pEnv = new CShadowMap(*this);
			pEnv->glInitEnvironment(width,height);
            break;
        }
        case CEnvironment::OMNI_SHADOW_MAP:
        {
            pEnv = new COmniShadowMap(*this);
			pEnv->glInitEnvironment(width,height);
            break;
        }
        case CEnvironment::SHADOW_VOLUME:
        {
            CShadowVolume* pSV = new CShadowVolume(*this);
			pSV->glInitEnvironment(width,height);
			pSV->initVolumes(m_pAttributes->getAllObjects());
            pEnv = pSV;
            break;
        }
		case CEnvironment::AMBIENT_OCCLUSION:
        {
			CAmbientOcclusion *pAO = new CAmbientOcclusion(*this);
			pAO->glInitEnvironment(width,height);
			pAO->initOcclusions(m_pAttributes->getAllObjects());
			pEnv = pAO;
			break;
		}
        default:
            return false;
    }

    m_pAttributes->m_pEnvironments.push_back(pEnv);
    return true;
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
    while (!data.empty())
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
    while (!data.empty())
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

