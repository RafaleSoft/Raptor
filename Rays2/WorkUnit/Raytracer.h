#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

#include <afxmt.h>			// CEvent
#include "..\simdlib\simd.h"
#include <afxtempl.h>		// CArray
#include "GenericRay.h"	

#define RAYS_WAIT_TIMEOUT	500
#define RAYS_MAX_LEVEL		64
#define DIM_FILM			0.024f

#define HUGE_REAL			1.0e6
#define	SMALL_REAL			1.0e-2

#define DEUX_SUR_65535		3.05180438e-5
#define UN_SUR_65536		0.000015259
#define UN_SUR_65535		1.52590219e-5
#define UN_SUR_256			0.00390625
#define UN_SUR_255          3.921568627e-3
#define UN_SUR_128			0.0078125
#define UN_SUR_127			7.874015748e-3

#ifndef M_PI
	#define M_PI            3.1415926535
#endif

#define PI_SUR_180          (M_PI/180.0)
#define PI_SUR_360          (M_PI/360.0)
#define UN_SUR_PI           (1.0/M_PI)
#define UN_SUR_2_PI         (0.5/M_PI)


#ifdef RAD
	#undef RAD
#endif
#define RAD                 (M_PI/180.0)

#ifdef DEG
	#undef DEG
#endif
#define DEG                 (180.0/M_PI)

#ifndef MAX
	#define MAX(a,b)    ( (a)>(b) ? (a):(b) )
#endif

#ifndef MIN
	#define MIN(a,b)    ( (a)<(b) ? (a):(b) )
#endif

#ifndef ABS
	#define ABS(a)      ( (a)>0.0 ? (a) : -(a) )
#endif

#ifndef SWAP
	#define SWAP(a,b)   ( (a^=b), (b^=a), (a^=b) )
#endif

#ifndef SWAP_DOUBLE
	#define SWAP_DOUBLE(a,b) {	double mom ;\
								mom = (a) ;\
								(a) = (b) ; (b) = mom ; }
#endif

#define SIGN(a)     ( (a)>0.0 ? 1.0 : -1.0 )
#define FRAC(a)     ( (a) - (int)(a) )

#ifndef CLIP
	#define CLIP(a,val,b)  ( (val)<=(a) ? (a) : ((val)>=(b) ? (b) : (val)) )
#endif

double exp(const double f ,const int n);

extern void DumpImage(const char* fname,unsigned char* image);


#include <fstream.h>	// ofstream
extern ofstream COUT;

class CWorkUnitApp;
class CGenericLight;
class CGenericRenderObject;
class CTexture;

typedef struct _camera_t
{
	CGenericVector<float>	origin ;				// --- def camera ---
	float					focale ;				// --- def camera ---
	float					aperture ;				// --- def camera ---
	float					object_plane ;			// --- def camera ---
	int						width;
	int						height;
	int						variance;
	int						reflection_depth;
	int						refraction_depth;
	float					crease;
	int						photon_map;
} CAMERA;
typedef CAMERA* LPCAMERA;

typedef struct _raytracer_data
{
	CWorkUnitApp	*app;
	CEvent			*lock;

	CAMERA			camera;
	CArray<CGenericLight *,CGenericLight *>
					lights;
	CArray<CGenericRenderObject *,CGenericRenderObject *>
					objects;
	CMapStringToPtr	textures;
	CMapStringToPtr	plugins;

	unsigned int	currentFrame;
	CMapWordToPtr	frames;

	CGenericRay		light_ray_levels[RAYS_MAX_LEVEL];
	CGenericRay		reflected_ray_levels[RAYS_MAX_LEVEL];
	CGenericRay		refracted_ray_levels[RAYS_MAX_LEVEL];

	unsigned int	start;
	unsigned int	end;
	unsigned int	percent;
	unsigned char	*image;
	float			*ZBuffer;

	bool			use_sse;
	bool			use_cmov;
	bool			use_mmx;
} RAYTRACERDATA;
typedef RAYTRACERDATA* LPRAYTRACERDATA;

#endif