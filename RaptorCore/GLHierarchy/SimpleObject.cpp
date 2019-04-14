/***************************************************************************/
/*                                                                         */
/*  SimpleObject.cpp                                                       */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


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

CSimpleObject::CSimpleObject(const std::string& name)
	: CObject3D(CObject3D::CObject3DClassID::GetClassId(), name)
{

}

CSimpleObject::~CSimpleObject()
{

}

void CSimpleObject::glRender(void)
{

}

