/***************************************************************************/
/*                                                                         */
/*  GenericMatrix.h                                                        */
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


#ifndef __GENERIC_MATRIX_H__
#define __GENERIC_MATRIX_H__

#include <iostream>
#include <math.h>

#ifndef __SIMDMACROS_H__
    #include "simdMacros.h"
#endif


template <class T,int D> class CGenericVector;
class CMMXWMatrix;

template<class T,int D=4> 
class CGenericMatrix
{
public:
	// construction/destruction
	CGenericMatrix();
	virtual ~CGenericMatrix();
	virtual void Zero();
	virtual void One();
	virtual void Ident();
	
	// data access
	T* const matrix(void) const { return (T*)(m_matrix); };
	T& SIMD_CALL operator[] ( const int i ) {  return m_matrix[i]; };
	const T operator[] ( const int i ) const {  return m_matrix[i]; };

	CGenericMatrix<T,D>& SIMD_CALL operator= ( const CGenericMatrix<T,D>& m );
	CGenericMatrix<T,D>& SIMD_CALL operator= ( const T m[D*D] );

	//	Store transpose
	CGenericMatrix<T,D>& SIMD_CALL operator^= ( const CGenericMatrix<T,D>& m );
	CGenericMatrix<T,D>& SIMD_CALL operator^= ( const T m[D*D] );

	//	the determinant
	T SIMD_CALL Det(void);

	//	inverse matrix
	CGenericMatrix<T,D>& SIMD_CALL Inv(void);

	//	transpose matrix
	CGenericMatrix<T,D>& SIMD_CALL Transpose(void);

	// unary operations
	CGenericMatrix<T,D>& SIMD_CALL operator!(void);	// negate all values
	CGenericMatrix<T,D>& SIMD_CALL operator-= (const CGenericMatrix<T,D>& m);
	CGenericMatrix<T,D>& SIMD_CALL operator+= (const CGenericMatrix<T,D>& m);
	CGenericMatrix<T,D>& SIMD_CALL operator*= (const CGenericMatrix<T,D>& m);
	CGenericMatrix<T,D>& SIMD_CALL operator*= ( T );
	

	// binary operations
	CGenericMatrix<T,D> SIMD_CALL operator*  (T) const;
	CGenericVector<T,D> SIMD_CALL operator*  (const CGenericVector<T,D>&) const;
	CGenericMatrix<T,D> SIMD_CALL operator*  (const CGenericMatrix<T,D>&) const;
    CGenericMatrix<T,D> SIMD_CALL operator+  (const CGenericMatrix<T,D>&) const;
    CGenericMatrix<T,D> SIMD_CALL operator-  (const CGenericMatrix<T,D>&) const;

protected:
	T m_matrix[D*D];
};


#endif	//__GENERIC_MATRIX_H__

