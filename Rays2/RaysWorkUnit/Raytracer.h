#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#include "../Messages.h"
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

#define PI_SUR_180          (PI/180.0)
#define PI_SUR_360          (PI/360.0)
#define UN_SUR_PI           (1.0/PI)
#define UN_SUR_2_PI         (0.5/PI)


#ifndef SWAP
	#define SWAP(a,b)   ( (a^=b), (b^=a), (a^=b) )
#endif

#ifndef SWAP_DOUBLE
	#define SWAP_DOUBLE(a,b) {	double mom ;\
								mom = (a) ;\
								(a) = (b) ; (b) = mom ; }
#endif

#define FRAC(a)     ( (a) - (int)(a) )

#ifndef CLIP
	#define CLIP(a,val,b)  ( (val)<=(a) ? (a) : ((val)>=(b) ? (b) : (val)) )
#endif

double exp(const double f ,const int n);



class CWorkUnitApp;
class CGenericLight;
class CGenericRenderObject;
class CTexture;
class CPlugin;

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

class CRaytracerData
{
public:
	CRaytracerData();
	virtual ~CRaytracerData();

	unsigned int getStart(void) const
	{ return start; }

	unsigned int getEnd(void) const
	{ return end; }

	const CAMERA& getCamera(void) const
	{ return camera; };

	unsigned char *getImage(void) const
	{ return image; }

	float *getZBuffer(void) const
	{ return ZBuffer; }

	unsigned int getNbObjects(void) const
	{ return objects.size(); }


private:
	CAMERA			camera;
	vector<CGenericLight *> lights;
	vector<CGenericRenderObject *> objects;
	map<std::string,CTexture*>	textures;
	map<std::string,CPlugin*> plugins;

	unsigned int	currentFrame;
	map<unsigned short,rays_frame_t*> frames;

	CGenericRay		light_ray_levels[RAYS_MAX_LEVEL];
	CGenericRay		reflected_ray_levels[RAYS_MAX_LEVEL];
	CGenericRay		refracted_ray_levels[RAYS_MAX_LEVEL];

	unsigned int	start;
	unsigned int	end;
	unsigned int	percent;
	unsigned char	*image;
	float			*ZBuffer;
};

#endif