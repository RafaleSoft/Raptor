#include "stdafx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/BoundingBox.h"
#endif


//#include "GenericRay.h"
#include "GenericLight.h"
#include "GenericRenderObject.h"
#include "Texture.h"
//#include "Triangle.h"
//#include "Mesh.h"
//#include "SSE_CMesh.h"
//#include "SSE_BoundingBox.h"
#include "Environment.h"
//#include "PhotonMap.h"
//#include "..\Plugin.h"

#include "RaysWorkUnit.h"
#include "Raytracer.h"

/////////////////////////////////////////////////////////////////////////////
// Raytracer data
//
unsigned int	Ray_ID = 0;
CWVector		*colorImage;

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
	
	currentFrame = 0;

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

	delete [] image;
	delete [] ZBuffer;
}

//
//	Pack Image from 64bits colors to 32bits colors
//
/////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
// Variance : recursive addaptative subsampling for antialiasing
//		and texture bilinear filtering
//
CWVector variance(float centerx,float centery,float scale,int depth,
				  CGenericRay &r,const CRaytracerData& raytracer_data)
{
	if (depth  == 0 )
	{
		CGenericRay vray;
		CWVector c;

		// preparation du rayon
		vray.origin = r.origin;
		vray.direction = -vray.origin;
		vray.direction.X() += centerx;	
		vray.direction.Y() += centery;
		vray.direction.Z() = -raytracer_data.camera.object_plane ;
		vray.direction.Normalize();
		vray.n = r.n;
		vray.level = r.level;
		vray.fact = r.fact;
		vray.id    = ++Ray_ID ;
		vray.Hit(&raytracer_data,c);

		return c;
	}
	else
	{
		int variancelimit = raytracer_data.camera.variance;
		CWVector m,v;
		CWVector c1,c2,c3,c4;
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
			/*
			m.X() = ((ULONG)c1.X() + (ULONG)c2.X() + (ULONG)c3.X() + (ULONG)c4.X()) * 0.25f;
			m.Y() = ((ULONG)c1.Y() + (ULONG)c2.Y() + (ULONG)c3.Y() + (ULONG)c4.Y()) * 0.25f;
			m.Z() = ((ULONG)c1.Z() + (ULONG)c2.Z() + (ULONG)c3.Z() + (ULONG)c4.Z()) * 0.25f;
			*/
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

			// calcul des variances des couleurs
			/*
			v = (c1-m)*(c1-m) + (c2-m)*(c2-m) + (c3-m)*(c3-m) + (c4-m)*(c4-m);
			*/
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
		}

		//	Return if hit background
		if (r.hit.Z() < 0.01f)
			return m;

		// test seuil
		if (((v.X()>=variancelimit)||(v.Y()>=variancelimit)||(v.Z()>=variancelimit))&&(depth>1))
		{
			c1=variance(centerx-scale,centery-scale,0.5f*scale,depth-1,r,raytracer_data);
			c2=variance(centerx+scale,centery-scale,0.5f*scale,depth-1,r,raytracer_data);
			c3=variance(centerx-scale,centery+scale,0.5f*scale,depth-1,r,raytracer_data);
			c4=variance(centerx+scale,centery+scale,0.5f*scale,depth-1,r,raytracer_data);

/*
			m.X() = ((ULONG)c1.X() + (ULONG)c2.X() + (ULONG)c3.X() + (ULONG)c4.X()) * 0.25f;
			m.Y() = ((ULONG)c1.Y() + (ULONG)c2.Y() + (ULONG)c3.Y() + (ULONG)c4.Y()) * 0.25f;
			m.Z() = ((ULONG)c1.Z() + (ULONG)c2.Z() + (ULONG)c3.Z() + (ULONG)c4.Z()) * 0.25f;
*/
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
		}

		return m;
	}
}




/////////////////////////////////////////////////////////////////////////////
// Raytracer core
//
UINT RaytraceFrame( LPVOID pParam )
{
	UNREFERENCED_PARAMETER(pParam);

	/////////////////////////////////////////////////////////////////////////////
	// loops controlers
	int				startscanline = raytracer_data.start;
	int				endscanline = raytracer_data.end;
	int				i,j,k = 0;
	bool			antialias = (raytracer_data.camera.variance > 0);
	int				aliaslevel = 2;
	float			x,y;

	/////////////////////////////////////////////////////////////////////////////
	// Init datas
	for (i=0;i<raytracer_data.objects.size();i++)
	{
		CGenericRenderObject *obj = raytracer_data.objects[i];
		if (obj->InitPlugins())
		{
			COUT << "Object plugins ready!" << endl;
		}
		else
		{
			COUT << "Some plugins are missing for objects!" << endl;
		}

		if (obj->GetType() == CRenderObject::MESH)
		{
			if (raytracer_data.use_sse)
			{
				COUT << "Building SSE octree..." << endl;
				((SSE_CMesh*)obj)->ReBuildOctree();
				COUT << "Computing SSE smoothing ... " << endl;
				((SSE_CMesh*)obj)->ReBuildNormals((float)(cos(raytracer_data.camera.crease)));
			}
			else
			{
				COUT << "Building octree..." << endl;
				((CMesh*)obj)->ReBuildOctree();
				COUT << "Computing smoothing ... " << endl;
				((CMesh*)obj)->ReBuildNormals((float)(cos(raytracer_data.camera.crease)));
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	//	optical settings
	//	1/P + 1/I = 1/F
	//	P/I = S/s = grandissement
	float			p_over_i = raytracer_data.camera.object_plane / (raytracer_data.camera.focale*0.001f) - 1.0f;
	float			grand = p_over_i * DIM_FILM;
	//float ouvert = (data->camera.aperture < 128.0f) ? (data->camera.focale*0.001f) / data->camera.aperture : 0.0f ;
	float			scale = grand / raytracer_data.camera.height ;


	/////////////////////////////////////////////////////////////////////////////
	//	photons
	// generate the photon maps for each light
	for (i=0;i<raytracer_data.lights.size();i++)
	{
		raytracer_data.lights[i]->BuildPhotonMap(&raytracer_data,raytracer_data.camera.photon_map);
	}
	
	/////////////////////////////////////////////////////////////////////////////
	//	rays
	// --- transform ray eminating from viewpoint ---
	CGenericRay		root;
	CGenericRay		root_copy;
	root_copy.origin = raytracer_data.camera.origin;
	// --- camera displacement  ---
	root_copy.direction.Set(-root_copy.origin.X(),
							-root_copy.origin.Y(),
							-raytracer_data.camera.object_plane, //raytracer_data.camera.object_plane-root_copy.origin.Z(),
							1.0);
	root_copy.n = 1.0 ;
	root_copy.level = 0 ;
	root_copy.fact  = 1.0 ;
	

	y = (startscanline-(raytracer_data.camera.height>>1))*scale;

	// --- loop for each scan line ---
	for (j=raytracer_data.end-1;j>=startscanline;j--)
	{
		x = -(raytracer_data.camera.width>>1)*scale;

	    // --- loop for each pixel ---
		for (i=0;i<raytracer_data.camera.width;i++)
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
				root.Hit(&raytracer_data,colorImage[k]) ;
			}
			else
			{
				colorImage[k]=variance(x,y,0.25f*scale,aliaslevel,root);
			}

			if ((root.hit.Z() > 0.01f) || (root.hit.Z() < -0.01f))
				raytracer_data.ZBuffer[k] = raytracer_data.camera.origin.Z() - root.hit.Z();
			else
				raytracer_data.ZBuffer[k] = 0.0f;

			k++;
			x+=scale;
		}	// width loop 

		COUT << "Row " << j << "done." << endl;
		y+=scale;
		raytracer_data.percent = (endscanline-j)*10000/(endscanline-startscanline);
	}	// height loop

	raytracer_data.percent = 10000 - 1;

	PackImage(k);
	delete [] colorImage;

	raytracer_data.percent = 10000;

	return 0;
}


UINT Raytrace( LPVOID pParam )
{
	UNREFERENCED_PARAMETER(pParam);

	CEnvironment	*pEnvironment = CEnvironment::GetInstance();

	/////////////////////////////////////////////////////////////////////////////
	// the computed data
	raytracer_data.ZBuffer = new float[(raytracer_data.end-raytracer_data.start)*raytracer_data.camera.width];
	colorImage = new CWVector[(raytracer_data.end-raytracer_data.start)*raytracer_data.camera.width];
	raytracer_data.image = new unsigned char[(raytracer_data.end-raytracer_data.start)*raytracer_data.camera.width*4];

	/////////////////////////////////////////////////////////////////////////////
	// render each frame
	for (int j=0;j<raytracer_data.frames.GetCount();j++)
	{
		//	
		//	Prepare frame data:
		//	- sub rays
		for (int i=0;i<RAYS_MAX_LEVEL;i++)
		{
			raytracer_data.light_ray_levels[i].level = i;
			raytracer_data.reflected_ray_levels[i].level = i;
			raytracer_data.refracted_ray_levels[i].level = i;
		}

		//
		//	- frame transforms
		//
		rays_frame_t *pFrame = NULL;
		raytracer_data.frames.Lookup(j,(void*&)pFrame);

		for (i=0;i<pFrame->nbTransforms;i++)
		{
			rays_transform_t t = pFrame->transforms[i];
			COUT << "Searching transform for " << t.target << endl;

			CString name = t.target;
			for (int k=0;k<raytracer_data.objects.GetSize();k++)
			{
				CGenericRenderObject *obj = raytracer_data.objects[k];

				if (obj->GetName() == name)
				{
					switch(t.transform)
					{
						case SCALE:
							obj->Scale(t.coeffs.x,t.coeffs.y,t.coeffs.z);
							COUT << "Scaling object " << LPCTSTR(name);
							COUT << " : " << t.coeffs.x << " " << t.coeffs.y << " " << t.coeffs.z << endl;
							break;
						case TRANSLATE:
							obj->Translate(t.coeffs.x,t.coeffs.y,t.coeffs.z);
							COUT << "Translating object " << LPCTSTR(name);
							COUT << " : " << t.coeffs.x << " " << t.coeffs.y << " " << t.coeffs.z << endl;
							break;
						case ROTATE:
							obj->Rotate(t.coeffs.x,t.coeffs.y,t.coeffs.z,t.coeffs.h);
							COUT << "Rotating object " << LPCTSTR(name);
							COUT << " : " << t.coeffs.x << " " << t.coeffs.y << " " << t.coeffs.z << " " << t.coeffs.h << endl;
							break;
						default:
							break;
					}
					
					break;
				}
			}
		}

		Ray_ID = 0;
		CBoundingBox::nbIntersect = 0;

		//
		//	Render a frame
		//
		RaytraceFrame(pParam);

		raytracer_data.lock->SetEvent();

		COUT << "Nb rays: " << Ray_ID << endl;
		COUT << "Nb triangle intersect: " << CTriangle::nbIntersect << endl;
		COUT << "Nb bbox intersect: " << CBoundingBox::nbIntersect << endl;

		COUT << "Frame " << raytracer_data.currentFrame << " done." << endl;
		raytracer_data.currentFrame++;
	}

	AfxEndThread(0);
	return 0;
}