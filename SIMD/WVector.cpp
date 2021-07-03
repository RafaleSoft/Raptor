/***************************************************************************/
/*                                                                         */
/*  WVector.cpp                                                            */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_WVector_H__7493DD94_5C3A_4025_9FA6_C8F0EC4BE053__INCLUDED_)
	#include "WVector.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWVector SIMD_CALL operator*(const CWVector& v1, const float& t)
{
	CWVector v;

	v[0] = (unsigned short)(v1[0] * t);
	v[1] = (unsigned short)(v1[1] * t);
	v[2] = (unsigned short)(v1[2] * t);
	v[3] = (unsigned short)(v1[3] * t);

	return v;
}

CWVector SIMD_CALL operator*(const float& t, const CWVector& v1)
{
	CWVector v;

	v[0] = (unsigned short)(v1[0] * t);
	v[1] = (unsigned short)(v1[1] * t);
	v[2] = (unsigned short)(v1[2] * t);
	v[3] = (unsigned short)(v1[3] * t);

	return v;
}

