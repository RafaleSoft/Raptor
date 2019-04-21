/***************************************************************************/
/*                                                                         */
/*  GenericVector.cxx                                                      */
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


#ifndef __GENERIC_VECTOR_IMPL__
#define __GENERIC_VECTOR_IMPL__

#ifndef __GENERIC_VECTOR_H__
	#include "GenericVector.h"
#endif


template <class T,int D>
__inline CGenericVector<T,D>::CGenericVector() NOEXCEPT
{
	for (int i=0;i<D-1;i++)
		m_vector[i] = 0; 
	m_vector[D-1] = 1;
};

template <class T,int D>
__inline CGenericVector<T,D>::CGenericVector(const T v[D])
{
	for (int i=0;i<D;i++)
		m_vector[i] = v[i]; 
};
	
template <class T,int D>
__inline CGenericVector<T,D>::CGenericVector(T v0,T v1,T v2,T v3)
{ 
	m_vector[0] = v0; 
	m_vector[1] = v1; 
	m_vector[2] = v2; 
	m_vector[3] = v3;
};

template <class T,int D>
__inline CGenericVector<T,D>::~CGenericVector() 
{
};


/////////////////////////////////////////////////////////////////////////////////////////////
// implementation
/////////////////////////////////////////////////////////////////////////////////////////////

template <class T,int D>
__inline void SIMD_CALL CGenericVector<T,D>::Zero()
{
	for (int i=0;i<D;i++)
		m_vector[i] = 0;
};

template <class T,int D>
__inline void SIMD_CALL CGenericVector<T,D>::One()
{
	for (int i=0;i<D;i++)
		m_vector[i] = 1;
};


template <class T,int D>
__inline double SIMD_CALL CGenericVector<T,D>::Normalize()
{
	#pragma warning(disable:4244)
	double n = sqrt( (float)(m_vector[0]*m_vector[0] + m_vector[1]*m_vector[1] + m_vector[2]*m_vector[2]) ) ;

	if (n != 0)
	{
		double oneovern = 1.0/n;

		m_vector[0] *= oneovern;
		m_vector[1] *= oneovern;
		m_vector[2] *= oneovern;
		m_vector[3] = 1;
	}

	return n;
	#pragma warning(default:4244)
};


template <class T,int D>
__inline CGenericVector<T,D>& SIMD_CALL CGenericVector<T,D>::operator= ( const CGenericVector<T,D>& v )
{
	for (int i=0;i<D;i++)
		m_vector[i] = v.m_vector[i];
	
	return *this; 
};

template <class T,int D>
__inline CGenericVector<T,D>& SIMD_CALL CGenericVector<T,D>::operator= ( const T v[D] )
{
	for (int i=0;i<D;i++)
		m_vector[i] = v[i];
	
	return *this; 
};

template <class T,int D>
__inline bool SIMD_CALL CGenericVector<T,D>::operator== ( const CGenericVector<T,D>& v ) const
{
	bool res = true;
	for (int i=0;(i<D) && res;i++)
		res = res && (m_vector[i] == v.m_vector[i]);
	return res;
};

template <class T,int D>
__inline bool SIMD_CALL CGenericVector<T,D>::operator== ( const T v[D] ) const
{
	bool res = true;
	for (int i=0;(i<D) && res;i++)
		res = res && (m_vector[i] == v[i]);
	return res;
};

template <class T,int D>
__inline double SIMD_CALL CGenericVector<T,D>::Norm() const
{ 
	return sqrt( (float)(m_vector[0]*m_vector[0] + m_vector[1]*m_vector[1] + m_vector[2]*m_vector[2] ) ); 
};

template <class T,int D>
__inline T SIMD_CALL CGenericVector<T,D>::Length() const NOEXCEPT
{ 
	return (T)(m_vector[0]+m_vector[1]+m_vector[2]+m_vector[3]); 
};


template <class T,int D>
__inline CGenericVector<T,D> SIMD_CALL CGenericVector<T,D>::operator-()
{
	CGenericVector<T,D> v;

	for (int i=0;i<D;i++)
		v.m_vector[i] = -m_vector[i];

	return v; 
};

template <class T,int D>
__inline CGenericVector<T,D>& SIMD_CALL CGenericVector<T,D>::operator!(void)
{
	for (int i=0;i<D;i++)
		m_vector[i] = -m_vector[i];
	
	return *this; 
};


template <class T,int D>
__inline CGenericVector<T,D>&	SIMD_CALL CGenericVector<T,D>::operator-= (const CGenericVector<T,D>& v)
{
	for (int i=0;i<D;i++)
		m_vector[i] -= v.m_vector[i];
	
	return *this; 
};

template <class T,int D>
__inline CGenericVector<T,D>&	SIMD_CALL CGenericVector<T,D>::operator+= (const CGenericVector<T,D>& v)
{
	for (int i=0;i<D;i++)
		m_vector[i] += v.m_vector[i];

	return *this; 
};

template <class T,int D>
__inline CGenericVector<T,D>&	SIMD_CALL CGenericVector<T,D>::operator*= (const CGenericVector<T,D>& v)
{
	for (int i=0;i<D;i++)
		m_vector[i] *= v.m_vector[i];
	
	return *this; 
};

template <class T,int D>
__inline CGenericVector<T,D>&	SIMD_CALL CGenericVector<T,D>::operator*= (const T& t)
{
	for (int i=0;i<D;i++)
		m_vector[i] *= t;
	
	return *this; 
};



template <class T,int D>
__inline CGenericVector<T,D>&	SIMD_CALL CGenericVector<T,D>::operator*= (const CGenericMatrix<T>& m)
{
	float vx = m_vector[0]*m[0]+m_vector[1]*m[1]+m_vector[2]*m[2]+m_vector[3]*m[3];
	float vy = m_vector[0]*m[4]+m_vector[1]*m[5]+m_vector[2]*m[6]+m_vector[3]*m[7];
	float vz = m_vector[0]*m[8]+m_vector[1]*m[9]+m_vector[2]*m[10]+m_vector[3]*m[11];
	m_vector[3] = m_vector[0]*m[12]+m_vector[1]*m[13]+m_vector[2]*m[14]+m_vector[3]*m[15];
    m_vector[2] = vz;
    m_vector[1] = vy;
    m_vector[0] = vx;

	return *this;
};

template <class T,int D>
__inline T SIMD_CALL CGenericVector<T,D>::operator% (const CGenericVector<T,D>& v2) const
{
	//	scalar product operates on 3 coordinates
	return	m_vector[0] * v2.m_vector[0] +
			m_vector[1] * v2.m_vector[1] +
			m_vector[2] * v2.m_vector[2];
			//v1.m_vector[3] * v2.m_vector[3];
};

template <class T,int D>
__inline CGenericVector<T,D> SIMD_CALL CGenericVector<T,D>::operator* (const CGenericVector<T,D>& v2) const
{
	CGenericVector<T,D> v;

	for (int i=0;i<D;i++)
		v.m_vector[i] = m_vector[i] * v2.m_vector[i];

	return v;
};

template <class T,int D>
__inline CGenericVector<T,D> SIMD_CALL CGenericVector<T,D>::operator*  (const CGenericMatrix<T,D>& m) const
{
	CGenericVector<T,D> v;

	v.m_vector[0] = m_vector[0]*m[0]+m_vector[1]*m[4]+m_vector[2]*m[8]+m_vector[3]*m[12];
	v.m_vector[1] = m_vector[0]*m[1]+m_vector[1]*m[5]+m_vector[2]*m[9]+m_vector[3]*m[13];
	v.m_vector[2] = m_vector[0]*m[2]+m_vector[1]*m[6]+m_vector[2]*m[10]+m_vector[3]*m[14];
	v.m_vector[3] = m_vector[0]*m[3]+m_vector[1]*m[7]+m_vector[2]*m[11]+m_vector[3]*m[15];

	return v;
};

template <class T,int D>
__inline CGenericVector<T,D> SIMD_CALL CGenericVector<T,D>::operator*  (T t) const
{
	CGenericVector<T,D> v;

	for (int i=0;i<D;i++)
		v.m_vector[i] = m_vector[i] * t;

	return v;
};

template <class T,int D>
__inline CGenericVector<T,D> SIMD_CALL CGenericVector<T,D>::operator+  (const CGenericVector<T,D>& v2) const
{
	CGenericVector<T,D> v;

	for (int i=0;i<D;i++)
		v.m_vector[i] = m_vector[i] + v2.m_vector[i];

	return v;
};

template <class T,int D>
__inline CGenericVector<T,D> SIMD_CALL CGenericVector<T,D>::operator-  (const CGenericVector<T,D>& v2) const
{
	CGenericVector<T,D> v;

	for (int i=0;i<D;i++)
		v.m_vector[i] = m_vector[i] - v2.m_vector[i];

	return v;
};

template <class T,int D>
__inline CGenericVector<T,D> SIMD_CALL CGenericVector<T,D>::operator^  (const CGenericVector<T,D>& v2) const
{
	CGenericVector<T,D> v;

	v.m_vector[0] = m_vector[1] * v2.m_vector[2] - v2.m_vector[1] * m_vector[2];
	v.m_vector[1] = m_vector[2] * v2.m_vector[0] - v2.m_vector[2] * m_vector[0];
	v.m_vector[2] = m_vector[0] * v2.m_vector[1] - v2.m_vector[0] * m_vector[1];
	v.m_vector[3] = 1;

	return v;
};

//
// input/output
//
template <class T,int D>
__inline std::istream& operator>>(std::istream& i, CGenericVector<T,D>& v)
{
	for (int dim=0;dim<D;dim++)
		i >> v.m_vector[dim];
	return i;
};

template <class T,int D>
__inline std::ostream& operator<<(std::ostream& o, const CGenericVector<T,D>& v)
{
	for (int dim=0;dim<D-1;dim++)
		o<<v.m_vector[dim]<<" ";

	return o<<v.m_vector[D-1];
};


#endif	// __GENERIC_VECTOR_IMPL__

