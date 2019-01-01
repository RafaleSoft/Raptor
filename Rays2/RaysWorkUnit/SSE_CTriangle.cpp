// SSE_CTriangle.cpp: implementation of the SSE_CTriangle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SSE_CTriangle.h"

//	Les offsets sont calcules par rapport à 128
//	pour pouvoir utiiser le mode d'adressage 
//	disp8[ESI]
#define		SHORT_OFFSET		128
#define		TMPVECT_OFFSET		184 - SHORT_OFFSET
#define		NORMAL_A_OFFSET		64 - SHORT_OFFSET
#define		NORMAL_B_OFFSET		84 - SHORT_OFFSET
#define		NORMAL_C_OFFSET		104 - SHORT_OFFSET
#define		NORMAL_U_OFFSET		124 - SHORT_OFFSET
#define		NORMAL_V_OFFSET		144 - SHORT_OFFSET
#define		NORMAL_W_OFFSET		164 - SHORT_OFFSET

static bool	addressOffsetsChecked = false;
int SSE_CTriangle::nbIntersect = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SSE_CTriangle::SSE_CTriangle(rays_vertex_t &v1,rays_vertex_t &v2,rays_vertex_t &v3):
flat(true)
{
	if (!addressOffsetsChecked)
	{
		std::cout << "Checking optimize offsets..." << std::endl;
		if ((int)(&tmpVect) - (int)(this) != (SHORT_OFFSET + TMPVECT_OFFSET))
			std::cout << "ERROR tmpVect offset is wrong: it should be " << (int)(&tmpVect) - (int)(this) << " instead of " << 128 + TMPVECT_OFFSET << std::endl;
		if ((int)(&na) - (int)(this) != (SHORT_OFFSET + NORMAL_A_OFFSET))
			std::cout << "ERROR na offset is wrong: it should be " << (int)(&na) - (int)(this) << " instead of " << 128 + NORMAL_A_OFFSET << std::endl;
		if ((int)(&nb) - (int)(this) != (SHORT_OFFSET + NORMAL_B_OFFSET))
			std::cout << "ERROR nb offset is wrong: it should be " << (int)(&nb) - (int)(this) << " instead of " << 128 + NORMAL_B_OFFSET << std::endl;
		if ((int)(&nc) - (int)(this) != (SHORT_OFFSET + NORMAL_C_OFFSET))
			std::cout << "ERROR nc offset is wrong: it should be " << (int)(&nc) - (int)(this) << " instead of " << 128 + NORMAL_C_OFFSET << std::endl;
		if ((int)(&u) - (int)(this) != (SHORT_OFFSET + NORMAL_U_OFFSET))
			std::cout << "ERROR u offset is wrong: it should be " << (int)(&u) - (int)(this) << " instead of " << 128 + NORMAL_U_OFFSET << std::endl;
		if ((int)(&v) - (int)(this) != (SHORT_OFFSET + NORMAL_V_OFFSET))
			std::cout << "ERROR v offset is wrong: it should be " << (int)(&v) - (int)(this) << " instead of " << 128 + NORMAL_V_OFFSET << std::endl;
		if ((int)(&w) - (int)(this) != (SHORT_OFFSET + NORMAL_W_OFFSET))
			std::cout << "ERROR w offset is wrong: it should be " << (int)(&w) - (int)(this) << " instead of " << 128 + NORMAL_W_OFFSET << std::endl;

		addressOffsetsChecked = true;
	}

	a.Set(v1.position.x,v1.position.y,v1.position.z,1.0f);
	b.Set(v2.position.x,v2.position.y,v2.position.z,1.0f);
	c.Set(v3.position.x,v3.position.y,v3.position.z,1.0f);

	u.Set(v1.texel.x,v1.texel.y,v1.texel.z,v1.texel.h);
	v.Set(v2.texel.x,v2.texel.y,v2.texel.z,v2.texel.h);
	w.Set(v3.texel.x,v3.texel.y,v3.texel.z,v3.texel.h);

	Init();
}

void SSE_CTriangle::Init()
{
	normal = (b-a)^(c-a);
	normal.Normalize();

	na = normal;
	nb = normal;
	nc = normal;
	//	Prepare intersections
	u.Z() = u.H() = (b.Y() - a.Y()) * (c.Z() - a.Z()) - (b.Z() - a.Z()) * (c.Y() - a.Y()) ;
	if ( u.Z() > 0.0f )	u.Z() = 0.0f ;	else u.H() = 0.0f ;

	v.Z() = v.H() = (b.Z() - a.Z()) * (c.X() - a.X()) - (b.X() - a.X()) * (c.Z() - a.Z()) ;
	if ( v.Z() > 0.0f )	v.Z() = 0.0f ;	else v.H() = 0.0f ;

	w.Z() = w.H() = (b.X() - a.X()) * (c.Y() - a.Y()) - (b.Y() - a.Y()) * (c.X() - a.X()) ;
	if ( w.Z() > 0.0f )	w.Z() = 0.0 ;	else w.H() = 0.0f ;

	d = a % normal;


	if ( normal.X() > normal.Y() )
	{
		if ( normal.X() > normal.Z() )
			index = 1;
		else 
			index = 3;
	}
	else
	{
		if ( normal.Z() > normal.Y() )
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


SSE_CTriangle::~SSE_CTriangle()
{

}

bool SSE_CTriangle::Interpolate(const CGenericVector<float> &hit,float& alpha, float& beta) const
{
	/* Resolution du systeme lineaire permettant	*/
	/* d'exprimer le point d'intersection en 	*/
	/* fonction des coordonnees des trois points	*/
	/* du triangle					*/
	// a+alpha(b-a)=c+beta(hit-c)

	if ((b.Y() != a.Y()) || ( b.X() != a.X()))
	{
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

CGenericVector<float>& SSE_CTriangle::GetTexel( const CGenericVector<float> &hit )
{
	float coeffs[4];	

	if (hit == c)
		return w;

	if (Interpolate(hit,coeffs[0],coeffs[1]))
	{
		//tmpVect = u + coeffs[0] * (v - u);
		//tmpVect = (tmpVect - w) * (1.0f / coeffs[1]) + w;
		
		coeffs[1] = 1.0f / coeffs[1];
		__asm
		{
			mov esi,this
			lea edi,coeffs
			add esi,SHORT_OFFSET
			sse_loadupsofs(XMM0_ESI,NORMAL_U_OFFSET+4)	//	xmm0 = u
			sse_loadups(XMM3_EDI)
			sse_loadaps(XMM4_XMM3)
			sse_loadupsofs(XMM1_ESI,NORMAL_V_OFFSET+4)	//	xmm1 = v
			sse_shufps(XMM4_XMM3,0x00)					//	xmm4 = alpha
			sse_loadaps(XMM5_XMM3)
			sse_loadupsofs(XMM2_ESI,NORMAL_W_OFFSET+4)	//	xmm2 = w
			sse_shufps(XMM5_XMM3,0x55)					//	xmm5 = beta

			sse_subps(XMM1_XMM0)						//	xmm1 = (v - u)
			sse_mulps(XMM1_XMM4)						//	xmm1 = alpha * ( v - u )
			sse_addps(XMM1_XMM0)						//	xmm1 = u + alpha * (v - u);
			sse_subps(XMM1_XMM2)						//	xmm1 = (tmpVect - w)
			mov edi,this
			sse_mulps(XMM1_XMM5)						//	xmm1 = (tmpVect - w) * (1.0f / beta)
			add edi,SHORT_OFFSET
			sse_addps(XMM1_XMM2)						//	xmm1 = (tmpVect - w) * (1.0f / beta) + w;

			sse_storeupsofs(XMM1_EDI,TMPVECT_OFFSET+4)
		}
	}
	else
		tmpVect.Set(0.0f,0.0f,0.0f,0.0f);

	return tmpVect;
}

CGenericVector<float>& SSE_CTriangle::Normal( const CGenericVector<float> &hit )
{ 
	float coeffs[4];

	if (hit == c)
		return w;

	if (Interpolate(hit,coeffs[0],coeffs[1]))
	{
		//tmpVect = na + coeffs[0] * (nb - na);
		//tmpVect = (tmpVect - nc) * (1.0f / coeffs[1]) + nc;

		coeffs[1] = 1.0f / coeffs[1];
		__asm
		{
			mov esi,this
			lea edi,coeffs
			add esi,SHORT_OFFSET
			sse_loadupsofs(XMM0_ESI,NORMAL_A_OFFSET+4)	//	xmm0 = na
			sse_loadups(XMM3_EDI)
			sse_loadaps(XMM4_XMM3)
			sse_loadupsofs(XMM1_ESI,NORMAL_B_OFFSET+4)	//	xmm1 = nb
			sse_shufps(XMM4_XMM3,0x00)					//	xmm4 = alpha
			sse_loadaps(XMM5_XMM3)
			sse_loadupsofs(XMM2_ESI,NORMAL_C_OFFSET+4)	//	xmm2 = nc
			sse_shufps(XMM5_XMM3,0x55)					//	xmm5 = beta

			sse_subps(XMM1_XMM0)						//	xmm1 = (nb - na)
			sse_mulps(XMM1_XMM4)						//	xmm1 = alpha * ( nb - na )
			sse_addps(XMM1_XMM0)						//	xmm1 = na + alpha * (nb - na);
			sse_subps(XMM1_XMM2)						//	xmm1 = (tmpVect - nc)
			mov edi,this
			sse_mulps(XMM1_XMM5)						//	xmm1 = (tmpVect - nc) * (1.0f / beta)
			add edi,SHORT_OFFSET
			sse_addps(XMM1_XMM2)						//	xmm1 = (tmpVect - nc) * (1.0f / beta) + nc;

			sse_storeupsofs(XMM1_EDI,TMPVECT_OFFSET+4)
		}
	}
	else
		tmpVect.Set(0.0f,0.0f,0.0f,0.0f);

	return tmpVect;
}



float SSE_CTriangle::Intersect( CGenericRay &ray )
{
	float 	t ;
	float 	alpha;	       // linear term coefficient
	float 	beta ;

	nbIntersect++;

	// --- calcule le point d'intersection de la droite et ---
	// --- du plan du polygonne (si il existe)	       --- 	
	alpha = normal % ray.direction ;

	if ( *((int*)&alpha) == 0 )
	    return( HUGE_REAL ) ;

	t =  (d - (normal % ray.origin)) / alpha ;
	// --- trouve le point d'intersection ---
	
	//	tmpVect = ray.origin  +  (t * ray.direction) ;
	__asm
	{
		lea edi,t
		mov esi,ray
		sse_loadss(XMM0_EDI)
		sse_loadupsofs(XMM1_ESI,DIRECTION_OFFSET+4)
		mov edi,this
		sse_shufps(XMM0_XMM0,0x0)
		sse_loadupsofs(XMM2_ESI,ORIGIN_OFFSET+4)
		sse_mulps(XMM0_XMM1)
		add edi,128
		sse_addps(XMM0_XMM2)
		sse_storeupsofs(XMM0_EDI,TMPVECT_OFFSET+4)
	}

	// --- ce point est il dans le polygonne ? ---
	switch( index ) 
	{
		case 1 :
		    // --- first test: outside bounding box ---
		    if ( tmpVect.Y() < YMin || tmpVect.Y() > YMax
		      || tmpVect.Z() < ZMin || tmpVect.Z() > ZMax )
				return( HUGE_REAL ) ;

			// --- second test: against the 3 edges ---
			beta = (c.Y() - b.Y()) * (tmpVect.Z() - b.Z())
				- (c.Z() - b.Z()) * (tmpVect.Y() - b.Y()) ;

			if ( beta < u.Z() || beta > u.H() ) 
				return( HUGE_REAL ) ;

			beta = (a.Y() - c.Y()) * (tmpVect.Z() - c.Z())
				- (a.Z() - c.Z()) * (tmpVect.Y() - c.Y()) ;

			if ( beta < u.Z() || beta > u.H() ) 
				return( HUGE_REAL ) ;

			beta = (b.Y() - a.Y()) * (tmpVect.Z() - a.Z())
				 - (b.Z() - a.Z()) * (tmpVect.Y() - a.Y()) ;

			if ( beta < u.Z() || beta > u.H() ) 
				return( HUGE_REAL ) ;

			return( t ) ;

		case 2 :
			// --- first test: outside bounding box ---
			if ( tmpVect.X() < XMin || tmpVect.X() > XMax
				|| tmpVect.Z() < ZMin || tmpVect.Z() > ZMax )
				return( HUGE_REAL ) ;

			// --- second test: against the 3 edges ---
			beta = (c.Z() - b.Z()) * (tmpVect.X() - b.X())
				- (c.X() - b.X()) * (tmpVect.Z() - b.Z()) ;

			if ( beta < v.Z() || beta > v.H() ) 
				return( HUGE_REAL ) ;

			beta = (a.Z() - c.Z()) * (tmpVect.X() - c.X())
				- (a.X() - c.X()) * (tmpVect.Z() - c.Z()) ;

			if ( beta < v.Z() || beta > v.H() ) 
				return( HUGE_REAL ) ;

			beta = (b.Z() - a.Z()) * (tmpVect.X() - a.X())
				- (b.X() - a.X()) * (tmpVect.Z() - a.Z()) ;

			if ( beta < v.Z() || beta > v.H() ) 
				return( HUGE_REAL ) ;

			return( t ) ;

		case 3 :
			// --- first test: outside bounding box ---
			if ( tmpVect.X() < XMin || tmpVect.X() > XMax
				|| tmpVect.Y() < YMin || tmpVect.Y() > YMax )
				return( HUGE_REAL ) ;

			// --- second test: against the 3 edges ---
			beta = (c.X() - b.X()) * (tmpVect.Y() - b.Y())
				- (c.Y() - b.Y()) * (tmpVect.X() - b.X()) ;

			if ( beta < w.Z() || beta > w.H() ) 
				return( HUGE_REAL ) ;

		    beta = (a.X() - c.X()) * (tmpVect.Y() - c.Y())
				- (a.Y() - c.Y()) * (tmpVect.X() - c.X()) ;

			if ( beta < w.Z() || beta > w.H() ) 
				return( HUGE_REAL ) ;

			beta = (b.X() - a.X()) * (tmpVect.Y() - a.Y())
				- (b.Y() - a.Y()) * (tmpVect.X() - a.X()) ;

			if ( beta < w.Z() || beta > w.H() ) 
				return( HUGE_REAL ) ;

			return( t ) ;
	}

	return( HUGE_REAL ) ;
}
