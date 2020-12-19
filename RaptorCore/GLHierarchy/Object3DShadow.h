/***************************************************************************/
/*                                                                         */
/*  Object3DShadow.h                                                       */
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


#if !defined(AFX_OBJECT3DSHADOW_H__A502A78F_0575_4F30_BCE2_0658B60EB490__INCLUDED_)
#define AFX_OBJECT3DSHADOW_H__A502A78F_0575_4F30_BCE2_0658B60EB490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Object3D.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CObject3D;
class CLight;
class CObject3DShadowAttributes;
class CEngineJob;
class CObject3DContour;


class RAPTOR_API CObject3DShadow : public CObject3D
{
public:
	typedef enum
	{
		SHADOW_PLANAR,                          //! A planar shadow is the projection of any object onto a plane
		SHADOW_BOUNDING_VOLUME,    //! A bounding volume is a shadow contour, not a shadow colume
		SHADOW_VOLUME,                         //! A full shadow volume
		SHADOW_NB_TYPE
	} SHADOW_TYPE;

public:
	CObject3DShadow(CObject3D *source,SHADOW_TYPE shadowType);
	virtual ~CObject3DShadow();


    //! Returns the shadow type ( only defined at creation ).
	SHADOW_TYPE getType() const { return m_type; };

	//!
	//!	Shadow configurations
	//!

	//!	Configures an easy light source for the shadow.
	//!	This method do not modify the light used by the 
	//!	method SetLight below because the shadow can exist
	//!	without any CLight object.
	void setLightPosition(float lpx,float lpy,float lpz);

	//!	This methods defines the light source for the shadow
	//!	It is not necessary to always use this method, the
	//!	later is sufficient in most cases. But if you define 
	//!	the light's position using a specific transform and
	//!	if you then define the shadow planar attributes with
	//!	another transform, this method will transpose the light's
	//!	position to the correct modeling transform. Otherwise, 
	//!	you will have to do it by yourself using the previous method
	//!	and a few mathematics.
	void setLight(CLight *light);

	//!	If the GPU is able to use ARB_imaging
	//!	 this method will define the shadow color 
	void setShadowColor(unsigned char cr,unsigned char cg,unsigned char cb,unsigned char ca);

	//!	Creates a shadow on the plane defined by :
	//!	- its normal : n
	//!	- a point belonging to the plane : o
	void setShadowPlane(CGenericVector<float> &o,CGenericVector<float> &n);

	//!	Same as above with :
	//!	- the normal of the plane is parallel to the vector eminating
	//!	from light position and going through object's center
	//!	- the point belonging to the plane is on the normal, at the
	//!	distance specified.
    //! Rq : the plane parameters are not updated when the light moves !
	void glSetShadowPlaneDistance(float distance);

    //! This methods defines volume extrusion for shadow contour and shadow volumes.
    //! ( not used in planar shadows )
    void setShadowExtrusion(float extrusion);

    //! This method creates an engine job to build shadows using SMP.    
    CEngineJob* createJob(unsigned int batchId);



	//!
	//!	Rendering methods
	//!

    //! This method is declared in DECLARE_OBJECT3D_MANIPULATORS
	//virtual void glRender(void);

	//!	Renders the planar shadow. Same as above axept that
	//! any object can be used.
	virtual void RAPTOR_FASTCALL glRender(CObject3D *source);

	//!	Renders the BBox of the shadow
	virtual void glRenderBBox(RENDER_BOX_MODEL filled = WIREFRAME);

	//!	Renders the BBox with occlusion query to clip the shadow
	bool glRenderBoxOcclusion(void);


    //!
	//!	Implements CObject3D, @see base class
    //! ( methods are not yet functionnal )
	//!
    DECLARE_OBJECT3D_NOMANIPULATORS

    //! See base class.
    //! This method is overridden because clipping is multipart for volumes :
    //! light cap, dark cap and sides can be clipped separately, so the base method
    //! cannot use a global clip result to optimise rendering of volumes.
    virtual void glClipRender(void) ;

    //! Specific clipping method to determine if the shadow intersects the frustum.
    //! This method is usefull because it computes light & dark cap visibilities.
    //! ( still need development : clipping of sides )
    virtual C3DEngine::CLIP_RESULT RAPTOR_FASTCALL glClip(void) const;

    //! This method select the apprpriate contours of the object that represent the shadow
    //! taking into account the current viewpoint with respect to object's position & orientation
    void glSelectContours(void);


	//! Implements CPersistence
	DECLARE_CLASS_ID(CObject3DShadowClassID,"Object3DShadow",CObject3D)



private:
	SHADOW_TYPE					m_type;
	CObject3DShadowAttributes	*m_pAttributes;
    vector<CObject3DContour*>   m_pVisibleContours;
    vector<CObject3DContour*>   m_pContours;

    //! Internal helper : initializes all contours and associated date, called from constructor.
    void initContours();

    //! This method computes the projection matrix applyied to the 
    //! object, so that its projection is the shadow on the virtual plane
    //! defined by the 2 vectors o and n.
    void buildPlanarShadow(CGenericVector<float> &o,CGenericVector<float> &n);


    //! This methods builds the shadow to be rendered in a second step. 
    //! This allows off-screen computing using SMP
    void buildShadow(CGenericMatrix<float> &transform);


    //!	Renders the shadow volume. It must be initialised using 
	//!	BuildShadowVolume first. Extrusion specifies the extrusion
	//!	distance in the opposite direction of the light source
	//!	from each point of the inner contour.
	//!	The whole volume will be rendered
	//!	Render method for shadow volume ( issued from the parent contour )
	//! ( extrusion < 0 means infinite shadow volume )
	void glRenderShadowVolume();
	void glRenderShaderShadowVolume();

    //!	Renders the contour of the shadow volume. It must be initialised using 
	//!	BuildShadowVolume first. Extrusion specifies the extrusion
	//!	distance in the opposite direction of the light source
	//!	from each point of the inner contour.
	//!	The planar projections of the shadow volume
	//!	wont be rendered, only the "sides", i.e. the extruded contour.
	void glRenderShadowContour();

    friend class CShadowVolumeJob;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OBJECT3DSHADOW_H__A502A78F_0575_4F30_BCE2_0658B60EB490__INCLUDED_)

