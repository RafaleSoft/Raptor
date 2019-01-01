// Surfacer.h: interface for the CSurfacer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURFACER_H__4C22C1FF_6660_4186_9167_97D476081526__INCLUDED_)
#define AFX_SURFACER_H__4C22C1FF_6660_4186_9167_97D476081526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Messages.h"

class CSurfacer  
{
public:
	CSurfacer();
	virtual ~CSurfacer();

	bool InitPlugins(const CRaytracerData& raytracer_data);

	bool InitNormalGenerator(rays_plugin_t& gen);
	bool GenerateNormal(CGenericVector<float> point,CGenericVector<float> &normal);
	bool PerturbNormal(CGenericVector<float> point,CGenericVector<float> &normal);
	

private:
	rays_plugin_t	normal;
	bool			generateNormal;
	void			*pNormal;

	CGenericVector<float> turb;
};

#endif // !defined(AFX_SURFACER_H__4C22C1FF_6660_4186_9167_97D476081526__INCLUDED_)
