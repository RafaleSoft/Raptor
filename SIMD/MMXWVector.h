// MMXWVector.h: interface for the CMMXWVector class.
//
//////////////////////////////////////////////////////////////////////

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

	CMMXWVector& operator= ( const CMMXWVector& v )
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
	CMMXWVector& operator= ( const CGenericVector<unsigned short>& v )
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
	CMMXWVector& operator= ( const unsigned short v[4] )
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
	bool operator== ( const CGenericVector<unsigned short>& v ) const
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

	bool operator== ( const CMMXWVector& v ) const
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

	bool operator== ( const unsigned short v[4] ) const
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

	void SIMD_CALL Zero() 
	{ 
		__asm 
		{
			mov edi,this
			xor eax,eax 
			mov [edi+8],eax
			mov [edi+4],eax 
		}
	};

	void SIMD_CALL One()
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
	CMMXWVector& operator!()
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
	
	CMMXWVector& operator-= (const CMMXWVector& v)
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

	CMMXWVector& operator+= (const CMMXWVector& v)
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

	CMMXWVector& operator*= (const CMMXWVector& v)
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

	CMMXWVector& operator*= (const unsigned short& t)
	{ 
		m_vector[0]=(unsigned short)(m_vector[0]*t);
		m_vector[1]=(unsigned short)(m_vector[1]*t);
		m_vector[2]=(unsigned short)(m_vector[2]*t);
		m_vector[3]=(unsigned short)(m_vector[3]*t);
		return *this; 
	};

	//	this = m*this
	CMMXWVector& SIMD_CALL operator*= (const CMMXWMatrix& m);

	// binary operations
	// cross product
	CMMXWVector& SIMD_CALL operator^  (const CMMXWVector& v2) const;

	// dot product
	CMMXWVector& SIMD_CALL operator*  (const CMMXWVector& v2) const;

	// linear systems
    // return = m * this
	CMMXWVector& SIMD_CALL operator*  (const CMMXWMatrix& m) const;
	
	// scalar operations
	CMMXWVector& SIMD_CALL operator+  (const CMMXWVector& v2) const;
	CMMXWVector& SIMD_CALL operator-  (const CMMXWVector& v2) const;
};

// return = v * m
CMMXWVector& SIMD_CALL operator*  (const CMMXWVector& v, const CMMXWMatrix& m);

#endif // !defined(AFX_MMXWVector_H__EDBB336A_5AF7_4AF0_9000_1F0F0CE7C0C0__INCLUDED_)


