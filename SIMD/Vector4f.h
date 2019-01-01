#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __VECTOR_4F_H__
#define __VECTOR_4F_H__

#ifndef __GENERIC_VECTOR_H__
	#include "GenericVector.h"
#endif

#ifndef __MATRIX_4F_H__
	#include "Matrix4f.h"
#endif

#pragma pack(push,16)
class CVector4f //: public CGenericVector<float,4>
{
#if 0
	#define M_VECTOR *((__m128*)&m_vector)
	#define VECTOR(v) *((__m128*)&v.m_vector)
#else
	#define M_VECTOR m_vector
	#define VECTOR(v) v.m_vector
#endif

public:
	CVector4f(void)
	{
		// can we avoid initialising homogeneous coordinate ?
		M_VECTOR = _mm_setzero_ps();
	};
	CVector4f(const CVector4f& v)
	{ M_VECTOR = VECTOR(v); };
	CVector4f(const float v[4])
	{ M_VECTOR = _mm_load_ps(v); };
	CVector4f(float v0,float v1,float v2,float v3)
	{ M_VECTOR = _mm_set_ps(v0,v1,v2,v3); };
	CVector4f &CVector4f::operator=(const CVector4f &v)
	{ M_VECTOR = VECTOR(v); return *this; };
	CVector4f &CVector4f::operator=(const CGenericVector<float,4> &v)
	{ M_VECTOR = _mm_load_ps(v.vector()); return *this; };
	virtual ~CVector4f(void) {};

	const float* vector() const { return m_vector.m128_f32; };
	virtual void Set(float v0,float v1,float v2,float v3)
	{ M_VECTOR = _mm_set_ps(v0,v1,v2,v3); }

	float& SIMD_CALL X() { return m_vector.m128_f32[0]; };
	float& SIMD_CALL Y() { return m_vector.m128_f32[1]; };
	float& SIMD_CALL Z() { return m_vector.m128_f32[2]; };
	float& SIMD_CALL H() { return m_vector.m128_f32[3]; };

	const float& SIMD_CALL X() const { return m_vector.m128_f32[0]; };
	const float& SIMD_CALL Y() const { return m_vector.m128_f32[1]; };
	const float& SIMD_CALL Z() const { return m_vector.m128_f32[2]; };
	const float& SIMD_CALL H() const { return m_vector.m128_f32[3]; };

	void X(float t) { m_vector.m128_f32[0] = t; };
	void Y(float t) { m_vector.m128_f32[1] = t; };
	void Z(float t) { m_vector.m128_f32[2] = t; };
	void H(float t) { m_vector.m128_f32[3] = t; };


	virtual float SIMD_CALL Length() const
	{ return (m_vector.m128_f32[0]+m_vector.m128_f32[1]+m_vector.m128_f32[2]+m_vector.m128_f32[3]); }

	virtual double SIMD_CALL Normalize()
	{
		double n = Norm();

		if (n != 0)
		{
			float oneovern = 1.0f/(float)n;

			m_vector.m128_f32[0] *= oneovern;
			m_vector.m128_f32[1] *= oneovern;
			m_vector.m128_f32[2] *= oneovern;
			m_vector.m128_f32[3] = 1;
		}

		return n;
	}

	virtual double SIMD_CALL Norm() const
	{ 
		return sqrt(m_vector.m128_f32[0]*m_vector.m128_f32[0] + 
					m_vector.m128_f32[1]*m_vector.m128_f32[1] + 
					m_vector.m128_f32[2]*m_vector.m128_f32[2]); 
	}

	// dot product
	float SIMD_CALL operator% (const CVector4f&) const;

	// scalar operations
    CVector4f SIMD_CALL operator*(const CVector4f& v) const
	{
		CVector4f res;
		VECTOR(res) = _mm_mul_ps(M_VECTOR,VECTOR(v));
		return res;
	}

	CVector4f SIMD_CALL operator*=(const CVector4f& v)
	{
		M_VECTOR = _mm_mul_ps(M_VECTOR,VECTOR(v));
		return *this;
	}

	CVector4f SIMD_CALL operator+(const CVector4f& v) const
	{
		CVector4f res;
		VECTOR(res) = _mm_add_ps(M_VECTOR,VECTOR(v));
		return res;
	}

	CVector4f& SIMD_CALL operator+=(const CVector4f& v)
	{
		M_VECTOR  = _mm_add_ps(M_VECTOR,VECTOR(v));
		return *this;
	}

	CVector4f SIMD_CALL operator-(const CVector4f& v) const
	{
		CVector4f res;
		VECTOR(res) = _mm_sub_ps(M_VECTOR,VECTOR(v));
		return res;
	}

	CVector4f& SIMD_CALL operator-=(const CVector4f& v)
	{
		M_VECTOR = _mm_sub_ps(M_VECTOR,VECTOR(v));
		return *this;
	}

	CVector4f SIMD_CALL operator*(float t) const
	{
		CVector4f v;
		__m128 T = _mm_load_ss(&t);
		VECTOR(v) = _mm_mul_ps(_mm_shuffle_ps(T,T,0),M_VECTOR);
		return v;
	}

	CVector4f& SIMD_CALL operator*=(float t)
	{
		__m128 T = _mm_load_ss(&t);
		M_VECTOR = _mm_mul_ps(_mm_shuffle_ps(T,T,0),M_VECTOR);
		return *this;
	}

	CVector4f SIMD_CALL operator^(const CVector4f& v) const;

	// Matrix operations
	CVector4f& SIMD_CALL operator*= (const CMatrix4f& m);
	CVector4f& SIMD_CALL operator*= (const CGenericMatrix<float,4>& m);

protected:
	friend class CMatrix4f;
	__m128 m_vector;
};
#pragma pack(pop)

#endif
