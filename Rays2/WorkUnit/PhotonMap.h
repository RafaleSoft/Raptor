// PhotonMap.h: interface for the CPhotonMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHOTONMAP_H__988F1B06_04FB_4506_B0AF_AAF4853A6909__INCLUDED_)
#define AFX_PHOTONMAP_H__988F1B06_04FB_4506_B0AF_AAF4853A6909__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Raytracer.h"

class CPhotonMap
{
public:
	typedef struct photon_t
	{
		float			position[4];
		unsigned short	energy[4];
		unsigned int	nb_neighbours;
		unsigned int	*pNeighbours;
	} photon;
	typedef photon* lpphoton;

public:
	CPhotonMap(unsigned int minsize);
	virtual ~CPhotonMap();

	//	Add to the photon map the photons corresponding to the lightRay
	void Hit( LPRAYTRACERDATA World, CGenericRay& lightRay ,CWVector& sourceColor);

	//	This function prepares the photon map
	//	to repond to queries : GetDensity.
	//	It must be called after the photon map has
	//	been filled with appropriate calls to Hit
	void BuildPhotonMap(void);

	unsigned int GetNbPhotons(void) const { return nbPhotons; };
	photon GetPhoton(unsigned int numPhoton) const
	{ if (numPhoton<photonsPerLight) return m_pPhotons[numPhoton]; else return m_pPhotons[0]; };

	void GetDensity(const CGenericVector<float> position,CWVector& target) const;

	//	For DEBUG purpose only
	void DumpPhotons(const char* name);

private:
	unsigned int			photonsPerLight;
	unsigned int			nbPhotons;
	lpphoton				m_pPhotons;

	int						*m_pHints;
	int						minHintIndex;
	int						maxHintIndex;

	float					m_fGlobalDensity;

	void AddPhoton(photon& p);
};

#endif // !defined(AFX_PHOTONMAP_H__988F1B06_04FB_4506_B0AF_AAF4853A6909__INCLUDED_)
