/***************************************************************************/
/*                                                                         */
/*  SIMDRegister.h                                                         */
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



#if !defined(AFX_SIMDREGISTER_H__7A170774_BE41_42B3_8BA3_415C50B6FD6D__INCLUDED_)
#define AFX_SIMDREGISTER_H__7A170774_BE41_42B3_8BA3_415C50B6FD6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __REGISTERS_H__
	#include "Registers.h"
#endif

#ifndef __SIMDMACROS_H__
    #include "simdMacros.h"
#endif


class CSSERegister0;
class CSSERegister1;
class CSSERegister2;
class CSSERegister3;
class CSSERegister4;
class CSSERegister5;
class CSSERegister6;
class CSSERegister7;

#if (!defined(SIMD_NO_ASSEMBLY) && !defined(_WIN64))

#pragma warning(disable:4035) // no return value
#define SSE_REGISTER_OPERATOR_EQ(thisClass,operandClass)\
	CSSERegister##thisClass & SIMD_CALL operator##=(const CSSERegister##operandClass &)\
	{\
		sse_loadaps(XMM##thisClass##_XMM##operandClass)\
	}

#define SSE_REGISTER_OPERATOR_ADD(thisClass,operandClass)\
	CSSERegister##thisClass & SIMD_CALL operator##+=(const CSSERegister##operandClass &)\
	{\
		sse_addps(XMM##thisClass##_XMM##operandClass)\
	}

#define SSE_REGISTER_OPERATOR_MUL(thisClass,operandClass)\
	CSSERegister##thisClass & SIMD_CALL operator##*=(const CSSERegister##operandClass &)\
	{\
		sse_mulps(XMM##thisClass##_XMM##operandClass)\
	}

#define SSE_REGISTER_OPERATOR_SUB(thisClass,operandClass)\
	CSSERegister##thisClass & SIMD_CALL operator##-=(const CSSERegister##operandClass &)\
	{\
		sse_subps(XMM##thisClass##_XMM##operandClass)\
	}

#define SSE_REGISTER_OPERATOR_DIV(thisClass,operandClass)\
	CSSERegister##thisClass & SIMD_CALL operator##/=(const CSSERegister##operandClass &)\
	{\
		sse_divps(XMM##thisClass##_XMM##operandClass)\
	}

#define SSE_REGISTER_SET_OPERATOR(thisClass)\
	CSSERegister##thisClass & SIMD_CALL operator=(const float *val)\
	{\
		sse_loadxmm##thisClass##(val)\
	}

#define SSE_REGISTER_SAVE(thisClass)\
	CSSERegister##thisClass & SIMD_CALL Save(const float *val)\
	{\
		sse_storexmm##thisClass##(val)\
	}

#define SSE_REGISTER_OPERATOR_AND(thisClass,operandClass)\
	CSSERegister##thisClass & SIMD_CALL operator&=(const CSSERegister##operandClass &)\
	{\
		sse_andps(XMM##thisClass##_XMM##operandClass)\
	}

#define SSE_REGISTER_OPERATOR_OR(thisClass,operandClass)\
	CSSERegister##thisClass & SIMD_CALL operator|=(const CSSERegister##operandClass &)\
	{\
		sse_orps(XMM##thisClass##_XMM##operandClass)\
	}

#define SSE_REGISTER_OPERATOR_XOR(thisClass,operandClass)\
	CSSERegister##thisClass & SIMD_CALL operator^=(const CSSERegister##operandClass &)\
	{\
		sse_xorps(XMM##thisClass##_XMM##operandClass)\
	}


#define SSE_REGISTER_OPERATORS_EQ(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_EQ(thisClass,a)\
	SSE_REGISTER_OPERATOR_EQ(thisClass,b)\
	SSE_REGISTER_OPERATOR_EQ(thisClass,c)\
	SSE_REGISTER_OPERATOR_EQ(thisClass,d)\
	SSE_REGISTER_OPERATOR_EQ(thisClass,e)\
	SSE_REGISTER_OPERATOR_EQ(thisClass,f)\
	SSE_REGISTER_OPERATOR_EQ(thisClass,g)\

#define SSE_REGISTER_OPERATORS_ADD(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_ADD(thisClass,a)\
	SSE_REGISTER_OPERATOR_ADD(thisClass,b)\
	SSE_REGISTER_OPERATOR_ADD(thisClass,c)\
	SSE_REGISTER_OPERATOR_ADD(thisClass,d)\
	SSE_REGISTER_OPERATOR_ADD(thisClass,e)\
	SSE_REGISTER_OPERATOR_ADD(thisClass,f)\
	SSE_REGISTER_OPERATOR_ADD(thisClass,g)\

#define SSE_REGISTER_OPERATORS_MUL(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_MUL(thisClass,a)\
	SSE_REGISTER_OPERATOR_MUL(thisClass,b)\
	SSE_REGISTER_OPERATOR_MUL(thisClass,c)\
	SSE_REGISTER_OPERATOR_MUL(thisClass,d)\
	SSE_REGISTER_OPERATOR_MUL(thisClass,e)\
	SSE_REGISTER_OPERATOR_MUL(thisClass,f)\
	SSE_REGISTER_OPERATOR_MUL(thisClass,g)\

#define SSE_REGISTER_OPERATORS_SUB(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_SUB(thisClass,a)\
	SSE_REGISTER_OPERATOR_SUB(thisClass,b)\
	SSE_REGISTER_OPERATOR_SUB(thisClass,c)\
	SSE_REGISTER_OPERATOR_SUB(thisClass,d)\
	SSE_REGISTER_OPERATOR_SUB(thisClass,e)\
	SSE_REGISTER_OPERATOR_SUB(thisClass,f)\
	SSE_REGISTER_OPERATOR_SUB(thisClass,g)\

#define SSE_REGISTER_OPERATORS_DIV(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_DIV(thisClass,a)\
	SSE_REGISTER_OPERATOR_DIV(thisClass,b)\
	SSE_REGISTER_OPERATOR_DIV(thisClass,c)\
	SSE_REGISTER_OPERATOR_DIV(thisClass,d)\
	SSE_REGISTER_OPERATOR_DIV(thisClass,e)\
	SSE_REGISTER_OPERATOR_DIV(thisClass,f)\
	SSE_REGISTER_OPERATOR_DIV(thisClass,g)\

#define SSE_REGISTER_OPERATORS_AND(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_AND(thisClass,a)\
	SSE_REGISTER_OPERATOR_AND(thisClass,b)\
	SSE_REGISTER_OPERATOR_AND(thisClass,c)\
	SSE_REGISTER_OPERATOR_AND(thisClass,d)\
	SSE_REGISTER_OPERATOR_AND(thisClass,e)\
	SSE_REGISTER_OPERATOR_AND(thisClass,f)\
	SSE_REGISTER_OPERATOR_AND(thisClass,g)\

#define SSE_REGISTER_OPERATORS_OR(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_OR(thisClass,a)\
	SSE_REGISTER_OPERATOR_OR(thisClass,b)\
	SSE_REGISTER_OPERATOR_OR(thisClass,c)\
	SSE_REGISTER_OPERATOR_OR(thisClass,d)\
	SSE_REGISTER_OPERATOR_OR(thisClass,e)\
	SSE_REGISTER_OPERATOR_OR(thisClass,f)\
	SSE_REGISTER_OPERATOR_OR(thisClass,g)\

#define SSE_REGISTER_OPERATORS_XOR(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_XOR(thisClass,a)\
	SSE_REGISTER_OPERATOR_XOR(thisClass,b)\
	SSE_REGISTER_OPERATOR_XOR(thisClass,c)\
	SSE_REGISTER_OPERATOR_XOR(thisClass,d)\
	SSE_REGISTER_OPERATOR_XOR(thisClass,e)\
	SSE_REGISTER_OPERATOR_XOR(thisClass,f)\
	SSE_REGISTER_OPERATOR_XOR(thisClass,g)

#define SSE_REGISTER_CLASS(thisClass,a,b,c,d,e,f,g)\
class CSSERegister##thisClass \
{\
public:\
	CSSERegister##thisClass () NOEXCEPT {};\
	virtual ~CSSERegister##thisClass () {};\
\
	SSE_REGISTER_SET_OPERATOR(thisClass)\
	SSE_REGISTER_SAVE(thisClass)\
\
	SSE_REGISTER_OPERATORS_EQ(thisClass,a,b,c,d,e,f,g)\
\
	SSE_REGISTER_OPERATORS_ADD(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_ADD(thisClass,thisClass)\
\
	SSE_REGISTER_OPERATORS_MUL(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_MUL(thisClass,thisClass)\
\
	SSE_REGISTER_OPERATORS_SUB(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_SUB(thisClass,thisClass)\
\
	SSE_REGISTER_OPERATORS_DIV(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_DIV(thisClass,thisClass)\
\
	SSE_REGISTER_OPERATORS_AND(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_AND(thisClass,thisClass)\
\
	SSE_REGISTER_OPERATORS_OR(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_OR(thisClass,thisClass)\
\
	SSE_REGISTER_OPERATORS_XOR(thisClass,a,b,c,d,e,f,g)\
	SSE_REGISTER_OPERATOR_XOR(thisClass,thisClass)\
};

SSE_REGISTER_CLASS(0,1,2,3,4,5,6,7)
SSE_REGISTER_CLASS(1,0,2,3,4,5,6,7)
SSE_REGISTER_CLASS(2,0,1,3,4,5,6,7)
SSE_REGISTER_CLASS(3,0,1,2,4,5,6,7)
SSE_REGISTER_CLASS(4,0,1,2,3,5,6,7)
SSE_REGISTER_CLASS(5,0,1,2,3,4,6,7)
SSE_REGISTER_CLASS(6,0,1,2,3,4,5,7)
SSE_REGISTER_CLASS(7,0,1,2,3,4,5,6)

#endif	//(!defined(SIMD_NO_ASSEMBLY) || defined(_WIN64))


#endif // !defined(AFX_SIMDREGISTER_H__7A170774_BE41_42B3_8BA3_415C50B6FD6D__INCLUDED_)

