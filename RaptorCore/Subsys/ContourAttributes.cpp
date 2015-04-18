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
		CGenericVector<float> lightVect;
		unsigned int contour_size = contourVolumeSize;

#ifdef RAPTOR_SSE_CODE_GENERATION
		float H[4] = { 0, 0, 0, 1.0f };
		const GL_COORD_VERTEX* v = &pContourVolume[0];
		const GL_COORD_VERTEX* v_ext = &pContourVolume[contourVolumeSize];

		__asm
		{
			lea esi,H
			sse_loadups(XMM7_ESI)
			lea esi,contour_size
			mov ecx,[esi]
			lea edi,extrusion
			sse_loadss(XMM6_EDI)
			sse_shufps(XMM6_XMM6,0)
			mov esi,pos
			sse_loadups(XMM5_ESI)

			// load
			mov esi,v
			mov edi,v_ext

		loop_extrude:
			sse_loadups(XMM0_ESI)		// xmm0 = V
			sse_loadaps(XMM4_XMM0)		// xmm4 = v
			sse_subps(XMM0_XMM5)		// xmm0 = v - pos
			sse_loadaps(XMM3_XMM0)		// xmm3 = v - pos

			// norm
			sse_mulps(XMM0_XMM0)	
			sse_loadaps(XMM1_XMM0)
			sse_loadaps(XMM2_XMM0)
			sse_shufps(XMM1_XMM0,SSE_R4_R1_R3_R2)
			sse_shufps(XMM2_XMM0,SSE_R4_R2_R1_R3)
			sse_addps(XMM1_XMM0)
			sse_addps(XMM1_XMM2)
			sse_addps(XMM1_XMM7)
			sse_rsqrtps(XMM2_XMM1)		// xmm2 = 1 / sqrt((v - pos) * (v - pos))

			// extrude
			sse_mulps(XMM2_XMM6)		// xmm2 = extrusion / norm(v-pos)
			sse_mulps(XMM2_XMM3)		// xmm2 = (v - pos) * extrusion / norm(v-pos)
			sse_addps(XMM2_XMM4)		// xmm2 = xmm2 + v

			// store
			sse_storeups(XMM2_EDI)
			add esi,16
			add edi,16
			dec ecx
			jnz loop_extrude
		}
#else
		for (int i=0;i<contour_size;i++)
		{
			GL_COORD_VERTEX& v = pContourVolume[i];
			GL_COORD_VERTEX& v_ext = pContourVolume[i+contour_size];

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

