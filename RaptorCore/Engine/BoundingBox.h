// BoundingBox.h: interface for the CBoundingBox class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
#define AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CSubBoundingBox;



class RAPTOR_API CBoundingBox
{
public:
	CBoundingBox();
	// boundingbox is used in operations, so we want to register
	//	the object in the persistence
	CBoundingBox(const CBoundingBox &r_box);	
	virtual ~CBoundingBox();

	float RAPTOR_FASTCALL xMin(void) const { return xmin; };
	float RAPTOR_FASTCALL yMin(void) const { return ymin; };
	float RAPTOR_FASTCALL zMin(void) const { return zmin; };
	float RAPTOR_FASTCALL xMax(void) const { return xmax; };
	float RAPTOR_FASTCALL yMax(void) const { return ymax; };
	float RAPTOR_FASTCALL zMax(void) const { return zmax; };
	
	void RAPTOR_FASTCALL set(const GL_COORD_VERTEX &r_min,const GL_COORD_VERTEX &r_max);
	void set(float xmin,float ymin,float zmin,float xmax,float ymax,float zmax);

	float RAPTOR_FASTCALL getDiameter(void) const;
	void RAPTOR_FASTCALL get(GL_COORD_VERTEX &r_min,GL_COORD_VERTEX &r_max) const;
	void get(float &r_xmin,float &r_ymin,float &r_zmin,float &r_xmax,float &r_ymax,float &r_zmax) const;

	//	This method is used to get a 'transformed' BBox
	virtual void get(GL_COORD_VERTEX &r_min,GL_COORD_VERTEX &r_max,const GL_MATRIX &m) const;

	//	Same as above but perspective divide is applyed
	virtual void getP(GL_COORD_VERTEX &r_min,GL_COORD_VERTEX &r_max,const GL_MATRIX &m,const GL_MATRIX &mm) const;

	//	Adding a bounding extends this to the minimum bounding box
	//	that contains this and r_box
	virtual void add(const CBoundingBox & r_box);

	//	Extends this so that the point r_x,r_y,r_z is
	//	contained in this
	void extendTo(float r_x,float r_y,float r_z);

	//	Geometrical center or the bbox ( necessary to objects
	//	to perform centered rotations )
	void getCenter(GL_COORD_VERTEX &r_center) const ;
	void getCenter(float &r_x,float &r_y,float &r_z) const;

	//	Returns the equivalent sphere
	void getSphere(GL_COORD_VERTEX &r_center,float &radius) const;

	CBoundingBox operator+(const CBoundingBox &r_box);
	CBoundingBox& operator=(const CBoundingBox &r_box);
	CBoundingBox& operator+=(const CBoundingBox &r_box);
	bool operator==(const CBoundingBox &r_box);

	//	Intersect generates the minimum BBox containing
	//	the intersection of this and r_box
	CBoundingBox RAPTOR_FASTCALL intersect(const CBoundingBox &r_box ) const;
	void RAPTOR_FASTCALL intersect(const CBoundingBox &r_box );

	virtual void scale(float sx,float sy,float sz);
	virtual void translate(float tx,float ty,float tz);
	virtual void translateAbsolute(float tx,float ty,float tz);
	
	virtual bool isInBox(GL_COORD_VERTEX &r_point) const;
	virtual bool isInBox(float x,float y,float z) const;

	//	Intersect returns a value 't' where t is the minimum
	//	value that verifies (origin + t.direction) intersects
	//	the bounding box. 
	virtual float intersect(GL_COORD_VERTEX &origin,GL_COORD_VERTEX &direction) const;

	//	Intersects return true if the 2 bboxes
	//	have a non void intersection.
	//	m is a transform between this and r_box. 
	//	( But the transform is not implemented for the moment )
	bool intersect(const CBoundingBox &r_box,GL_MATRIX &m) const;


	//	1 bit bsp management
	//	
	//	Creates the BSP of the specified depth
	bool createBSP(unsigned int depth);

	//	Fills the BSP element in which lies vertex
	bool addToBSP(GL_COORD_VERTEX &vertex);

	//	Fills every elements of the BSP that intersect
	//	the triangle defined by (v1,v2,v3)
	bool fillBSP(GL_COORD_VERTEX &v1,GL_COORD_VERTEX &v2,GL_COORD_VERTEX &v3);

	//	A basic rendering of the state of the BSP
	float renderBSP(void) const;


protected:
	float			xmin;
	float			ymin;
	float			zmin;
	float			hmin;

	float			xmax;
	float			ymax;
	float			zmax;
	float			hmax;

	//	1 bit bounding box bsp octree
	CSubBoundingBox	*bsp;

};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)

