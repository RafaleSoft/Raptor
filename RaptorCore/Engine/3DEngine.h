// 3DEngine.h: interface for the C3DEngine class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
#define AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"


#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN

// forward definitions
class C3DSet;
class CPhysics;
class CBoundingBox;
class CCubeBTree;
class CCollisionStatus;
class C3DEngineAttributes;
class C3DEngineTaskManager;
class CViewPoint;


class RAPTOR_API C3DEngine : public CPersistence  
{
public:
    typedef enum
    {
        CLIP_NONE,		// no area of the object need to be clipped : it is fully visible.
        CLIP_FULL,		// the whole object is clipped : it is invisible from current viewpoint.
        CLIP_PARTS,		// some parts of the object need to be clipped further : the object intersects viewport edges
        CLIP_UNKNOWN    // Clipping result is undefined ( uninitialised or impossible to determine )
    } CLIP_RESULT;

public:
	C3DEngine(const std::string& name = "3D_ENGINE");
	virtual ~C3DEngine();

	//! This method defines the unique active engine in the process	
	static void Set3DEngine(C3DEngine *Engine);

	//!	This method returns the unique engine
	static C3DEngine *Get3DEngine(void);

    //! This method computes the normal of a triangle built with the 3 vectors v1,v2 & v3.
    //! The result is returned in the dest parameter.
	static void TriangleNormal(	GL_COORD_VERTEX& v1,
								GL_COORD_VERTEX& v2,
								GL_COORD_VERTEX& v3,
								GL_COORD_VERTEX& dest);

	//!	Generate a rotation matrix :
	//!	- rotation of angle 'angle' ( degrees )
	//!	- around axis 'axis'
	//!	The result is computed in last parameter 'matrix'
	virtual void generateRotation(double angle,const CGenericVector<float> &axis,CGenericMatrix<float> &matrix) const;

	//!	3 dimension system solver:
	//!	A.x = b => compute b
	virtual void solve(const CGenericVector<float> &b,const CGenericMatrix<float> &A,CGenericVector<float> &x) const;

	//!	Computes the normal and the binormal
	//!	of the given vector
	//!	Assumption: normal 'vertical' coordinate is 0
	static void normals(GL_COORD_VERTEX &v,GL_COORD_VERTEX &normal,GL_COORD_VERTEX &binormal);
	static void normals(CGenericVector<float> &v,CGenericVector<float> &normal,CGenericVector<float> &binormal);


	//!	Converters : format is always function(dest &,src &)
	static void RAPTOR_FASTCALL Generic_to_MATRIX(GL_MATRIX &m,const CGenericMatrix<float> &gm);



	//!
	//!	engine visibility processing
	//!
	//!	Defines the clipping zone limit. By default, RaptorDisplays configure
	//! the engine properly. ( in window coordinates ).
    //! If you do not use all RaptorClasses, you can call it directly
	virtual void setClip(int xmin, int ymin, int xmax, int ymax);
    //!
    //! Defines the projection matrix for clipping or projective results.
    //! Raptor sets the projection properly when needed.
    //! If you do not use all RaptorClasses, you can call it directly
    virtual void setProjection(const CGenericMatrix<float> &projection);
    virtual CGenericMatrix<float> &getProjection(void) const;
    //!
	//! Defines the far plane for early clipping computations. This value will
	//!	be set by the viewpoint when needed, but another value can be set.
	void setFarPlane(float farPlane) const;
	//!
    //! Returns the far clipping plane, provided the projection matrix has been properly set.
    float getFarPlane(void) const;
	//!
	//!	Defines the clipping planes used for sphere clipping strategy
	void setClipPlanes(vector<GL_COORD_VERTEX> &planes) const;
	//!
	//!	This method is used to set 3D engine datas necessary to viewing operations
	//!	because computations such as clipping depend on viewpoint configuration.
	//!	@param pViewPoint : the viewpoint from datas can be queried, or NULL if no viewpoint is available.
	//!	It is not necessary to explicitely call this method except when there is no viewpoint
	//!	and a 3Dengine is used.
	void glConfigureEngine(CViewPoint* pViewPoint);


    //!
    //! Defines the modelview matrix jobengine tasks
    //! Raptor sets the projection properly when needed.
    //! If you do not use all RaptorClasses, you can call it directly
    virtual void setModelview(const CGenericMatrix<float> &modelview);
    virtual CGenericMatrix<float> &getModelview(void) const;
    //!
    //!	Computes the projection of the given vertex, according current OpenGL status
	virtual void glProject(GL_COORD_VERTEX &v) const;
	//!
	//!	Return true if the given bbox is visible on the viewport according OpenGL status.
    //! The second parameter controls the use of OpenGL modelview for the current call :
    //! if false, the current engine's modelview is used. If true, OGL modelview is rescanned.
	virtual CLIP_RESULT glClip(const CBoundingBox *const bbox,bool scanModelview,const CGenericMatrix<float> *transform) const;
	//!
	//!	Clipping method using bounding sphere ( transform is not necessary any more )
	virtual CLIP_RESULT glClip(const CBoundingBox *const bbox,bool scanModelview) const;

	//!
	//!	Initialise visibility computations from a point
	//! of view whose projection matrix is given in parameter.
	//!	( if projection is NULL, current OGL settings are used )
	//! Further versions will provide a camera.
	virtual void initVisibilitySet(CGenericMatrix<float> *projection);
	//!
	//! Compute the next visibility set with the current BBox
	//!
	virtual void addToVisibleSet(const CBoundingBox *bbox,CGenericMatrix<float> &boxTransform);

	virtual void resolveVisibility(void);
	virtual bool isVisible(const CBoundingBox* bbox);



	//!
	//!	engine collision processing
	//!
	void setCameraBBox(float xmin,float ymin,float zmin,float xmax,float ymax,float zmax);

	//!	adds obj to the collision management
	//!	( must be done each frame to handle movement
	//!	of objects )
	bool glAddForCollision(CPhysics *obj);

	//!	Returns true if engine is ready for collision management
	//!	( it will be free at every frame before any AddForCollision
	//!	is done )
	bool collisionReady(void) const;

	//!	clears collision engine : the whole BSP will be recomputed
	virtual void clearCollision(void);

	//!		Statistics
	static int GetNbCollisions();

	//! Inherited from CPersistence
	DECLARE_CLASS_ID(C3DEngineClassID,"Engine",CPersistence)


	//	Intersects 2 BBoxes,
	//	M is a matrix that transforms B2 coordinates
	//	into B1 coordinates space
	//	Intersection status is reported into 'status'
	static void IntersectBBox(const CBoundingBox *B1,const CBoundingBox *B2,GL_MATRIX &M,CCollisionStatus &status);
	//	Compute the status of the collision
	//	between two colliding BBoxes. ( the first box is given, the second 
	//	is transformed using the relative transforms between the 2 BBoxes
	//	and is passed as xmin & xmax, extreme corners )
	static void IntersectStatus(const CBoundingBox *Box,GL_COORD_VERTEX &min,GL_COORD_VERTEX &max,CCollisionStatus &status);
	//	Generate the forces issued from
	//	a collision between obj1 & obj2
	void collideObjects(CPhysics *obj1,CPhysics *obj2) const;

	// callbacks from engine processing
	virtual void onCameraCollision(CPhysics *obj) const;
	virtual void onObjectCollision(CPhysics *obj1,CPhysics *obj2) const;


protected:
	//	Bounding box of camera
	//	to compute near interactions
	CBoundingBox				*m_camera;
	CCubeBTree					*m_collision;
	
	C3DEngineAttributes* const getAttributes(void) const { return m_pAttributes; };

private:
	C3DEngineAttributes			*m_pAttributes;	
};



__inline void RAPTOR_FASTCALL C3DEngine::Generic_to_MATRIX(GL_MATRIX &m,const CGenericMatrix<float> &gm)
{
	memcpy(&m,gm.matrix(),sizeof(GL_MATRIX));
}

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)

