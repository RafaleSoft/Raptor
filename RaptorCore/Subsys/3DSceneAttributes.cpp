/***************************************************************************/
/*                                                                         */
/*  3DSceneAttributes.cpp                                                  */
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


#if !defined(AFX_3DSCENEATTRIBUTES_H__8AA18F35_C0F4_40D6_A4EC_18DD32C20C87__INCLUDED_)
    #include "Subsys/3DSceneAttributes.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_OCTREE_H__FC2A5101_AB9A_11D1_B467_444553540000__INCLUDED_)
    #include "Subsys/Octree.cxx"
#endif
#if !defined(AFX_KDTREE_H__2BD065C8_BF3F_4AA1_BD52_C072ADEC3A53__INCLUDED_)
	#include "Subsys/KDTree.cxx"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_MIRROR_H__BA9C578A_40A8_451B_9EA3_C27CB04288FA__INCLUDED_)
	#include "Engine/Mirror.h"
#endif
#if !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)
	#include "Subsys/UniformAllocator.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSceneAttributes::C3DSceneAttributes(C3DScene *owner):
	m_pOwner(owner),m_pCurrentLight(NULL),
    m_bUseZSort(false),m_iCurrentPass(0),m_bQueriesReady(false),
    m_bUseGlobalAmbient(false),m_bMirrorsRendered(false),
	m_pSceneTree(NULL), lightProducts(NULL)
{
    m_ambient = CColor::RGBA(0.2f,0.2f,0.2f,1.0f);
	m_lightProductsShaderBuffer.address = NULL;
	m_lightProductsShaderBuffer.size = 0;
	m_transformsShaderBuffer.address = NULL;
	m_transformsShaderBuffer.size = 0;
}

C3DSceneAttributes::~C3DSceneAttributes()
{
	vector<C3DSceneObject*>::const_iterator it2 = m_pObjects.begin();
	while (it2 != m_pObjects.end())
		delete (*it2++);

    if (m_pSceneTree != NULL)
        delete m_pSceneTree;

	if (NULL != lightProducts)
		delete[] lightProducts;

	if (m_lightProductsShaderBuffer.address != NULL)
	{
		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
		pUAllocator->releaseUniforms(m_lightProductsShaderBuffer.address);
		m_lightProductsShaderBuffer.address = NULL;
		m_lightProductsShaderBuffer.size = 0;
	}
	if (m_transformsShaderBuffer.address != NULL)
	{
		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
		pUAllocator->releaseUniforms(m_transformsShaderBuffer.address);
		m_transformsShaderBuffer.address = NULL;
		m_transformsShaderBuffer.size = 0;
	}
	
}

void C3DSceneAttributes::addObjet(C3DSceneObject *sceneObject)
{
	m_pObjects.push_back(sceneObject);
}

void C3DSceneAttributes::glMakeQueries(void)
{
#if defined(GL_ARB_occlusion_query)
	if (!m_bQueriesReady)
	{
		m_bQueriesReady = true;

		if (m_pObjects.size() < 1)
			return;

		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
        if (pExtensions->glGenQueriesARB != NULL)
        {
		    vector<C3DSceneObject*>::iterator itr = m_pObjects.begin();
		    while (itr != m_pObjects.end())
		    {
			    C3DSceneObject* h = *itr++;
				CObject3D* obj = h->getObject();

				if (obj->getProperties().getClippingMethod() == CObjectProperties::CLIP_BBOX_OCCLUSION)
				    pExtensions->glGenQueriesARB(C3DSceneObject::NB_PASSES,&h->visibilityQuery[0]);
		    }
        }

		CATCH_GL_ERROR
	}
#endif
}

void C3DSceneAttributes::glResetQueries(void)
{
#if defined(GL_ARB_occlusion_query)
    if (m_bQueriesReady)
	{
        m_bQueriesReady = false;

        const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
        if (pExtensions->glDeleteQueriesARB != NULL)
        {
		    vector<C3DSceneObject*>::iterator itr = m_pObjects.begin();
		    while (itr != m_pObjects.end())
		    {
			    C3DSceneObject* h = *itr++;
				
				//! Assume that if first element is a valid query, the following are also valid.
				if (pExtensions->glIsQueryARB(h->visibilityQuery[0]))
					pExtensions->glDeleteQueriesARB(C3DSceneObject::NB_PASSES,&h->visibilityQuery[0]);
		    }
        }

		CATCH_GL_ERROR
    }
#endif
}

void C3DSceneAttributes::prepareData(void)
{
	if ((m_pSceneTree == NULL) && m_bUseZSort)
    {
#if 0
		CKDTree<C3DSceneObject*> *pTree = new CKDTree<C3DSceneObject*>;
#else
		COctree<C3DSceneObject*> *pTree = new COctree<C3DSceneObject*>;
#endif
		std::vector<C3DSceneObject*>::const_iterator it = m_pObjects.begin();
		while (it != m_pObjects.end())
		{
			C3DSceneObject *sc = (*it++);
			CObject3D* obj = sc->getObject();
			pTree->addObject(obj,sc);
		}

		pTree->compress();
		m_pSceneTree = pTree;
	}

	//! Allocate light products buffer for uniform loading:
	//!	- enough storage to handle full object list rendering
	//!	- TODO: this allocation is not dynamic
	if (NULL == lightProducts)
	{
		lightProducts = new CLight::R_LightProducts[m_pObjects.size()];
	}

	//!	Determine the number of shaders for curent rendering
	size_t nb_shaders = 0;
	if ((NULL == m_lightProductsShaderBuffer.address) || (NULL == m_transformsShaderBuffer.address))
	{
		std::vector<CShader*>	pShaders;

		std::vector<C3DSceneObject*>::const_iterator it = m_pObjects.begin();
		while (it != m_pObjects.end())
		{
			C3DSceneObject *sc = (*it++);
			CObject3D* obj = sc->getObject();

			std::vector<CShader*> list;
			obj->getShaders(list);
			for (size_t i = 0; i < list.size(); i++)
			{
				CShader *pShader = list[i];
				if (pShader->hasShaderBloc())
					pShaders.push_back(list[i]);
			}
		}
		nb_shaders = pShaders.size();
	}

	//!	Allocate Uniform bloc for Shader Uniform blocs used for lighting
	if (NULL == m_lightProductsShaderBuffer.address)
	{
		uint64_t sz = nb_shaders * sizeof(CLight::R_LightProducts);
		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
		bool relocated = pUAllocator->isMemoryLocked();
		if (relocated)
			pUAllocator->glvkLockMemory(false);

		m_lightProductsShaderBuffer.address = pUAllocator->allocateUniforms(sz);
		m_lightProductsShaderBuffer.size = sz;
		
		if (relocated)
			pUAllocator->glvkLockMemory(true);
	}

	//!	Allocate Uniform bloc for Shader Uniform blocs used for transformation
	if (NULL == m_transformsShaderBuffer.address)
	{
		uint64_t sz = nb_shaders * 4 * sizeof(GL_MATRIX);
		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
		bool relocated = pUAllocator->isMemoryLocked();
		if (relocated)
			pUAllocator->glvkLockMemory(false);

		m_transformsShaderBuffer.address = pUAllocator->allocateUniforms(sz);
		m_transformsShaderBuffer.size = sz;

		if (relocated)
			pUAllocator->glvkLockMemory(true);
	}

    m_iCurrentPass = 0;
    m_bMirrorsRendered = false;

	transparents.clear();
	mirrors.clear();
	unsortedObjects.clear();

    //!  Apply ambient lighting
    if (m_bUseGlobalAmbient)
    {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_ambient);
        CShader::setAmbient(m_ambient.r,m_ambient.g,m_ambient.b,1.0f);
    }

    if (m_pLights.size() > 0)
    {
		m_pCurrentLight = m_pLights[0];
        for (unsigned int i=0;i<m_pLights.size();i++)
            m_pLights[i]->glProject(false);
    }
	else
		m_pCurrentLight = NULL;
}

void C3DSceneAttributes::sortObjects(const std::vector<C3DSceneObject*>& objects)
{
	transparents.clear();
	mirrors.clear();
	unsortedObjects.clear();
	
	//! View point must be recomputed because engine modelview will be
	//! recomputed only on next viewpoint rendering.
	GL_MATRIX m;
	glGetFloatv(GL_MODELVIEW_MATRIX, m);

	set<C3DSceneObject*, C3DSceneObject::zorder>	sortedObjects;

	//
	// Sort objects from viewPoint
	//
	for (unsigned int i = 0; i < objects.size(); i++)
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
			if (m_bUseZSort)
			{
				const CBoundingBox * const bbox = obj->boundingBox();

				GL_COORD_VERTEX r_min;
				GL_COORD_VERTEX r_max;
				bbox->get(r_min, r_max, m);

				h->z_order = r_max.z;
				h->z_span = r_min.z;
				sortedObjects.insert(h);
			}
			else
				unsortedObjects.push_back(h);
		}
	}

	if (m_bUseZSort)
	{
		set<C3DSceneObject*, C3DSceneObject::zorder>::reverse_iterator ritr = sortedObjects.rbegin();
		while (ritr != sortedObjects.rend())
		{
			C3DSceneObject* sc = *ritr++;
			unsortedObjects.push_back(sc);
		}
	}
}

vector<C3DSceneObject*>	C3DSceneAttributes::glGetObjects(void)
{
    // Temporary, until octree is not finished
    //if (m_pOctree != NULL)
    //    m_pOctree->glRender();
        //return m_pObjects;

    vector<C3DSceneObject*> result;

    if (m_pSceneTree == NULL)
        result = m_pObjects;
    else
    {
	    CGenericMatrix<float> Identity;
	    Identity.Ident();

        vector<CBaseTree<C3DSceneObject*>::TREE_ITEM_t> clip = m_pSceneTree->glClip(true);
        vector<CBaseTree<C3DSceneObject*>::TREE_ITEM_t>::const_iterator it = clip.begin();
        while (it != clip.end())
        {
            it->pObject->fullClip(Identity);
            result.push_back((*it++).userData);
        }
    }

    CATCH_GL_ERROR

    return result;
}

vector<CLight*>	C3DSceneAttributes::glGetLights(const vector<C3DSceneObject*>& objects)
{
	map<CLight*,CLight*> uniqueLights;

    CGenericMatrix<float> T = C3DEngine::Get3DEngine()->getModelview();
	T.Transpose();

    vector<C3DSceneObject*>::const_iterator it = objects.begin();
    while (it != objects.end())
    {
		C3DSceneObject* const h = (*it++);

        h->selectLights(m_pLights,T);
        for (unsigned int i=0;i<CLightAttributes::MAX_LIGHTS;i++)
        {
            CLight *pLight = h->effectiveLights[i];
            if (pLight != NULL)
                uniqueLights[pLight] = pLight;
        }
    }

	vector<CLight*> result;
    map<CLight*,CLight*>::const_iterator itm = uniqueLights.begin();
    while (itm != uniqueLights.end())
        result.push_back((*itm++).second);

    return result;
}

void C3DSceneAttributes::glRenderLights(const std::vector<C3DSceneObject*>& objects, bool proceedLights)
{
	uint64_t offset = 0;

	vector<C3DSceneObject*>::const_iterator it = objects.begin();
	while (it != objects.end())
	{
		C3DSceneObject* const h = (*it++);
		size_t nb_shaders = h->glRenderLights(lightProducts, offset, m_lightProductsShaderBuffer.address, proceedLights);
		if (nb_shaders > 0)
			offset += nb_shaders;
	}

	CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
	if (proceedLights)
		pUAllocator->glvkSetPointerData(m_lightProductsShaderBuffer.address, 
										(uint8_t*)lightProducts,
										offset * sizeof(CLight::R_LightProducts));
	else	// Store only one empty bloc if no lighting.
	{
		for (int j = 0; j < 5; j++)
			lightProducts[0].lights[j].enable = 0;
		lightProducts[0].scene_ambient = CShader::getAmbient();
		pUAllocator->glvkSetPointerData(m_lightProductsShaderBuffer.address,
										(uint8_t*)lightProducts,
										sizeof(CLight::R_LightProducts));
	}
}

void C3DSceneAttributes::glComputeBBoxOcclusion(const vector<C3DSceneObject*> &occluded)
{
#if defined(GL_ARB_occlusion_query)
	//  actual values should be 'get' and then restored after bbox is rendered
	GLboolean cMask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, cMask);
	GLboolean dMask;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &dMask);
	GLboolean sMask;
	glGetBooleanv(GL_STENCIL_TEST, &sMask);
	GLint dFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &dFunc);
	GLboolean cFace;
	glGetBooleanv(GL_CULL_FACE, &cFace);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glDepthMask(GL_FALSE);
	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_LESS);
	if (!cFace)
		glEnable(GL_CULL_FACE);

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	CShader *pShader = instance.m_pFilledBboxShader;

	CResourceAllocator::CResourceBinder *binder = instance.m_pBoxBinder;
	binder->glvkBindArrays();
	pShader->glRender();
	
	vector<C3DSceneObject*>::const_iterator itr = occluded.begin();
	while (itr != occluded.end())
	{
		C3DSceneObject* sc = *itr++;
		sc->glRenderBBoxOcclusion(m_iCurrentPass);

		instance.iRenderedObjects++;
		instance.iRenderedTriangles += 12;
	}

	pShader->glStop();
	binder->glvkUnbindArrays();

	glDepthMask(dMask);
	glColorMask(cMask[0], cMask[1], cMask[2], cMask[3]);
	if (sMask)
		glEnable(GL_STENCIL_TEST);
	glDepthFunc(dFunc);
	if (!cFace)
		glDisable(GL_CULL_FACE);

	CATCH_GL_ERROR
#endif
}

void C3DSceneAttributes::glRenderBBoxes(const vector<C3DSceneObject*> &objects)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	CShader *pShader = instance.m_pWiredBboxShader;

	CResourceAllocator::CResourceBinder *binder = instance.m_pBoxBinder;
	binder->glvkBindArrays();
	pShader->glRender();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	vector<C3DSceneObject*>::const_iterator itr = objects.begin();
	while (itr != objects.end())
	{
		C3DSceneObject* sc = *itr++;
		CObject3D* obj = sc->getObject();

		obj->glRenderBBox(CObject3D::RAW);

		instance.iRenderedObjects++;
		instance.iRenderedTriangles += 12;
	}

	pShader->glStop();
	binder->glvkUnbindArrays();

	CATCH_GL_ERROR
}

void C3DSceneAttributes::glRenderMirrors(C3DScene::PASS_KIND passKind)
{
	bool proceedMirrors = ((passKind == C3DScene::DEPTH_PASS) || (passKind == C3DScene::FULL_PASS)) && !m_bMirrorsRendered;
	
	for (unsigned int i = 0; i < m_pMirrors.size(); i++)
	{
		CMirror *pMirror = m_pMirrors[i];

		if (proceedMirrors)
		{
			m_iCurrentPass++;
			pMirror->glApplyMirror(true);

			GLboolean cMask[4];

			if (passKind != C3DScene::FULL_PASS)
			{
				glGetBooleanv(GL_COLOR_WRITEMASK, cMask);
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			}

			glRenderLights(unsortedObjects, true);

			vector<C3DSceneObject*> occludedObjects;
			vector<C3DSceneObject*>::const_iterator itr = unsortedObjects.begin();
			while (itr != unsortedObjects.end())
			{
				C3DSceneObject* sc = *itr++;

				if (!sc->glRenderPass(m_iCurrentPass, m_pLights, true))
					occludedObjects.push_back(sc);
			}
			if (!occludedObjects.empty())
				glComputeBBoxOcclusion(occludedObjects);

			//  Transparent objects must also be rendered here if they are mirrored ?
			pMirror->glApplyMirror(false);

			if (passKind != C3DScene::FULL_PASS)
				glColorMask(cMask[0], cMask[1], cMask[2], cMask[3]);

			pMirror->glClipRender();
			m_bMirrorsRendered = true;

			//	Need to deactivate lights because Objects above activated them
			for (unsigned int j = 0; j < m_pLights.size(); j++)
				m_pLights[j]->glDeActivate();
		}
		else if (((passKind == C3DScene::AMBIENT_PASS) || (passKind == C3DScene::LIGHT_PASS)) && m_bMirrorsRendered)
		{
			pMirror->glClipRender();
		}
	}
}