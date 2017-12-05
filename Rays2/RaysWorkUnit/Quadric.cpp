// Quadric.cpp: implementation of the CQuadric class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/BoundingBox.h"
#endif
#if !defined(AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_)
	#include "SSE_Engine/SSE_BoundingBox.h"
#endif


#include "Quadric.h"

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuadric::CQuadric(rays_sphere_t &s):
CRenderObject(s.base)
{
	q.Ident();
	float *m = q.matrix();
	m[3] = m[12] = -s.center.x;
	m[7] = m[13] = -s.center.y;
	m[11] = m[14] = -s.center.z;
	m[15] = (s.center.x * s.center.x) +
			(s.center.y * s.center.y) +
			(s.center.z * s.center.z) -
			s.radius * s.radius;
	
	bBox->set(	s.center.x - s.radius,
				s.center.y - s.radius,
				s.center.z - s.radius,
				s.center.x + s.radius,
				s.center.y + s.radius,
				s.center.z + s.radius);

}

CQuadric::~CQuadric()
{
}

CGenericVector<float>& CQuadric::GetTexel( const CGenericVector<float> &hit )
{
	float	cx;
	float	cy;
	float	cz;
	float	theta;

	cx = -q[3];
	cy = -q[7];
	cz = -q[11];
	
	if (hit.X() != cx)
	{
		theta = (float)(sqrt((hit.X() - cx) * (hit.X() - cx) + (hit.Z() - cz) * (hit.Z() - cz)));
		theta = (float)(atan((hit.Y() - cy) / theta));

		tmpVect.Y() = (float)(UN_SUR_PI * (0.5f * PI + theta));
	}
	else
		tmpVect.Y() = (float)(0.5f * (SGN(hit.Y() - cy) + 1));

	theta = (float)(atan2((hit.Z() - cz),(hit.X() - cx)));
	tmpVect.X() = (float)(1.25f - UN_SUR_2_PI * (PI + theta));

	if (tmpVect.X() > 1.0f)
		tmpVect.X() -= 1.0f;

	return tmpVect;
}

float CQuadric::Intersect( CGenericRay &ray )
{
	tmpVect = q * ray.origin;
	tmpVect.H(0);

	float NBq = (ray.direction * tmpVect).Length();

	tmpVect.Set(	(q[0]*ray.direction.X() + q[1]*(2*ray.direction.Y()) + q[2]*(2*ray.direction.Z())),
					(q[5]*ray.direction.Y() + q[6]*(2*ray.direction.Z())),
					(q[10]*ray.direction.Z()),
					0
				);

	float Aq = (ray.direction * tmpVect).Length();

	tmpVect.Set(	(q[0]*ray.origin.X() + q[1]*(2*ray.origin.Y()) + q[2]*(2*ray.origin.Z()) + 2*q[3]),
					(q[5]*ray.origin.Y() + q[6]*(2*ray.origin.Z()) + 2*q[7]),
					(q[10]*ray.origin.Z() + 2*q[11]),
					q[15]
				);

	float Cq = (ray.origin * tmpVect).Length();

	// Aq shouldn't be < SMALL_REAL
	//	because there would not be a quadric to display
	//	surface would be on a plane
	float Ka = -NBq/Aq;
	float Kb = Cq/Aq;
	float delta = Ka*Ka - Kb;

	/* --- if (delta<0.0f) no REAL solution --- */
	if (delta<-SMALL_REAL)
		return HUGE_REAL;

	/* --- une seule solution --- 
		a priori, compte tenu de l'imprecision
		sur un float, ce cas peut etre assimile au suivant */
	
	/* --- deux solutions a tester --- */
	float t0= (float)sqrt(delta);
	float t1= Ka + t0;
	t0 = Ka - t0;

	if (t1<t0)
	{
		float t(t0);
		t0=t1;
		t1=t;
	}
	if (t1<SMALL_REAL)	
		return HUGE_REAL;

	if (t0>SMALL_REAL)
		return t0;

	return t1;
}


CGenericVector<float>& CQuadric::Normal( const CGenericVector<float> &hit )
{
	tmpVect = q*hit;
	tmpVect.Normalize();

	return tmpVect;
}
