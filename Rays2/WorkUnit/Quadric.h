// Quadric.h: interface for the CQuadric class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUADRIC_H__2F8A8BDA_0AD3_44DA_881A_0F55BBB3F4AC__INCLUDED_)
#define AFX_QUADRIC_H__2F8A8BDA_0AD3_44DA_881A_0F55BBB3F4AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WorkUnit.h"
#include "RenderObject.h"

class CQuadric : public CRenderObject  
{
public:
	CQuadric(rays_sphere_t &s);
	virtual ~CQuadric();

	virtual void Scale(float sx,float sy,float sz) {};
	virtual void Translate(float tx,float ty,float tz) {};
	virtual void Rotate(float angle,float ax,float ay,float az) {};

	virtual CGenericVector<float>& GetTexel(const CGenericVector<float> &hit );

	virtual OBJECTTYPE GetType() const { return QUADRIC; };

	virtual float Intersect( CGenericRay &ray );

private:
	CGenericMatrix<float> q;

	virtual CGenericVector<float>& Normal( const CGenericVector<float> &hit );

};

#endif // !defined(AFX_QUADRIC_H__2F8A8BDA_0AD3_44DA_881A_0F55BBB3F4AC__INCLUDED_)
