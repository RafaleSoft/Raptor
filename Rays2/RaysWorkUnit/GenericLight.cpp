// GenericLight.cpp: implementation of the CGenericLight class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaysWorkUnit.h"
#include "GenericLight.h"
#include "PhotonMap.h"


extern unsigned int	Ray_ID;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGenericLight::CGenericLight(rays_light_t &l)
{
	position.Set(l.position.x,l.position.y,l.position.z,l.position.h);
	color.Set(	(short)(0xFFFF*l.color.x),
				(short)(0xFFFF*l.color.y),
				(short)(0xFFFF*l.color.z),
				(short)(0xFFFF*l.color.h));
	intensity = l.intensity;

	direction.Set(	l.direction.x - l.position.x,
					l.direction.y - l.position.y,
					l.direction.z - l.position.z,
					l.direction.h - l.position.h);
	direction.Normalize();

	cutOff = l.cutoff;
	m_pPhotonMap = NULL;
	model = l.model;
}

CGenericLight::~CGenericLight()
{
	if (m_pPhotonMap != NULL)
		delete m_pPhotonMap;
}

float CGenericLight::GetSpotCoefficient(const CGenericVector<float> &vector) const
{
	float spot = 1.0f;

	if ((model & SPOT_EXP) == SPOT_EXP)
	{
		float n = vector.Norm();
		float cos_angle = -(direction % vector) / n;
		if (cos_angle > cos(TO_RADIAN(cutOff)))
		{
			float angle = 0.25f * acos(cos_angle);
			spot = exp(-angle * DEG);
		}
		else
			spot = 0.0f;
	}
	else if ((model & SPOT_COS) == SPOT_COS)
	{
		float n = vector.Norm();
		float cos_angle = -(direction % vector) / n;
		if (cos_angle > cos(cutOff * RAD))
		{
			float angle = acos(cos_angle);
			angle = angle * M_PI / (2.0f * cutOff * RAD);
			spot = cos(angle);

		}
		else
			spot = 0.0f;
	}
	else if ((model & SPOT) == SPOT)
	{
		if (-(direction % vector) < cos(cutOff * RAD) * vector.Norm())
			spot = 0.0f;
	}
	
	return spot;
}


void CGenericLight::BuildPhotonMap(const CRaytracerData& World,unsigned int nbPhotons)
{
	if ((model & PHOTON_MAP) == PHOTON_MAP)
	{
		m_pPhotonMap = new CPhotonMap(nbPhotons);

		//	find a direction of emission
		CGenericRay	light_ray;

		light_ray.origin = position;
		int oldPercent = 0;

		float xmin = HUGE_REAL;
		float xmax = -HUGE_REAL;
		float ymin = HUGE_REAL;
		float ymax = -HUGE_REAL;
		float zmin = HUGE_REAL;
		float zmax = -HUGE_REAL;

		float cosCutOff = cos(cutOff * RAD);
		float sinCutOff = sin(cutOff * RAD);

		float angle = acos(direction.X());
		if (asin(direction.X()) < 0)
			angle = -angle;
		float x = cos(cutOff * RAD + angle);
		if (x > xmax) xmax = x;
		if (x < xmin) xmin = x;
		x = cos(-cutOff * RAD + angle);
		if (x > xmax) xmax = x;
		if (x < xmin) xmin = x;

		angle = acos(direction.Y());
		if (asin(direction.Y()) < 0)
			angle = -angle;
		x = cos(cutOff * RAD + angle);
		if (x > ymax) ymax = x;
		if (x < ymin) ymin = x;
		x = cos(-cutOff * RAD + angle);
		if (x > ymax) ymax = x;
		if (x < ymin) ymin = x;

		angle = acos(direction.Z());
		if (asin(direction.Z()) < 0)
			angle = -angle;
		x = cos(cutOff * RAD + angle);
		if (x > zmax) zmax = x;
		if (x < zmin) zmin = x;
		x = cos(-cutOff * RAD + angle);
		if (x > zmax) zmax = x;
		if (x < zmin) zmin = x;

		//CWVector photonColor;
		while (m_pPhotonMap->GetNbPhotons() < nbPhotons)
		{
			bool valid = false;
			while (!valid)
			{
				light_ray.direction.X() = ((xmax - xmin) * rand()) / RAND_MAX + xmin;
				light_ray.direction.Y() = ((ymax - ymin) * rand()) / RAND_MAX + ymin;
				light_ray.direction.Z() = ((zmax - zmin) * rand()) / RAND_MAX + zmin;
				light_ray.direction.H() = 1.0f;
				light_ray.direction.Normalize();

				valid = (cosCutOff < (direction % light_ray.direction));
			}

			light_ray.n = 1.0f;
			light_ray.fact = 1.0f;
			light_ray.level = 0;
			light_ray.id = ++Ray_ID;
			light_ray.surface = NULL;
			light_ray.t = HUGE_REAL;

			//photonColor = color * GetSpotCoefficient(-light_ray.direction);
			m_pPhotonMap->Hit(World,light_ray,color);

			int percent = 100 * m_pPhotonMap->GetNbPhotons() / nbPhotons;
			if (percent != oldPercent)
			{
				COUT << percent << " %" << endl;
				oldPercent = percent;
			}
		}

		m_pPhotonMap->BuildPhotonMap();

COUT << "Photon map : nb photons = " << m_pPhotonMap->GetNbPhotons() << endl;
COUT << "Photon map END" << endl;
		SavePhotonMap(World,"photonmap");
	}
	else
	{
COUT << "Light has no photon map specification" << endl;
	}
}

void CGenericLight::GetPhotonColor(const CGenericVector<float>& hit,CWVector& color)
{
	if (m_pPhotonMap != NULL)
	{
		m_pPhotonMap->GetDensity(hit,tmpColor);

		CWVector &c = tmpColor;
		__asm
		{
			mov edi,color
			mov esi,c
			movq mm0,[edi+4]	//	mm0 = color
			movq mm1,[esi+4]	//	mm1 = tmpColor
			paddusw mm0,mm1
			movq [edi+4],mm0
			emms
		}
	}
}

void CGenericLight::SavePhotonMap(const CRaytracerData& World,const char* name)
{
	CGenericRay photonRay;
	photonRay.origin = World->camera.origin;
	unsigned char *photonMap = new unsigned char[World->camera.height*World->camera.width*4];
	memset(photonMap,0,World->camera.height*World->camera.width*4);

	/////////////////////////////////////////////////////////////////////////////
	//	optical settings
	//	1/P + 1/I = 1/F
	//	P/I = S/s = grandissement
	float			p_over_i = World->camera.object_plane / (World->camera.focale*0.001f) - 1.0f;
	float			grand = p_over_i * DIM_FILM;
	//float ouvert = (data->camera.aperture < 128.0f) ? (data->camera.focale*0.001f) / data->camera.aperture : 0.0f ;
	float			scale = grand / World->camera.height ;	

	int max = m_pPhotonMap->GetNbPhotons();
	float x;
	float y;

	for (int p=0;p<max;p++)
	{
		CPhotonMap::photon ph = m_pPhotonMap->GetPhoton(p);
	
		photonRay.direction = ph.position;
		float t = World->camera.object_plane / (photonRay.origin.Z() - photonRay.direction.Z());

		if (t > 0)
		{
			x = t * (photonRay.direction.X() - photonRay.origin.X());
			y = t * (photonRay.direction.Y() - photonRay.origin.Y());

			x = (x / scale) + (World->camera.width>>1);
			y = (y / scale) + (World->camera.height>>1);

			if ((x >= 0) && ( x < World->camera.width) &&
				(y >= 0) && ( y < World->camera.height))
			{
				unsigned int ix = (floor(x)) * 4;
				unsigned int iy = (floor(y)) * 4;
				unsigned int offset = (unsigned int)(iy * World->camera.width + ix);

				int val = photonMap[offset] + (ph.energy[0] >> 8);
				if (val > 255) val = 255;
				photonMap[offset] = (unsigned char)(val & 0xff);

				val = photonMap[offset+1] + (ph.energy[1] >> 8);
				if (val > 255) val = 255;
				photonMap[offset+1] = (unsigned char)(val & 0xff);

				val = photonMap[offset+2] + (ph.energy[2] >> 8);
				if (val > 255) val = 255;
				photonMap[offset+2] = (unsigned char)(val & 0xff);

				photonMap[offset+3] = 255;
			}
		}
	}

	int numLight = 0;
	for (int i=0;i<World->lights.GetSize();i++)
	{
		if (World->lights[i] == this)
			numLight = i;
	}
/*
	CString txtname = name;
	txtname.Format("%s_%d.txt",name,numLight);
	m_pPhotonMap->DumpPhotons(LPCTSTR(txtname));
*/
	CString tganame = name;
	tganame.Format("%s_%d.tga",name,numLight);
	DumpImage(LPCTSTR(tganame),photonMap);

	delete [] photonMap;
}