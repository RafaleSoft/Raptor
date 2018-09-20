/////////////////////////////////////////////////////////////////////////////////////////////
// definition
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GENERIC_ALIGNED_VECTOR_IMPL__
#define __GENERIC_ALIGNED_VECTOR_IMPL__

#ifndef __GENERIC_ALIGNED_VECTOR_H__
	#include "GenericAlignedVector.h"
#endif



	//		16 bytes more space is allocated,
	//		to store alignment and to store align offset
	//	char *pT = new char[4*sizeof(T)+16]; 
	//		align data
	//	m_vector = (T*)((int(pT)+0x00000010) & 0xfffffff0);
	//		store offset
	//	*((char*)m_vector-1)=char(int(pT)&0xf);

#define ALLOC_VECTOR(T) \
{\
char *pT = new char[4*sizeof(T)+16]; \
m_vector = (T*)((long(pT)+0x00000010) & 0xfffffff0);\
*((char*)m_vector-1)=char(long(pT)&0xf);\
}



//	Vector is aligned to a 16 bytes boundery
template <class T>
CGenericAlignedVector<T>::CGenericAlignedVector()
{ 
	ALLOC_VECTOR(T)
}

//	Copy constructor, because of pointer
template <class T>
CGenericAlignedVector<T>::CGenericAlignedVector(const CGenericAlignedVector& rsh)
{
	ALLOC_VECTOR(T)
	m_vector[0] = rsh[0]; 
	m_vector[1] = rsh[1]; 
	m_vector[2] = rsh[2]; 
	m_vector[3] = rsh[3];
}

//	Vector is aligned to a 16 bytes boundery
template <class T>
CGenericAlignedVector<T>::CGenericAlignedVector(const T *v)
{ 
	ALLOC_VECTOR(T)

	m_vector[0] = v[0]; 
	m_vector[1] = v[1]; 
	m_vector[2] = v[2]; 
	m_vector[3] = v[3];
}

//	Vector is aligned to a 16 bytes boundery
template <class T>
CGenericAlignedVector<T>::CGenericAlignedVector(T v0,T v1,T v2,T v3)
{ 
	ALLOC_VECTOR(T)

	m_vector[0] = v0; 
	m_vector[1] = v1; 
	m_vector[2] = v2; 
	m_vector[3] = v3;
}
	
template <class T>
CGenericAlignedVector<T>::~CGenericAlignedVector()
{
	//	delete exact allocated space
	char *pT = (char*)m_vector;
	pT-=*(pT-1);
	//	translation offset in case that
	//	m_vector has initially been allocated
	//	on a 16bytes boundery
	if (pT==(char*)m_vector)
		pT-=16;
	delete [] pT;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// implementation
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
__inline CGenericAlignedVector<T>& SIMD_CALL CGenericAlignedVector<T>::operator= ( const CGenericAlignedVector<T>& v )
{ 
	m_vector[0] = v.m_vector[0]; 
	m_vector[1] = v.m_vector[1]; 
	m_vector[2] = v.m_vector[2]; 
	m_vector[3] = v.m_vector[3]; 
	
	return *this; 
};

template <class T>
__inline CGenericAlignedVector<T>& SIMD_CALL CGenericAlignedVector<T>::operator= ( const T v[4] )
{ 
	m_vector[0] = v[0]; 
	m_vector[1] = v[1]; 
	m_vector[2] = v[2]; 
	m_vector[3] = v[3]; 
	
	return *this; 
};

template <class T>
__inline bool SIMD_CALL CGenericAlignedVector<T>::operator== ( const CGenericAlignedVector<T>& v ) const
{ 
	return (
			(m_vector[0] == v.m_vector[0]) && 
			(m_vector[1] == v.m_vector[1]) && 
			(m_vector[2] == v.m_vector[2]) &&
			(m_vector[3]==v.m_vector[3])
			);
};

template <class T>
__inline bool SIMD_CALL CGenericAlignedVector<T>::operator== ( const T v[4] ) const
{ 
	return (
			(m_vector[0] == v[0]) &&
			(m_vector[1] == v[1]) &&
			(m_vector[2] == v[2]) &&
			(m_vector[3] == v[3])
			); 
};

template <class T>
__inline double SIMD_CALL CGenericAlignedVector<T>::Norm() const
{ 
	return sqrt( m_vector[0]*m_vector[0] + m_vector[1]*m_vector[1] + m_vector[2]*m_vector[2] ) ; 
};

template <class T>
__inline double SIMD_CALL CGenericAlignedVector<T>::Normalize()
{
	#pragma warning(disable:4244)
	double n = sqrt( m_vector[0]*m_vector[0] + m_vector[1]*m_vector[1] + m_vector[2]*m_vector[2] ) ;

	if (n != 0)
	{
		double oneovern = 1.0/n;
		m_vector[0] *= oneovern;
		m_vector[1] *= oneovern;
		m_vector[2] *= oneovern;
	}

	return n;
	#pragma warning(default:4244)
};

template <class T>
__inline T SIMD_CALL CGenericAlignedVector<T>::Length() const
{ 
	return (T)(m_vector[0]+m_vector[1]+m_vector[2]+m_vector[3]); 
};


template <class T>
__inline CGenericAlignedVector<T>& SIMD_CALL CGenericAlignedVector<T>::operator!(void)
{ 
	m_vector[0] = -m_vector[0];
	m_vector[1] = -m_vector[1];
	m_vector[2] = -m_vector[2];
	m_vector[3] = -m_vector[3];
	
	return *this; 
};

template <class T>
__inline CGenericAlignedVector<T> SIMD_CALL CGenericAlignedVector<T>::operator-()
{	
	CGenericAlignedVector<T> v;

	v.m_vector[0] = -m_vector[0];
	v.m_vector[1] = -m_vector[1];
	v.m_vector[2] = -m_vector[2];
	v.m_vector[3] = -m_vector[3];

	return v; 
};

template <class T>
__inline CGenericAlignedVector<T>&	SIMD_CALL CGenericAlignedVector<T>::operator-= (const CGenericAlignedVector<T>& v)
{ 
	m_vector[0] -= (T)(v.m_vector[0]);
	m_vector[1] -= (T)(v.m_vector[1]);
	m_vector[2] -= (T)(v.m_vector[2]);
	m_vector[3] -= (T)(v.m_vector[3]);
	
	return *this; 
};

template <class T>
__inline CGenericAlignedVector<T>&	SIMD_CALL CGenericAlignedVector<T>::operator+= (const CGenericAlignedVector<T>& v)
{ 
#pragma warning (disable : 4244)	// conversion from 'int' to 'unsigned short', possible loss of data
	m_vector[0] += (T)(v.m_vector[0]);
	m_vector[1] += (T)(v.m_vector[1]);
	m_vector[2] += (T)(v.m_vector[2]);
	m_vector[3] += (T)(v.m_vector[3]);
	
	return *this; 
};

template <class T>
__inline CGenericAlignedVector<T>&	SIMD_CALL CGenericAlignedVector<T>::operator*= (const CGenericAlignedVector<T>& v)
{ 
	m_vector[0] *= (T)(v.m_vector[0]);
	m_vector[1] *= (T)(v.m_vector[1]);
	m_vector[2] *= (T)(v.m_vector[2]);
	m_vector[3] *= (T)(v.m_vector[3]);
	
	return *this; 
};

template <class T>
__inline CGenericAlignedVector<T>&	SIMD_CALL CGenericAlignedVector<T>::operator*= (const T& t)
{ 
	m_vector[0] *= t;
	m_vector[1] *= t;
	m_vector[2] *= t;
	m_vector[3] *= t;
	
	return *this; 
};


template <class T>
__inline CGenericAlignedVector<T>&	SIMD_CALL CGenericAlignedVector<T>::operator*= (const CGenericAlignedMatrix<T>& m)
{
	CGenericAlignedVector<T> v = *this;

	m_vector[0] = v.m_vector[0]*m.matrix[0]+v.m_vector[1]*m.matrix[1]+v.m_vector[2]*m.matrix[2]+v.m_vector[3]*m.matrix[3];
	m_vector[1] = v.m_vector[0]*m.matrix[4]+v.m_vector[1]*m.matrix[5]+v.m_vector[2]*m.matrix[6]+v.m_vector[3]*m.matrix[7];
	m_vector[2] = v.m_vector[0]*m.matrix[8]+v.m_vector[1]*m.matrix[9]+v.m_vector[2]*m.matrix[10]+v.m_vector[3]*m.matrix[11];
	m_vector[3] = v.m_vector[0]*m.matrix[12]+v.m_vector[1]*m.matrix[13]+v.m_vector[2]*m.matrix[14]+v.m_vector[3]*m.matrix[15];
	return *this;
};

template <class T>
__inline CGenericAlignedVector<T> SIMD_CALL CGenericAlignedVector<T>::operator*  (const CGenericAlignedVector<T>& v2) const
{
	CGenericAlignedVector<T> v;

	v.m_vector[0] = m_vector[0] * v2.m_vector[0];
	v.m_vector[1] = m_vector[1] * v2.m_vector[1];
	v.m_vector[2] = m_vector[2] * v2.m_vector[2];
	v.m_vector[3] = m_vector[3] * v2.m_vector[3];

	return v;
}
/*
template <class T>
__inline CGenericAlignedVector<T> SIMD_CALL operator*  (const T& t, const CGenericAlignedVector<T>& v1)
{
	CGenericAlignedVector<T> v;

	v.m_vector[0] = v1.m_vector[0] * t;
	v.m_vector[1] = v1.m_vector[1] * t;
	v.m_vector[2] = v1.m_vector[2] * t;
	v.m_vector[3] = v1.m_vector[3] * t;

	return v;
}
*/

template <class T>
__inline CGenericAlignedVector<T> SIMD_CALL CGenericAlignedVector<T>::operator*  (const CGenericAlignedMatrix<T>& m) const
{
	CGenericAlignedVector<T> v;

	v.m_vector[0] = m_vector[0]*m.matrix[0]+m_vector[1]*m.matrix[4]+m_vector[2]*m.matrix[8]+m_vector[3]*m.matrix[12];
	v.m_vector[1] = m_vector[0]*m.matrix[1]+m_vector[1]*m.matrix[5]+m_vector[2]*m.matrix[9]+m_vector[3]*m.matrix[13];
	v.m_vector[2] = m_vector[0]*m.matrix[2]+m_vector[1]*m.matrix[6]+m_vector[2]*m.matrix[10]+m_vector[3]*m.matrix[14];
	v.m_vector[3] = m_vector[0]*m.matrix[3]+m_vector[1]*m.matrix[7]+m_vector[2]*m.matrix[11]+m_vector[3]*m.matrix[15];

	return v;
}

template <class T>
__inline CGenericAlignedVector<T> SIMD_CALL CGenericAlignedVector<T>::operator*  (T t) const
{
	CGenericAlignedVector<T> v;

	v.m_vector[0] = m_vector[0] * t;
	v.m_vector[1] = m_vector[1] * t;
	v.m_vector[2] = m_vector[2] * t;
	v.m_vector[3] = m_vector[3] * t;

	return v;
}

template <class T>
__inline CGenericAlignedVector<T> SIMD_CALL CGenericAlignedVector<T>::operator+  (const CGenericAlignedVector<T>& v2) const
{
	CGenericAlignedVector<T> v;

	v.m_vector[0] = m_vector[0] + v2.m_vector[0];
	v.m_vector[1] = m_vector[1] + v2.m_vector[1];
	v.m_vector[2] = m_vector[2] + v2.m_vector[2];
	v.m_vector[3] = m_vector[3] + v2.m_vector[3];

	return v;
}

template <class T>
__inline CGenericAlignedVector<T> SIMD_CALL CGenericAlignedVector<T>::operator-  (const CGenericAlignedVector<T>& v2) const
{
	CGenericAlignedVector<T> v;

	v.m_vector[0] = m_vector[0] - v2.m_vector[0];
	v.m_vector[1] = m_vector[1] - v2.m_vector[1];
	v.m_vector[2] = m_vector[2] - v2.m_vector[2];
	v.m_vector[3] = m_vector[3] - v2.m_vector[3];

	return v;
}

template <class T>
__inline CGenericAlignedVector<T> SIMD_CALL CGenericAlignedVector<T>::operator^  (const CGenericAlignedVector<T>& v2) const
{
	CGenericAlignedVector<T> v;

	v.m_vector[0] = m_vector[1] * v2.m_vector[2] - v2.m_vector[1] * m_vector[2];
	v.m_vector[1] = m_vector[2] * v2.m_vector[0] - v2.m_vector[2] * m_vector[0];
	v.m_vector[2] = m_vector[0] * v2.m_vector[1] - v2.m_vector[0] * m_vector[1];
	v.m_vector[3] = 1;

	return v;
}


#endif

