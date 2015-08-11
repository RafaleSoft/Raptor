// GenericRay.h: interface for the CGenericRay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENERICRAY_H__DA58263D_57F2_46F0_91FD_F217300CBC21__INCLUDED_)
#define AFX_GENERICRAY_H__DA58263D_57F2_46F0_91FD_F217300CBC21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ray.h"
#include "..\simdlib\simd.h"

#define		DIRECTION_OFFSET	48
#define		ORIGIN_OFFSET		28

class CGenericLight;

class CGenericRay : public CRay
{
public:
	CGenericVector<float>	origin ;	// ray origin
	CGenericVector<float>	direction ;	// ray direction

	CGenericVector<float>	hit ;		// coordinates of intersection
	CGenericVector<float>	normal ;	// surface normal at point of intersection

public:
	CGenericRay();
	virtual ~CGenericRay();

	void Hit( LPRAYTRACERDATA World , CWVector &c);

private:
	virtual float Light_Hit( LPRAYTRACERDATA World, CWVector &c,CGenericLight *light);
};

#endif // !defined(AFX_GENERICRAY_H__DA58263D_57F2_46F0_91FD_F217300CBC21__INCLUDED_)
