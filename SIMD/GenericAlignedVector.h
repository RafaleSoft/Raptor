/***************************************************************************/
/*                                                                         */
/*  GenericAlignedVector.h                                                 */
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



#ifndef __GENERIC_ALIGNED_VECTOR_H__
#define __GENERIC_ALIGNED_VECTOR_H__

#include <iostream>
#include <math.h>

#ifndef __GENERIC_ALIGNED_MATRIX_H__
	#include "GenericAlignedMatrix.h"	// for linear systems
#endif


template<class T> 
class CGenericAlignedVector
{	
public:
	// construction/destruction
	CGenericAlignedVector();
	CGenericAlignedVector(const CGenericAlignedVector&);
	CGenericAlignedVector(const T *v);
	CGenericAlignedVector(T v0,T v1,T v2,T v3);
	virtual ~CGenericAlignedVector();

	virtual void Zero()	NOEXCEPT { m_vector[0] = m_vector[1] = m_vector[2] = m_vector[3] = 0; };

	virtual void One()	NOEXCEPT { m_vector[0] = m_vector[1] = m_vector[2] = m_vector[3] = 1; };

	virtual void Set(T v0,T v1,T v2,T v3) NOEXCEPT { m_vector[0] = v0;m_vector[1] = v1;m_vector[2] = v2;m_vector[3] = v3;};

	
	// data access
	T* const vector(void) const { return (T*)(m_vector); };
	T& SIMD_CALL operator[] ( const int i ) const NOEXCEPT {  return m_vector[i]; };

	T& X() { return m_vector[0]; };
	T& Y() { return m_vector[1]; };
	T& Z() { return m_vector[2]; };
	T& H() { return m_vector[3]; };

	const T X() const { return m_vector[0]; };
	const T Y() const { return m_vector[1]; };
	const T Z() const { return m_vector[2]; };
	const T H() const { return m_vector[3]; };

	void X(T t) { m_vector[0] = t; };
	void Y(T t) { m_vector[1] = t; };
	void Z(T t) { m_vector[2] = t; };
	void H(T t) { m_vector[3] = t; };

	CGenericAlignedVector<T>& SIMD_CALL operator= ( const CGenericAlignedVector<T>& v );
	CGenericAlignedVector<T>& SIMD_CALL operator= ( const T v[4] );


	//operations
	bool SIMD_CALL operator== ( const CGenericAlignedVector<T>& v ) const;
	bool SIMD_CALL operator== ( const T v[4] ) const;
	virtual double SIMD_CALL Norm() const NOEXCEPT;
	virtual double SIMD_CALL Normalize() NOEXCEPT;
	virtual T SIMD_CALL Length() const NOEXCEPT;


	// unary operations
	CGenericAlignedVector<T>	SIMD_CALL operator-(void);
	CGenericAlignedVector<T>&	SIMD_CALL operator!(void);
	CGenericAlignedVector<T>&	SIMD_CALL operator-= (const CGenericAlignedVector<T>& v);
	CGenericAlignedVector<T>&	SIMD_CALL operator+= (const CGenericAlignedVector<T>& v);
	CGenericAlignedVector<T>&	SIMD_CALL operator*= (const CGenericAlignedVector<T>& v);
	CGenericAlignedVector<T>&	SIMD_CALL operator*= (const T& t);
	CGenericAlignedVector<T>&	SIMD_CALL operator*= (const CGenericAlignedMatrix<T>& m);


	// binary operations
	// cross product
	CGenericAlignedVector<T> SIMD_CALL operator^  (const CGenericAlignedVector<T>&) const;
	// dot product
	CGenericAlignedVector<T> SIMD_CALL operator*  (const CGenericAlignedVector<T>&) const;
	// scalar operations
	CGenericAlignedVector<T> SIMD_CALL operator*  (const CGenericAlignedMatrix<T>&) const;
	CGenericAlignedVector<T> SIMD_CALL operator*  (T) const;
	CGenericAlignedVector<T> SIMD_CALL operator+  (const CGenericAlignedVector<T>&) const;
	CGenericAlignedVector<T> SIMD_CALL operator-  (const CGenericAlignedVector<T>&) const;

protected:
	T *m_vector;
};


#endif	// __GENERIC_ALIGNED_VECTOR_H__

