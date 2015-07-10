// SSEFMatrix.h: interface for the CSSEFMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSEFMATRIX_H__7923E89B_7F26_4EE5_8038_15480F2B7985__INCLUDED_)
#define AFX_SSEFMATRIX_H__7923E89B_7F26_4EE5_8038_15480F2B7985__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __GENERIC_ALIGNED_MATRIX_H__
	#include "GenericAlignedMatrix.h"
#endif

#ifndef __REGISTERS_H__
	#include "Registers.h"
#endif


class CSSEFMatrix : public CGenericAlignedMatrix<float>  
{
public:
	CSSEFMatrix();
	virtual ~CSSEFMatrix();

	#pragma warning(disable:4100) // unreferenced parameter
	#pragma warning(disable:4035) // no return value
	CSSEFMatrix& SIMD_CALL operator= ( const CSSEFMatrix& m )
	{ 
		__asm
		{
			mov ecx,0x10
			mov ebx,m
			mov eax,this
			mov esi,[ebx+4]
			mov edi,[eax+4]
			rep movsd
		}
	};

	#pragma warning(disable:4100) // unreferenced parameter
	CSSEFMatrix& SIMD_CALL operator= ( const float m[16] )
	{ 
		__asm
		{
			mov ecx,0x10
			mov esi,m
			mov eax,this
			mov edi,[eax+4]
			rep movsd
		} 
	};

	#pragma warning(disable:4100) // unreferenced parameter
	CSSEFMatrix& SIMD_CALL operator= ( const CGenericAlignedMatrix<float>& m )
	{ 
		__asm
		{
			mov ecx,0x10
			mov ebx,m
			mov eax,this
			mov esi,[ebx+4]
			mov edi,[eax+4]
			rep movsd
		}
	};

	//	the determinant
	float SIMD_CALL Det(void);

	// unary operations
	CSSEFMatrix& SIMD_CALL operator-(void);
	CSSEFMatrix& SIMD_CALL operator!(void);
	CSSEFMatrix& SIMD_CALL operator-=(const CSSEFMatrix& m);
	CSSEFMatrix& SIMD_CALL operator+=(const CSSEFMatrix& m);

	CSSEFMatrix& SIMD_CALL operator*=(const CSSEFMatrix& m);
	CSSEFMatrix& SIMD_CALL operator*=(const float& t);

	// binary operations
	CSSEFMatrix& SIMD_CALL operator+(const CSSEFMatrix& m2) const;
	CSSEFMatrix& SIMD_CALL operator-(const CSSEFMatrix& m2) const;
};

CSSEFMatrix& SIMD_CALL operator*(const CSSEFMatrix& m1, const CSSEFMatrix& m2);
CSSEFMatrix& SIMD_CALL operator*(const float& v, const CSSEFMatrix& m);
CSSEFMatrix& SIMD_CALL operator*(const CSSEFMatrix& m, const float& v);

/////////////////////////////////////////////////////////////////////////////////////////////
// implementation
/////////////////////////////////////////////////////////////////////////////////////////////
extern CSSEFMatrix _alignedSSEFloatMatrix;

__inline CSSEFMatrix& SIMD_CALL CSSEFMatrix::operator-(void)
{
	__asm
	{
		mov ebx,this
		lea eax,_alignedSSEFloatMatrix
		mov esi,[ebx+4]
		mov edi,[eax+4]
	}

	sse_xorps(XMM0_XMM0)
	sse_subps(0x06)
	sse_xorps(XMM1_XMM1)
	sse_storeaps(0x07)		// v1
	__asm add esi,16
	sse_xorps(XMM2_XMM2)
	__asm add edi,16
	sse_subps(0x0e)
	sse_xorps(XMM3_XMM3)
	__asm add esi,16
	sse_storeaps(0x0f)		// v2
	__asm add edi,16
	sse_subps(0x16)
	__asm add esi,16
	sse_storeaps(0x17)		// v3
	__asm add edi,16
	sse_subps(0x1e)
	sse_storeaps(0x1f)		// v4
}

__inline CSSEFMatrix& SIMD_CALL CSSEFMatrix::operator!(void)
{
	__asm
	{
		mov eax,this
		mov esi,[eax+4]
		mov edi,[eax+4]
	}

	sse_xorps(XMM0_XMM0)
	sse_subps(0x06)
	sse_xorps(XMM1_XMM1)
	sse_storeaps(0x06)		// v1
	__asm add esi,16
	sse_xorps(XMM2_XMM2)
	__asm add edi,16
	sse_subps(0x0e)
	sse_xorps(XMM3_XMM3)
	__asm add esi,16
	sse_storeaps(0x0f)		// v2
	__asm add edi,16
	sse_subps(0x16)
	__asm add esi,16
	sse_storeaps(0x17)		// v3
	__asm add edi,16
	sse_subps(0x1e)
	sse_storeaps(0x1f)		// v4
}

__inline CSSEFMatrix& SIMD_CALL CSSEFMatrix::operator-=(const CSSEFMatrix& m)
{
	__asm
	{
		mov eax,this
		mov ebx,m
		mov esi,[ebx+4]
		mov edi,[eax+4]
	}
	sse_loadaps(0x07)
	sse_subps(0x06)
	sse_storeaps(0x07)
	__asm add esi,16
	__asm add edi,16
	sse_loadaps(0x0f)
	sse_subps(0x0e)
	sse_storeaps(0x0f)
	__asm add esi,16
	__asm add edi,16
	sse_loadaps(0x17)
	sse_subps(0x16)
	sse_storeaps(0x17)
	__asm add esi,16
	__asm add edi,16
	sse_loadaps(0x1f)
	sse_subps(0x1e)
	sse_storeaps(0x1f)
}

__inline CSSEFMatrix& SIMD_CALL CSSEFMatrix::operator+=(const CSSEFMatrix& m)
{
	__asm
	{
		mov eax,this
		mov ebx,m
		mov esi,[ebx+4]
		mov edi,[eax+4]
	}
	sse_loadaps(0x07)
	sse_addps(0x06)
	sse_storeaps(0x07)
	__asm add esi,16
	__asm add edi,16
	sse_loadaps(0x0f)
	sse_addps(0x0e)
	sse_storeaps(0x0f)
	__asm add esi,16
	__asm add edi,16
	sse_loadaps(0x17)
	sse_addps(0x16)
	sse_storeaps(0x17)
	__asm add esi,16
	__asm add edi,16
	sse_loadaps(0x1f)
	sse_addps(0x1e)
	sse_storeaps(0x1f)
}

__inline CSSEFMatrix& SIMD_CALL CSSEFMatrix::operator*=(const float& t)
{
	__asm
	{
		mov eax,this
		mov edi,t
		mov esi,[eax+4]
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
}




#endif // !defined(AFX_SSEFMATRIX_H__7923E89B_7F26_4EE5_8038_15480F2B7985__INCLUDED_)

