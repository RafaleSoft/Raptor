// ContourAttributes.cpp: implementation of the CContourAttributes class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"


#if !defined(AFX_CONTOURATTRIBUTES_H__6ADB3B0A_FE00_4385_B7EC_3A239298653D__INCLUDED_)
	#include "ContourAttributes.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContourAttributes::CContourAttributes()
{
	pContourVolume = NULL;
	contourVolumeSize = 0;
	
	contourSize = 0;
	pContourEdges = NULL;

	darkCapSize = 0;
	lightCapSize = 0;

	volume = NULL;
	darkcap = NULL;
	lightcap = NULL;

	backfaces = NULL;
}

CContourAttributes::~CContourAttributes()
{

}


void CContourAttributes::findBackFaces(const GL_COORD_VERTEX &pos, size_t nbFace)
{
	unsigned int index = 0;

	unsigned int csize = contourVolumeSize;

	lightCapSize = 0;
	darkCapSize = 0;

	unsigned int *pLightCap = &lightcap[0];
	unsigned int *pDarkCap = &darkcap[0];

	for (size_t i = 0; i < nbFace; i++)
	{
		unsigned int p1, p2, p3;

		p1 = pContourFaces[index];
		p2 = pContourFaces[index + 1];
		p3 = pContourFaces[index + 2];

		//	lightVect % nomal should be tested using the mean point 
		//	of the triangle ( p1 + p2 + p3 ) / 3 ...
		float angle =	(pContourVolume[p3].x - pos.x) * pContourNormals[i].x +
						(pContourVolume[p3].y - pos.y) * pContourNormals[i].y +
						(pContourVolume[p3].z - pos.z) * pContourNormals[i].z;

		if (angle < 0)
		{
			backfaces[i] = false;
			*pLightCap++ = p1;
			*pLightCap++ = p2;
			*pLightCap++ = p3;
		}
		else
		{
			backfaces[i] = true;
			*pDarkCap++ = p1 + csize;
			*pDarkCap++ = p2 + csize;
			*pDarkCap++ = p3 + csize;
		}

		index += 3;
	}

	lightCapSize = (pLightCap - lightcap);
	darkCapSize = (pDarkCap - darkcap);
}

void CContourAttributes::extrude(const GL_COORD_VERTEX &pos,float extrusion,
								 GL_COORD_VERTEX &min, GL_COORD_VERTEX &max)
{
    if (contourVolumeSize == 0)
        return;

	min.x = min.y = min.z = FLT_MAX;
	max.x = max.y = max.z = -FLT_MAX;

	if (extrusion < 0)
	{
		for (unsigned int i=0;i<contourVolumeSize;i++)
		{
			GL_COORD_VERTEX& v = pContourVolume[i];
			GL_COORD_VERTEX& v_ext = pContourVolume[i+contourVolumeSize];

			v_ext.x = v.x - pos.x;
			v_ext.y = v.y - pos.y;
			v_ext.z = v.z - pos.z;
			v_ext.h = 0.0f;

			min.x = min(min.x, v_ext.x);
			min.y = min(min.y, v_ext.y);
			min.z = min(min.z, v_ext.z);

			max.x = max(max.x, v_ext.x);
			max.y = max(max.y, v_ext.y);
			max.z = max(max.z, v_ext.z);
		}
	}
	else
	{
#if defined(RAPTOR_SSE_CODE_GENERATION) && defined(RAPTOR_SSE41_CODE_GENERATION) && !defined(SIMD_NO_ASSEMBLY)
		__m128 p = _mm_loadu_ps(pos.operator const float *());
		__m128 e = _mm_load1_ps(&extrusion);
		__m128 m = _mm_loadu_ps(min.operator const float *());
		__m128 M = _mm_loadu_ps(max.operator const float *());

		e.m128_f32[3] = 0.0f;
		for (unsigned int i = 0; i<contourVolumeSize; i++)
		{
			__m128 v = _mm_loadu_ps(pContourVolume[i].operator const float *());

			__m128 lightvect = _mm_sub_ps(v, p);
			// SSE4.1 dot product
			__m128 norm = _mm_dp_ps(lightvect, lightvect, 0xff);
			norm = _mm_mul_ps(e,_mm_rsqrt_ps(norm));
			__m128 ext = _mm_add_ps(v, _mm_mul_ps(lightvect, norm));

			m = _mm_min_ps(m, ext);
			M = _mm_max_ps(M, ext);

			_mm_storeu_ps(pContourVolume[i + contourVolumeSize].operator float *(), ext);
		}

		_mm_storeu_ps(min.operator float *(), m);
		_mm_storeu_ps(max.operator float *(), M);
#else
		CGenericVector<float> lightVect;
		for (unsigned int i=0;i<contourVolumeSize;i++)
		{
			GL_COORD_VERTEX& v = pContourVolume[i];
			GL_COORD_VERTEX& v_ext = pContourVolume[i+contourVolumeSize];

			lightVect.Set(	v.x - pos.x, v.y - pos.y, v.z - pos.z, 	1.0f);
			float norm = extrusion / sqrt(lightVect.X()*lightVect.X() + lightVect.Y()*lightVect.Y() + lightVect.Z()*lightVect.Z());
	
			v_ext.x = lightVect.X()*norm + v.x;
			v_ext.y = lightVect.Y()*norm + v.y;
			v_ext.z = lightVect.Z()*norm + v.z;
			v_ext.h = 1.0f;

			min.x = min(min.x, v_ext.x);
			min.y = min(min.y, v_ext.y);
			min.z = min(min.z, v_ext.z);

			max.x = max(max.x, v_ext.x);
			max.y = max(max.y, v_ext.y);
			max.z = max(max.z, v_ext.z);
		}
#endif
	}
}

