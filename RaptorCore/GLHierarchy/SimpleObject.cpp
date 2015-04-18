// SimpleObject.cpp: implementation of the CSimpleObject class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_SIMPLEOBJECT_H__D7942271_77C5_4514_A44F_67F653C82A16__INCLUDED_)
#include "SimpleObject.h"
#endif

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimpleObject::CSimpleObject()
	:CObject3D(CObject3D::CObject3DClassID::GetClassId(),"SIMPLE_OBJECT")
{

}

CSimpleObject::~CSimpleObject()
{

}

void CSimpleObject::glRender(void)
{

}

