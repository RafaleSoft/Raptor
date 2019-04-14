/***************************************************************************/
/*                                                                         */
/*  MMXWVector.h                                                           */
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



#if !defined(AFX_MMXWVector_H__EDBB336A_5AF7_4AF0_9000_1F0F0CE7C0C0__INCLUDED_)
#define AFX_MMXWVector_H__EDBB336A_5AF7_4AF0_9000_1F0F0CE7C0C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __GENERIC_VECTOR_H__
	#include "GenericVector.h"
#endif


class CMMXWVector : public CGenericVector<unsigned short>  
{
public:
	CMMXWVector(){};
	virtual ~CMMXWVector(){};

	CMMXWVector& operator*= (const unsigned short& t) NOEXCEPT
	{
		m_vector[0] = m_vector[0] * t;
		m_vector[1] = m_vector[1] * t;
		m_vector[2] = m_vector[2] * t;
		m_vector[3] = m_vector[3] * t;
		return *this;
	};

#ifndef SIMD_NO_ASSEMBLY
	CMMXWVector& operator= ( const CMMXWVector& v ) NOEXCEPT
	{ 
		__asm
		{
			mov esi,v
			mov edi,this
			movq mm0,[esi+4]
			movq [edi+4],mm0
			emms
		}
		return *this;
	};
	CMMXWVector& operator= ( const CGenericVector<unsigned short>& v ) NOEXCEPT
	{ 
		__asm
		{
			mov esi,v
			mov edi,this
			movq mm0,[esi+4]
			movq [edi+4],mm0
			emms
		}
		return *this;
	};
	CMMXWVector& operator= ( const unsigned short v[4] ) NOEXCEPT
	{
		__asm
		{
			mov esi,v
			mov edi,this
			movq mm0,[esi]
			movq [edi+4],mm0
			emms
		}
		return *this; 
	};	

	//operations
	bool operator== ( const CGenericVector<unsigned short>& v ) const NOEXCEPT
	{ 
		__asm
		{
			mov edi,this
			mov esi,v
			xor eax,eax
			mov ecx,[edi+4]
			cmp ecx,[esi+4]
			jne noteq
			mov ecx,[edi+8]
			cmp ecx,[esi+8]
			jne noteq
			mov eax,1
			noteq:
		}
// no return value
#pragma warning(disable:4035)
	};
#pragma warning(default:4035)

	bool operator== ( const CMMXWVector& v ) const NOEXCEPT
	{ 
		__asm
		{
			mov edi,this
			mov esi,v
			xor eax,eax
			mov ecx,[edi+4]
			cmp ecx,[esi+4]
			jne noteq
			mov ecx,[edi+8]
			cmp ecx,[esi+8]
			jne noteq
			mov eax,1
			noteq:		
		}
// no return value
#pragma warning(disable:4035)
	};
#pragma warning(default:4035)

	bool operator== ( const unsigned short v[4] ) const NOEXCEPT
	{ 
		__asm
		{
			mov edi,this
			mov esi,v
			xor eax,eax
			mov ecx,[edi+4]
			cmp ecx,[esi]
			jne noteq
			mov ecx,[edi+8]
			cmp ecx,[esi+4]
			jne noteq
			mov eax,1
			noteq:
		}	
// no return value
#pragma warning(disable:4035)
	};
#pragma warning(error:4035)

	void SIMD_CALL Zero() NOEXCEPT
	{ 
		__asm 
		{
			mov edi,this
			xor eax,eax 
			mov [edi+8],eax
			mov [edi+4],eax 
		}
	};

	void SIMD_CALL One() NOEXCEPT
	{ 
		__asm 
		{
			mov edi,this
			mov eax,0x00010001
			mov [edi+8],eax
			mov [edi+4],eax 
		}
	};


//	Real MMX Stuff starts here...
	CMMXWVector& operator!() NOEXCEPT
	{
		__asm
		{
			mov edi,this
			pxor mm0,mm0
			psubsw mm0,[edi+4]
			movq [edi+4],mm0
			emms
		}
		return *this;
	}
	
	CMMXWVector& operator-= (const CMMXWVector& v) NOEXCEPT
	{ 
		__asm
		{
			mov esi,this
			mov edi,v
			movq mm0,[esi+4]
			psubsw mm0,[edi+4]
			movq [esi+4],mm0
			emms
		}
		return *this; 
	};

	CMMXWVector& operator+= (const CMMXWVector& v) NOEXCEPT
	{ 
		__asm
		{
			mov esi,this
			mov edi,v
			movq mm0,[esi+4]
			paddsw mm0,[edi+4]
			movq [esi+4],mm0
			emms
		}
		return *this; 
	};

	CMMXWVector& operator*= (const CMMXWVector& v) NOEXCEPT
	{ 
		__asm
		{
			mov esi,this
			mov edi,v
			movq mm0,[esi+4]
			pmullw mm0,[edi+4]
			movq [esi+4],mm0
			emms
		}
		return *this; 
	};


	//	this = m*this
	CMMXWVector& SIMD_CALL operator*= (const CMMXWMatrix& m) NOEXCEPT;

	// binary operations
	// cross product
	CMMXWVector& SIMD_CALL operator^  (const CMMXWVector& v2) const NOEXCEPT;

	// dot product
	CMMXWVector& SIMD_CALL operator*  (const CMMXWVector& v2) const NOEXCEPT;

	// linear systems
    // return = m * this
	CMMXWVector& SIMD_CALL operator*  (const CMMXWMatrix& m) const NOEXCEPT;
	
	// scalar operations
	CMMXWVector& SIMD_CALL operator+  (const CMMXWVector& v2) const NOEXCEPT;
	CMMXWVector& SIMD_CALL operator-  (const CMMXWVector& v2) const NOEXCEPT;
#endif
};

// return = v * m
CMMXWVector& SIMD_CALL operator*  (const CMMXWVector& v, const CMMXWMatrix& m);

#endif // !defined(AFX_MMXWVector_H__EDBB336A_5AF7_4AF0_9000_1F0F0CE7C0C0__INCLUDED_)


