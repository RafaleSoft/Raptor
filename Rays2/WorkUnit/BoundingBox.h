// BoundingBox.h: interface for the CBoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
#define AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GenericRay.h"

class CBoundingBox
{
public:
	CBoundingBox();
	// boundingbox is used in operations, so we want to register
	//	the object in the persistence
	CBoundingBox(const CBoundingBox &r_box);	
	virtual ~CBoundingBox();
	virtual unsigned int Size() const { return sizeof(CBoundingBox); };

	
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
	void Add(const CBoundingBox * const r_box);
	//	Extends this so that the point r_x,r_y,r_z is
	//	contained in this
	void ExtendTo(float r_x,float r_y,float r_z);

	//	Geometrical center or the bbox ( necessary to objects
	//	to perform centered rotations
	void GetCenter(CGenericVector<float> &r_center) const ;
	void GetCenter(float& r_x,float& r_y,float& r_z) const;

	CBoundingBox operator+(const CBoundingBox& r_box);
	CBoundingBox& operator=(const CBoundingBox& r_box);
	CBoundingBox& operator+=(const CBoundingBox& r_box);
	bool operator==(const CBoundingBox& r_box);

	//	Intersect generates the minimum BBox containing
	//	the intersection of this and r_box
	CBoundingBox Intersect(CBoundingBox& r_box ) const;
	void Intersect(CBoundingBox& r_box );

	void Scale(float sx,float sy,float sz);
	void Translate(float tx,float ty,float tz);
	
	bool IsInBox(CGenericVector<float>& r_point) const;
	bool IsInBox(float x,float y,float z) const;


	float Intersect( CGenericRay &ray ) const ;

public:
	static int nbIntersect;
	//	1 bit bsp management
	//bool CreateBSP(SubBBoxDepth depth);
	//bool AddToBSP(C3DEngine::_4dVertex& vertex);


private:
	float			xmin;
	float			ymin;
	float			zmin;

	float			xmax;
	float			ymax;
	float			zmax;

	//	1 bit bounding box bsp octree
	//CSubBoundingBox	*bsp;
};

#endif // !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
