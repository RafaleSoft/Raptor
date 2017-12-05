/////////////////////////////////////////////////////////////////////////////////////////////
// definition
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GENERIC_MATRIX_IMPL__
#define __GENERIC_MATRIX_IMPL__

#ifndef __GENERIC_MATRIX_H__
	#include "GenericMatrix.h"
#endif


template <class T,int D>
__inline CGenericMatrix<T,D>::CGenericMatrix() 
{
	Ident();
};

template <class T,int D>
__inline CGenericMatrix<T,D>::~CGenericMatrix() 
{
};


/////////////////////////////////////////////////////////////////////////////
// Implementation
/////////////////////////////////////////////////////////////////////////////

template <class T,int D>
__inline void CGenericMatrix<T,D>::Zero() 
{ 
	memset(m_matrix,0,sizeof(T)*D*D);
}

template <class T,int D>
__inline void CGenericMatrix<T,D>::One() 
{ 
	for (int i=0;i<D*D;i++)
		m_matrix[i] = 1;
}

template <class T,int D>
__inline void CGenericMatrix<T,D>::Ident() 
{ 
	memset(m_matrix,0,sizeof(T)*D*D);
	m_matrix[0] = m_matrix[5] = m_matrix[10] = m_matrix[15]=1; 
}

template <class T,int D>
__inline CGenericMatrix<T,D>& SIMD_CALL CGenericMatrix<T,D>::operator= ( const CGenericMatrix<T,D>& m )
{ 
	for (int i=0;i<D*D;i++)
		m_matrix[i] = m.m_matrix[i];
	return *this; 
}

template <class T,int D>
__inline CGenericMatrix<T,D>& SIMD_CALL CGenericMatrix<T,D>::operator= ( const T m[D*D] )
{ 
	for (int i=0;i<D*D;i++)
		m_matrix[i] = m[i];

	return *this; 
}

template <class T,int D>
__inline CGenericMatrix<T,D>& SIMD_CALL CGenericMatrix<T,D>::operator^= ( const CGenericMatrix<T,D>& m )
{ 
	m_matrix[0] = m.m_matrix[0]; m_matrix[4] = m.m_matrix[1]; m_matrix[8] = m.m_matrix[2]; m_matrix[12] = m.m_matrix[3];
	m_matrix[1] = m.m_matrix[4]; m_matrix[5] = m.m_matrix[5]; m_matrix[9] = m.m_matrix[6]; m_matrix[13] = m.m_matrix[7];
	m_matrix[2] = m.m_matrix[8]; m_matrix[6] = m.m_matrix[9]; m_matrix[10] = m.m_matrix[10]; m_matrix[14] = m.m_matrix[11];
	m_matrix[3] = m.m_matrix[12]; m_matrix[7] = m.m_matrix[13]; m_matrix[11] = m.m_matrix[14]; m_matrix[15] = m.m_matrix[15];

	return *this; 
}

template <class T,int D>
__inline CGenericMatrix<T,D>& SIMD_CALL CGenericMatrix<T,D>::operator^= ( const T m[D*D] )
{ 
	m_matrix[0] = m[0]; m_matrix[4] = m[1]; m_matrix[8] = m[2]; m_matrix[12] = m[3];
	m_matrix[1] = m[4]; m_matrix[5] = m[5]; m_matrix[9] = m[6]; m_matrix[13] = m[7];
	m_matrix[2] = m[8]; m_matrix[6] = m[9]; m_matrix[10] = m[10]; m_matrix[14] = m[11];
	m_matrix[3] = m[12]; m_matrix[7] = m[13]; m_matrix[11] = m[14]; m_matrix[15] = m[15];

	return *this; 
}


template <class T,int D>
__inline CGenericMatrix<T,D>& SIMD_CALL CGenericMatrix<T,D>::operator!(void)
{ 
	for (int i=0;i<D*D;i++)
		m_matrix[i]=-m_matrix[i];
	return *this; 
}

template <class T,int D>
__inline CGenericMatrix<T,D>& SIMD_CALL CGenericMatrix<T,D>::Transpose(void)
{ 
	T t;

	t = m_matrix[1]; m_matrix[1] = m_matrix[4]; m_matrix[4] = t;
	t = m_matrix[2]; m_matrix[2] = m_matrix[8]; m_matrix[8] = t;
	t = m_matrix[6]; m_matrix[6] = m_matrix[9]; m_matrix[9] = t;
	t = m_matrix[12]; m_matrix[12] = m_matrix[3]; m_matrix[3] = t;
	t = m_matrix[13]; m_matrix[13] = m_matrix[7]; m_matrix[7] = t;
	t = m_matrix[11]; m_matrix[11] = m_matrix[14]; m_matrix[14] = t;
	
	return *this; 
}

template <class T,int D>
__inline CGenericMatrix<T,D>& SIMD_CALL CGenericMatrix<T,D>::operator-= (const CGenericMatrix<T,D>& m)
{ 
	for (int i=0;i<D*D;i++)
		m_matrix[i]-=m.m_matrix[i];
	return *this; 
}

template <class T,int D>
__inline CGenericMatrix<T,D>& SIMD_CALL CGenericMatrix<T,D>::operator+= (const CGenericMatrix<T,D>& m)
{ 
	for (int i=0;i<D*D;i++)
		m_matrix[i]+=m.m_matrix[i];
	return *this; 
}

template <class T,int D>
__inline CGenericMatrix<T,D>& SIMD_CALL CGenericMatrix<T,D>::operator*= (const CGenericMatrix<T,D>& m2)
{
	for (int j=0 ; j<D*D ; j+=D)
	{
		T t0 = m_matrix[j];
		T t1 = m_matrix[j+1];
		T t2 = m_matrix[j+2];
		T t3 = m_matrix[j+3];

        m_matrix[j] = t0 * m2.m_matrix[0] + t1 * m2.m_matrix[4] + t2 * m2.m_matrix[8] + t3 * m2.m_matrix[12];
        m_matrix[j+1] = t0 * m2.m_matrix[1] + t1 * m2.m_matrix[5] + t2 * m2.m_matrix[9] + t3 * m2.m_matrix[13];
        m_matrix[j+2] = t0 * m2.m_matrix[2] + t1 * m2.m_matrix[6] + t2 * m2.m_matrix[10] + t3 * m2.m_matrix[14];
        m_matrix[j+3] = t0 * m2.m_matrix[3] + t1 * m2.m_matrix[7] + t2 * m2.m_matrix[11] + t3 * m2.m_matrix[15];
	}

	return *this;
}

template <class T,int D>
__inline CGenericMatrix<T,D>& SIMD_CALL CGenericMatrix<T,D>::operator*= (T t)
{
	for (int i=0;i<D*D;i++)
		m_matrix[i]*=t;
	return *this;
}

template <class T,int D>
__inline CGenericMatrix<T,D> SIMD_CALL CGenericMatrix<T,D>::operator*  (const CGenericMatrix<T,D>& m2) const
{
	CGenericMatrix<T,D> m;

	for (int j=0;j<D;j++)
	{
		T t0 = m2.m_matrix[j];
		T t1 = m2.m_matrix[j+4];
		T t2 = m2.m_matrix[j+8];
		T t3 = m2.m_matrix[j+12];

		for (int i=0;i<D*D;i+=D)
		{
			m.m_matrix[j+i] =	m_matrix[i] * t0 +
							    m_matrix[i+1] * t1 +
							    m_matrix[i+2] * t2 +
							    m_matrix[i+3] * t3;
		}
	}
	return m;
}

template <class T,int D>
__inline CGenericVector<T,D> SIMD_CALL CGenericMatrix<T,D>::operator*  (const CGenericVector<T,D>& v1) const
{
	CGenericVector<T,D> v;

	v[0] = v1[0]*m_matrix[0]+v1[1]*m_matrix[1]+v1[2]*m_matrix[2]+v1[3]*m_matrix[3];
	v[1] = v1[0]*m_matrix[4]+v1[1]*m_matrix[5]+v1[2]*m_matrix[6]+v1[3]*m_matrix[7];
	v[2] = v1[0]*m_matrix[8]+v1[1]*m_matrix[9]+v1[2]*m_matrix[10]+v1[3]*m_matrix[11];
	v[3] = v1[0]*m_matrix[12]+v1[1]*m_matrix[13]+v1[2]*m_matrix[14]+v1[3]*m_matrix[15];

	return v;
};

template <class T,int D>
__inline CGenericMatrix<T,D> SIMD_CALL CGenericMatrix<T,D>::operator*  (T t) const
{
	CGenericMatrix<T,D> m;

	for (int i=0;i<D*D;i++)
		m.m_matrix[i] = m_matrix[i]*t;
	return m; 
}

template <class T,int D>
__inline CGenericMatrix<T,D> SIMD_CALL CGenericMatrix<T,D>::operator+  (const CGenericMatrix<T,D>& m2) const
{
	CGenericMatrix<T,D> m;

	for (int i=0;i<D*D;i++)
		m.m_matrix[i] = m_matrix[i] + m2.m_matrix[i];
	return m; 
}

template <class T,int D>
__inline CGenericMatrix<T,D> SIMD_CALL CGenericMatrix<T,D>::operator-  (const CGenericMatrix<T,D>& m2) const
{
	CGenericMatrix<T,D> m;

	for (int i=0;i<D*D;i++)
		m.m_matrix[i] = m_matrix[i] - m2.m_matrix[i];
	return m; 
}


#define DET2(a,b)\
	(m_matrix[a]*m_matrix[b+D]-m_matrix[b]*m_matrix[a+D])

#define DET3(a,b,c)\
	(m_matrix[a] * DET2(b+D,c+D) -\
	m_matrix[b] * DET2(a+D,c+D) +\
	m_matrix[c] * DET2(a+D,b+D))

template <class T,int D>
__inline T SIMD_CALL CGenericMatrix<T,D>::Det(void)
{
	return	m_matrix[0]*DET3(5,6,7) -
			m_matrix[1]*DET3(4,6,7) +
			m_matrix[2]*DET3(4,5,7) -
			m_matrix[3]*DET3(4,5,6);
}	

template <class T,int D>
__inline CGenericMatrix<T,D>& CGenericMatrix<T,D>::Inv(void)
{
	float * const m = m_matrix;

	float inv[D*D];
    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det != 0)
	{
		det = 1.0 / det;
	    for (int i = 0; i < 16; i++)
			m[i] = inv[i] * det;
	}

	return *this;
}

//
// input/output
//
template <class T,int D>
std::istream& operator>> (std::istream& i, CGenericMatrix<T,D>& m)
{
	for (int k=0;k<D;k++)
		for (int j=0;j<D;j++)
			i >> m.m_matrix[j+k*D];
	return i;
};

template <class T,int D>
std::ostream& operator<< (std::ostream& o, const CGenericMatrix<T,D>& m)
{
	for (int k=0;k<D-1;k++)
	{
		for (int j=0;j<D-1;j++)
			o << m.m_matrix[j+k*D] << " ";
		o << m.m_matrix[D-1+k*D] <<std::endl;
	}
	for (int j=0;j<D-1;j++)
		o << m.m_matrix[j+(D-1)*D] << " ";
	o << m.m_matrix[D-1+(D-1)*D];
	return o;
};

#endif

