// ViewPoint.h: interface for the CViewPoint class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_VIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
#define AFX_VIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif

#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "TimeObject.h"
#endif

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN
//
//	Predeclarations
//
class C3DPath;
class CObject3D;
class CLight;



class RAPTOR_API CViewPoint :	public CPersistence, 
								public CTimeObject
{
public:
	typedef enum
	{
		ORTHOGRAPHIC,
		PERSPECTIVE
	} VIEW_POINT_MODEL;

    typedef enum
    {
        EYE,
        TARGET
    } VIEW_POINT_POSITION;

public:
	CViewPoint(const std::string& name = "VIEW_POINT");
	virtual ~CViewPoint();

	//!	Set the viewer's eye position or target position.
    void setPosition(float x,float y,float z,VIEW_POINT_POSITION p);

	//!	Returns the value set above.
    CGenericVector<float> getPosition(VIEW_POINT_POSITION p) const;
	

	//!	Definition of the viewed region through clip planes and a projection:
	//!	- up to 6 planes : left, right, bottom, top, near and far.
	//!	- a volume model
	void setViewVolume(	float left,float right,
						float bottom, float up,
						float near, float far,
						VIEW_POINT_MODEL m);

    //! Retrieve the parameters set by SetViewVolume
    void getViewVolume(	float &left,float &right,
						float &bottom, float &up,
						float &near, float &far,
						VIEW_POINT_MODEL &m) const;

	//! Return the projection matrix corresponding to the view volume
	void getFrustum(CGenericMatrix<float, 4>& frustum) const;

	//!
	//!	Transforms
	//!
	//! Rotations around axis center
	void rotationX(float rx);
	void rotationY(float ry);
	void rotationZ(float rz);
	//! Translations
	void translate(float tx,float ty,float tz);
	//!	Scaling
	void scale(float sx,float sy,float sz);


	//!
	//!	Rendering:
	//!
	//! This method applies the viewpoint transform to the current modelview transform.
	//!	The modelview is initialized from identity, thus rendering is not cumulative 
	//!	with current modelview status.
	virtual void glvkRender(void);
	//! This method is added to render the 'projection' model
	//! of the viewpoint. It is not part of the generic 'Render'
	//! method because it should be called only once, except
	//! in somme specific case ( camera effects )
	virtual void glvkRenderViewPointModel(void);


	//!
	//!	Animations management
	//!
	//!	Set time relations for animation refresh
	void setTimeInterval(float tMin, float tMax);
	void setTimePos(float tpos);
	float getTimePos(void) const { return timePos; };
    //!  Object tracking is usefull for automated view point update
    //! ( e.g. in f.p.s. or car simulation, the view point is holded by a real volumic object )
    //! Rq: only one object at a time can be tracked !
    void trackObject(CObject3D *object, VIEW_POINT_POSITION p);
    void trackObject(CLight *object, VIEW_POINT_POSITION p);
	//!	Specify current path for viewpoint rendering.
	//!	- nPath is the index ( 0 based ) of the added path to set current.
	//!	- continus specifies that when a path reaches its end, viewpoint will continue to next path
	//!	- loop specifies that when the last path is rendered, viewpoint will start again from from the first.
	void setCurrentPath(int nPath,bool continus = true,bool loop = true);
    //! Returns the current active path
	int getCurrentPath(void) const { return currentPath; };
    //! This method adds a new path for the viewpoint. This path can be selected using
    //! the method setCurrentPath here above. Both eyePath and targetPath must not be NULL.
	void addPath(C3DPath *eyePositionPath,C3DPath *targetPath);


    //! Persistence I/O
	DECLARE_IO
	DECLARE_CLASS_ID(CViewPointClassID,"ViewPoint",CPersistence)



private:
    //!	Compute physics resultant after dt time increment
	virtual void RAPTOR_FASTCALL deltaTime(float dt);

    //! Helper to import 3DPaths
    void ImportPaths(CRaptorIO& io);

    //! Helper to precompute datas for rendering
    void recomputeViewPoint();


	//	basic datas
	VIEW_POINT_MODEL	model;
	float				viewVolume[6];

	//	path position management
	int			currentPath;
	float		timePos;	// current position between starttime & endtime
	float		startTime;	// first time position
	float		endTime;	// last time position

	//	viewpoint transformation
	CGenericVector<float>	Origin;		// Define a translation in initial view position
	CGenericVector<float>	Target;
	CGenericVector<float>	Scale;		// Define a scaling in final view position

	double		m_lfAlpha;
	double		m_lfBeta;
	double		m_lfGamma;
	double		m_lfLength;
	bool		m_bContinus;
	bool		m_bLoop;

	vector<C3DPath*>	eyePositionPaths;
	vector<C3DPath*>	targetPaths;


    CObject3D   *pEyeTrack;
    CObject3D   *pTargetTrack;
    CLight		*pEyeTrack2;
    CLight		*pTargetTrack2;

};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)

