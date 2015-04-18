// SSE_3DEngine.cpp: implementation of the CSSE_3DEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_SSE_3DENGINE_H__247539DD_1572_4C73_86BB_CD4C1DD82614__INCLUDED_)
	#include "SSE_3DEngine.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
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
	GLfloat		winz;

	float xmin,xmax,ymin,ymax,zmin,zmax;
	bbox->get(xmin,ymin,zmin,xmax,ymax,zmax);

    CGenericMatrix<float>   SSE_clipMatrix;
	//	Check if the object is in front of the viewer
    if (scanModelview)
		glGetTransposeFloatv(GL_MODELVIEW_MATRIX,SSE_clipMatrix);
    else
        SSE_clipMatrix = getAttributes()->modelview;

    if (transform != NULL)
        SSE_clipMatrix *= *transform;
	
    winz =  MIN(xmin * SSE_clipMatrix[8],xmax * SSE_clipMatrix[8]) + 
                MIN(ymin * SSE_clipMatrix[9],ymax * SSE_clipMatrix[9]) +
                MIN(zmin * SSE_clipMatrix[10],zmax * SSE_clipMatrix[10]) + 
                SSE_clipMatrix[11];
    if (winz > 0)
		return CLIP_FULL;
    winz = MAX(xmin * SSE_clipMatrix[8],xmax * SSE_clipMatrix[8]) + 
               MAX(ymin * SSE_clipMatrix[9],ymax * SSE_clipMatrix[9]) +
               MAX(zmin * SSE_clipMatrix[10],zmax * SSE_clipMatrix[10]) + 
               SSE_clipMatrix[11] ;
    if (winz < -getAttributes()->farPlane)
        return CLIP_FULL;
    
	//
	// project BBox and check if it intersects Viewport
	//
	CGenericMatrix<float> SSE_projection  = getAttributes()->projection;
    SSE_projection *= SSE_clipMatrix;
	CGenericVector<float> vect;

    CGenericVector<float> XMin(xmin*SSE_projection[0],xmin*SSE_projection[4],0,xmin*SSE_projection[12]);
    CGenericVector<float> XMax(xmax*SSE_projection[0],xmax*SSE_projection[4],0,xmax*SSE_projection[12]);

    CGenericVector<float> YMin(ymin*SSE_projection[1],ymin*SSE_projection[5],0,ymin*SSE_projection[13]);
    CGenericVector<float> YMax(ymax*SSE_projection[1],ymax*SSE_projection[5],0,ymax*SSE_projection[13]);

    CGenericVector<float> ZMin(zmin*SSE_projection[2],zmin*SSE_projection[6],0,zmin*SSE_projection[14]);
    CGenericVector<float> ZMax(zmax*SSE_projection[2],zmax*SSE_projection[6],0,zmax*SSE_projection[14]);

    CGenericVector<float> H(SSE_projection[3],SSE_projection[7],SSE_projection[11],SSE_projection[15]);

    float   winxyzh[8*4];
#define winx(i) winxyzh[4*i]
#define winy(i) winxyzh[1+4*i]

    __asm
    {
        lea esi , H
        sse_loadupsofs(XMM7_ESI,4)      //  xmm7 = H
        lea edi , YMin
        sse_loadupsofs(XMM6_EDI,4)      //  xmm6 = YMin
        lea esi , XMin
        sse_loadupsofs(XMM0_ESI,4)      //  xmm0 = XMin
        lea edi , ZMin
        sse_loadupsofs(XMM1_EDI,4)      //  xmm1 = ZMin
        lea esi , YMax
        sse_loadupsofs(XMM5_ESI,4)      //  xmm5 = YMax
        lea edi , ZMax
        sse_loadupsofs(XMM4_EDI,4)      //  xmm4 = ZMax

        sse_addps(XMM6_XMM7)
        sse_addps(XMM5_XMM7)

        sse_addps(XMM0_XMM1)
        sse_addps(XMM0_XMM6)
        sse_loadaps(XMM2_XMM0)
        sse_shufps(XMM2_XMM2,0xff)
        sse_rcpps(XMM2_XMM2)
        sse_mulps(XMM0_XMM2)
        lea edx,winxyzh
        sse_storeups(XMM0_EDX)          // store winx(0) & winy(0)
        
        lea esi , XMin
        sse_loadupsofs(XMM0_ESI,4)      //  xmm0 = XMin
        sse_addps(XMM0_XMM4)            // xmm4 = ZMax
        sse_addps(XMM0_XMM6)
        sse_loadaps(XMM2_XMM0)
        sse_shufps(XMM2_XMM2,0xff)
        sse_rcpps(XMM2_XMM2)
        sse_mulps(XMM0_XMM2)
        sse_storeupsofs(XMM0_EDX,4*4)          // store winx(1) & winy(1)

        lea esi , XMax
        sse_loadupsofs(XMM0_ESI,4)      //  xmm0 = XMax
        sse_addps(XMM0_XMM4)            // xmm4 = ZMax
        sse_addps(XMM0_XMM6)
        sse_loadaps(XMM2_XMM0)
        sse_shufps(XMM2_XMM2,0xff)
        sse_rcpps(XMM2_XMM2)
        sse_mulps(XMM0_XMM2)
        sse_storeupsofs(XMM0_EDX,4*8)          // store winx(2) & winy(2)

        lea esi , XMax
        sse_loadupsofs(XMM0_ESI,4)      //  xmm0 = XMax
        sse_addps(XMM0_XMM1)            //  xmm1 = ZMin
        sse_addps(XMM0_XMM6)
        sse_loadaps(XMM2_XMM0)
        sse_shufps(XMM2_XMM2,0xff)
        sse_rcpps(XMM2_XMM2)
        sse_mulps(XMM0_XMM2)
        sse_storeupsofs(XMM0_EDX,4*12)          // store winx(3) & winy(3)

        ///

        lea esi , XMin
        sse_loadupsofs(XMM0_ESI,4)      //  xmm0 = XMin
        sse_addps(XMM0_XMM1)            //  xmm1 = ZMin
        sse_addps(XMM0_XMM5)
        sse_loadaps(XMM2_XMM0)
        sse_shufps(XMM2_XMM2,0xff)
        sse_rcpps(XMM2_XMM2)
        sse_mulps(XMM0_XMM2)
        sse_storeupsofs(XMM0_EDX,4*16)          // store winx(4) & winy(4)

        lea esi , XMin
        sse_loadupsofs(XMM0_ESI,4)      //  xmm0 = XMin
        sse_addps(XMM0_XMM4)            //  xmm4 = ZMax
        sse_addps(XMM0_XMM5)
        sse_loadaps(XMM2_XMM0)
        sse_shufps(XMM2_XMM2,0xff)
        sse_rcpps(XMM2_XMM2)
        sse_mulps(XMM0_XMM2)
        sse_storeupsofs(XMM0_EDX,4*20)          // store winx(5) & winy(5)

        lea esi , XMax
        sse_loadupsofs(XMM0_ESI,4)      //  xmm0 = XMax
        sse_addps(XMM0_XMM4)            //  xmm4 = ZMax
        sse_addps(XMM0_XMM5)
        sse_loadaps(XMM2_XMM0)
        sse_shufps(XMM2_XMM2,0xff)
        sse_rcpps(XMM2_XMM2)
        sse_mulps(XMM0_XMM2)
        sse_storeupsofs(XMM0_EDX,4*24)          // store winx(6) & winy(6)

        lea esi , XMax
        sse_loadupsofs(XMM0_ESI,4)      //  xmm0 = XMax
        sse_addps(XMM0_XMM1)           //  xmm1 = ZMin
        sse_addps(XMM0_XMM5)
        sse_loadaps(XMM2_XMM0)
        sse_shufps(XMM2_XMM2,0xff)
        sse_rcpps(XMM2_XMM2)
        sse_mulps(XMM0_XMM2)
        sse_storeupsofs(XMM0_EDX,4*28)          // store winx(7) & winy(7)
    }

/*   
    vect.H(XMin.H()+YMin.H()+ZMin.H()+H.H());
    vect.X(XMin.X()+YMin.X()+ZMin.X()+H.X());
    vect.Y(XMin.Y()+YMin.Y()+ZMin.Y()+H.Y());
    winx(0) = vect.X() / vect.H();
	winy(0) = vect.Y() / vect.H();
    
    vect.H(XMin.H()+YMin.H()+ZMax.H()+H.H());
    vect.X(XMin.X()+YMin.X()+ZMax.X()+H.X());
    vect.Y(XMin.Y()+YMin.Y()+ZMax.Y()+H.Y());
    winx(1) = vect.X() / vect.H();
	winy(1) = vect.Y() / vect.H();

    vect.H(XMax.H()+YMin.H()+ZMax.H()+H.H());
    vect.X(XMax.X()+YMin.X()+ZMax.X()+H.X());
    vect.Y(XMax.Y()+YMin.Y()+ZMax.Y()+H.Y());
    winx(2) = vect.X() / vect.H();
	winy(2) = vect.Y() / vect.H();

    vect.H(XMax.H()+YMin.H()+ZMin.H()+H.H());
    vect.X(XMax.X()+YMin.X()+ZMin.X()+H.X());
    vect.Y(XMax.Y()+YMin.Y()+ZMin.Y()+H.Y());
    winx(3) = vect.X() / vect.H();
	winy(3) = vect.Y() / vect.H();

    vect.H(XMin.H()+YMax.H()+ZMin.H()+H.H());
    vect.X(XMin.X()+YMax.X()+ZMin.X()+H.X());
    vect.Y(XMin.Y()+YMax.Y()+ZMin.Y()+H.Y());
    winx(4) = vect.X() / vect.H();
	winy(4) = vect.Y() / vect.H();

    vect.H(XMin.H()+YMax.H()+ZMax.H()+H.H());
    vect.X(XMin.X()+YMax.X()+ZMax.X()+H.X());
    vect.Y(XMin.Y()+YMax.Y()+ZMax.Y()+H.Y());
    winx(5) = vect.X() / vect.H();
	winy(5) = vect.Y() / vect.H();

    vect.H(XMax.H()+YMax.H()+ZMax.H()+H.H());
    vect.X(XMax.X()+YMax.X()+ZMax.X()+H.X());
    vect.Y(XMax.Y()+YMax.Y()+ZMax.Y()+H.Y());
    winx(6) = vect.X() / vect.H();
	winy(6) = vect.Y() / vect.H();

    vect.H(XMax.H()+YMax.H()+ZMin.H()+H.H());
    vect.X(XMax.X()+YMax.X()+ZMin.X()+H.X());
    vect.Y(XMax.Y()+YMax.Y()+ZMin.Y()+H.Y());
    winx(7) = vect.X() / vect.H();
	winy(7) = vect.Y() / vect.H();
*/
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


