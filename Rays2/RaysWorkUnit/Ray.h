// Ray.h: interface for the CRay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAY_H__38D6EEC4_7291_11D2_9142_44C9FCC00000__INCLUDED_)
#define AFX_RAY_H__38D6EEC4_7291_11D2_9142_44C9FCC00000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "stdafx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_COLOR_H__3770AC59_0D0E_49EF_99C8_037268A33CE4__INCLUDED_)
	#include "System/Color.h"
#endif
RAPTOR_NAMESPACE

class CGenericRenderObject;
class CWVector;
class CRaytracerData;

class CRay  
{
public:
	CRay();
	virtual ~CRay();

	float				t ;			// distance
	CGenericRenderObject		
						*surface ;	// point. to intersected surface

	float				fact ;		// coef lumiere
	float				n ; 		// indice de refraction du milieu courant
	int					level ;		// depth of this branch of ray tree
	unsigned			int id ;	// numero du rayon

public:
	virtual void Hit(CRaytracerData& World , CColor::RGBA &c) = 0;
};

#endif // !defined(AFX_RAY_H__38D6EEC4_7291_11D2_9142_44C9FCC00000__INCLUDED_)
