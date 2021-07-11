// 3DSceneAttributes.h: interface for the C3DSceneAttributes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSCENEATTRIBUTES_H__8AA18F35_C0F4_40D6_A4EC_18DD32C20C87__INCLUDED_)
#define AFX_3DSCENEATTRIBUTES_H__8AA18F35_C0F4_40D6_A4EC_18DD32C20C87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif
#if !defined(AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_)
	#include "Subsys/ResourceAllocator.h"
#endif
#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CLight;
class C3DSceneObject;
class CObject3D;
class CShader;
template <class USER_DATA_t> class CBaseTree;

class C3DSceneAttributes
{
public:
    typedef struct zsort_object_t
    {
        GL_COORD_VERTEX min;
	    GL_COORD_VERTEX max;
        CObject3D*  obj;
    } zsort_object;

public:
	C3DSceneAttributes(C3DScene *owner);
	virtual ~C3DSceneAttributes();

    //! Prepare occlusion queries on each object
	void glMakeQueries(void);

    //! Release occlusion queries if they have been allocated.
    void glResetQueries(void);

    //! Prepare data structures : octrees, light observers and so on.
	void prepareData(void);

	//! Adds an object to owner scene.
	//!	Graphics pipeline is updated
	void addObjet(C3DSceneObject *sceneObject);

	//! Returns he appropriate list of objects extracted from octree )
	const std::vector<C3DSceneObject*> &getAllObjects(void) const { return m_pObjects; }

    //! Returns he appropriate list of objects extracted from octree )
    std::vector<C3DSceneObject*>	glGetObjects(void);

    //! Returns the appropriate list of lights that should apply to the list of objects
    std::vector<CLight*>	glGetLights(const std::vector<C3DSceneObject*>& objects);

	//! Renders light products with shader materials from the list of objects into shader buffers.
	void glRenderLights(const std::vector<C3DSceneObject*>& objects, bool proceedLights);

	//! This method computes initial occlusion using bbox rendering
	void glComputeBBoxOcclusion(const std::vector<C3DSceneObject*> &occluded);

	//!	This method sorts input objets into mirrors, 
	//! transparent and z_ordered objects ready for rendering.
	void C3DSceneAttributes::sortObjects(const std::vector<C3DSceneObject*>& objects);

	//! Renders scene mirrors.
	void glRenderMirrors(C3DScene::PASS_KIND passKind);


	//! This method is used for debugging purpose
	void glRenderBBoxes(const std::vector<C3DSceneObject*> &objects);


public:
    //! The owner Scene
    C3DScene        *m_pOwner;
    //! true if zsort of objects is used in C3DScene::glRenderObjects
	bool			m_bUseZSort;
    //! true if occlusion queries have been prepared
	bool			m_bQueriesReady;
    //! Current rendering pass. Up to 8 passes can be used in C3DSceneObject.
    //! There is no need to increase that figure or to make extensible in most cases.
    unsigned int	m_iCurrentPass;
    //! current global ambient color used in unlighted pass
    CColor::RGBA	m_ambient;
    //! true if global ambient is controlled by scene rather externally from user
    bool			m_bUseGlobalAmbient;

    //! Internal use only : true if mirror pass has been issued.
    bool			m_bMirrorsRendered;
	
	std::vector<CLight*>			m_pLights;
    std::vector<CMirror*>		m_pMirrors;
	CLight*					m_pCurrentLight;
	
	std::vector<CObject3D*>			transparents;
	std::vector<CObject3D*>			mirrors;
	std::vector<C3DSceneObject*>	unsortedObjects;

	CResourceAllocator::data_bloc	m_lightProductsShaderBuffer;
	CResourceAllocator::data_bloc	m_transformsShaderBuffer;
	
private:
	std::vector<C3DSceneObject*>		m_pObjects;
	CLight::R_LightProducts				*lightProducts;

    CBaseTree<C3DSceneObject*>			*m_pSceneTree;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DSCENEATTRIBUTES_H__8AA18F35_C0F4_40D6_A4EC_18DD32C20C87__INCLUDED_)

