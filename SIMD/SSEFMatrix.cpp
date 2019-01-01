// SSEFMatrix.cpp: implementation of the CSSEFMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSEFMATRIX_H__7923E89B_7F26_4EE5_8038_15480F2B7985__INCLUDED_)
	#include "SSEFMatrix.h"
#endif

#include "SSEFVector.h"
extern CSSEFVector	_alignedSSEFloatVector;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4100)

CSSEFMatrix::CSSEFMatrix()
{
}

CSSEFMatrix::~CSSEFMatrix()
{
}

#ifndef SIMD_NO_ASSEMBLY

SIMD_ENTRY CSSEFMatrix& SIMD_CALL CSSEFMatrix::operator-(const CSSEFMatrix& m2) const
{
	__asm
	{
		push esi
		push edi
		lea eax,_alignedSSEFloatMatrix
		mov esi,ecx
		mov edi,edx
		mov esi,[esi+4]
		mov edi,[edi+4]
	}

	sse_loadaps(XMM0_ESI)
	__asm add esi,16
	sse_subps(XMM0_EDI)
	__asm add edi,16

	sse_loadaps(XMM1_ESI)
	__asm add esi,16
	sse_subps(XMM1_EDI)
	__asm add edi,16

	sse_loadaps(XMM2_ESI)
	__asm add esi,16
	sse_subps(XMM2_EDI)
	__asm add edi,16

	sse_loadaps(XMM3_ESI)
	sse_subps(XMM3_EDI)
	__asm mov edi,[eax+4]

	sse_storeaps(XMM0_EDI)
	__asm add edi,16
	sse_storeaps(XMM1_EDI)
	__asm add edi,16
	sse_storeaps(XMM2_EDI)
	__asm add edi,16
	sse_storeaps(XMM3_EDI)

	__asm
	{
		pop edi
		pop esi
		ret
	}
}

SIMD_ENTRY CSSEFMatrix& SIMD_CALL CSSEFMatrix::operator+(const CSSEFMatrix& m2) const
{
	__asm
	{
		push esi
		push edi
		lea eax,_alignedSSEFloatMatrix
		mov esi,ecx
		mov edi,edx
		mov esi,[esi+4]
		mov edi,[edi+4]
	}

	sse_loadaps(XMM0_ESI)
	__asm add esi,16
	sse_addps(XMM0_EDI)
	__asm add edi,16

	sse_loadaps(XMM1_ESI)
	__asm add esi,16
	sse_addps(XMM1_EDI)
	__asm add edi,16

	sse_loadaps(XMM2_ESI)
	__asm add esi,16
	sse_addps(XMM2_EDI)
	__asm add edi,16

	sse_loadaps(XMM3_ESI)
	sse_addps(XMM3_EDI)
	__asm mov edi,[eax+4]

	sse_storeaps(XMM0_EDI)
	__asm add edi,16
	sse_storeaps(XMM1_EDI)
	__asm add edi,16
	sse_storeaps(XMM2_EDI)
	__asm add edi,16
	sse_storeaps(XMM3_EDI)
	
	__asm
	{
		pop edi
		pop esi
		ret
	}
}


SIMD_ENTRY CSSEFMatrix& SIMD_CALL operator*(const CSSEFMatrix& m1,const CSSEFMatrix& m2)
{
	__asm
	{
		push esi
		push edi
		lea eax,_alignedSSEFloatMatrix
		mov esi,ecx
		mov edi,edx
		mov esi,[esi+4]
		mov edi,[edi+4]
	}

	sse_loadaps(0x07)
	__asm add edi,16
	sse_loadaps(0x0f)
	__asm add edi,16
	sse_loadaps(0x17)
	__asm add edi,16
	sse_loadaps(0x1f)
	__asm mov edi,[eax+4]

		//	row0
		sse_loadaps(0x26)
		sse_shufps(XMM4_XMM4,0x00)
		sse_loadaps(0x2e)
		sse_shufps(XMM5_XMM5,0x55)
		sse_loadaps(0x36)
		sse_shufps(XMM6_XMM6,0xaa)
		sse_loadaps(0x3e)
		sse_shufps(XMM7_XMM7,0xff)
		
		sse_mulps(XMM4_XMM0)
		sse_mulps(XMM5_XMM1)
		sse_mulps(XMM6_XMM2)
		sse_mulps(XMM7_XMM3)
		sse_addps(XMM4_XMM5)
		sse_addps(XMM6_XMM7)
		sse_addps(XMM4_XMM6)
		sse_storeaps(0x27)
		__asm add edi,16
		__asm add esi,16

		//	row1
		sse_loadaps(0x26)
		sse_shufps(XMM4_XMM4,0x00)
		sse_loadaps(0x2e)
		sse_shufps(XMM5_XMM5,0x55)
		sse_loadaps(0x36)
		sse_shufps(XMM6_XMM6,0xaa)
		sse_loadaps(0x3e)
		sse_shufps(XMM7_XMM7,0xff)
		
		sse_mulps(XMM4_XMM0)
		sse_mulps(XMM5_XMM1)
		sse_mulps(XMM6_XMM2)
		sse_mulps(XMM7_XMM3)
		sse_addps(XMM4_XMM5)
		sse_addps(XMM6_XMM7)
		sse_addps(XMM4_XMM6)
		sse_storeaps(0x27)
		__asm add edi,16
		__asm add esi,16

		//	row2
		sse_loadaps(0x26)
		sse_shufps(XMM4_XMM4,0x00)
		sse_loadaps(0x2e)
		sse_shufps(XMM5_XMM5,0x55)
		sse_loadaps(0x36)
		sse_shufps(XMM6_XMM6,0xaa)
		sse_loadaps(0x3e)
		sse_shufps(XMM7_XMM7,0xff)
		
		sse_mulps(XMM4_XMM0)
		sse_mulps(XMM5_XMM1)
		sse_mulps(XMM6_XMM2)
		sse_mulps(XMM7_XMM3)
		sse_addps(XMM4_XMM5)
		sse_addps(XMM6_XMM7)
		sse_addps(XMM4_XMM6)
		sse_storeaps(0x27)
		__asm add edi,16
		__asm add esi,16

		//	row3
		sse_loadaps(0x26)
		sse_shufps(XMM4_XMM4,0x00)
		sse_loadaps(0x2e)
		sse_shufps(XMM5_XMM5,0x55)
		sse_loadaps(0x36)
		sse_shufps(XMM6_XMM6,0xaa)
		sse_loadaps(0x3e)
		sse_shufps(XMM7_XMM7,0xff)
		
		sse_mulps(XMM4_XMM0)
		sse_mulps(XMM5_XMM1)
		sse_mulps(XMM6_XMM2)
		sse_mulps(XMM7_XMM3)
		sse_addps(XMM4_XMM5)
		sse_addps(XMM6_XMM7)
		sse_addps(XMM4_XMM6)
		sse_storeaps(0x27)
	
	__asm
	{
		pop edi
		pop esi
		ret
	}
}


SIMD_ENTRY CSSEFMatrix& SIMD_CALL CSSEFMatrix::operator*=(const CSSEFMatrix& m)
{
	__asm
	{
		push esi
		push edi
		mov eax,ecx
		mov esi,ecx
		mov edi,edx
		mov esi,[esi+4]
		mov edi,[edi+4]
	}

	sse_loadaps(0x07)
	__asm add edi,16
	sse_loadaps(0x0f)
	__asm add edi,16
	sse_loadaps(0x17)
	__asm add edi,16
	sse_loadaps(0x1f)
	__asm mov edi,[eax+4]

		//	row0
		sse_loadaps(0x26)
		sse_shufps(XMM4_XMM4,0x00)
		sse_loadaps(0x2e)
		sse_shufps(XMM5_XMM5,0x55)
		sse_loadaps(0x36)
		sse_shufps(XMM6_XMM6,0xaa)
		sse_loadaps(0x3e)
		sse_shufps(XMM7_XMM7,0xff)
		
		sse_mulps(XMM4_XMM0)
		sse_mulps(XMM5_XMM1)
		sse_mulps(XMM6_XMM2)
		sse_mulps(XMM7_XMM3)
		sse_addps(XMM4_XMM5)
		sse_addps(XMM6_XMM7)
		sse_addps(XMM4_XMM6)
		sse_storeaps(0x27)
		__asm add edi,16
		__asm add esi,16

		//	row1
		sse_loadaps(0x26)
		sse_shufps(XMM4_XMM4,0x00)
		sse_loadaps(0x2e)
		sse_shufps(XMM5_XMM5,0x55)
		sse_loadaps(0x36)
		sse_shufps(XMM6_XMM6,0xaa)
		sse_loadaps(0x3e)
		sse_shufps(XMM7_XMM7,0xff)
		
		sse_mulps(XMM4_XMM0)
		sse_mulps(XMM5_XMM1)
		sse_mulps(XMM6_XMM2)
		sse_mulps(XMM7_XMM3)
		sse_addps(XMM4_XMM5)
		sse_addps(XMM6_XMM7)
		sse_addps(XMM4_XMM6)
		sse_storeaps(0x27)
		__asm add edi,16
		__asm add esi,16

		//	row2
		sse_loadaps(0x26)
		sse_shufps(XMM4_XMM4,0x00)
		sse_loadaps(0x2e)
		sse_shufps(XMM5_XMM5,0x55)
		sse_loadaps(0x36)
		sse_shufps(XMM6_XMM6,0xaa)
		sse_loadaps(0x3e)
		sse_shufps(XMM7_XMM7,0xff)
		
		sse_mulps(XMM4_XMM0)
		sse_mulps(XMM5_XMM1)
		sse_mulps(XMM6_XMM2)
		sse_mulps(XMM7_XMM3)
		sse_addps(XMM4_XMM5)
		sse_addps(XMM6_XMM7)
		sse_addps(XMM4_XMM6)
		sse_storeaps(0x27)
		__asm add edi,16
		__asm add esi,16

		//	row3
		sse_loadaps(0x26)
		sse_shufps(XMM4_XMM4,0x00)
		sse_loadaps(0x2e)
		sse_shufps(XMM5_XMM5,0x55)
		sse_loadaps(0x36)
		sse_shufps(XMM6_XMM6,0xaa)
		sse_loadaps(0x3e)
		sse_shufps(XMM7_XMM7,0xff)
		
		sse_mulps(XMM4_XMM0)
		sse_mulps(XMM5_XMM1)
		sse_mulps(XMM6_XMM2)
		sse_mulps(XMM7_XMM3)
		sse_addps(XMM4_XMM5)
		sse_addps(XMM6_XMM7)
		sse_addps(XMM4_XMM6)
		sse_storeaps(0x27)
	
	__asm
	{
		pop edi
		pop esi
		ret
	}
}

SIMD_ENTRY CSSEFMatrix& SIMD_CALL operator* (const CSSEFMatrix& m, const float& v)
{
	__asm
	{
		push esi
		push edi
		lea eax,_alignedSSEFloatMatrix
		mov esi,ecx
		mov edi,edx
		mov esi,[esi+4]
	}

	sse_loadss(0x07)
	__asm mov edi,[eax+4]
	sse_shufps(XMM0_XMM0,0)

	sse_loadaps(0x0e)
	__asm add esi,16
	sse_loadaps(0x16)
	__asm add esi,16
	sse_loadaps(0x1e)
	__asm add esi,16
	sse_loadaps(0x26)

	sse_mulps(XMM1_XMM0)
	sse_mulps(XMM2_XMM0)
	sse_mulps(XMM3_XMM0)
	sse_mulps(XMM4_XMM0)

	sse_storeaps(0x0f)
	__asm add edi,16
	sse_storeaps(0x17)
	__asm add edi,16
	sse_storeaps(0x1f)
	__asm add edi,16
	sse_storeaps(0x27)

	__asm
	{
		pop edi
		pop esi
		ret
	}
}


SIMD_ENTRY CSSEFMatrix& SIMD_CALL operator* (const float& v, const CSSEFMatrix& m)
{
	__asm
	{
		push esi
		push edi
		lea eax,_alignedSSEFloatMatrix
		mov esi,edx
		mov edi,ecx
		mov esi,[esi+4]
	}

	sse_loadss(0x07)
	__asm mov edi,[eax+4]
	sse_shufps(XMM0_XMM0,0)

	sse_loadaps(0x0e)
	__asm add esi,16
	sse_loadaps(0x16)
	__asm add esi,16
	sse_loadaps(0x1e)
	__asm add esi,16
	sse_loadaps(0x26)

	sse_mulps(XMM1_XMM0)
	sse_mulps(XMM2_XMM0)
	sse_mulps(XMM3_XMM0)
	sse_mulps(XMM4_XMM0)

	sse_storeaps(0x0f)
	__asm add edi,16
	sse_storeaps(0x17)
	__asm add edi,16
	sse_storeaps(0x1f)
	__asm add edi,16
	sse_storeaps(0x27)

	__asm
	{
		pop edi
		pop esi
		ret
	}
}


float SIMD_CALL CSSEFMatrix::Det(void)
{
	//	matrix = [  m0  m1  m2  m3 ]
	//			 [  m4  m5  m6  m7 ]
	//			 [  m8  m9 m10 m11 ]
	//			 [ m12 m13 m14 m15 ]

	//	d = row0 . M3
	//	M3 = m'1*M2 - m'2*M'2 + m'3*M"2
	//
	//	m'1 = [ m5 m4 m4 m4 ]			D21 = m10 * m15 - m11 * m14
	//	m'2 = [ m5 m6 m5 m5 ]			D22 = m9  * m15 - m11 * m13
	//	m'3 = [ m7 m7 m7 m6 ]			D23 = m9  * m14 - m10 * m13
	//	M2  = [ D21 D21 D22 D23 ]		D24 = m8  * m15 - m11 * m12
	//	M'2 = [ D22 D24 D24 D25 ]		D25 = m8  * m14 - m10 * m12
	//	M"2 = [ D23 D25 D26 D26 ]		D26 = m8  * m13 - m9  * m12

	__asm
	{
		//mov eax,this
		//mov edi,[eax+4]
		mov edi,[ecx+4]
		add edi,32
	}

	//	row2
	sse_loadaps(XMM0_EDI)
	//	row3
	__asm add edi,16
	sse_loadaps(XMM1_EDI)
	
	sse_loadaps(XMM2_XMM0)
	sse_loadaps(XMM3_XMM1)
	sse_loadaps(XMM4_XMM0)
	sse_loadaps(XMM5_XMM1)
	sse_loadaps(XMM6_XMM0)
	sse_loadaps(XMM7_XMM1)

	sse_shufps(XMM2_XMM2,0x01)	//SSE_R2_R1_R1_R1
	sse_shufps(XMM3_XMM3,0xbf)	//SSE_R4_R4_R4_R3
	sse_shufps(XMM4_XMM4,0xbf)
	sse_shufps(XMM5_XMM5,0x01)
	sse_shufps(XMM6_XMM6,0x5a)	//SSE_R3_R3_R2_R2
	sse_shufps(XMM7_XMM7,0x5a)	//SSE_R3_R3_R2_R2

	sse_loadaps(XMM0_XMM2)
	sse_loadaps(XMM1_XMM4)

	//	row1
	__asm sub edi,32

	sse_mulps(XMM0_XMM3)
	sse_mulps(XMM1_XMM5)
	sse_mulps(XMM2_XMM7)
	sse_mulps(XMM5_XMM6)
	sse_mulps(XMM4_XMM7)
	sse_mulps(XMM3_XMM6)

	sse_loadaps(XMM7_EDI)
	
	//	M'2
	sse_subps(XMM0_XMM1)
	sse_loadaps(XMM6_XMM7)
	//	M"2
	sse_subps(XMM2_XMM5)
	sse_loadaps(XMM5_XMM7)
	//	M2
	sse_subps(XMM3_XMM4)
	sse_loadaps(XMM4_XMM7)

	//	m'1
	sse_shufps(XMM6_XMM7,0x01)
	//	m'2
	sse_shufps(XMM5_XMM7,0x5a)
	//	m'3
	sse_shufps(XMM4_XMM7,0xbf)

	//	row0
	__asm sub edi,16
	sse_loadaps(XMM1_EDI)

	//	M3
	sse_mulps(XMM3_XMM6)
	sse_mulps(XMM0_XMM5)
	sse_mulps(XMM2_XMM4)
	sse_subps(XMM3_XMM0)
	sse_addps(XMM3_XMM2)

	__asm
	{
		lea eax,_alignedSSEFloatVector
		mov edi,[eax+4]
	}
	//	and finally, the det
	//	( the effective result is the sum of the vector )
	sse_mulps(XMM1_XMM3)
	sse_storeaps(XMM1_EDI)

	return  _alignedSSEFloatVector[0]-
			_alignedSSEFloatVector[1]+
			_alignedSSEFloatVector[2]-
			_alignedSSEFloatVector[3];
}

#endif
