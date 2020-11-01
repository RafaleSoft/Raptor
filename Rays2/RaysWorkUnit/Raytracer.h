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

class CCamera
{
public:
	CCamera() {};
	virtual ~CCamera() {};

	CGenericVector<float>	origin ;				// --- def camera ---
	float					focale ;				// --- def camera ---
	float					aperture ;				// --- def camera ---
	float					object_plane ;			// --- def camera ---
	unsigned int			width;
	unsigned int			height;
	int						variance;
	unsigned int			reflection_depth;
	unsigned int			refraction_depth;
	float					crease;
	int						photon_map;
};

class CRaytracerData
{
public:
	CRaytracerData();
	virtual ~CRaytracerData();

	void allocateBuffer();

	void updateCamera(const rays_config_t &config);

	unsigned int getStart(void) const { return start; }
	void setStart(unsigned int s) { start = s; }

	unsigned int getEnd(void) const { return end; }
	void setEnd(unsigned int e) { end = e; }

	unsigned int getPercent(void) const { return percent; }
	void setPercent(unsigned int p) { percent = p; }

	const CCamera& getCamera(void) const
	{ return camera; };

	unsigned char *getImage(void) const
	{ return image; }

	float *getZBuffer(void) const
	{ return ZBuffer; }

	size_t getNbObjects(void) const { return objects.size(); }
	void addObject(CGenericRenderObject *pObject) { objects.push_back(pObject); }
	CGenericRenderObject * const getObject(size_t numObject) const
	{ if (numObject < objects.size()) return objects[numObject]; else return NULL; }

	size_t getNbLights(void) const { return lights.size(); }
	void addLight(CGenericLight* pLight) { lights.push_back(pLight); }
	CGenericLight * const getLight(size_t numLight) const
	{ if (numLight < lights.size()) return lights[numLight]; else return NULL; }

	size_t getNbTextures(void) const
	{ return textures.size(); }

	map<std::string, CTexture*>::const_iterator getFirstTexture() const
	{ return textures.begin(); }

	CTexture * const getNextTexture(map<std::string, CTexture*>::const_iterator& pos,std::string &key) const
	{
		if (pos != textures.end())
		{
			key = (*pos).first;
			return (*pos++).second;
		}
		else
			return NULL;
	}

	CTexture* const getTexture(const std::string &tname) const
	{
		map<std::string, CTexture*>::const_iterator tpos = textures.find(tname);
		if (tpos != textures.end())
			return (*tpos).second;
		else
			return NULL;
	}

	bool addTexture(CTexture* txt,const std::string &tname)
	{
		map<std::string, CTexture*>::const_iterator tpos = textures.find(tname);
		if (tpos != textures.end())
			return false;
		else
		{
			textures[tname] = txt;
			return true;
		}
	}

	size_t getNbPlugins(void) const
	{ return plugins.size(); }

	map<std::string, CPlugin*>::const_iterator getFirstPlugin() const
	{ return plugins.begin(); }

	CPlugin * const getNextPlugin(map<std::string, CPlugin*>::const_iterator& pos, std::string &key) const
	{
		if (pos != plugins.end())
		{
			key = (*pos).first;
			return (*pos++).second;
		}
		else
			return NULL;
	}

	CPlugin* const getPlugin(const std::string &tname) const
	{
		map<std::string, CPlugin*>::const_iterator tpos = plugins.find(tname);
		if (tpos != plugins.end())
			return (*tpos).second;
		else
			return NULL;
	}

	bool addPlugin(CPlugin* p, const std::string &pname)
	{
		map<std::string, CPlugin*>::const_iterator ppos = plugins.find(pname);
		if (ppos != plugins.end())
			return false;
		else
		{
			plugins[pname] = p;
			return true;
		}
	}

	size_t getNbFrames(void) const
	{ return frames.size(); }
	void addFrame(rays_frame_t* f) { frames.push_back(f); }
	rays_frame_t* getFrame(unsigned int f)
	{
		if (f < frames.size()) return frames[f]; else return NULL; 
	}

	CGenericRay& getLightRay(unsigned int level)
	{
		if (level < RAYS_MAX_LEVEL) return light_ray_levels[level]; else return light_ray_levels[RAYS_MAX_LEVEL - 1];
	}
	CGenericRay& getReflectedRay(unsigned int level)
	{
		if (level < RAYS_MAX_LEVEL) return reflected_ray_levels[level]; else return reflected_ray_levels[RAYS_MAX_LEVEL - 1];
	}
	CGenericRay& getRefractedRay(unsigned int level)
	{
		if (level < RAYS_MAX_LEVEL) return refracted_ray_levels[level]; else return refracted_ray_levels[RAYS_MAX_LEVEL - 1];
	}

private:
	CCamera						camera;
	vector<CGenericLight *>		lights;
	vector<CGenericRenderObject *> objects;
	map<std::string,CTexture*>	textures;
	map<std::string,CPlugin*>	plugins;
	vector<rays_frame_t*>		frames;

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