/***************************************************************************/
/*                                                                         */
/*  SIMDRegister.cxx                                                       */
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


#ifndef __SIMD_REGISTER_IMPL__
#define __SIMD_REGISTER_IMPL__

#include "SIMDRegister.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4035) // no return value
#pragma warning(disable:4514) // unreferenced inline function has been removed

#ifndef SIMD_NO_ASSEMBLY

#define SSE_REGISTER_UNARY_OPERATOR_IMPL(thisClass,operandClass,operatorClass)\
__inline SIMD_ENTRY CSSERegister##thisClass & SIMD_CALL CSSERegister##thisClass::operator##operatorClass(const CSSERegister##operandClass &)\
{\
	sse_loadaps(XMM##thisClass##_XMM##operandClass)\
	__asm ret\
}

#define SSE_REGISTER_SET_OPERATOR_IMPL(thisClass)\
__inline SIMD_ENTRY CSSERegister##thisClass & SIMD_CALL CSSERegister##thisClass::operator=(const float *)\
{\
	sse_loadups(XMM##thisClass##_EDX)\
	__asm ret\
}

#endif	// SIMD_NO_ASSEMBLY

#endif	// __SIMD_REGISTER_IMPL__

