/***************************************************************************/
/*                                                                         */
/*  SSEFVector.h                                                           */
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



#if !defined(AFX_SSEFLOATVECTOR_H__9AAB93A4_DB7C_4486_802A_1B4CE4E6CEC4__INCLUDED_)
#define AFX_SSEFLOATVECTOR_H__9AAB93A4_DB7C_4486_802A_1B4CE4E6CEC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __REGISTERS_H__
	#include "Registers.h"
#endif

#ifndef __GENERIC_VECTOR_H__
	#include "GenericVector.h"
#endif

#ifndef __GENERIC_ALIGNED_VECTOR_H__
	#include "GenericAlignedVector.h"
#endif

class CSSEFVector : public CGenericAlignedVector<float>
{
public:
	CSSEFVector() {};
	virtual ~CSSEFVector() {};

#if (!defined(SIMD_NO_ASSEMBLY) && !defined(_WIN64))
	CSSEFVector& operator= ( const CSSEFVector& v ) NOEXCEPT
	{
		__asm
		{
			mov ebx,v
			mov edx,this
			mov esi,[ebx+4]
			mov edi,[edx+4]
			sse_loadaps(XMM0_ESI)
			sse_storeaps(XMM0_EDI)
		}
		return *this;
	};
	CSSEFVector& operator= ( const CGenericAlignedVector<float>& v ) NOEXCEPT
	{
		__asm
		{
			mov ebx,v
			mov edx,this
			mov esi,[ebx+4]
			mov edi,[edx+4]
			sse_loadaps(XMM0_ESI)
			sse_storeaps(XMM0_EDI)
		}
		return *this;
	};
	CSSEFVector& operator= ( const CGenericVector<float>& v ) NOEXCEPT
	{
		__asm
		{
			mov esi,v
			mov ebx,this
			sse_loadupsofs(XMM0_ESI,4)
			mov edi,[ebx+4]
			sse_storeaps(XMM0_EDI)
		}
		return *this;
	};
	CSSEFVector& operator= ( const float v[4] ) NOEXCEPT
	{
		__asm
		{
			mov esi,v
			mov ebx,this
			sse_loadups(XMM0_ESI)
			mov edi,[ebx+4]
			sse_storeaps(XMM0_EDI)
		}
		return *this;
	};		

	bool operator== ( const CGenericAlignedVector<float>& v ) const NOEXCEPT
	{
		__asm
		{
			mov ebx,v
			mov edx,this
			mov esi,[ebx+4]
			mov edi,[edx+4]
		
			xor eax,eax
			mov ecx,[edi]
			cmp ecx,[esi]
			jne noteq
			mov ecx,[edi+4]
			cmp ecx,[esi+4]
			jne noteq
			mov ecx,[edi+8]
			cmp ecx,[esi+8]
			jne noteq
			mov ecx,[edi+12]
			cmp ecx,[esi+12]
			jne noteq
			mov eax,1
			noteq:
		}
// no return value
#pragma warning(disable:4035)
	};
#pragma warning(default:4035)

	bool operator== ( const CSSEFVector& v ) const NOEXCEPT
	{
		__asm
		{
			mov ebx,v
			mov edx,this
			mov esi,[ebx+4]
			mov edi,[edx+4]
		
			xor eax,eax
			mov ecx,[edi]
			cmp ecx,[esi]
			jne noteq
			mov ecx,[edi+4]
			cmp ecx,[esi+4]
			jne noteq
			mov ecx,[edi+8]
			cmp ecx,[esi+8]
			jne noteq
			mov ecx,[edi+12]
			cmp ecx,[esi+12]
			jne noteq
			mov eax,1
			noteq:
		}
// no return value
#pragma warning(disable:4035)
	};
#pragma warning(default:4035)


	virtual double SIMD_CALL Norm() const NOEXCEPT;
	virtual double SIMD_CALL Normalize() NOEXCEPT;


	CSSEFVector& SIMD_CALL operator-(void) NOEXCEPT;
	CSSEFVector& SIMD_CALL operator!(void) NOEXCEPT;
	CSSEFVector& SIMD_CALL operator-=(const CGenericAlignedVector<float>& v) NOEXCEPT;
	CSSEFVector& SIMD_CALL operator+=(const CGenericAlignedVector<float>& v) NOEXCEPT;
	CSSEFVector& SIMD_CALL operator*=(const CGenericAlignedVector<float>& v) NOEXCEPT;
	CSSEFVector& SIMD_CALL operator*= (const float& t) NOEXCEPT;
	
	//	this = m*this
	CSSEFVector& SIMD_CALL operator*= (const CGenericAlignedMatrix<float>& m) NOEXCEPT;

	CSSEFVector& SIMD_CALL operator+  (const CSSEFVector&) const NOEXCEPT;
  CSSEFVector& SIMD_CALL operator-  (const CSSEFVector&) const NOEXCEPT;
#endif
};

#if (!defined(SIMD_NO_ASSEMBLY) && !defined(_WIN64))
// cross product
CSSEFVector& SIMD_CALL operator^  (const CSSEFVector&, const CSSEFVector&) NOEXCEPT;
// scalar operations
CSSEFVector& SIMD_CALL operator*  (const CSSEFVector&, const CSSEFVector&) NOEXCEPT;
CSSEFVector& SIMD_CALL operator*  (const float&, const CSSEFVector&) NOEXCEPT;
CSSEFVector& SIMD_CALL operator*  (const CSSEFVector&, const float&) NOEXCEPT;
CSSEFVector& SIMD_CALL operator*  (const CSSEFMatrix&, const CSSEFVector&) NOEXCEPT;
CSSEFVector& SIMD_CALL operator*  (const CSSEFVector&, const CSSEFMatrix&) NOEXCEPT;


/////////////////////////////////////////////////////////////////////////////////////////////
// implementation
/////////////////////////////////////////////////////////////////////////////////////////////
extern CSSEFVector _alignedSSEFloatVector;

__inline CSSEFVector& SIMD_CALL CSSEFVector::operator-(void) NOEXCEPT
{
	__asm
	{
		mov ebx,this
		sse_xorps(XMM0_XMM0)
		mov esi,[ebx+4]
		lea edx,_alignedSSEFloatVector
		sse_subps(XMM0_ESI)
		mov edi,[edx+4]
		sse_storeaps(XMM0_EDI)
	}
	return _alignedSSEFloatVector;
}

__inline CSSEFVector& SIMD_CALL CSSEFVector::operator!(void) NOEXCEPT
{
		__asm
	{
		mov ebx,this
		sse_xorps(XMM0_XMM0)
		mov esi,[ebx+4]
		sse_subps(XMM0_ESI)
		sse_storeaps(XMM0_ESI)
	}
	return *this;
}


__inline CSSEFVector& SIMD_CALL CSSEFVector::operator-=(const CGenericAlignedVector<float>& v) NOEXCEPT
{
	__asm
	{
		mov ebx,this
		mov edx,v
		mov edi,[ebx+4]
		mov esi,[edx+4]
		sse_loadaps(XMM0_EDI)
		sse_subps(XMM0_ESI)
		sse_storeaps(XMM0_EDI)
	}
	return *this;
}

__inline CSSEFVector& SIMD_CALL CSSEFVector::operator+=(const CGenericAlignedVector<float>& v) NOEXCEPT
{
	__asm
	{
		mov ebx,this
		mov edx,v
		mov edi,[ebx+4]
		mov esi,[edx+4]
		sse_loadaps(XMM0_EDI)
		sse_addps(XMM0_ESI)
		sse_storeaps(XMM0_EDI)
	}
	return *this;
}

__inline CSSEFVector& SIMD_CALL CSSEFVector::operator*=(const CGenericAlignedVector<float>& v) NOEXCEPT
{
	__asm
	{
		mov ebx,this
		mov edx,v
		mov edi,[ebx+4]
		mov esi,[edx+4]
		sse_loadaps(XMM0_EDI)
		sse_mulps(XMM0_ESI)
		sse_storeaps(XMM0_EDI)
	}
	return *this;
}

__inline CSSEFVector& SIMD_CALL CSSEFVector::operator*=(const float& t) NOEXCEPT
{
	__asm
	{
		mov ebx,this
		mov edi,[ebx+4]
		mov esi,t
		sse_loadss(XMM0_ESI)
		sse_shufps(XMM0_XMM0,0)	
		sse_mulps(XMM0_EDI)
		sse_storeaps(XMM0_EDI)
	}
	return *this;
}


__inline double SIMD_CALL CSSEFVector::Norm() const NOEXCEPT
{
	float res = 0.0f;
	__asm
	{
		mov ebx,this
		mov esi,[edx+4]
		sse_loadaps(XMM0_ESI)
		sse_mulps(XMM0_XMM0)
		sse_loadaps(XMM1_XMM0)
		sse_loadaps(XMM2_XMM0)
		sse_shufps(XMM1_XMM0,SSE_R4_R1_R3_R2)
		sse_shufps(XMM2_XMM0,SSE_R4_R2_R1_R3)
		sse_addps(XMM1_XMM0)
		sse_addps(XMM1_XMM2)
		lea edi,res
		sse_sqrtss(XMM2_XMM1)
		sse_storess(XMM2_EDI)
	}
	return res;
}

__inline double SIMD_CALL CSSEFVector::Normalize() NOEXCEPT
{
	float res = 0.0f;
	__asm
	{
		mov ebx,this
		mov esi,[edx+4]
		sse_loadaps(XMM0_ESI)
		sse_loadaps(XMM3_ESI)
		sse_mulps(XMM0_XMM0)
		sse_loadaps(XMM1_XMM0)
		sse_loadaps(XMM2_XMM0)
		sse_shufps(XMM1_XMM0,SSE_R4_R1_R3_R2)
		sse_shufps(XMM2_XMM0,SSE_R4_R2_R1_R3)
		sse_addps(XMM1_XMM0)
		sse_addps(XMM1_XMM2)
		lea edi,res
		sse_sqrtps(XMM2_XMM1)
		sse_divps(XMM3_XMM2)
		sse_storeaps(XMM3_ESI)
		sse_storess(XMM2_EDI)
	}
	return res;
}
#endif

#endif // !defined(AFX_SSEFLOATVECTOR_H__9AAB93A4_DB7C_4486_802A_1B4CE4E6CEC4__INCLUDED_)


