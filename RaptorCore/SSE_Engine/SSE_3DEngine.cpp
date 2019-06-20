// SSE_3DEngine.cpp: implementation of the CSSE_3DEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_SSE_3DENGINE_H__247539DD_1572_4C73_86BB_CD4C1DD82614__INCLUDED_)
	#include "SSE_3DEngine.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif

#if !defined(AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_)
	#include "SSE_Engine/SSE_BoundingBox.h"
#endif

#include "Subsys/3DEngineAttributes.h"

RAPTOR_NAMESPACE_BEGIN

//	bbox clipping indexes
static int indexes[24] = 
{
	0 , 1,
	1 , 2,
	2 , 3,
	3 , 0,
	4 , 5,
	5 , 6,
	6 , 7,
	7 , 4,
	0 , 4,
	1 , 5,
	2 , 6,
	3 , 7
};

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

#ifdef RAPTOR_SSE_CODE_GENERATION

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSSE_3DEngine::CSSE_3DEngine(const std::string& name)
	:C3DEngine(name)
{
    const CPU_INFO &info = getCPUINFO();

	if (info.SSE)
    {
        delete m_camera;
		m_camera = new CSSE_BoundingBox();
    }
}

CSSE_3DEngine::~CSSE_3DEngine()
{

}

C3DEngine::CLIP_RESULT CSSE_3DEngine::glClip(const CBoundingBox *const bbox,bool scanModelview,const CGenericMatrix<float> *transform) const
{
    CGenericMatrix<float>   M;
	//	Check if the object is in front of the viewer
    if (scanModelview)
		glGetTransposeFloatv(GL_MODELVIEW_MATRIX,M);
    else
        M = getAttributes()->modelview;

    if (transform != NULL)
        M *= *transform;

	float xmin, xmax, ymin, ymax, zmin, zmax;
	bbox->get(xmin, ymin, zmin, xmax, ymax, zmax);
    GLfloat winz =  MIN(xmin * M[8], xmax * M[8]) + 
            MIN(ymin * M[9], ymax * M[9]) +
            MIN(zmin * M[10],zmax * M[10]) + 
                M[11];
    if (winz > 0)
		return CLIP_FULL;
    winz = MAX(xmin * M[8], xmax * M[8]) + 
           MAX(ymin * M[9], ymax * M[9]) +
           MAX(zmin * M[10],zmax * M[10]) + 
			   M[11] ;
    if (winz < -getAttributes()->farPlane)
        return CLIP_FULL;
    
	//
	// project BBox and check if it intersects Viewport
	//
	CGenericMatrix<float> P  = getAttributes()->projection;
    P *= M;
	float   winxyzh[8 * 4];
#define winx(i) winxyzh[4*i]
#define winy(i) winxyzh[1+4*i]
	
	__m128 Hh = _mm_set_ps(P[15], P[11], P[7], P[3]);
	__m128 Xm = _mm_set_ps(xmin*P[12], 0, xmin*P[4], xmin*P[0]);
	__m128 Ym = _mm_set_ps(ymin*P[13], 0, ymin*P[5], ymin*P[1]);
	__m128 Zm = _mm_set_ps(zmin*P[14], 0, zmin*P[6], zmin*P[2]);

	__m128 tmp = _mm_add_ps(Hh, Ym);
	__m128 tmp2 = _mm_add_ps(tmp, Xm);
	__m128 V = _mm_add_ps(tmp2, Zm);
	_mm_storeu_ps(&winx(0), _mm_mul_ps(V, _mm_rcp_ps(_mm_shuffle_ps(V, V, 0xff))));

	__m128 ZM = _mm_set_ps(zmax*P[14], 0, zmax*P[6], zmax*P[2]);
	V = _mm_add_ps(tmp2, ZM);
	_mm_storeu_ps(&winx(1), _mm_mul_ps(V, _mm_rcp_ps(_mm_shuffle_ps(V, V, 0xff))));

	__m128 XM = _mm_set_ps(xmax*P[12], 0, xmax*P[4], xmax*P[0]);
	tmp2 = _mm_add_ps(tmp, XM);
	V = _mm_add_ps(tmp2, ZM);
	_mm_storeu_ps(&winx(2), _mm_mul_ps(V, _mm_rcp_ps(_mm_shuffle_ps(V, V, 0xff))));
	
	V = _mm_add_ps(tmp2, Zm);
	_mm_storeu_ps(&winx(3), _mm_mul_ps(V, _mm_rcp_ps(_mm_shuffle_ps(V, V, 0xff))));


	__m128 YM = _mm_set_ps(ymax*P[13], 0, ymax*P[5], ymax*P[1]);
	tmp = _mm_add_ps(Hh, YM);
	tmp2 = _mm_add_ps(tmp, Xm);
	V = _mm_add_ps(tmp2, Zm);
	_mm_storeu_ps(&winx(4), _mm_mul_ps(V, _mm_rcp_ps(_mm_shuffle_ps(V, V, 0xff))));

	V = _mm_add_ps(tmp2, ZM);
	_mm_storeu_ps(&winx(5), _mm_mul_ps(V, _mm_rcp_ps(_mm_shuffle_ps(V, V, 0xff))));

	tmp2 = _mm_add_ps(tmp, XM);
	V = _mm_add_ps(tmp2, ZM);
	_mm_storeu_ps(&winx(6), _mm_mul_ps(V, _mm_rcp_ps(_mm_shuffle_ps(V, V, 0xff))));

	V = _mm_add_ps(tmp2, Zm);
	_mm_storeu_ps(&winx(7), _mm_mul_ps(V, _mm_rcp_ps(_mm_shuffle_ps(V, V, 0xff))));


	//	clip the 12 segments of the BBox
    C3DEngine::CLIP_RESULT intersect = CLIP_FULL;
	int nbIntersect = 0;
	int pos = 0;

	while ((pos < 24) && 
              ((pos == 2*nbIntersect) || (nbIntersect == 0)))
	{
		int idx0 = indexes[pos];
		int idx1 = indexes[pos+1];

		if (winx(idx1) < winx(idx0))
		{
			xmin = winx(idx1);
			xmax = winx(idx0);
		}
		else
		{
			xmin = winx(idx0);
			xmax = winx(idx1);
		}

		if (winy(idx1) < winy(idx0))
		{
			ymin = winy(idx1);
			ymax = winy(idx0);
		}
		else
		{
			ymin = winy(idx0);
			ymax = winy(idx1);
		}

		if ((-1.0f < xmax) && (1.0f > xmin))
		{
			if ((-1.0f < ymax) && (1.0f > ymin))
				nbIntersect++;
		}
		pos+=2;
	}

	if (nbIntersect == 12)
		intersect = C3DEngine::CLIP_NONE;
	else if (nbIntersect > 0)
		intersect = C3DEngine::CLIP_PARTS;
	
	CATCH_GL_ERROR

	return intersect;
}

#endif


