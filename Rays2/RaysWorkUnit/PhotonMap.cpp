// PhotonMap.cpp: implementation of the CPhotonMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaysWorkUnit.h"
#include "PhotonMap.h"
#include "GenericLight.h"
#include "GenericRenderObject.h"


extern unsigned int	Ray_ID;

static  float DIST = 0.0f;

#define DELTA_PHOTON_DENSITY	0.2
#define VOLUME_PHOTON_DENSITY	(DELTA_PHOTON_DENSITY*DELTA_PHOTON_DENSITY*DELTA_PHOTON_DENSITY)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPhotonMap::CPhotonMap(unsigned int minsize)
	:nbPhotons(0)
{
	if (minsize < 1)
		photonsPerLight = 1;
	else
		photonsPerLight = minsize;

	m_pPhotons = new photon[photonsPerLight];
	m_pHints = NULL;
	minHintIndex = maxHintIndex = 0;

	//	densite theorique en supposant que l'energie est
	//	proportionnelle au carré de la distance :
	//	65535 comme valeur maximum devient neglieable pour 
	//	d = 256, ce qui donne un volume ~ 4*256*256*256.
	//	d'ou la densite globale moyenne
	m_fGlobalDensity = ((float)photonsPerLight) / (4 * 256 * 256 * 256);
	DIST = m_fGlobalDensity * DELTA_PHOTON_DENSITY * 1e4;
}

CPhotonMap::~CPhotonMap()
{
	if (m_pPhotons != NULL)
		delete m_pPhotons;
	if (m_pHints != NULL)
		delete m_pHints;
}


void CPhotonMap::DumpPhotons(const std::string& name)
{
	char	buffer[256] ;
	FILE *out = fopen( name.c_str(), "wb" );
	for (unsigned int p=0;p<photonsPerLight;p++)
	{
		const CPhotonMap::photon &ph = m_pPhotons[p];
		sprintf(buffer,"x=%f y=%f z=%f - r=%d g=%d b=%d\n",
				ph.position[0],
				ph.position[1],
				ph.position[2],
				ph.energy[0],
				ph.energy[1],
				ph.energy[2]);
		fwrite(buffer,strlen(buffer),1,out);
	}

	fclose(out);
}


int photonOrder(const void *elem1, const void *elem2 )
{
	if (((CPhotonMap::lpphoton)elem1)->position[0] < ((CPhotonMap::lpphoton)elem2)->position[0])
		return -1;
	else if (((CPhotonMap::lpphoton)elem1)->position[0] > ((CPhotonMap::lpphoton)elem2)->position[0])
		return 1;
	else
		return 0;
}


void CPhotonMap::GetDensity(const CGenericVector<float> position, CColor::RGBA& target) const
{
	//	Find nearest X
	int step = photonsPerLight / 4;
	unsigned int pos = photonsPerLight / 2;

	int min = (int)(floor(position.X())) - minHintIndex;
	if ((min < 0) || (min >= maxHintIndex))
	{
		target.r = 0;
		target.b = 0;
		target.g = 0;
		return;
	}
	
	pos = (m_pHints[min+1] + m_pHints[min]) >> 1;
	step = (m_pHints[min+1] - m_pHints[min]) >> 2;

	while (step > 1)
	{
		if (m_pPhotons[pos].position[0] > position.X())
			pos -= step;
		else
			pos += step;
		step = step >> 1;
	}

	step = pos;

	float R = 0;
	float G = 0;
	float B = 0;

	float deltax = ABS(m_pPhotons[step].position[0] - position.X());
	while ((step > 0) && (deltax < DELTA_PHOTON_DENSITY))
	{
		if (DELTA_PHOTON_DENSITY > (ABS(m_pPhotons[step].position[1] - position.Y())))
			if (DELTA_PHOTON_DENSITY > (ABS(m_pPhotons[step].position[2] - position.Z())))
			{
				float dist = 	1e-5 + deltax*deltax +
								(m_pPhotons[step].position[1] - position.Y())*(m_pPhotons[step].position[1] - position.Y()) +
								(m_pPhotons[step].position[2] - position.Z())*(m_pPhotons[step].position[2] - position.Z());

				dist =  DIST * sqrt(dist);
				float scale = 1.0f / dist;
								
				R += scale * m_pPhotons[step].energy[0];
				G += scale * m_pPhotons[step].energy[1];
				B += scale * m_pPhotons[step].energy[2];
			}
		step--;
		deltax = ABS(m_pPhotons[step].position[0] - position.X());
	}
	step = pos;
	deltax = ABS(m_pPhotons[step].position[0] - position.X());
	while ((step < photonsPerLight) && (deltax < DELTA_PHOTON_DENSITY))
	{
		if (DELTA_PHOTON_DENSITY > (ABS(m_pPhotons[step].position[1] - position.Y())))
			if (DELTA_PHOTON_DENSITY > (ABS(m_pPhotons[step].position[2] - position.Z())))
			{
				float dist = 	1e-5 + deltax*deltax +
								(m_pPhotons[step].position[1] - position.Y())*(m_pPhotons[step].position[1] - position.Y()) +
								(m_pPhotons[step].position[2] - position.Z())*(m_pPhotons[step].position[2] - position.Z());

				dist =  DIST * sqrt(dist);
				float scale = 1.0f / dist;

				R += scale * m_pPhotons[step].energy[0];
				G += scale * m_pPhotons[step].energy[1];
				B += scale * m_pPhotons[step].energy[2];
			}
		step++;
		deltax = ABS(m_pPhotons[step].position[0] - position.X());
	}

	target.r = R;
	target.g = G;
	target.b = B;
}

void CPhotonMap::AddPhoton(photon& p)
{
	if (nbPhotons < photonsPerLight)
		m_pPhotons[nbPhotons++] = p;
}

void CPhotonMap::BuildPhotonMap(void)
{
	qsort( &m_pPhotons[0], photonsPerLight, sizeof(photon), photonOrder );

	int min = (int)floor(m_pPhotons[0].position[0]);
	int max = (int)ceil(m_pPhotons[photonsPerLight-1].position[0]);

	unsigned int step = 0;
	unsigned int nb = max - min + 1;
	m_pHints = new int[nb];
	m_pHints[0] = 0;
	minHintIndex = min;
	maxHintIndex = max - min;

	min++;
	for (unsigned int i=1;i<nb;i++)
	{
		while ((step < photonsPerLight) && (m_pPhotons[step].position[0] < min))
			step++;
		if (step < photonsPerLight)
		{
			m_pHints[i] = step;
			min++;
		}
	}
	m_pHints[nb-1] = photonsPerLight - 1;

	vector<unsigned int> neighbours;
	for (unsigned int i=0;i<photonsPerLight;i++)
	{
		CGenericVector<float> position(m_pPhotons[i].position[0],m_pPhotons[i].position[1],m_pPhotons[i].position[2],1.0f);
		neighbours.resize(5000,100);
		unsigned int pos = 0;
		unsigned int nb = 0;

		while ((pos < photonsPerLight) && ((m_pPhotons[pos].position[0] - position.X()) < DELTA_PHOTON_DENSITY))
		{
			if (DELTA_PHOTON_DENSITY > (ABS(m_pPhotons[pos].position[0] - position.X())))
			if (DELTA_PHOTON_DENSITY > (ABS(m_pPhotons[pos].position[1] - position.Y())))
			if (DELTA_PHOTON_DENSITY > (ABS(m_pPhotons[pos].position[2] - position.Z())))
			{
				neighbours.push_back(pos);
				nb++;
			}
			pos++;
		}

		std::cout << ".Photon " << i << ": " << nb << std::endl;
		neighbours.clear();
	}
}

void CPhotonMap::Hit( CRaytracerData& World, CGenericRay& lightRay,CColor::RGBA& sourceColor )
{
	unsigned int level = lightRay.level;
	CColor::RGBA local_color;
	float		 new_t = 0;
	unsigned int numSurfaces = 0;

	CGenericRay	&reflected_ray = World.getReflectedRay(level+1);
	CGenericRay	&refracted_ray = World.getRefractedRay(level+1);

	if ( (numSurfaces = World.getNbObjects()) > 0 )
	{
		// ========================== INTERSECTION ==========================
		new_t = lightRay.t = HUGE_REAL ;
		CGenericRenderObject	*tmpSurface = NULL;
		CGenericRenderObject	*oldSurface = lightRay.surface;

		// --- loop: Intersection with all surfaces ---
		for ( unsigned int i=0;i<numSurfaces; i++ ) 
		{
			tmpSurface = World.getObject(i);

			if ( HUGE_REAL > tmpSurface->FastIntersect( lightRay ) )
			{
				new_t = tmpSurface->Intersect( lightRay ) ;
			}
			
		    // --- if this is the closest value of dist so far, ---
		    // --- then record the identity of the surface --------
		    if ( new_t < lightRay.t && new_t > SMALL_REAL) 
			{
				lightRay.t = new_t ;
				lightRay.surface = tmpSurface;
			}
		}

		// --- hit an object before back-ground ---
		if ( lightRay.t < HUGE_REAL )
		{
			// --- compute the intersection point ---
			lightRay.hit = lightRay.origin + (lightRay.direction * lightRay.t);
			lightRay.hit.H(1);

			local_color = lightRay.surface->GetLocalColor( lightRay );
			local_color.r = (local_color.r * lightRay.surface->shading.diffuse);
			local_color.g = (local_color.g * lightRay.surface->shading.diffuse);
			local_color.b = (local_color.b * lightRay.surface->shading.diffuse);

			// --- create a photon at the point of indirect illumination ---
			if ((oldSurface != lightRay.surface) && (level > 1))
			{
				photon p;
				memcpy(&p.position,lightRay.hit.vector(),4*sizeof(float));
				memcpy(&p.energy,sourceColor.operator const float *(),4*sizeof(float));
				p.nb_neighbours = 0;
				p.pNeighbours = NULL;
				AddPhoton(p);
			}

			//	cast a 'reflective' photon if reflectance permits
			//if (lightRay.surface->shading.reflection * lightRay.fact > UN_SUR_255)
			if (level <= World.getCamera().reflection_depth)
			{
				// --- compute the normal ---
				lightRay.normal = lightRay.surface->GetNormal(lightRay.hit);

				// --- prepare the light ray ---
				float tmp = lightRay.direction % lightRay.normal;

				reflected_ray.direction = lightRay.normal * (-tmp - tmp);
				reflected_ray.direction += lightRay.direction;

				reflected_ray.direction.Normalize();
				reflected_ray.origin  = lightRay.hit ;
				reflected_ray.fact = lightRay.surface->shading.reflection * lightRay.fact ;
				reflected_ray.id   = ++Ray_ID ;
				reflected_ray.level = level + 1 ;
				reflected_ray.n = lightRay.n;
				reflected_ray.surface = lightRay.surface;

				//	Recursively cast this ray to generate reflections
				CColor::RGBA tmpColor;
				tmpColor.r = (local_color.r * lightRay.surface->shading.reflection);
				tmpColor.g = (local_color.g * lightRay.surface->shading.reflection);
				tmpColor.b = (local_color.b * lightRay.surface->shading.reflection);
				Hit(World,reflected_ray,tmpColor);
			}

			//	cast a 'refractive' photon if reflectance permits
			//if (lightRay.surface->shading.refraction * lightRay.fact > UN_SUR_255)
			if (level <= World.getCamera().refraction_depth)
			{
				float costheta1,costheta2,coef,n1,n2;
				float n1n2,n2n1;
		
				n1 = lightRay.n;
				n2 = lightRay.surface->shading.indice;
				costheta1 = lightRay.direction % lightRay.normal;

				//sortie du volume refringeant
				if (costheta1 >= 0.0 )
				{
					n2 = 1.0;
					coef = 1 - costheta1 * costheta1;

					// si inferieur a l'angle limite de refraction
					n2n1 = n2 / n1;
					n1n2 = n1 / n2;

					if (cos(asin(n2n1))<=costheta1)
					{
						costheta2 = (float)(sqrt(1 - (n1n2 * n1n2 * coef)));
						coef = costheta2 - n1n2 * costheta1;
						refracted_ray.direction = lightRay.normal * coef + lightRay.direction * n1n2;
						refracted_ray.direction.Normalize();
						refracted_ray.fact = lightRay.surface->shading.refraction * lightRay.fact ;
					}

					// sinon, on reflete.
					else
					{
						refracted_ray.fact = lightRay.surface->shading.refraction * lightRay.fact ;
						refracted_ray.direction = lightRay.normal * (-2.0f*costheta1);
						refracted_ray.direction += lightRay.direction;
						refracted_ray.direction.Normalize();
					}
				}

				//entree du volume refringeant
				else
				{
					n1n2 = n1 / n2;
					costheta1 = -costheta1;
					coef = 1 - costheta1 * costheta1;
					costheta2 = (float)(sqrt(1 - (n1n2 * n1n2 * coef)));
					coef = n1n2 * costheta1 - costheta2;

					refracted_ray.fact = lightRay.surface->shading.refraction * lightRay.fact ;
					refracted_ray.direction = (lightRay.normal * coef);
					refracted_ray.direction += (lightRay.direction * n1n2);
					refracted_ray.direction.Normalize();
				}
		
				refracted_ray.origin = lightRay.hit;
				refracted_ray.id   = ++Ray_ID ;
				refracted_ray.level = level + 1 ;
				refracted_ray.n = n2;
				refracted_ray.surface = lightRay.surface;

				//	Recursively cast this ray to generate refractions
				CColor::RGBA tmpColor;
				tmpColor.r = (local_color.r * lightRay.surface->shading.refraction);
				tmpColor.g = (local_color.g * lightRay.surface->shading.refraction);
				tmpColor.b = (local_color.b * lightRay.surface->shading.refraction);
				Hit(World,refracted_ray,tmpColor);
			}
		}
		else
		{
			//	Hit background.
			//	The photon is lost in deeeeeep space.
		}
	}
}