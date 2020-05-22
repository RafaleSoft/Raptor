/***************************************************************************/
/*                                                                         */
/*  3DScene.h                                                              */
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

#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
#define AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class C3DSceneAttributes;
class CLight;
class CObject3D;
class C3DSceneObject;
class CMirror;
class CVulkanCommandBuffer;
class CEnvironment;


class RAPTOR_API C3DScene : public CPersistence
{
public:
	C3DScene(const std::string& sceneName);
	virtual ~C3DScene();

	//!
	//!	Rendering
	//!----------
	//! An enum that defines a kind for a rendering pass
	typedef enum
	{
		DEPTH_PASS,
		AMBIENT_PASS,
		LIGHT_PASS,
		FULL_PASS,
	} PASS_KIND;


	//!	Renders the whole scene.
	virtual void glRender(void);
	virtual void vkRender(CVulkanCommandBuffer& commandBuffer);
	
    //! Defines the scene ambient intensity, which is multiplied by each object's material ambient.
    //! Global ambient is by default set for each display at 0.2, 0.2, 0.2, 1.0 (@see CRaptorDisplayConfig ).
    //! If global ambient is specified for a scene, it overrides the display ambient.
    //! To be used properly, CLights' ambient parameter can be set to 0,0,0,1 and objects' ambient set to 
    //! the same value as diffuse. Rendering aspect is then more easy to control.
    //! Any invalid value ( such as negatives ) disables scene global ambient.
    //! The result is : true if scene global ambient is used, false otherwise ( default ).
    bool useSceneGlobalAmbient(CColor::RGBA ambient);


	//!
	//!	Object management
	//!------------------

    //!	Use object z-sorting optimisation
	void useZSort(bool use = true);

    //! Adds generic object to the scene
    //! @return true if the object is correctly added to the scene.
    bool addObject(CObject3D *object);

    //!  Adds lights to the scene
    //! @return true if the light is correctly added to the scene..
    bool addLight(CLight *light);

    //! Adds a mirror to the scene
    //! @return true if the mirror is correctly added to the scene.
    bool addMirror(CMirror *mirror);


    //!
	//!	Environment parameters
	//!----------------------

    //! Manage environments : initialise an environment of the requested kind and dimensions ( for texturebinding )
    //! Rq : only one environment of each kind can be manage in a scene.
    bool glManageEnvironment(CEnvironment *pEnv);

    //! This method retrieves an environment of the requested kind, if it exists.
    //! It returns NULL if none has been managed yet.
	CEnvironment * const getEnvironment() const { return m_pEnvironment; };

	//!	Internal helper to draw objects using current settings.
	void glRenderObjects(const vector<C3DSceneObject*>& objects, C3DScene::PASS_KIND passKind);

    //!
    //! Persistence
    //!
    DECLARE_IO
	DECLARE_CLASS_ID(C3DSceneClassID,"Scene",CPersistence)


private:
	//! Forbid copy constructor
	C3DScene(const C3DScene&);

	//! Forbid assignment operator
	C3DScene& operator=(const C3DScene&);

	//!	Load a mirror object from xml RaptorData file.
    void importMirror(CRaptorIO& io);

	PASS_KIND				m_currentPass;

	CEnvironment			*m_pEnvironment;
	C3DSceneAttributes		*m_pAttributes;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)

