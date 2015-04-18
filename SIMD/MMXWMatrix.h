// MMXWMatrix.h: interface for the CMMXWMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MMXWMATRIX_H__6CB7BBD5_103F_4C28_AA90_88AA8AD9E466__INCLUDED_)
#define AFX_MMXWMATRIX_H__6CB7BBD5_103F_4C28_AA90_88AA8AD9E466__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __GENERIC_MATRIX_H__
	#include "GenericMatrix.h"
#endif

class CMMXWMatrix : public CGenericMatrix<unsigned short>
{
public:
	CMMXWMatrix(){};
	virtual ~CMMXWMatrix(){};

	CMMXWMatrix&  operator= ( const unsigned short m[16] )
	{ 
		for (int i=0;i<16;i++)
			m_matrix[i]=m[i];
		return *this; 
	};

	CMMXWMatrix&  operator= ( const CMMXWMatrix& m )
	{ 
		for (int i=0;i<16;i++)
			m_matrix[i]=m.m_matrix[i];
		return *this; 
	};

	CMMXWMatrix&  operator= ( const CGenericMatrix<unsigned short>& m )
	{ 
		for (int i=0;i<16;i++)
			m_matrix[i]=m.m_matrix[i];
		return *this; 
	};

	CMMXWMatrix&	SIMD_CALL operator-= (const CMMXWMatrix& m)
	{ 
		_asm
		{
			mov esi,m
			mov edi,this

			add esi,4
			movq mm0,[esi]
			add edi,4
			psubsw mm0,[edi]
			movq [edi],mm0

			add esi,8
			movq mm0,[esi]
			add edi,8
			psubsw mm0,[edi]
			movq [edi],mm0

			add esi,8
			movq mm0,[esi]
			add edi,8
			psubsw mm0,[edi]
			movq [edi],mm0

			add esi,8
			movq mm0,[esi]
			add edi,8
			psubsw mm0,[edi]
			movq [edi],mm0

			emms
		}
		return *this; 
	};

	CMMXWMatrix&	SIMD_CALL operator+= (const CMMXWMatrix& m)
	{ 
		_asm
		{
			mov esi,m
			mov edi,this

			add esi,4
			movq mm0,[esi]
			add edi,4
			paddsw mm0,[edi]
			movq [edi],mm0

			add esi,8
			movq mm0,[esi]
			add edi,8
			paddsw mm0,[edi]
			movq [edi],mm0

			add esi,8
			movq mm0,[esi]
			add edi,8
			paddsw mm0,[edi]
			movq [edi],mm0

			add esi,8
			movq mm0,[esi]
			add edi,8
			paddsw mm0,[edi]
			movq [edi],mm0

			emms
		}
		return *this; 
	};

	CMMXWMatrix&	SIMD_CALL operator*= (const CMMXWMatrix& m);
	
	/*
	CGenericMatrix<T>&	SIMD_CALL operator*= (const T& t);
	

	// binary operations
	friend CGenericMatrix<T> SIMD_CALL operator*  (const CGenericMatrix<T>&, const CGenericMatrix<T>&);
	friend CGenericMatrix<T> SIMD_CALL operator*  (const T&, const CGenericMatrix<T>&);
	friend CGenericMatrix<T> SIMD_CALL operator*  (const CGenericMatrix<T>&, const T&);
	friend CGenericMatrix<T> SIMD_CALL operator+  (const CGenericMatrix<T>&,const CGenericMatrix<T>&);
	friend CGenericMatrix<T> SIMD_CALL operator-  (const CGenericMatrix<T>&,const CGenericMatrix<T>&);
	*/
};

#endif // !defined(AFX_MMXWMATRIX_H__6CB7BBD5_103F_4C28_AA90_88AA8AD9E466__INCLUDED_)


