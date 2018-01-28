// MMXByteVector.h: interface for the CMMXBVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MMXBYTEVECTOR_H__8C6D4565_1B55_4069_9E6A_54484A482CA1__INCLUDED_)
#define AFX_MMXBYTEVECTOR_H__8C6D4565_1B55_4069_9E6A_54484A482CA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __GENERIC_VECTOR_H__
	#include "GenericVector.h"
#endif

class CMMXBVector : public CGenericVector<char,4>  
{
public:
	CMMXBVector(){};
	virtual ~CMMXBVector(){};

	CMMXBVector& operator= ( const CMMXBVector& v )
	{ 	
		int t = *((int*)v.m_vector);
		*((int*)m_vector) = t;
		return *this;
	};
	CMMXBVector& operator= ( const CGenericVector<char,4>& v )
	{ 
		int t = *((int*)v.vector());
		*((int*)m_vector) = t;
		return *this;
	};
	CMMXBVector& operator= ( const char v[4] )
	{
		int t = *((int*)v);
		*((int*)m_vector) = t;
		return *this; 
	};	

	//operations
	bool operator== ( const CGenericVector<char,4>& v ) const
	{
		return (*((int*)v.vector()) == *((int*)m_vector));
	};

	bool operator== ( const CMMXBVector& v ) const
	{
		return (*((int*)v.vector()) == *((int*)m_vector));
	};

	bool operator== ( const char v[4] ) const
	{
		return (*((int*)v) == *((int*)m_vector));
	};

	void SIMD_CALL Zero() 
	{ 
		*((int*)m_vector) = 0;
	};

	void SIMD_CALL One()
	{ 
		*((int*)m_vector) = 0x01010101;
	};

	CMMXBVector& SIMD_CALL operator*= (const char& t)
	{
		m_vector[0] = (char)(m_vector[0] * t);
		m_vector[1] = (char)(m_vector[1] * t);
		m_vector[2] = (char)(m_vector[2] * t);
		m_vector[3] = (char)(m_vector[3] * t);
		return *this;
	};


	//	Real MMX Stuff starts here...
#ifndef SIMD_NO_ASSEMBLY
	CMMXBVector& operator!()
	{
		__asm
		{
			mov edi,this
			pxor mm0,mm0
			movd mm1,[edi+4]
			psubsb mm0,mm1
			movd [edi+4],mm0
			emms
		}
		return *this;
	}
	
	CMMXBVector& operator-= (const CMMXBVector& v)
	{ 
		__asm
		{
			mov esi,this
			mov edi,v
			movd mm0,[esi+4]
			movd mm1,[edi+4]
			psubsb mm0,mm1
			movd [esi+4],mm0
			emms
		}
		return *this; 
	};

	CMMXBVector& operator+= (const CMMXBVector& v)
	{ 
		__asm
		{
			mov esi,this
			mov edi,v
			movd mm0,[esi+4]
			movd mm1,[edi+4]
			paddsb mm0,mm1
			movd [esi+4],mm0
			emms
		}
		return *this; 
	};

	CMMXBVector& operator*= (const CMMXBVector& v)
	{ 
		__asm
		{
			mov esi,this
			mov edi,v
			movd mm0,[esi+4]
			pxor mm2,mm2
			movd mm1,[edi+4]
			punpcklbw mm0,mm2
			punpcklbw mm1,mm2
			pmullw mm0,mm1
			packsswb mm0,mm2
			movd [esi+4],mm0
			emms
		}
		return *this; 
	};

	// dot product
	CMMXBVector SIMD_CALL operator*  (const CMMXBVector& v2) const;
	
	// scalar operations
	CMMXBVector SIMD_CALL operator+  (const CMMXBVector& v2) const;
	
	CMMXBVector SIMD_CALL operator-  (const CMMXBVector& v2) const;
#endif
};

#endif // !defined(AFX_MMXBYTEVECTOR_H__8C6D4565_1B55_4069_9E6A_54484A482CA1__INCLUDED_)

