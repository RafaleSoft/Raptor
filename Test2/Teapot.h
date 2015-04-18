// Teapot.h: interface for the CTeapot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEAPOT_H__22A96FA5_9DEB_11D3_985D_3E6FE7000000__INCLUDED_)
#define AFX_TEAPOT_H__22A96FA5_9DEB_11D3_985D_3E6FE7000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Raptordll.h"
#include "Test2.h"

#include "ShadowDisplay.h"
#include "SplineDisplay.h"
#include "SkinningDisplay.h"
#include "BumpDisplay.h"
#include "CollisionDisplay.h"
#include "LodDisplay.h"
#include "ParticleDisplay.h"
#include "WarpingDisplay.h"
#include "ProjectionDisplay.h"
#include "VertexShadersDisplay.h"
#include "ShadowMapDisplay.h"
#include "EMBMDisplay.h"
#include "AmbientOcclusionDisplay.h"

#include "MFCExtension/CWnd/GLDocument.h"


class CTeapot : public CGLDocument  
{
protected:
	CTeapot();
	DECLARE_DYNCREATE(CTeapot)

public:
	int					numdemo;
	int					stopdemo;

	int					ry;
	int					rx;
	int					rz;
	CTextureSet			*t;
	
	//	rotating engine
	C3DSet				*set;
	CObject3DInstance	*instance;

	//	objects
	CBumppedGeometry	*teapot;
	CGeometry			*object;

	CGeometry			*smallTeapot;

    //
    CGenericDisplay     *currentDisplay;

	//	demo displays
	CSkinningDisplay		skinningDisplay;
	CSplineDisplay			splineDisplay;
	CShadowDisplay			shadowDisplay;
	CBumpDisplay			bumpDisplay;
	CCollisionDisplay		collisionDisplay;
	CLodDisplay				lodDisplay;
	CParticleDisplay		particleDisplay;
	CWarpingDisplay			warpingDisplay;
	CProjectionDisplay		projectionDisplay;
	CVertexShadersDisplay	vertexShadersDisplay;
	CShadowMapDisplay		shadowMapDisplay;
	CEMBMDisplay			embmDisplay;
	CAmbientOcclusionDisplay ambientOcclusionDisplay;

public:
	virtual ~CTeapot();

	virtual	void GLInitContext(void);
	virtual void GLDisplayFunc(int viewID);

	void SwitchDemo(unsigned int numDemo);

private:
	CRaptorDisplay	*m_globalDisplay;

	CGenericDisplay*	displays[CTest2App::MAX_DEMO];
};

#endif // !defined(AFX_TEAPOT_H__22A96FA5_9DEB_11D3_985D_3E6FE7000000__INCLUDED_)
