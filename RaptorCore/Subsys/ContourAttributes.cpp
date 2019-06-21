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

void CContourAttributes::extrude(const GL_COORD_VERTEX &pos,float extrusion)
{
    if (contourVolumeSize == 0)
        return;

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
		}
	}
	else
	{
#if defined(RAPTOR_SSE_CODE_GENERATION) && defined(RAPTOR_SSE41_CODE_GENERATION) && !defined(SIMD_NO_ASSEMBLY)
		__m128 p = _mm_loadu_ps(pos.operator const float *());
		__m128 e = _mm_load1_ps(&extrusion);
		e.m128_f32[3] = 0.0f;
		for (unsigned int i = 0; i<contourVolumeSize; i++)
		{
			__m128 v = _mm_loadu_ps(pContourVolume[i].operator const float *());
			__m128 v_ext = _mm_loadu_ps(pContourVolume[i + contourVolumeSize].operator const float *());

			__m128 lightvect = _mm_sub_ps(v, p);
			// SSE4.1 dot product
			__m128 norm = _mm_dp_ps(lightvect, lightvect, 0xff);
			norm = _mm_mul_ps(e,_mm_rsqrt_ps(norm));

			_mm_storeu_ps(pContourVolume[i + contourVolumeSize].operator float *(),_mm_add_ps(v,_mm_mul_ps(lightvect,norm)));
		}
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
		}
#endif
	}
}

