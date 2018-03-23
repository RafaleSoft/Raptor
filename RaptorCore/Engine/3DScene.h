// 3DScene.h: interface for the C3DScene class.
//
//////////////////////////////////////////////////////////////////////

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
#if !defined(AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_)
    #include "Engine/Environment.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class C3DSceneAttributes;
class CLight;
class CObject3D;
class CRenderingProperties;
class C3DSceneObject;
class CMirror;
class CVulkanCommandBuffer;


class RAPTOR_API C3DScene : public CPersistence
{
public:
	C3DScene(const std::string& sceneName);
	virtual ~C3DScene();

	//!
	//!	Rendering
	//!----------

	//!	Renders the whole scene.
	virtual void glRender(void);
	virtual void vkRender(	CVulkanCommandBuffer& commandBuffer,
							VkBuffer vertexBinding,
							VkBuffer indexBinding);
	
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

    //! Adds an opaque pointer to the scene 
    //! ( usable for OGL display lists for example, and in this cas it is checked ).
    //! @return true if the handle is correctly added to the scene.
    bool glAddObject(RAPTOR_HANDLE handle);

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
    bool glManageEnvironment(CEnvironment::ENVIRONMENT_KIND kind,unsigned int width,unsigned int height);

    //! This method retrieves an environment of the requested kind, if it exists.
    //! It returns NULL if none has been managed yet.
    CEnvironment * const getEnvironment(CEnvironment::ENVIRONMENT_KIND kind);

	//!	Internal helper to draw objects using current settings.
	void glRenderObjects(const vector<C3DSceneObject*>& objects);

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

    void importMirror(CRaptorIO& io);

	C3DSceneAttributes		*m_pAttributes;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)

