// Triangle.cpp: implementation of the CTriangle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BoundingBox.h"
#include "Triangle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int CTriangle::nbIntersect = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTriangle::CTriangle(rays_vertex_t &v1,rays_vertex_t &v2,rays_vertex_t &v3):
flat(true)
{
	a.Set(v1.position.x,v1.position.y,v1.position.z,1.0f);
	b.Set(v2.position.x,v2.position.y,v2.position.z,1.0f);
	c.Set(v3.position.x,v3.position.y,v3.position.z,1.0f);

	u.Set(v1.texel.x,v1.texel.y,v1.texel.z,v1.texel.h);
	v.Set(v2.texel.x,v2.texel.y,v2.texel.z,v2.texel.h);
	w.Set(v3.texel.x,v3.texel.y,v3.texel.z,v3.texel.h);

	Init();
}

void CTriangle::Init()
{
	normal = (b-a)^(c-a);
	normal.Normalize();

	na = normal;
	nb = normal;
	nc = normal;

	d = a % normal;

	float x = normal.X();
	float y = normal.Y();
	float z = normal.Z();

	if ( x > y )
	{
		if ( x > z )
			index = 1;
		else 
			index = 3;
	}
	else
	{
		if ( z > y )
			index = 3;
		else 
			index = 2;
	}

	XMin = MIN( a.X(), MIN(b.X(),c.X()) ) ;
	YMin = MIN( a.Y(), MIN(b.Y(),c.Y()) ) ;
	ZMin = MIN( a.Z(), MIN(b.Z(),c.Z()) ) ;

	XMax = MAX( a.X(), MAX(b.X(),c.X()) ) ;
	YMax = MAX( a.Y(), MAX(b.Y(),c.Y()) ) ;
	ZMax = MAX( a.Z(), MAX(b.Z(),c.Z()) ) ;
}

CTriangle::~CTriangle()
{

}

bool CTriangle::Interpolate(const CGenericVector<float> &hit,float& alpha, float& beta) const
{
	/* Resolution du systeme lineaire permettant	*/
	/* d'exprimer le point d'intersection en 	*/
	/* fonction des coordonnees des trois points	*/
	/* du triangle					*/
	// a+alpha(b-a)=c+beta(hit-c)

	if ((b.Y() != a.Y()) || ( b.X() != a.X()))
	{
	//	beta = ((b.Y() - a.Y()) * (c.X() - a.X()) - (b.X() - a.X()) * (c.Y() - a.Y()))/
	//			((hit.Y() - c.Y()) * (b.X() - a.X()) - (hit.X() - c.X()) * (b.Y() - a.Y()));
		float fdiv = ((hit.Y() - c.Y()) * (b.X() - a.X()) - (hit.X() - c.X()) * (b.Y() - a.Y()));
		if ( *((int*)&fdiv) == 0 )
			return false;

		beta = ((b.Y() - a.Y()) * (c.X() - a.X()) - (b.X() - a.X()) * (c.Y() - a.Y())) / fdiv;


		if (b.X() != a.X())
			alpha = ((c.X() - a.X()) + beta * (hit.X() - c.X())) / (b.X() - a.X());
		else
			alpha = ((c.Y() - a.Y()) + beta * (hit.Y() - c.Y())) / (b.Y() - a.Y());

		return true;
	}
	else if (b.Z() != a.Z())
	{
		if (hit.X() != c.X())
			beta = -c.X() / (hit.X() - c.X());
		else
			beta = -c.Y() / (hit.Y() - c.Y());

		alpha = c.Z() + beta * (hit.Z() - c.Z());
		alpha = (alpha - a.Z()) / (b.Z() - a.Z());

		return true;
	}
	else 
		return false;
}


CGenericVector<float>& CTriangle::GetTexel( const CGenericVector<float> &hit )
{
	float beta,alpha;

	if (hit == c)
		return w;

	if (Interpolate(hit,alpha,beta))
	{
		tmpVect = u + alpha * (v - u);
		tmpVect = (tmpVect - w) * (1.0f / beta) + w;
	}
	else
		tmpVect.Set(0.0f,0.0f,0.0f,0.0f);

	return tmpVect;
}

CGenericVector<float>& CTriangle::Normal( const CGenericVector<float> &hit )
{ 
	float beta,alpha;

	if (hit == c)
		return w;

	if (Interpolate(hit,alpha,beta))
	{
		tmpVect = na + alpha * (nb - na);
		tmpVect = (tmpVect - nc) * (1.0f / beta) + nc;
	}
	else
		tmpVect.Set(0.0f,0.0f,0.0f,0.0f);

	return tmpVect;
}

float CTriangle::Intersect( CGenericRay &ray )
{
	float 	t ;
	float 	alpha;	       // linear term coefficient
	float 	beta ;
	float 	t1, t2 ;

	nbIntersect++;

	// --- calcule le point d'intersection de la droite et ---
	// --- du plan du polygonne (si il existe)	       --- 	
	alpha = normal % ray.direction ;
	//if ( alpha == 0.0 )
	if ( *((int*)&alpha) == 0 )
	    return( HUGE_REAL ) ;

	t =  (d - (normal % ray.origin)) / alpha ;
	// --- trouve le point d'intersection ---
	tmpVect = ray.origin  +  (t * ray.direction) ;

	// --- ce point est il dans le polygonne ? ---
	switch( index ) 
	{
		case 1 :
		    // --- first test: outside bounding box ---
		    if ( tmpVect.Y() < YMin || tmpVect.Y() > YMax
		      || tmpVect.Z() < ZMin || tmpVect.Z() > ZMax )
				return( HUGE_REAL ) ;

			// --- second test: against the 3 edges ---
			t1 = t2 = (b.Y() - a.Y()) * (c.Z() - a.Z())
				- (b.Z() - a.Z()) * (c.Y() - a.Y()) ;

			if ( t1 > 0.0 )	t1 = 0.0 ;
			else t2 = 0.0 ;

			beta = (c.Y() - b.Y()) * (tmpVect.Z() - b.Z())
				- (c.Z() - b.Z()) * (tmpVect.Y() - b.Y()) ;

			if ( beta < t1 || beta > t2 ) 
				return( HUGE_REAL ) ;

			beta = (a.Y() - c.Y()) * (tmpVect.Z() - c.Z())
				- (a.Z() - c.Z()) * (tmpVect.Y() - c.Y()) ;

			if ( beta < t1 || beta > t2 ) 
				return( HUGE_REAL ) ;

			beta = (b.Y() - a.Y()) * (tmpVect.Z() - a.Z())
				 - (b.Z() - a.Z()) * (tmpVect.Y() - a.Y()) ;

			if ( beta < t1 || beta > t2 ) 
				return( HUGE_REAL ) ;

			return( t ) ;

		case 2 :
			// --- first test: outside bounding box ---
			if ( tmpVect.X() < XMin || tmpVect.X() > XMax
				|| tmpVect.Z() < ZMin || tmpVect.Z() > ZMax )
				return( HUGE_REAL ) ;

			// --- second test: against the 3 edges ---
			t1 = t2 = (b.Z() - a.Z()) * (c.X() - a.X()) 
				- (b.X() - a.X()) * (c.Z() - a.Z()) ;

			if ( t1 > 0.0 )	t1 = 0.0 ;
			else t2 = 0.0 ;

			beta = (c.Z() - b.Z()) * (tmpVect.X() - b.X())
				- (c.X() - b.X()) * (tmpVect.Z() - b.Z()) ;

			if ( beta < t1 || beta > t2 ) 
				return( HUGE_REAL ) ;

			beta = (a.Z() - c.Z()) * (tmpVect.X() - c.X())
				- (a.X() - c.X()) * (tmpVect.Z() - c.Z()) ;

			if ( beta < t1 || beta > t2 ) 
				return( HUGE_REAL ) ;

			beta = (b.Z() - a.Z()) * (tmpVect.X() - a.X())
				- (b.X() - a.X()) * (tmpVect.Z() - a.Z()) ;

			if ( beta < t1 || beta > t2 ) 
				return( HUGE_REAL ) ;

			return( t ) ;

		case 3 :
			// --- first test: outside bounding box ---
			if ( tmpVect.X() < XMin || tmpVect.X() > XMax
				|| tmpVect.Y() < YMin || tmpVect.Y() > YMax )
				return( HUGE_REAL ) ;

			// --- second test: against the 3 edges ---
			t1 = t2 = (b.X() - a.X()) * (c.Y() - a.Y())
				- (b.Y() - a.Y()) * (c.X() - a.X()) ;

			if ( t1 > 0.0 )	t1 = 0.0 ;
			else t2 = 0.0 ;

			beta = (c.X() - b.X()) * (tmpVect.Y() - b.Y())
				- (c.Y() - b.Y()) * (tmpVect.X() - b.X()) ;

			if ( beta < t1 || beta > t2 ) 
				return( HUGE_REAL ) ;

		    beta = (a.X() - c.X()) * (tmpVect.Y() - c.Y())
				- (a.Y() - c.Y()) * (tmpVect.X() - c.X()) ;

			if ( beta < t1 || beta > t2 ) 
				return( HUGE_REAL ) ;

			beta = (b.X() - a.X()) * (tmpVect.Y() - a.Y())
				- (b.Y() - a.Y()) * (tmpVect.X() - a.X()) ;

			if ( beta < t1 || beta > t2 ) 
				return( HUGE_REAL ) ;

			return( t ) ;
	}

	return( HUGE_REAL ) ;
}
