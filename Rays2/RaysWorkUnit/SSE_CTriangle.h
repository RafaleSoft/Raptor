// SSE_CTriangle.h: interface for the SSE_CTriangle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSE_CTRIANGLE_H__DFFC3D90_2CE0_4B15_8276_364C7A5D552F__INCLUDED_)
#define AFX_SSE_CTRIANGLE_H__DFFC3D90_2CE0_4B15_8276_364C7A5D552F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RaysWorkUnit.h"
#include "SSE_CRenderObject.h"

class SSE_CTriangle
{
public:
	static int nbIntersect;

public:
	SSE_CTriangle(rays_vertex_t &v1,rays_vertex_t &v2,rays_vertex_t &v3);
	virtual ~SSE_CTriangle();

	//	Initialize the triangle according to public datas
	//	( adjust normals with corners, ... )
	void Init();

	virtual CGenericVector<float>& GetTexel(const CGenericVector<float> &hit );

	virtual SSE_CRenderObject::OBJECTTYPE GetType() const { return SSE_CRenderObject::TRIANGLE; };

	virtual float Intersect( CGenericRay &ray );

	//	Normals stuff ...
	//	Set normals
	void SetNormals(CGenericVector<float>& a,CGenericVector<float>& b,CGenericVector<float>& c,bool isFlat)
	{ na = a; nb = b; nc = c; flat = isFlat; };

	//	return interpolated normal
	virtual CGenericVector<float>& Normal( const CGenericVector<float> &hit );

	//	return polygon's plane normal
	CGenericVector<float>& FlatNormal( void ) { return normal ; };


public:
	CGenericVector<float>	a;				// 3 corners of polygon
	CGenericVector<float>	b;
	CGenericVector<float>	c;

	CGenericVector<float>	na;				// 3 normals of polygon
	CGenericVector<float>	nb;
	CGenericVector<float>	nc;

	CGenericVector<float>	u;				// 3 texels of polygon
	CGenericVector<float>	v;				//	only X & Y are used,
	CGenericVector<float>	w;				//	Z & H are for intersections

private:
	CGenericVector<float>	tmpVect;
	CGenericVector<float>	normal ;		// normal to polygon's plane

	char					index ;			// dir max of normal
	float					d ;				// coef of polygon's plane
	bool					flat;

	float XMin, YMin, ZMin;
	float XMax,	YMax, ZMax;

	bool Interpolate(const CGenericVector<float>& hit,float& alpha, float& beta) const;
};


#endif // !defined(AFX_SSE_CTRIANGLE_H__DFFC3D90_2CE0_4B15_8276_364C7A5D552F__INCLUDED_)
