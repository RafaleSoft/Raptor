// Triangle.h: interface for the CTriangle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIANGLE_H__6910A888_750C_4629_93AB_8B882B28A0B4__INCLUDED_)
#define AFX_TRIANGLE_H__6910A888_750C_4629_93AB_8B882B28A0B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RaysWorkUnit.h"
#include "RenderObject.h"

class CTriangle
{
public:
	static int nbIntersect;

public:
	CTriangle(rays_vertex_t &v1,rays_vertex_t &v2,rays_vertex_t &v3);
	virtual ~CTriangle();

	//	Initialize the triangle according to public datas
	//	( adjust normals with corners, ... )
	void Init();

	virtual CGenericVector<float>& GetTexel(const CGenericVector<float> &hit );

	virtual CRenderObject::OBJECTTYPE GetType() const { return CRenderObject::TRIANGLE; };

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
	CGenericVector<float>	v;
	CGenericVector<float>	w;

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

#endif // !defined(AFX_TRIANGLE_H__6910A888_750C_4629_93AB_8B882B28A0B4__INCLUDED_)
