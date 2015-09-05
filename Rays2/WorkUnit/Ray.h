// Ray.h: interface for the CRay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAY_H__38D6EEC4_7291_11D2_9142_44C9FCC00000__INCLUDED_)
#define AFX_RAY_H__38D6EEC4_7291_11D2_9142_44C9FCC00000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

typedef struct _raytracer_data * LPRAYTRACERDATA;
class CGenericRenderObject;
class CWVector;

class CRay  
{
public:
	CRay();
	virtual ~CRay();

	float				t ;			// distance
	CGenericRenderObject		
						*surface ;	// point. to intersected surface

	float				fact ;		// coef lumiere
//	double				Tot_Dist ;	// distance totale parcourue
	float				n ; 		// indice de refraction du milieu courant
	int					level ;		// depth of this branch of ray tree
	unsigned			int id ;	// numero du rayon

public:
	virtual void Hit( LPRAYTRACERDATA World , CWVector &c) = 0;
};

#endif // !defined(AFX_RAY_H__38D6EEC4_7291_11D2_9142_44C9FCC00000__INCLUDED_)
