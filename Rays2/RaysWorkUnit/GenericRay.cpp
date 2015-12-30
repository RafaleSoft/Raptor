// GenericRay.cpp: implementation of the CGenericRay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenericRay.h"
#include "GenericLight.h"
#include "RenderObject.h"
#include "Surfacer.h"
#include "Raytracer.h"
#include "Environment.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern unsigned int	Ray_ID;

//////////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////////
static CWVector color_buffer;
__inline void __fastcall A_plus_x_B_saturate(CWVector &A,CWVector &B,float f)
{
	color_buffer[1] = color_buffer[0] = (unsigned short)(255 * f);

	__asm
	{
		mov edi,B
		lea esi,color_buffer
		movq mm0,[edi+4]	//	mm0 = B
		mov eax,[esi+4]
		mov edi,A
		mov [esi+8],eax
		psrlw mm0,8
		movq mm1,[esi+4]	//	mm1 = color_buffer
		movq mm2,[edi+4]	//	mm2 = A

		pmullw mm0,mm1
		paddusw mm0,mm2
		movq [edi+4],mm0
		emms
	}
}

__inline void __fastcall A_plus_x_B_plus_C_saturate(CWVector &A,CWVector &B,CWVector &C,float f)
{
	color_buffer[1] = color_buffer[0] = (unsigned short)(255 * f);

	__asm
	{
		mov edi,C
		movq mm3,[edi+4]	//	mm3 = C
		mov edi,B
		lea esi,color_buffer
		movq mm0,[edi+4]	//	mm0 = B
		mov eax,[esi+4]
		mov edi,A
		mov [esi+8],eax
		psrlw mm0,8
		movq mm1,[esi+4]	//	mm1 = color_buffer
		movq mm2,[edi+4]	//	mm2 = A

		pmullw mm0,mm1
		paddusw mm0,mm2
		paddusw mm0,mm3
		movq [edi+4],mm0
		emms
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGenericRay::CGenericRay()
{
	n = 1.0f;
	fact = 1.0f;
	level = 0;
	id = 0;
	surface = NULL;
	t = HUGE_REAL;
}

CGenericRay::~CGenericRay()
{
}

void CGenericRay::Hit( const CRaytracerData& world, CWVector &c)
{
	CGenericRay	&light_ray = world->light_ray_levels[level+1];
	CGenericRay	&reflected_ray = world->reflected_ray_levels[level+1];
	CGenericRay	&refracted_ray = world->refracted_ray_levels[level+1];

	CWVector	local_color;
	CWVector	photon_color;
	
	float		new_t, Gouraud, Phong ;
	int			numSurfaces,numLight;
	
	c.Zero();
	local_color.Zero();
	photon_color.Zero();

	// --- no surf. in list ? ---
	if ( (numSurfaces = world.getNbObjects()) > 0 )
	{
		// ========================== INTERSECTION ==========================
		new_t = t = HUGE_REAL ;
		CGenericRenderObject	*tmpSurface = NULL;

		// --- loop: Intersection with all surfaces ---
		for ( int i=0;i<numSurfaces; i++ ) 
		{
			tmpSurface = World->objects[i];

			if ( HUGE_REAL > tmpSurface->FastIntersect( *this ) )
			{
				new_t = tmpSurface->Intersect( *this ) ;
			}
			
		    // --- if this is the closest value of dist so far, ---
		    // --- then record the identity of the surface --------
		    if ( new_t < t && new_t > SMALL_REAL) 
			{
				t = new_t ;
				surface = tmpSurface;
			}
		}

		// --- hit an object before back-ground ---
		if ( t < HUGE_REAL )
		{
			// --- compute the intersection point ---
			hit = origin + (t * direction) ;
			hit.H(1);

			// --- compute the normal ---
			normal = surface->GetNormal(hit);

			// ========================== LIGHT =================================
			// --- si l'objet est color-map ---
			//	local_color = ... 
			//	local_color = this->surf->col ;
			local_color = surface->GetLocalColor( *this );

			// --- add the ambiant color ---
			c = local_color * surface->shading.ambient;

			// --- compute shading contribution of lights ---
			Gouraud = Phong = 0.0f ;
			numLight = World->lights.GetSize();

			float tmp;
			float light_factor;

			for ( i=0 ; i<numLight; i++ ) 
			{
				CGenericLight *light = World->lights[i];

				// --- prepare the light ray ---
				light_ray.direction = light->position;
				light_ray.direction -= hit ;

				// --- add the photon map if any ---
				light->GetPhotonColor(hit,photon_color);

				// --- no shadow if surf & light > 180 deg. ---
				if ( (light_ray.direction % normal) <= 0.0 )
					continue ;

				// --- prepare the light ray ---
				light_ray.origin  = hit ;
				light_ray.surface = surface ;
				light_ray.fact = light->intensity ;
				light_ray.id = ++Ray_ID ;

				// --- test light ray with all objets ---
				light_factor = light_ray.Light_Hit(World, local_color,light ) ;	
				if ( light_factor > 0.0 ) 
				{
					// --- facteur de Gouraud ---
					light_ray.direction.Normalize() ;
					tmp = normal % light_ray.direction;

					Gouraud += light_factor *  tmp;

					// --- facteur de Phong ---
					reflected_ray.direction = (tmp + tmp) * normal;
					reflected_ray.direction  -= light_ray.direction ;

					tmp = -(reflected_ray.direction % direction);

					if (tmp>0.0f)
						Phong += (float)(light_factor * pow(tmp,surface->shading.exponent));

				}
			}

			A_plus_x_B_plus_C_saturate(	c,
										local_color,
										photon_color,
										Gouraud * surface->shading.diffuse + Phong  * surface->shading.specular
									   );

			// ========================== REFLECTION ============================

			// --- compute shading contribution from reflections ---
			if ((surface->shading.reflection * fact > UN_SUR_255)&&(level < World->camera.reflection_depth))
			{
				// --- prepare the light ray ---
				tmp = direction % normal;
				reflected_ray.direction = (-tmp - tmp) * normal;
				reflected_ray.direction += direction;

				reflected_ray.direction.Normalize();
				reflected_ray.origin  = hit ;
				reflected_ray.fact = surface->shading.reflection * fact ;
				reflected_ray.id   = ++Ray_ID ;
				reflected_ray.level = level + 1 ;
				reflected_ray.n = n;

				//	Recursively cast this ray to generate reflections
				reflected_ray.Hit(World,local_color);

				A_plus_x_B_saturate(	c,
										local_color,
										reflected_ray.fact
									);
			}

			// ========================== REFRACTION ============================

			// --- compute shading contribution from refractions ---
			if ((surface->shading.refraction * fact > UN_SUR_255) && (level < World->camera.refraction_depth)) 
			{
				float costheta1,costheta2,coef,n1,n2;
				float n1n2,n2n1;
		
				n1 = n;
				n2 = surface->shading.indice;
				costheta1 = direction % normal;

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
						refracted_ray.direction = coef * normal + n1n2 * direction;
						refracted_ray.direction.Normalize();
						refracted_ray.fact = surface->shading.refraction * fact ;
					}

					// sinon, on reflete.
					else
					{
						refracted_ray.fact = surface->shading.refraction * fact ;
						refracted_ray.direction = ((-2.0f*costheta1) * (normal));
						refracted_ray.direction += direction;
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

					refracted_ray.fact = surface->shading.refraction * fact ;
					refracted_ray.direction = (coef * normal);
					refracted_ray.direction += (n1n2 * direction);
					refracted_ray.direction.Normalize();
				}
		
				refracted_ray.origin = hit;
				refracted_ray.id   = ++Ray_ID ;
				refracted_ray.level = level + 1 ;
				refracted_ray.n = n2;

				//	Recursively cast this ray to generate refractions
				refracted_ray.Hit(world,local_color);

				A_plus_x_B_saturate(	c,
										local_color,
										refracted_ray.fact
									);
			}
		}

		//	
		//	Environment
		//
		else
		{
			c = CEnvironment::GetInstance()->GetLocalColor(direction);
		}
	}

	//	alpha is set to 1.0
	//	but maybe not necessary
	c.H(65535);
}

float CGenericRay::Light_Hit( const CRaytracerData& world, CWVector &c,CGenericLight *light)
{
	float			fact = this->fact ;
	float			t,k,d;
	unsigned int	size=world.getNbObjects();
	
	CGenericRenderObject	*surface;

	float spot = light->GetSpotCoefficient(this->direction);
	if (spot > 0)
		fact *= spot;
	else
		return 0;

	//	For some intersections, emited ray must
	//	be normalized
	float N = this->direction.Norm();
	this->direction.Normalize();

	// --- loop once per surface --- 
	for ( int i=0; i<size; i++ ) 
	{
		surface = world->objects[i];

	    // --- no self shadowing --- 
	  //  if ( this->surface == surface )
		//	continue ;

		t = surface->FastIntersect( *this );
	    if (( t <= 0.0f) || (t >= N))
			continue ;
		
		t = surface->Intersect( *this ) ;

	    // --- if this is the closest value of dist so far, --- 
	    // --- then record the identity of the surface -- 
	    //if ( t < 1.0f && t > 0.0f )
		if ( t < N && t > 0.0f ) 
		{
			k=surface->shading.refraction;
			// --- if alpha-texture map ---
			// --- if translucent material
			
			if ( k > 0.0f ) 
			{
				CWVector &lc = surface->GetLocalColor(*this);
				fact *= k;
				d = surface->shading.diffuse * k;
				
				c *= (1 - d);
				lc *= d;
				c += lc;
				
			}
			else
				return( 0 ) ;
		} // if		( object found )
	} // for	( all surfaces )

	return( fact );
}

