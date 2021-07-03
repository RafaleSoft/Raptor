/***************************************************************************/
/*                                                                         */
/*  GenericAlignedMatrix.h                                                 */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef __GENERIC_ALIGNED_MATRIX_H__
#define __GENERIC_ALIGNED_MATRIX_H__

#include <iostream>
#include <math.h>
#include <stdint.h>

#ifndef __SIMDMACROS_H__
    #include "simdMacros.h"
#endif



template <class T> class CGenericAlignedVector;
class CSSEFMatrix;

template<class T>
class CGenericAlignedMatrix
{
protected:
	T *m_matrix;
	
public:
	// construction/destruction
	CGenericAlignedMatrix() NOEXCEPT;
	virtual ~CGenericAlignedMatrix();
	virtual void Zero() NOEXCEPT
	{
		for (int i=0;i<16;i++)
			m_matrix[i]=0;
	};
	virtual void One() NOEXCEPT
	{
		for (int i=0;i<16;i++)
			m_matrix[i]=1;
	};
	virtual void Ident() NOEXCEPT
	{
		for (int i=0;i<16;i++)
			m_matrix[i]=0;
		m_matrix[0]=m_matrix[5]=m_matrix[10]=m_matrix[15]=1;
	};
	
	// data access
	T* const matrix(void) const { return (T*)(m_matrix); };
	T& SIMD_CALL operator[] ( const int i ) {  return m_matrix[i]; };
	const T operator[] ( const int i ) const {  return m_matrix[i]; };
	

	CGenericAlignedMatrix<T>& SIMD_CALL operator= ( const CGenericAlignedMatrix<T>& m )
	{
		for (int i=0;i<16;i++)
			m_matrix[i]=m.m_matrix[i];
		return *this;
	};
	CGenericAlignedMatrix<T>& SIMD_CALL operator= ( const T m[16] )
	{
		for (int i=0;i<16;i++)
			m_matrix[i]=m[i];
		return *this;
	};

		//	Store transpose
	CGenericAlignedMatrix<T>& SIMD_CALL operator^= ( const CGenericAlignedMatrix<T>& m )
	{
		m_matrix[0]=m.m_matrix[0];m_matrix[4]=m.m_matrix[1];m_matrix[8]=m.m_matrix[2];m_matrix[12]=m.m_matrix[3];
		m_matrix[1]=m.m_matrix[4];m_matrix[5]=m.m_matrix[5];m_matrix[9]=m.m_matrix[6];m_matrix[13]=m.m_matrix[7];
		m_matrix[2]=m.m_matrix[8];m_matrix[6]=m.m_matrix[9];m_matrix[10]=m.m_matrix[10];m_matrix[14]=m.m_matrix[11];
		m_matrix[3]=m.m_matrix[12];m_matrix[7]=m.m_matrix[13];m_matrix[11]=m.m_matrix[14];m_matrix[15]=m.m_matrix[15];
		return *this;
	};
	CGenericAlignedMatrix<T>& SIMD_CALL operator^= ( const T m[16] )
	{
		m_matrix[0]=m[0];m_matrix[4]=m[1];m_matrix[8]=m[2];m_matrix[12]=m[3];
		m_matrix[1]=m[4];m_matrix[5]=m[5];m_matrix[9]=m[6];m_matrix[13]=m[7];
		m_matrix[2]=m[8];m_matrix[6]=m[9];m_matrix[10]=m[10];m_matrix[14]=m[11];
		m_matrix[3]=m[12];m_matrix[7]=m[13];m_matrix[11]=m[14];m_matrix[15]=m[15];
		return *this;
	};

	// T SIMD_CALL Det(void);

	// unary operations
	CGenericAlignedMatrix<T> SIMD_CALL operator-(void);
	CGenericAlignedMatrix<T>& SIMD_CALL operator!(void)
	{
		for (int i=0;i<16;i++)
			m_matrix[i]=-m_matrix[i];
		return *this;
	};
	CGenericAlignedMatrix<T>& SIMD_CALL operator~(void)
	{
		T elt;

		elt = m_matrix[1]; m_matrix[1] = m_matrix[4]; m_matrix[4] = elt;
		elt = m_matrix[2]; m_matrix[2] = m_matrix[8]; m_matrix[8] = elt;
		elt = m_matrix[6]; m_matrix[6] = m_matrix[9]; m_matrix[9] = elt;
		elt = m_matrix[12]; m_matrix[12] = m_matrix[3]; m_matrix[3] = elt;
		elt = m_matrix[13]; m_matrix[13] = m_matrix[7]; m_matrix[7] = elt;
		elt = m_matrix[11]; m_matrix[11] = m_matrix[14]; m_matrix[14] = elt;
		return *this;
	};
	CGenericAlignedMatrix<T>&	SIMD_CALL operator-= (const CGenericAlignedMatrix<T>& m)
	{
		for (int i=0;i<16;i++)
			m_matrix[i]-=m.m_matrix[i];
		return *this;
	};
	CGenericAlignedMatrix<T>&	SIMD_CALL operator+= (const CGenericAlignedMatrix<T>& m)
	{
		for (int i=0;i<16;i++)
			m_matrix[i]+=m.m_matrix[i];
		return *this;
	};
	CGenericAlignedMatrix<T>&	SIMD_CALL operator*= (const CGenericAlignedMatrix<T>& m);
	CGenericAlignedMatrix<T>&	SIMD_CALL operator*= (T t);
	

	// binary operations
	CGenericAlignedMatrix<T> SIMD_CALL operator*  (const CGenericAlignedMatrix<T>&) const;
	CGenericAlignedMatrix<T> SIMD_CALL operator*  (T) const;
	CGenericAlignedMatrix<T> SIMD_CALL operator+  (const CGenericAlignedMatrix<T>&) const;
	CGenericAlignedMatrix<T> SIMD_CALL operator-  (const CGenericAlignedMatrix<T>&) const;
    CGenericAlignedVector<T> SIMD_CALL operator*  (const CGenericAlignedVector<T>&) const;
};


/////////////////////////////////////////////////////////////////////////////////////////////
// construction/destruction
/////////////////////////////////////////////////////////////////////////////////////////////

//	Matrix is aligned to a 16 bytes boundery
template <class T>
CGenericAlignedMatrix<T>::CGenericAlignedMatrix() NOEXCEPT
{
	//		16 bytes more space is allocated,
	//		to store alignment and to store align offset
	//	char *pT = new char[16*sizeof(##T)+16];
	//		align data
	//	vector = (##T*)((int(pT)+0x00000010) & 0xfffffff0);
	//		store offset
	//	*((char*)vector-1)=char(int(pT)&0xf);

	char *pT = new char[16 * sizeof(T) + 16];

#if defined(_WIN64)
	m_matrix = (T*)((uint64_t(pT) + 0x0000000000000010) & 0xfffffffffffffff0);
	*((char*)m_matrix - 1) = char(uint64_t(pT) & 0xf);
#elif defined(WIN32) || defined(LINUX)
	m_matrix = (T*)((long(pT) + 0x00000010) & 0xfffffff0);
	*((char*)m_matrix - 1) = char(long(pT) & 0xf);
#endif
}

template <class T>
CGenericAlignedMatrix<T>::~CGenericAlignedMatrix()
{
	//	delete exact allocated space
	char *pT = (char*)m_matrix;
	pT-=*(pT-1);
	//	translation offset in case that
	//	vector has initially been allocated
	//	on a 16bytes boundery
	if (pT==(char*)m_matrix)
		pT-=16;
	delete [] pT;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// implementation
/////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
__inline CGenericAlignedMatrix<T> SIMD_CALL CGenericAlignedMatrix<T>::operator-()
{
	CGenericAlignedMatrix<T> m;

	for (int i=0;i<16;i++)
		m.m_matrix[i]=-m_matrix[i];
	return m;
}

template <class T>
__inline CGenericAlignedMatrix<T>&	SIMD_CALL CGenericAlignedMatrix<T>::operator*= (const CGenericAlignedMatrix<T>& m2)
{
	CGenericAlignedMatrix<T> m;
	T t0,t1,t2,t3;

	for (int j=0;j<4;j++)
	{
		t0 = m2.m_matrix[j];
		t1 = m2.m_matrix[j+4];
		t2 = m2.m_matrix[j+8];
		t3 = m2.m_matrix[j+12];

		for (int i=0;i<16;i+=4)
		{
			m.m_matrix[j+i] =	m_matrix[i] * t0 +
							m_matrix[i+1] * t1 +
							m_matrix[i+2] * t2 +
							m_matrix[i+3] * t3;
		}
	}
	for (int i=0;i<16;i++)
		m_matrix[i]=m.m_matrix[i];
	return *this;
}

template <class T>
__inline CGenericAlignedMatrix<T>&	SIMD_CALL CGenericAlignedMatrix<T>::operator*= (T t)
{
	for (int i=0;i<16;i++)
		m_matrix[i]*=t;
	return *this;
}

template <class T>
__inline CGenericAlignedMatrix<T> SIMD_CALL CGenericAlignedMatrix<T>::operator*  (const CGenericAlignedMatrix<T>& m2) const
{
	CGenericAlignedMatrix<T> m;
	T t0,t1,t2,t3;

	for (int j=0;j<4;j++)
	{
		t0 = m2.m_matrix[j];
		t1 = m2.m_matrix[j+4];
		t2 = m2.m_matrix[j+8];
		t3 = m2.m_matrix[j+12];

		for (int i=0;i<16;i+=4)
		{
			m.m_matrix[j+i] =	m_matrix[i] * t0 +
							    m_matrix[i+1] * t1 +
							    m_matrix[i+2] * t2 +
							    m_matrix[i+3] * t3;
		}
	}
	return m;
}


template <class T>
__inline CGenericAlignedMatrix<T> SIMD_CALL CGenericAlignedMatrix<T>::operator*  (T t) const
{
	CGenericAlignedMatrix<T> m;

	for (int i=0;i<16;i++)
		m.m_matrix[i] = m_matrix[i] * t;
	return m;
}

template <class T>
__inline CGenericAlignedMatrix<T> SIMD_CALL CGenericAlignedMatrix<T>::operator+  (const CGenericAlignedMatrix<T>& m2) const
{
	CGenericAlignedMatrix<T> m;

	for (int i=0;i<16;i++)
		m.m_matrix[i] = m_matrix[i] + m2.m_matrix[i];
	return m;
}

template <class T>
__inline CGenericAlignedMatrix<T> SIMD_CALL CGenericAlignedMatrix<T>::operator-  (const CGenericAlignedMatrix<T>& m2) const
{
	CGenericAlignedMatrix<T> m;

	for (int i=0;i<16;i++)
		m.m_matrix[i] = m_matrix[i] - m2.m_matrix[i];
	return m;
}


template <class T>
__inline CGenericAlignedVector<T> SIMD_CALL CGenericAlignedMatrix<T>::operator*  (const CGenericAlignedVector<T>& v1) const
{
	CGenericAlignedVector<T> v;

	v.m_vector[0] = v1.m_vector[0]*m_matrix[0]+v1.m_vector[1]*m_matrix[1]+v1.m_vector[2]*m_matrix[2]+v1.m_vector[3]*m_matrix[3];
	v.m_vector[1] = v1.m_vector[0]*m_matrix[4]+v1.m_vector[1]*m_matrix[5]+v1.m_vector[2]*m_matrix[6]+v1.m_vector[3]*m_matrix[7];
	v.m_vector[2] = v1.m_vector[0]*m_matrix[8]+v1.m_vector[1]*m_matrix[9]+v1.m_vector[2]*m_matrix[10]+v1.m_vector[3]*m_matrix[11];
	v.m_vector[3] = v1.m_vector[0]*m_matrix[12]+v1.m_vector[1]*m_matrix[13]+v1.m_vector[2]*m_matrix[14]+v1.m_vector[3]*m_matrix[15];

	return v;
}

#endif	// __GENERIC_ALIGNED_MATRIX_H__

