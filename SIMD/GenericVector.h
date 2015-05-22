#ifndef __GENERIC_VECTOR_H__
#define __GENERIC_VECTOR_H__

#include <iostream>
#include <math.h>

#ifndef __GENERIC_MATRIX_H__
	#include "GenericMatrix.h"	// for linear systems
#endif

class CBVector;
class CMMXBVector;
class CMMXWVector;

template<class T,int D=4>
class CGenericVector
{	
public:
	// construction/destruction
	CGenericVector();
	CGenericVector(const T v[D]);

	//! Deprecated
	CGenericVector(T v0,T v1,T v2,T v3);
	virtual void SIMD_CALL Set(T v0,T v1,T v2,T v3)
	{ m_vector[0]=v0;m_vector[1]=v1;m_vector[2]=v2;m_vector[3]=v3;};


	virtual ~CGenericVector();

	//! Initialize all dimensions to 0
	virtual void SIMD_CALL Zero();

	//! Initialise all dimensions to 1
	virtual void SIMD_CALL One();
	
	// data access
	T* const vector(void) const { return (T*)(m_vector); };
	const T& SIMD_CALL operator[] ( int i ) const {  return m_vector[i]; };
	T& SIMD_CALL operator[] ( int i ) {  return m_vector[i]; };

	T& SIMD_CALL X() { return m_vector[0]; };
	T& SIMD_CALL Y() { return m_vector[1]; };
	T& SIMD_CALL Z() { return m_vector[2]; };
	T& SIMD_CALL H() { return m_vector[3]; };

	const T& SIMD_CALL X() const { return m_vector[0]; };
	const T& SIMD_CALL Y() const { return m_vector[1]; };
	const T& SIMD_CALL Z() const { return m_vector[2]; };
	const T& SIMD_CALL H() const { return m_vector[3]; };

	void X(T t) { m_vector[0] = t; };
	void Y(T t) { m_vector[1] = t; };
	void Z(T t) { m_vector[2] = t; };
	void H(T t) { m_vector[3] = t; };

	CGenericVector<T,D>& SIMD_CALL operator= ( const CGenericVector<T,D>& v );
	CGenericVector<T,D>& SIMD_CALL operator= ( const T v[D] );

	//operations
	bool SIMD_CALL operator== ( const CGenericVector<T,D>& v ) const;
	bool SIMD_CALL operator== ( const T v[D] ) const;
	virtual double SIMD_CALL Norm() const;
	virtual double SIMD_CALL Normalize();
	virtual T SIMD_CALL Length() const;


	// unary operations
	CGenericVector<T,D> SIMD_CALL operator-(void);
	CGenericVector<T,D>& SIMD_CALL operator!(void);
	CGenericVector<T,D>& SIMD_CALL operator-= (const CGenericVector<T,D>& v);
	CGenericVector<T,D>& SIMD_CALL operator+= (const CGenericVector<T,D>& v);
	CGenericVector<T,D>& SIMD_CALL operator*= (const CGenericVector<T,D>& v);
	CGenericVector<T,D>& SIMD_CALL operator*= (const T& t);

	//	this = m*this
	CGenericVector<T,D>&	SIMD_CALL operator*= (const CGenericMatrix<T>& m);

	// binary operations
	// cross product
	CGenericVector<T,D> SIMD_CALL operator^  (const CGenericVector<T,D>&) const;

	// dot product
	T SIMD_CALL operator% (const CGenericVector<T,D>&) const;

	// scalar operations
    CGenericVector<T,D> SIMD_CALL operator*  (const CGenericVector<T,D>&) const;
    CGenericVector<T,D> SIMD_CALL operator+  (const CGenericVector<T,D>&) const;
    CGenericVector<T,D> SIMD_CALL operator-  (const CGenericVector<T,D>&) const;
	CGenericVector<T,D> SIMD_CALL operator*  (const CGenericMatrix<T,D>&) const;
	CGenericVector<T,D> SIMD_CALL operator*  (T) const;
	

protected:
	T m_vector[D];
};



#endif

