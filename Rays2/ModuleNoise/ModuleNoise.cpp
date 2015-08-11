// ModuleNoise.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ModuleNoise.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CModuleNoiseApp

BEGIN_MESSAGE_MAP(CModuleNoiseApp, CWinApp)
	//{{AFX_MSG_MAP(CModuleNoiseApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModuleNoiseApp construction

CModuleNoiseApp::CModuleNoiseApp()
{
	srand( (unsigned)time( NULL ) );
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CModuleNoiseApp object

CModuleNoiseApp theApp;

const int HALF_SURFACE_ELEMENTS = 64;
const int SURFACE_ELEMENTS = 128;

#ifndef ABS
	#define ABS(a)      ( (a)>0.0 ? (a) : -(a) )
#endif

CGenericVector<float>	G[SURFACE_ELEMENTS];
int						P[SURFACE_ELEMENTS];


void CModuleNoiseApp::InitNoiseGeneration()
{
	bool	Ok[SURFACE_ELEMENTS] ;
	int		i;
	int		essai;
	float	norm;

	// --- ---  construction de P  --- --- 
	// P contain apseudorandom permutation 
	// of the first N integers             

	memset( Ok, 0, SURFACE_ELEMENTS) ;

	for ( i=0; i<SURFACE_ELEMENTS; i++ ) 
	{
	    essai = (rand() % 0x7FFF) % SURFACE_ELEMENTS ;
	    while ( Ok[essai] )
			essai = (essai+1) % SURFACE_ELEMENTS ;

	    P[i] = essai ;
	    Ok[essai] = true ;
	}

	// --- ---  construction de G  --- --- 
	// G contain N pseudorandom vectors    
	// uniformely distributed on the unit  
	// sphere			       
	CGenericVector<float> v;
	for ( i=0; i<SURFACE_ELEMENTS; i++ ) 
	{
	    do 
		{  
			v.X() = ((float)(rand() - (RAND_MAX/2))) / RAND_MAX;
			v.Y() = ((float)(rand() - (RAND_MAX/2))) / RAND_MAX;
			v.Z() = ((float)(rand() - (RAND_MAX/2))) / RAND_MAX;

			norm = (float)(v.Norm());

	    } while ( norm > 1.0f ) ;

		v.Normalize();
		G[i] = v;
	}	
}


void CModuleNoiseApp::V_Noise_P( float x, float y, float z )
{
	float	fact ;
	float	d,  dd,  ddd ;
	float	w_x[2],  w_y[2],  w_z[2] ;

	tmpVect.Zero();

	int f_x = (int) floor(x) ; 
	int f_y = (int) floor(y) ; 
	int f_z = (int) floor(z) ; 

	d   = x - f_x ;
	d   = ABS(d) ;
	dd  = d * d ;
	ddd = dd * d ;
	w_x[0] = ddd + ddd  -  3.0f * dd  +  1.0f ;

	d   = x - (f_x+1) ;
	d   = ABS(d) ;
	dd  = d * d ;
	ddd = dd * d ;
	w_x[1] = ddd + ddd  -  3.0f * dd  +  1.0f ;

	d   = y - f_y ;
	d   = ABS(d) ;
	dd  = d * d ;
	ddd = dd * d ;
	w_y[0] = ddd + ddd  -  3.0f * dd  +  1.0f ;

	d   = y - (f_y+1) ;
	d   = ABS(d) ;
	dd  = d * d ;
	ddd = dd * d ;
	w_y[1] = ddd + ddd  -  3.0f * dd  +  1.0f ;

	d   = z - f_z ;
	d   = ABS(d) ;
	dd  = d * d ;
	ddd = dd * d ;
	w_z[0] = ddd + ddd  -  3.0f * dd  +  1.0f ;

	d   = z - (f_z+1) ;
	d   = ABS(d) ;
	dd  = d * d ;
	ddd = dd * d ;
	w_z[1] = ddd + ddd  -  3.0f * dd  +  1.0f ;

	for (int i=0; i <= 1; i++ ) 
	{
	    v1.X() = x - f_x ;

	    for (int j=0; j <= 1; j++ ) 
		{
			v1.Y() = y - f_y ;

			for (int k=0; k <= 1; k++ ) 
			{    
			    v1.Z() = z - f_z ;

				v2 = GammaHalf(f_x,f_y,f_z) ^ v1;

			    fact = w_x[i] * w_y[j] * w_z[k] ;

				tmpVect += v2 * fact;
			    
				f_z++;
			}

			f_z -= 2;
			f_y++;
	    }

		f_y -= 2;
		f_x++;
	}

	tmpVect.H(1.0);
}

  
//	function	Random index in vect table
//			c.f:	Hypertexture
//				Ken Perlin
//				SIGGRAPH-89
CGenericVector<float> &CModuleNoiseApp::GammaHalf( int i, int j, int k )
{    
	int	index ;	

	index = (k % HALF_SURFACE_ELEMENTS) + HALF_SURFACE_ELEMENTS ;

	index = P[index] ;

	index = ((j+index) % HALF_SURFACE_ELEMENTS) + HALF_SURFACE_ELEMENTS ;

	index = P[index] ;

	index = ((i+index) % HALF_SURFACE_ELEMENTS) + HALF_SURFACE_ELEMENTS ;

	index = P[index] ;

	return( G[index] ) ;
}

//	function	Random index in vect table
//			c.f:	Hypertexture
//				Ken Perlin
//				SIGGRAPH-89
CGenericVector<float> &CModuleNoiseApp::GammaFull( int i, int j, int k )
{    
	int	index ;	

	index = (k % SURFACE_ELEMENTS);

	index = P[index] ;

	index = ((j+index) % SURFACE_ELEMENTS);

	index = P[index] ;

	index = ((i+index) % SURFACE_ELEMENTS);

	index = P[index] ;

	return( G[index] ) ;
}



/* ------------------------------------------------------------------------ */
/*									    */
/*	V_Turb()							    */
/*									    */
/*	Turbulence (bruit 1/f)						    */
/*									    */
/* ------------------------------------------------------------------------ */
CGenericVector<float> &CModuleNoiseApp::V_Turb( CGenericVector<float> &pos, float freq_base, int nb_octaves ) 
{
	float	freq = 1.0f;

	CGenericVector<float>	a;
    
	a = pos * freq_base;
	turbulence.Zero();

	for ( int i=0; i < nb_octaves; i++) 
	{
	    V_Noise_P(a.X() * freq, a.Y() * freq, a.Z() * freq) ;

		turbulence += tmpVect * (1.0f / freq);
	  
		freq *= 2.0f;
	}

	turbulence.H(1.0f);
	return( turbulence ) ;

}

/* ------------------------------------------------------------------------ */
/*									    */
/*	V_Turb_Abs()							    */
/*									    */
/*	Turbulence (bruit 1/f)						    */
/*									    */
/* ------------------------------------------------------------------------ */
/*
Vector	V_Turb_Abs( Vector *pos, float freq_base, int nb_octaves ) 
{

int	i ;
float	freq ;
float	un_sur ;
Vector	a, v, turb ;
    
	a.x = pos->x * freq_base ;
	a.y = pos->y * freq_base ;
	a.z = pos->z * freq_base ;

	turb.x = turb.y = turb.z = 0.0 ;

	for ( freq=1.0,i=0; i < nb_octaves; i++, freq*=1.9 ) {
	    v = V_Noise_P(a.x*freq, a.y*freq, a.z*freq) ;
	    un_sur = 1.0 / freq ;
	    turb.x += ABS(v.x) * un_sur ;
	    turb.y += ABS(v.y) * un_sur ;
	    turb.z += ABS(v.z) * un_sur ;
	}

	return( turb ) ;
}

*/
