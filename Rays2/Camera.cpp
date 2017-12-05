// Camera.cpp: implementation of the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Camera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCamera::CCamera()
{
//	origin.x=origin.y=0.0;
//	origin.z=300;
	focale=50.0f;
	aperture=1.4f;
	object_plane=10.0f;
	reflection_depth=5;
	refraction_depth=5;
	
	width=320;height=240;
	depth=0;variance=0;
}

CCamera::~CCamera()
{

}
