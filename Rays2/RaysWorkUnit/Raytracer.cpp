#include "stdafx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/BoundingBox.h"
#endif


//#include "GenericRay.h"
#include "GenericLight.h"
#include "RenderObject.h"
#include "Texture.h"
#include "Triangle.h"
#include "Mesh.h"
#include "SSE_CMesh.h"
//#include "SSE_BoundingBox.h"
//#include "Environment.h"
//#include "PhotonMap.h"
#include "..\Plugin.h"

#include "RaysWorkUnit.h"
#include "Raytracer.h"

/////////////////////////////////////////////////////////////////////////////
// Raytracer data
//
unsigned int	Ray_ID = 0;
CColor::RGBA	*colorImage;

CRaytracerData::CRaytracerData()
{
	camera.origin.Set(0.0,0.0,20.0,1.0);	// centre optique
	camera.focale = 50.0f;	// en millimètres, 50 correspond ~ à la vision humaine 
							// longueur focale ou distance focale ?
	camera.aperture = 2.8f;
	camera.object_plane = 10.0f;	// plan objet, en mètres
	camera.width = 320;
	camera.height = 240;
	camera.variance = 5;
	camera.reflection_depth = 5;
	camera.refraction_depth = 5;
	camera.crease = 0;
	camera.photon_map = 200000;
	
	start = 0;
	end = 0;
	percent = 0;
	image = NULL;
	ZBuffer = NULL;
}

CRaytracerData::~CRaytracerData()
{
	for (unsigned int i=0;i<lights.size();i++)
		delete lights[i];
	lights.clear();

	for (unsigned int i=0;i<objects.size();i++)
		delete objects[i];
	objects.clear();

	for (unsigned int i=0;i<textures.size();i++)
	{
		delete(textures.begin()->second);
		textures.erase(textures.begin());
	}
	textures.clear();

	for (unsigned int i=0;i<plugins.size();i++)
	{
		delete(plugins.begin()->second);
		plugins.erase(plugins.begin());
	}
	plugins.clear();

	if (NULL != image)
		delete [] image;
	if (NULL != ZBuffer)
		delete [] ZBuffer;
}

void CRaytracerData::allocateBuffer()
{
	if (NULL != ZBuffer)
		delete[] ZBuffer;
	ZBuffer = new float[(end - start)*camera.width];
	if (NULL != image)
		delete[] image;
	image = new unsigned char[(end - start)*camera.width * 4];
}

void CRaytracerData::updateCamera(const rays_config_t &config)
{
	camera.width = config.width;
	camera.height = config.height;
	camera.variance = config.variance;
	camera.reflection_depth = config.reflection;
	camera.refraction_depth = config.refraction;
	camera.focale = config.focale;
	camera.object_plane = config.object_plane;
	camera.crease = (float)(PI_SUR_180 * config.crease);
	camera.photon_map = config.photon_map;
}

//
//	Pack Image from 64bits colors to 32bits colors
//
/////////////////////////////////////////////////////////////////////////////
/*
void PackImage(int size,const CRaytracerData& raytracer_data)
{
	unsigned short *c1,*c2;
	int i=0;
	int max = size & 0xFFFFFFFE;
	unsigned int *ofs = (unsigned int*)raytracer_data.getImage();
	unsigned short mask[4] = { 0x00FF, 0x00FF, 0x00FF, 0x00FF };

	__asm pxor mm5,mm5;
	__asm pxor mm6,mm6;
	__asm 
	{
		lea esi,mask
		movq mm7,[esi]
	}

	while (i<max)
	{
		c1 = colorImage[i++].vector();
		c2 = colorImage[i++].vector();
		
		__asm 
		{
			mov esi,c1
			movq mm0,[esi]
			
			mov esi,c2
			movq mm1,[esi]
			
			mov edi,ofs
			
			paddusw mm0,mm5
			paddusw mm1,mm6

			movq mm5,mm0
			movq mm6,mm1

			pand mm5,mm7
			pand mm6,mm7

			psrlw mm0,8
			psrlw mm1,8

			packuswb mm0,mm1
			movq [edi],mm0
		}
		ofs+=2;
	}
	__asm emms;
}
*/
/////////////////////////////////////////////////////////////////////////////
// Variance : recursive addaptative subsampling for antialiasing
//		and texture bilinear filtering
//
CColor::RGBA variance(float centerx,float centery,float scale,int depth,
					  CGenericRay &r,CRaytracerData& raytracer_data)
{
	if (depth  == 0 )
	{
		CGenericRay vray;
		CColor::RGBA c;

		// preparation du rayon
		vray.origin = r.origin;
		vray.direction = -vray.origin;
		vray.direction.X() += centerx;	
		vray.direction.Y() += centery;
		vray.direction.Z() = -raytracer_data.getCamera().object_plane ;
		vray.direction.Normalize();
		vray.n = r.n;
		vray.level = r.level;
		vray.fact = r.fact;
		vray.id    = ++Ray_ID ;
		vray.Hit(raytracer_data,c);

		return c;
	}
	else
	{
		int variancelimit = raytracer_data.getCamera().variance;
		CColor::RGBA m,v;
		CColor::RGBA c1,c2,c3,c4;
		float f1,f2,f3,f4;

		if ((variancelimit>0)||(depth==1))
		{
			c1 = variance(centerx-scale,centery-scale,0.5f*scale,0,r,raytracer_data);
			f1 = r.hit.Z();

			c2 = variance(centerx+scale,centery-scale,0.5f*scale,0,r,raytracer_data);
			f2 = r.hit.Z();

			c3 = variance(centerx-scale,centery+scale,0.5f*scale,0,r,raytracer_data);
			f3 = r.hit.Z();

			c4 = variance(centerx+scale,centery+scale,0.5f*scale,0,r,raytracer_data);
			f4 = r.hit.Z();

			r.hit.Z(0.25f*(f1+f2+f3+f4));
			

			// special care for data overflow
#if !defined(_WIN64)
			__asm
			{
				lea esi,m
				lea edi,c1
				movq mm0,[edi+4]

				lea edi,c2
				psrlw mm0,2
				movq mm1,[edi+4]

				lea edi,c3
				psrlw mm1,2
				movq mm2,[edi+4]

				lea edi,c4
				psrlw mm2,2
				movq mm3,[edi+4]

				paddw mm0,mm1
				psrlw mm3,2

				paddw mm2,mm3
				paddw mm0,mm2

				movq [esi+4],mm0
			}
#else
			//m.X() = ((ULONG)c1.X() + (ULONG)c2.X() + (ULONG)c3.X() + (ULONG)c4.X()) * 0.25f;
			//m.Y() = ((ULONG)c1.Y() + (ULONG)c2.Y() + (ULONG)c3.Y() + (ULONG)c4.Y()) * 0.25f;
			//m.Z() = ((ULONG)c1.Z() + (ULONG)c2.Z() + (ULONG)c3.Z() + (ULONG)c4.Z()) * 0.25f;
#endif

			// calcul des variances des couleurs
#if !defined(_WIN64)
			__asm
			{
				//lea esi,m			// already set
				lea edi,c1
				//movq mm0,[esi+4]	// already set
				movq mm1,[edi+4]
				psubsw mm1,mm0

				lea edi,c2
				psraw mm1,8
				movq mm2,[edi+4]
				pmullw mm1,mm1
				psubsw mm2,mm0

				lea edi,c3
				psraw mm2,8
				movq mm3,[edi+4]
				pmullw mm2,mm2
				psubsw mm3,mm0

				lea edi,c4
				psraw mm3,8
				movq mm4,[edi+4]
				pmullw mm3,mm3
				psubsw mm4,mm0

				lea edi,v
				psraw mm4,8
				paddw mm1,mm2
				pmullw mm4,mm4
				paddw mm3,mm4
				paddw mm1,mm3
				movq [edi+4],mm1

				emms
			}
#else
			//v = (c1 - m)*(c1 - m) + (c2 - m)*(c2 - m) + (c3 - m)*(c3 - m) + (c4 - m)*(c4 - m);
#endif
		}

		//	Return if hit background
		if (r.hit.Z() < 0.01f)
			return m;

		// test seuil
		if (((v.r>=variancelimit)||(v.g>=variancelimit)||(v.b>=variancelimit))&&(depth>1))
		{
			c1=variance(centerx-scale,centery-scale,0.5f*scale,depth-1,r,raytracer_data);
			c2=variance(centerx+scale,centery-scale,0.5f*scale,depth-1,r,raytracer_data);
			c3=variance(centerx-scale,centery+scale,0.5f*scale,depth-1,r,raytracer_data);
			c4=variance(centerx+scale,centery+scale,0.5f*scale,depth-1,r,raytracer_data);

#if !defined(_WIN64)
			__asm
			{
				lea esi,m
				lea edi,c1
				movq mm0,[edi+4]

				lea edi,c2
				psrlw mm0,2
				movq mm1,[edi+4]

				lea edi,c3
				psrlw mm1,2
				movq mm2,[edi+4]

				lea edi,c4
				psrlw mm2,2
				movq mm3,[edi+4]

				paddw mm0,mm1
				psrlw mm3,2

				paddw mm2,mm3
				paddw mm0,mm2

				movq [esi+4],mm0

				emms
			}
#else
			/*
			m.X() = ((ULONG)c1.X() + (ULONG)c2.X() + (ULONG)c3.X() + (ULONG)c4.X()) * 0.25f;
			m.Y() = ((ULONG)c1.Y() + (ULONG)c2.Y() + (ULONG)c3.Y() + (ULONG)c4.Y()) * 0.25f;
			m.Z() = ((ULONG)c1.Z() + (ULONG)c2.Z() + (ULONG)c3.Z() + (ULONG)c4.Z()) * 0.25f;
			*/

#endif
		}

		return m;
	}
}




/////////////////////////////////////////////////////////////////////////////
// Raytracer core
//
UINT RaytraceFrame( LPVOID pParam )
{
	CRaytracerData &raytracer_data = *((CRaytracerData*)pParam);

	/////////////////////////////////////////////////////////////////////////////
	// loops controlers
	int				startscanline = raytracer_data.getStart();
	int				endscanline = raytracer_data.getEnd();
	bool			antialias = (raytracer_data.getCamera().variance > 0);
	int				aliaslevel = 2;
	float			x,y;

	const CPU_INFO& cpu = getCPUINFO();
	/////////////////////////////////////////////////////////////////////////////
	// Init datas
	for (unsigned int i=0;i<raytracer_data.getNbObjects();i++)
	{
		CGenericRenderObject *obj = raytracer_data.getObject(i);
		if (obj->InitPlugins(raytracer_data))
		{
			std::cout << "Object plugins ready!" << std::endl;
		}
		else
		{
			std::cout << "Some plugins are missing for objects!" << std::endl;
		}

		if (obj->GetType() == CRenderObject::MESH)
		{
			if (cpu.hasFeature(CPUINFO::SSE))
			{
				std::cout << "Building SSE octree..." << std::endl;
				((SSE_CMesh*)obj)->ReBuildOctree();
				std::cout << "Computing SSE smoothing ... " << std::endl;
				((SSE_CMesh*)obj)->ReBuildNormals((float)(cos(raytracer_data.getCamera().crease)));
			}
			else
			{
				std::cout << "Building octree..." << std::endl;
				((CMesh*)obj)->ReBuildOctree();
				std::cout << "Computing smoothing ... " << std::endl;
				((CMesh*)obj)->ReBuildNormals((float)(cos(raytracer_data.getCamera().crease)));
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	//	optical settings
	//	1/P + 1/I = 1/F
	//	P/I = S/s = grandissement
	float			p_over_i = raytracer_data.getCamera().object_plane / (raytracer_data.getCamera().focale*0.001f) - 1.0f;
	float			grand = p_over_i * DIM_FILM;
	//float ouvert = (data->camera.aperture < 128.0f) ? (data->camera.focale*0.001f) / data->camera.aperture : 0.0f ;
	float			scale = grand / raytracer_data.getCamera().height ;


	/////////////////////////////////////////////////////////////////////////////
	//	photons
	// generate the photon maps for each light
	for (unsigned int i=0;i<raytracer_data.getNbLights();i++)
	{
		raytracer_data.getLight(i)->BuildPhotonMap(raytracer_data,raytracer_data.getCamera().photon_map);
	}
	
	/////////////////////////////////////////////////////////////////////////////
	//	rays
	// --- transform ray eminating from viewpoint ---
	CGenericRay		root;
	CGenericRay		root_copy;
	root_copy.origin = raytracer_data.getCamera().origin;
	// --- camera displacement  ---
	root_copy.direction.Set(-root_copy.origin.X(),
							-root_copy.origin.Y(),
							-raytracer_data.getCamera().object_plane, //raytracer_data.camera.object_plane-root_copy.origin.Z(),
							1.0);
	root_copy.n = 1.0 ;
	root_copy.level = 0 ;
	root_copy.fact  = 1.0 ;
	
	unsigned int k = 0;
	y = (startscanline - (raytracer_data.getCamera().height >> 1))*scale;

	// --- loop for each scan line ---
	for (int j=raytracer_data.getEnd()-1;j>=startscanline;j--)
	{
		x = -(int)(raytracer_data.getCamera().width >> 1)*scale;

	    // --- loop for each pixel ---
		for (unsigned int i = 0; i<raytracer_data.getCamera().width; i++)
		{
			root = root_copy;
			// --- projete le point d'interet ds le plan focal ---
			root.direction.X() += x ;
			root.direction.Y() += y ;

			// --- init. values ---
			root.id    = ++Ray_ID ;

			// --- compute hit color recursively ---
			if (!antialias)
			{
				root.direction.Normalize() ;
				root.Hit(raytracer_data,colorImage[k]) ;
			}
			else
			{
				colorImage[k]=variance(x,y,0.25f*scale,aliaslevel,root,raytracer_data);
			}

			if ((root.hit.Z() > 0.01f) || (root.hit.Z() < -0.01f))
				raytracer_data.getZBuffer()[k] = raytracer_data.getCamera().origin.Z() - root.hit.Z();
			else
				raytracer_data.getZBuffer()[k] = 0.0f;

			k++;
			x+=scale;
		}	// width loop 

		std::cout << "Row " << j << "done." << std::endl;
		y+=scale;
		raytracer_data.setPercent((endscanline-j)*10000/(endscanline-startscanline));
	}	// height loop

	raytracer_data.setPercent(10000 - 1);

	//PackImage(k,raytracer_data);

	raytracer_data.setPercent(10000);

	return 0;
}


DWORD __stdcall Raytrace(LPVOID pParam)
{
	CRaytracerData &raytracer_data = *((CRaytracerData*)pParam);

	/////////////////////////////////////////////////////////////////////////////
	// the computed data
	const CCamera& camera = raytracer_data.getCamera();
	colorImage = new CColor::RGBA[(raytracer_data.getEnd() - raytracer_data.getStart())*camera.width];
	raytracer_data.allocateBuffer();

	/////////////////////////////////////////////////////////////////////////////
	// render each frame
	for (unsigned int j = 0; j<raytracer_data.getNbFrames(); j++)
	{
		//	
		//	Prepare frame data:
		//	- sub rays
		for (int i=0;i<RAYS_MAX_LEVEL;i++)
		{
			raytracer_data.getLightRay(i).level = i;
			raytracer_data.getReflectedRay(i).level = i;
			raytracer_data.getRefractedRay(i).level = i;
		}

		//
		//	- frame transforms
		//
		rays_frame_t *pFrame = raytracer_data.getFrame(j);

		for (unsigned int i=0;i<pFrame->nbTransforms;i++)
		{
			rays_transform_t t = pFrame->transforms[i];
			std::cout << "Searching transform for " << t.target << std::endl;

			std::string name = t.target;
			for (unsigned int k=0;k<raytracer_data.getNbObjects();k++)
			{
				CGenericRenderObject *obj = raytracer_data.getObject(k);
				if (obj->GetName() == name)
				{
					switch(t.transform)
					{
						case SCALE:
							obj->Scale(t.coeffs.x,t.coeffs.y,t.coeffs.z);
							std::cout << "Scaling object " << name;
							std::cout << " : " << t.coeffs.x << " " << t.coeffs.y << " " << t.coeffs.z << std::endl;
							break;
						case TRANSLATE:
							obj->Translate(t.coeffs.x,t.coeffs.y,t.coeffs.z);
							std::cout << "Translating object " << name;
							std::cout << " : " << t.coeffs.x << " " << t.coeffs.y << " " << t.coeffs.z << std::endl;
							break;
						case ROTATE:
							obj->Rotate(t.coeffs.x,t.coeffs.y,t.coeffs.z,t.coeffs.h);
							std::cout << "Rotating object " << name;
							std::cout << " : " << t.coeffs.x << " " << t.coeffs.y << " " << t.coeffs.z << " " << t.coeffs.h << std::endl;
							break;
						default:
							break;
					}
					
					break;
				}
			}
		}

		Ray_ID = 0;
		//CBoundingBox::nbIntersect = 0;

		//
		//	Render a frame
		//
		RaytraceFrame(pParam);

		std::cout << "Nb rays: " << Ray_ID << std::endl;
		std::cout << "Nb triangle intersect: " << CTriangle::nbIntersect << std::endl;
		//std::cout << "Nb bbox intersect: " << CBoundingBox::nbIntersect << std::endl;

		std::cout << "Frame " << j << " done." << std::endl;
	}

	delete[] colorImage;
	return 0;
}