/***************************************************************************/
/*                                                                         */
/*  SimpleObject.h                                                         */
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


#if !defined(AFX_SIMPLEOBJECT_H__D7942271_77C5_4514_A44F_67F653C82A16__INCLUDED_)
#define AFX_SIMPLEOBJECT_H__D7942271_77C5_4514_A44F_67F653C82A16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
    #include "Object3D.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//!
//!	This class implements a simple object 
//!	when only a simple render is needed
//!	( simple glCalls when a Geometry is not needed )
class RAPTOR_API CSimpleObject : public CObject3D  
{
public:
	CSimpleObject();
	virtual ~CSimpleObject();

	//! Void manipulators as we do not need a Geometry,
	//!	We only add a an overridable glRender call
	//! @see CObject3D.
	DECLARE_OBJECT3D_NOMANIPULATORS


protected:
	//!	Dedicated constructor for subclasses.
	//! @see CObject3D.
	CSimpleObject(const std::string& name);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SIMPLEOBJECT_H__D7942271_77C5_4514_A44F_67F653C82A16__INCLUDED_)

