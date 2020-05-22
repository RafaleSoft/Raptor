/***************************************************************************/
/*                                                                         */
/*  3DSceneAttributes.cpp                                                  */
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
#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
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


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSceneAttributes::C3DSceneAttributes(C3DScene *owner):
	m_pOwner(owner),m_pCurrentLight(NULL),
    m_bUseZSort(false),m_iCurrentPass(0),m_bQueriesReady(false),
    m_bUseGlobalAmbient(false),m_bMirrorsRendered(false),
	m_bDataPrepared(false),m_pSceneTree(NULL),
	maxboxes(0), numboxes(0), boxes(NULL),m_pBinder(NULL)
{
    m_ambient = CColor::RGBA(0.2f,0.2f,0.2f,1.0f);
}

C3DSceneAttributes::~C3DSceneAttributes()
{
	vector<C3DSceneObject*>::const_iterator it2 = m_pObjects.begin();
	while (it2 != m_pObjects.end())
		delete (*it2++);

    if (m_pSceneTree != NULL)
        delete m_pSceneTree;
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
    if (!m_bDataPrepared)
    {
        m_bDataPrepared = true;

		if (maxboxes <= numboxes)
		{
			maxboxes += 1024;

			// size is 2 coordinates * 4 floats per box, * maxboxes
			size_t sz = 2 * sizeof(GL_COORD_VERTEX) / sizeof(float);

			// TODO: allocate increqsed space and copy previous box buffers.
			CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
			bool bLocked = pAllocator->isMemoryLocked();
			if (bLocked)
				pAllocator->glvkLockMemory(false);
			if (NULL == boxes)
				boxes = (GL_COORD_VERTEX*)(pAllocator->allocateVertices(maxboxes * sz));
			if (bLocked)
				pAllocator->glvkLockMemory(true);

			if (NULL == m_pBinder)
			{
				m_pBinder = new CResourceAllocator::CResourceBinder();
				m_pBinder->setArray(CProgramParameters::POSITION, boxes);
				m_pBinder->useVertexArrayObjects();
			}
		}


        if ((m_pSceneTree == NULL) && m_bUseZSort)
        {
#if 0
			CKDTree<C3DSceneObject*> *pTree = new CKDTree<C3DSceneObject*>;
#else
            COctree<C3DSceneObject*> *pTree = new COctree<C3DSceneObject*>;
#endif
            vector<C3DSceneObject*>::const_iterator it = m_pObjects.begin();
            while (it != m_pObjects.end())
            {
				C3DSceneObject *sc = (*it++);
                CObject3D* obj = sc->getObject();
                pTree->addObject(obj,sc);

				if (sc->bbox > numboxes)
				{
					// bbox offset.
					sc->bbox = numboxes;
					numboxes += 2;

					float *dst = boxes[sc->bbox];

					float xmin, xmax, ymin, ymax, zmin, zmax;
					const CBoundingBox * const box = obj->boundingBox();

					box->get(xmin, ymin, zmin, xmax, ymax, zmax);
					GL_COORD_VERTEX src[2] = { {xmin,ymin,zmin,1.0f}, {xmax,ymax,zmax,1.0f} };

					CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
					size_t sz = 2 * sizeof(GL_COORD_VERTEX) / sizeof(float);
					pAllocator->glvkSetPointerData(dst, (float*)src, sz);
				}
            }

            pTree->compress();
			m_pSceneTree = pTree;
        }
    }

    m_iCurrentPass = 0;
    m_bMirrorsRendered = false;

    //  Apply ambient lighting
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