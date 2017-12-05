// TestEngine.cpp: implementation of the CTestEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestEngine::CTestEngine()
{

}

CTestEngine::~CTestEngine()
{

}


void CTestEngine::onCameraCollision(CPhysics *obj) const
{
	C3DEngine::onCameraCollision(obj);
}


void CTestEngine::onObjectCollision(CPhysics *obj1,CPhysics *obj2) const
{
	C3DEngine::onObjectCollision(obj1,obj2);
}