// TestEngine.h: interface for the CTestEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTENGINE_H__91BDF941_D86E_4BA6_A135_99F9C2598896__INCLUDED_)
#define AFX_TESTENGINE_H__91BDF941_D86E_4BA6_A135_99F9C2598896__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Raptordll.h"
#include "Engine/3DEngine.h"
RAPTOR_NAMESPACE

class CTestEngine : public C3DEngine  
{
public:
	CTestEngine();
	virtual ~CTestEngine();

	virtual void onCameraCollision(CPhysics *obj) const;
	virtual void onObjectCollision(CPhysics *obj1,CPhysics *obj2) const;
};

#endif // !defined(AFX_TESTENGINE_H__91BDF941_D86E_4BA6_A135_99F9C2598896__INCLUDED_)
