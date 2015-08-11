// SSE_BoundingBox.h: interface for the SSE_BoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_)
#define AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericRay.h"


class SSE_CBoundingBox
{
public:
	SSE_CBoundingBox();
	// boundingbox is used in operations, so we want to register
	//	the object in the persistence
	SSE_CBoundingBox(const SSE_CBoundingBox &r_box);	
	virtual ~SSE_CBoundingBox();
	virtual unsigned int Size() const { return sizeof(SSE_CBoundingBox); };

	
	float XMin(void) const { return xmin; };
	float YMin(void) const { return ymin; };
	float ZMin(void) const { return zmin; };
	float XMax(void) const { return xmax; };
	float YMax(void) const { return ymax; };
	float ZMax(void) const { return zmax; };

	void Set(CGenericVector<float>& r_min,CGenericVector<float>& r_max);
	void Set(float xmin,float ymin,float zmin,float xmax,float ymax,float zmax);

	void Get(CGenericVector<float>& r_min,CGenericVector<float>& r_max) const;
	void Get(float& r_xmin,float& r_ymin,float& r_zmin,float& r_xmax,float& r_ymax,float& r_zmax) const;

	//	Adding a bounding extends this to the minimum bounding box
	//	that contains this and r_box
	void Add(const SSE_CBoundingBox * const r_box);
	//	Extends this so that the point r_x,r_y,r_z is
	//	contained in this
	void ExtendTo(float r_x,float r_y,float r_z);

	//	Geometrical center or the bbox ( necessary to objects
	//	to perform centered rotations
	void GetCenter(CGenericVector<float> &r_center) const ;
	void GetCenter(float& r_x,float& r_y,float& r_z) const;

	SSE_CBoundingBox operator+(const SSE_CBoundingBox& r_box);
	SSE_CBoundingBox& operator=(const SSE_CBoundingBox& r_box);
	SSE_CBoundingBox& operator+=(const SSE_CBoundingBox& r_box);
	bool operator==(const SSE_CBoundingBox& r_box);

	//	Intersect generates the minimum BBox containing
	//	the intersection of this and r_box
	SSE_CBoundingBox Intersect(SSE_CBoundingBox& r_box ) const;
	void Intersect(SSE_CBoundingBox& r_box );

	void Scale(float sx,float sy,float sz);
	void Translate(float tx,float ty,float tz);
	
	bool IsInBox(CGenericVector<float>& r_point) const;
	bool IsInBox(float x,float y,float z) const;


	float Intersect( CGenericRay &ray ) const ;

public:
	//static int nbIntersect;

	//	1 bit bsp management
	//bool CreateBSP(SubBBoxDepth depth);
	//bool AddToBSP(C3DEngine::_4dVertex& vertex);


private:

	float			xmin;
	float			ymin;
	float			zmin;
	float			hmin;

	float			xmax;
	float			ymax;
	float			zmax;
	float			hmax;
	
	//	1 bit bounding box bsp octree
	//CSubBoundingBox	*bsp;
};


#endif // !defined(AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_)
