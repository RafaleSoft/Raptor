// GenericLight.h: interface for the CGenericLight class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENERICLIGHT_H__58510328_5BD8_4C5E_AC59_16E33A42B10C__INCLUDED_)
#define AFX_GENERICLIGHT_H__58510328_5BD8_4C5E_AC59_16E33A42B10C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RaysWorkUnit.h"
#include "Raytracer.h"

class CPhotonMap;

class CGenericLight
{
public:
	CGenericLight(rays_light_t &l);
	virtual ~CGenericLight();

public:
	CGenericVector<float>	position;
	CGenericVector<float>	direction;

	float					intensity;
	float					cutOff;

	float GetSpotCoefficient(const CGenericVector<float> &vector) const;

	void BuildPhotonMap(const CRaytracerData& World,unsigned int nbPhotons);

	void GetPhotonColor(const CGenericVector<float>& hit,CWVector& color);

	void SavePhotonMap(const CRaytracerData& World,const char* name);

private:
	CPhotonMap		*m_pPhotonMap;
	CWVector		color;
	CWVector		tmpColor;
	DWORD			model;
};

#endif // !defined(AFX_GENERICLIGHT_H__58510328_5BD8_4C5E_AC59_16E33A42B10C__INCLUDED_)
