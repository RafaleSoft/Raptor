// SIMDRegister.cpp: implementation of the CSIMDRegister class.
//
//////////////////////////////////////////////////////////////////////


#ifndef __SIMD_REGISTER_IMPL__
#define __SIMD_REGISTER_IMPL__

#include "SIMDRegister.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4035) // no return value
#pragma warning(disable:4514) // unreferenced inline function has been removed

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



#endif

