// Camera.h: interface for the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERA_H__A20284B1_13FC_4604_9859_032740298CC0__INCLUDED_)
#define AFX_CAMERA_H__A20284B1_13FC_4604_9859_032740298CC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "RenderObject.h"
#include "SimdLib/simd.h"

class CCamera  : public CRenderObject
{
	// attributes
public:
	CGenericVector<float>	origin ;
	float	focale ;				// --- def camera ---
	float	aperture ;				// --- def camera ---
	float	object_plane ;			// --- def camera ---
	int		width;
	int		height;
	int		depth;
	int		variance;
	int		reflection_depth;
	int		refraction_depth;

public:
	CCamera();
	virtual ~CCamera();

};

#endif // !defined(AFX_CAMERA_H__A20284B1_13FC_4604_9859_032740298CC0__INCLUDED_)
