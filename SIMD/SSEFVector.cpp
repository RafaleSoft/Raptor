/***************************************************************************/
/*                                                                         */
/*  SSEFVector.cpp                                                         */
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


#ifndef __REGISTERS_H__
	#include "registers.h"
#endif

#include "SSEFVector.h"

//	unreferenced parameter
#pragma warning(disable : 4100)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#if (!defined(SIMD_NO_ASSEMBLY) && !defined(_WIN64))

SIMD_ENTRY CSSEFVector& SIMD_CALL operator^  (const CSSEFVector& v1, const CSSEFVector& v2) NOEXCEPT
{
	__asm
	{
		mov ecx,[ecx+4]
		mov edx,[edx+4]
		sse_loadaps(XMM0_ECX)
		sse_loadaps(XMM1_EDX)
		sse_loadaps(XMM2_XMM0)
		sse_loadaps(XMM3_XMM1)
		
		sse_shufps(XMM0_XMM0,SSE_R4_R1_R3_R2) //11 00 10 01 => y z x h
		sse_shufps(XMM1_XMM1,SSE_R4_R2_R1_R3) //11 01 00 10 => z' x' y' h'
		sse_shufps(XMM2_XMM2,SSE_R4_R2_R1_R3) //11 01 00 10 => z x y h
		sse_shufps(XMM3_XMM3,SSE_R4_R1_R3_R2) //11 00 10 01 => y' z' x' h'
		
		sse_mulps(XMM0_XMM1)
		sse_mulps(XMM2_XMM3)
		
		lea eax,_alignedSSEFloatVector;
		sse_subps(XMM0_XMM2)
		mov ecx,[eax+4]
		sse_storeaps(XMM0_ECX)
		ret
	}
}

SIMD_ENTRY CSSEFVector& SIMD_CALL operator*  (const CSSEFVector& v1, const CSSEFVector& v2) NOEXCEPT
{
	__asm
	{
		mov ecx,[ecx+4]
		mov edx,[edx+4]
		sse_loadaps(XMM0_ECX)
		sse_mulps(XMM0_EDX)
		lea eax,_alignedSSEFloatVector;
		mov ecx,[eax+4]
		sse_storeaps(XMM0_ECX)
		ret
	}
}

SIMD_ENTRY CSSEFVector& SIMD_CALL CSSEFVector::operator+  (const CSSEFVector& v2) const NOEXCEPT
{
	__asm
	{
		mov ecx,[ecx+4]
		mov edx,[edx+4]
		sse_loadaps(XMM0_ECX)
		sse_addps(XMM0_EDX)
		lea eax,_alignedSSEFloatVector;
		mov ecx,[eax+4]
		sse_storeaps(XMM0_ECX)
		ret
	}
}

SIMD_ENTRY CSSEFVector& SIMD_CALL CSSEFVector::operator-  (const CSSEFVector& v2) const NOEXCEPT
{
	__asm
	{
		mov ecx,[ecx+4]
		mov edx,[edx+4]
		sse_loadaps(XMM0_ECX)
		sse_subps(XMM0_EDX)
		lea eax,_alignedSSEFloatVector;
		mov ecx,[eax+4]
		sse_storeaps(XMM0_ECX)
		ret
	}
}

SIMD_ENTRY CSSEFVector& SIMD_CALL operator*  (const float& v, const CSSEFVector& v2) NOEXCEPT
{
	__asm
	{
		push edi
		push esi
		mov edi,[edx+4]
		//movss xmm0,[ecx]
			_emit 0xf3
			_emit 0x0f
			_emit 0x10
			_emit 0x01
		//shufps xmm0,xmm0,0
			_emit 0x0f
			_emit 0xc6
			_emit 0xc0
			_emit 0x00
		// mulps xmm0,[edi]
			_emit 0x0f
			_emit 0x59
			_emit 0x07
		lea eax,_alignedSSEFloatVector;
		mov esi,[eax+4]
		//movntps [esi],xmm0
			_emit 0x0f
			_emit 0x2b
			_emit 0x06

		pop esi
		pop edi
		ret
	}
}

SIMD_ENTRY CSSEFVector& SIMD_CALL operator*  (const CSSEFVector& v2, const float& v) NOEXCEPT
{
	__asm
	{
		push edi
		push esi
		mov edi,[ecx+4]
		//movss xmm0,[edx]
			_emit 0xf3
			_emit 0x0f
			_emit 0x10
			_emit 0x02
		//shufps xmm0,xmm0,0
			_emit 0x0f
			_emit 0xc6
			_emit 0xc0
			_emit 0x00
		// mulps xmm0,[edi]
			_emit 0x0f
			_emit 0x59
			_emit 0x07
		lea eax,_alignedSSEFloatVector;
		mov esi,[eax+4]
		//movntps [esi],xmm0
			_emit 0x0f
			_emit 0x2b
			_emit 0x06

		pop esi
		pop edi
		ret
	}
}

SIMD_ENTRY CSSEFVector& SIMD_CALL CSSEFVector::operator*= (const CGenericAlignedMatrix<float>& m) NOEXCEPT
{

	__asm
	{
		push esi
		push edi
		mov esi,[edx+4]
		mov edi,[ecx+4]

		// chargement du vecteur, de la matrice et multiplication
		//movaps xmm4,[edi]
			_emit 0x0f
			_emit 0x28
			_emit 0x27
		//movaps xmm0,[esi]
			_emit 0x0f
			_emit 0x28
			_emit 0x06
		//movaps xmm1,[esi+16]
			_emit 0x0f
			_emit 0x28
			_emit 0x4e
			_emit 0x10
		//movaps xmm2,[esi+32]
			_emit 0x0f
			_emit 0x28
			_emit 0x56
			_emit 0x20
		//movaps xmm3,[esi+48]
			_emit 0x0f
			_emit 0x28
			_emit 0x5e
			_emit 0x30
	}
	sse_mulps(XMM0_XMM4)
	sse_mulps(XMM1_XMM4)
	sse_mulps(XMM2_XMM4)
	sse_mulps(XMM3_XMM4)

	//	calcul du vecteur resultat :
	//	xmm0 = x1,x2,x3,x4
	//	xmm1 = y1,y2,y3,y4
	//	xmm3 = z1,z2,z3,z4
	//	xmm3 = h1,h2,h3,h4
		
	sse_loadaps(XMM4_XMM0)
	sse_loadaps(XMM6_XMM2)

	sse_unpckhps(XMM0_XMM1)
	sse_unpckhps(XMM2_XMM3)
	sse_unpcklps(XMM4_XMM1)
	sse_unpcklps(XMM6_XMM3)

	sse_addps(XMM0_XMM4)
	sse_addps(XMM2_XMM6)

	sse_loadaps(XMM1_XMM0)
	sse_unpckhps(XMM0_XMM2)
	sse_unpcklps(XMM1_XMM2)

	sse_addps(XMM0_XMM1)
	sse_shufps(XMM0_XMM0,SSE_R4_R2_R3_R1)
	__asm
	{
		mov eax,ecx
	//movntps [edi],xmm0
			_emit 0x0f
			_emit 0x2b
			_emit 0x07

		pop edi
		pop esi
		ret
	}
}


SIMD_ENTRY CSSEFVector& SIMD_CALL operator* (const CSSEFMatrix& m, const CSSEFVector& v) NOEXCEPT
{

	__asm
	{
		push esi
		push edi
		mov esi,[ecx+4]
		mov edi,[edx+4]

		// chargement du vecteur, de la matrice et multiplication
		//movaps xmm4,[edi]
			_emit 0x0f
			_emit 0x28
			_emit 0x27
		//movaps xmm0,[esi]
			_emit 0x0f
			_emit 0x28
			_emit 0x06
		//movaps xmm1,[esi+16]
			_emit 0x0f
			_emit 0x28
			_emit 0x4e
			_emit 0x10
		//movaps xmm2,[esi+32]
			_emit 0x0f
			_emit 0x28
			_emit 0x56
			_emit 0x20
		//movaps xmm3,[esi+48]
			_emit 0x0f
			_emit 0x28
			_emit 0x5e
			_emit 0x30
	}
	sse_mulps(XMM0_XMM4)
	sse_mulps(XMM1_XMM4)
	sse_mulps(XMM2_XMM4)
	sse_mulps(XMM3_XMM4)

	//	calcul du vecteur resultat :
	//	xmm0 = x1,x2,x3,x4
	//	xmm1 = y1,y2,y3,y4
	//	xmm3 = z1,z2,z3,z4
	//	xmm3 = h1,h2,h3,h4
		
	sse_loadaps(XMM4_XMM0)
	sse_loadaps(XMM6_XMM2)

	sse_unpckhps(XMM0_XMM1)
	sse_unpckhps(XMM2_XMM3)
	sse_unpcklps(XMM4_XMM1)
	sse_unpcklps(XMM6_XMM3)

	sse_addps(XMM0_XMM4)
	sse_addps(XMM2_XMM6)

	sse_loadaps(XMM1_XMM0)
	sse_unpckhps(XMM0_XMM2)
	sse_unpcklps(XMM1_XMM2)

	sse_addps(XMM0_XMM1)
	sse_shufps(XMM0_XMM0,SSE_R4_R2_R3_R1)
	__asm
	{
		lea eax,_alignedSSEFloatVector;
		mov edi,[eax+4]
		//movntps [edi],xmm0
			_emit 0x0f
			_emit 0x2b
			_emit 0x07

		pop edi
		pop esi
		ret
	}
}

SIMD_ENTRY CSSEFVector& SIMD_CALL operator* (const CSSEFVector& m, const CSSEFMatrix& v) NOEXCEPT
{

	__asm
	{
		push esi
		push edi
		mov esi,[edx+4]
		mov edi,[ecx+4]

		// chargement du vecteur, de la matrice et multiplication
		//movaps xmm4,[edi]
			_emit 0x0f
			_emit 0x28
			_emit 0x27
		//movaps xmm0,[esi]
			_emit 0x0f
			_emit 0x28
			_emit 0x06
		//movaps xmm1,[esi+16]
			_emit 0x0f
			_emit 0x28
			_emit 0x4e
			_emit 0x10
		//movaps xmm2,[esi+32]
			_emit 0x0f
			_emit 0x28
			_emit 0x56
			_emit 0x20
		//movaps xmm3,[esi+48]
			_emit 0x0f
			_emit 0x28
			_emit 0x5e
			_emit 0x30
	}

	sse_loadaps(XMM5_XMM4)
	sse_loadaps(XMM6_XMM4)
	sse_loadaps(XMM7_XMM4)
	sse_shufps(XMM4_XMM4,0)
	sse_shufps(XMM5_XMM5,0x55)
	sse_shufps(XMM6_XMM6,0xaa)
	sse_shufps(XMM7_XMM7,0xff)

	sse_mulps(XMM0_XMM4)
	sse_mulps(XMM1_XMM5)
	sse_mulps(XMM2_XMM6)
	sse_mulps(XMM3_XMM7)

	sse_addps(XMM0_XMM1)
	sse_addps(XMM2_XMM3)
	sse_addps(XMM0_XMM2)

	__asm
	{
		lea eax,_alignedSSEFloatVector;
		mov edi,[eax+4]
		//movntps [edi],xmm0
			_emit 0x0f
			_emit 0x2b
			_emit 0x07

		pop edi
		pop esi
		ret
	}
}

#endif	// SIMD_NO_ASSEMBLY

