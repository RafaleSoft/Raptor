/***************************************************************************/
/*                                                                         */
/*  Object3D.h                                                             */
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


#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
#define AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/BoundingBox.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif
#if !defined(AFX_OBJECTPROPERTIES_H__1D80C74F_6792_4B6E_BF78_613553722315__INCLUDED_)
    #include "GLHierarchy/ObjectProperties.h"
#endif


//////////////////////////////////////////////////////////////////////
//!
//!	Some helper macros to quickly define a class derived from CObject3D
//!
//! DECLARE_OBJECT3D_MANIPULATORS declares the basic geometry manipulators :
//! translation, scaling, rotating and a generic rendering.
//!	As they are virtuals, they also must be overridden
//! All geometric manipulations are relative to the referential of 'self'
//!
//! DECLARE_OBJECT3D_NOMANIPULATORS only requires  a rendering
//! method and assumes no geometric manipulators are needed.


#define DECLARE_OBJECT3D_MANIPULATORS \
virtual void rotationX(float rx);\
virtual void rotationY(float ry);\
virtual void rotationZ(float rz);\
virtual void rotationX(float rx,float cy,float cz);\
virtual void rotationY(float ry,float cx,float cz);\
virtual void rotationZ(float rz,float cx,float cy);\
virtual void rotation(double angle,float ax,float ay,float az);\
virtual void scale(float sx,float sy,float sz);\
virtual void translateAbsolute(float tx,float ty,float tz);\
virtual void translate(float tx,float ty,float tz);\
virtual void transform(GL_MATRIX &m);\
virtual void glRender(void);

#define DECLARE_OBJECT3D_NOMANIPULATORS \
virtual void rotationX(float rx) {};\
virtual void rotationY(float ry) {};\
virtual void rotationZ(float rz) {};\
virtual void rotationX(float rx,float cy,float cz) {};\
virtual void rotationY(float ry,float cx,float cz) {};\
virtual void rotationZ(float rz,float cx,float cy) {};\
virtual void rotation(double angle,float ax,float ay,float az) {};\
virtual void scale(float sx,float sy,float sz) {};\
virtual void translateAbsolute(float tx,float ty,float tz) {};\
virtual void translate(float tx,float ty,float tz) {};\
virtual void transform(GL_MATRIX &m) {};\
virtual void glRender(void);
//
//////////////////////////////////////////////////////////////////////


RAPTOR_NAMESPACE_BEGIN

class CVulkanCommandBuffer;

class CObject3DContour;
template <class T> class CContainerNotifier;


//!		This class is not intended to work standalone
//!		because it is the parent class of all geometric 'drawable'
//!		Raptor object. It defines the behaviour and the
//!		necessary links to manage the object in a 3D hierarchy.
//!		This is an abstract class and all pure members
//!		should be completely implemented.
class RAPTOR_API CObject3D : public CPersistence
{
public:
	virtual ~CObject3D();

	//! Bounding Box management:
	//!	Each 3D Object has an internal aligned axis bounding box ( AABBox ) that
	//!	extends to all its child BBoxes if any.
	//!	This method returns the center of the object, which is the center of its AABBox.
	void getCenter(GL_COORD_VERTEX &center) const { BBox->getCenter(center); };

	//!	This method returns the bounding box dimensions through its
	//! minimum and maximum coordinates ( aligned axis coordinates )
    //! @see getCenter.
	void getBoundingBox(GL_COORD_VERTEX &Min,GL_COORD_VERTEX &Max) const { BBox->get(Min,Max); };

	//!	This method returns the bounding box
    //! @see getCenter.
	const CBoundingBox * const boundingBox(void) const { return BBox; };

	//!	This method returns the bounding box index is current instance resources.
	uint64_t getBoundingBoxIndex(void) const { return bbox; };

	//! Clipping hints:
	//!	This method returns true if the object intersects the viewing volume,
	//!	i.e. the object has to be rendered. ( a clip hint can be sent to
	//!	the GPU to disable clipping )
    virtual C3DEngine::CLIP_RESULT RAPTOR_FASTCALL glClip(void) const;

	//!	Clipping hints:
	//! This method use the current engine to clip object but without
	//! using the current glTransform. Instead, it uses the modelview registered in
    //! the current engine and multiplies with the supplied transform matrix before 
    //! computing clipping results. ( Engine's modelview is usually initialised by viewpoint )
    //! Objects then set the earlyClip hint in depth to prepare clipping off-rendering.
    //! @return true if clipping operation succeeded ( ealyClip is usable because not UNKNOWN )
	virtual bool RAPTOR_FASTCALL fullClip(const CGenericMatrix<float> &transform);

    static bool isEarlyClipEnabled(void);
    static void enableEarlyClip(bool enable);


	//!	Manipulators:
	//! rotation around object's X aligned axis going trhough its center
	virtual void rotationX(float rx)=0;

	//!	Manipulators:
	//! rotation around object's Y aligned axis going trhough its center
	virtual void rotationY(float ry)=0;

	//!	Manipulators:
	//! rotation around object's Z aligned axis going trhough its center
	virtual void rotationZ(float rz)=0;

	//!	Manipulators:
	//! Rotations around an arbitrary X aligned axis.
	virtual void rotationX(float rx,float cy,float cz)=0;

	//!	Manipulators:
	//! Rotations around an arbitrary Y aligned axis.
	virtual void rotationY(float ry,float cx,float cz)=0;

	//!	Manipulators:
	//! Rotations around an arbitrary Z aligned axis.
	virtual void rotationZ(float rz,float cx,float cy)=0;

	//!	Manipulators:
	//! Rotation around an arbitrary axis :
	//! - the angle of the rotation is angle
	//!	- the axis of rotation is a vector (ax,ay,az,1.0f)
	virtual void rotation(double angle,float ax,float ay,float az)=0;

	//!	Manipulators:
	//! Scales the object in each aligned axes direction :
	//!	- alpha for X
	//!	- beta for Y
	//!	- gamma for Z
	virtual void scale(float alpha,float beta,float gamma)=0;

	//!	Manipulators:
	//! TranslateAbsolute moves the object to the absolute position given
	//!	in the vector (alpha, beta, gamma, 1.0f). This position besomes the
	//! new center of the object and its bounding box.
	virtual void translateAbsolute(float alpha,float beta,float gamma)=0;

	//!	Manipulators:
	//!	Translate adds some increments to the X,Y and Z coordinates
	//! of the object so that :
	//!	- X becomes X + alpha
	//!	- Y becomes Y + beta
	//!	- Z becomes Z + alpha
	virtual void translate(float alpha,float beta,float gamma)=0;

	//!	Manipulators:
	//! This method performs an arbitrary transform using the matrix m.
	virtual void transform(GL_MATRIX &m)=0;

	//! Rendering:
	//!	This method renders the BBox of 'this' using a line geometry shader
	//!	or a triangle strip geometry shader ( in this case, the BBox looks a 
	//!	flat shaded parallelepipedic box )
	//!	A rendering model for boxes is used to render wire, filled or 
	//!	raw drawcall when pipeline is set for box rendering pass and render
	//!	all at once.
	typedef enum
	{
		WIREFRAME,
		FILLED,
		RAW
	} RENDER_BOX_MODEL;
	virtual void glRenderBBox(RENDER_BOX_MODEL filled = WIREFRAME);

	//! Rendering:
	//! This methods performs clipping before rendering. Easy to use for
	//!	a single object an it can avoid the rendering of a large hierarchy
	//!	but it is not suitable for a very efficient clipping technique.
	//!	Expect a 'scene' object to appear in C3DEngine object in future releases.
    virtual void glClipRender(void);

	//! Rendering:
	//!	And finally, after all this stuff, some drawings.
	//!	Better late than never !.
	//!	Derived classes should check the visibility flag
	//! to eventually skip rendering
	virtual void glRender(void) = 0;
	virtual void vkRender(CVulkanCommandBuffer& , VkBuffer ,VkBuffer ) {};

	//! Returns a generic pointer on the objet.
	//! Use the global CRaptorDisplay::Render(RAPTOR_HANDLE) to
	//! render any generic object.
	virtual operator RAPTOR_HANDLE() const;

    //! Returns the object properties. Method is not const to allow properties update
    CObjectProperties& getProperties(void) { return properties; };

    //! Creates the list of contours for this objet.
    //! Default implementation returns an empty list, derived classes must implement this method
    virtual vector<CObject3DContour*> createContours(void);


    //!
    //! Container notification
    //! These methods enable a C3DObject to notify a container that it has changed somehow.
    //! Any kind of container can be added, a simple notification method can forward an update request
    //! to all the containers registered.
    //!

    //! Sets a container notifier.
    //! @return false if the notifier already exists or if an error raised
    bool addContainerNotifier(CContainerNotifier<CObject3D*> *pNotifier);

    //! Removes a container notifier.
    //! @return false if the notifier does not exist or if an error raised
    bool removeContainerNotifier(CContainerNotifier<CObject3D*> *pNotifier);


    //! Inherited from CPersistence
    DECLARE_IO
	DECLARE_CLASS_ID(CObject3DClassID,"Object3D",CPersistence)



protected:
    //! The only available constructor for derived classes.
	CObject3D(	const CPersistence::CPersistenceClassID &,
				const std::string& name = "OBJECT3D");

    //! Assignment operator is permited for derived classes but must be used with caution
    //! when derived classes handle the bbox in a non standard way.
    CObject3D& operator=(const CObject3D& rsh);

    //! Moves the object to a new position. It is derived classes' responsibility to call this method properly
    void translateCenter(float tx,float ty,float tz);

    //! Moves the object to a new position. It is derived classes' responsibility to call this method properly
    void scaleBoundingBox(float sx,float sy,float sz);

    //! Defines new object's dimensions.
    void setBoundingBox(const GL_COORD_VERTEX &Min,const GL_COORD_VERTEX &Max);

    //!	Extend current dimensions. The resultant BBox contains is always larger and 
    //! contains min & max.
	void extendBoundingBox(const GL_COORD_VERTEX& min, const GL_COORD_VERTEX& max);

    //! Only notifies an update of the BBox ( needed to use notification enable/disable )
	void notifyBoundingBox(void);

    //! The object holds basic generic properties
    CObjectProperties   properties;

    //! This attribute maintain the result of early clip operations. Only Raptor can manage this attribute,
    //! do not use it or modify it, it's validity may not be available all time.
    C3DEngine::CLIP_RESULT  earlyClip;


private:
    //! Forbidden operators
    CObject3D();
    CObject3D(const CObject3D&);

	//!	This method updates boudiong box data in instence's buffer object.
	void CObject3D::glvkUpdateBBox(void);

    //! A 3D object must always have a bounding box ( pointer cannot be NULL )
	CBoundingBox	*BBox;

    //! The object also holds a container notifier.
    vector< CContainerNotifier<CObject3D*>* >  m_pNotifiers;

    //! A global status to indicate that objects are early clip aware.
    static bool earlyClipEnabled;

    //! These datas are identifiers to BBox values for GL server
	bool			updateBBox;
	
	//! BBox buffer object pointer.
	uint64_t		bbox;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)

