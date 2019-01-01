/////////////////////////////////////////////////////////////////////////////////////////////
// definition
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __VECTOR_4F_IMPL__
#define __VECTOR_4F_IMPL__

#ifndef __VECTOR_4F_H__
	#include "Vector4f.h"
#endif


__inline CVector4f SIMD_CALL CVector4f::operator^(const CVector4f& v) const
{
	CVector4f res(
					m_vector.m128_f32[1] * v.m_vector.m128_f32[2] - v.m_vector.m128_f32[1] * m_vector.m128_f32[2],
					m_vector.m128_f32[2] * v.m_vector.m128_f32[0] - v.m_vector.m128_f32[2] * m_vector.m128_f32[0],
					m_vector.m128_f32[0] * v.m_vector.m128_f32[1] - v.m_vector.m128_f32[0] * m_vector.m128_f32[1],
					1);
	return res;
}

__inline float SIMD_CALL CVector4f::operator%(const CVector4f& v2) const
{
	//	scalar product operates on 3 coordinates
	return	m_vector.m128_f32[0] * v2.m_vector.m128_f32[0] +
			m_vector.m128_f32[1] * v2.m_vector.m128_f32[1] +
			m_vector.m128_f32[2] * v2.m_vector.m128_f32[2];
}

__inline CVector4f& SIMD_CALL CVector4f::operator*= (const CMatrix4f& m)
{
	__m128 vx = _mm_mul_ps(M_VECTOR,MATRIX(m)[0]);
	__m128 vy = _mm_mul_ps(M_VECTOR,MATRIX(m)[1]);
	__m128 vz = _mm_mul_ps(M_VECTOR,MATRIX(m)[2]);
	__m128 vh = _mm_mul_ps(M_VECTOR,MATRIX(m)[3]);

	m_vector.m128_f32[3] = vh.m128_f32[0]+vh.m128_f32[1]+vh.m128_f32[2]+vh.m128_f32[3];
	m_vector.m128_f32[2] = vz.m128_f32[0]+vz.m128_f32[1]+vz.m128_f32[2]+vz.m128_f32[3];
	m_vector.m128_f32[1] = vy.m128_f32[0]+vy.m128_f32[1]+vy.m128_f32[2]+vy.m128_f32[3];
	m_vector.m128_f32[0] = vx.m128_f32[0]+vx.m128_f32[1]+vx.m128_f32[2]+vx.m128_f32[3];

	return *this;
}

__inline CVector4f& SIMD_CALL CVector4f::operator*= (const CGenericMatrix<float,4>& m)
{
	__m128 vx = _mm_mul_ps(M_VECTOR,_mm_load_ps(&m.matrix()[0]));
	__m128 vy = _mm_mul_ps(M_VECTOR,_mm_load_ps(&m.matrix()[4]));
	__m128 vz = _mm_mul_ps(M_VECTOR,_mm_load_ps(&m.matrix()[8]));
	__m128 vh = _mm_mul_ps(M_VECTOR,_mm_load_ps(&m.matrix()[0]));

	m_vector.m128_f32[3] = vh.m128_f32[0]+vh.m128_f32[1]+vh.m128_f32[2]+vh.m128_f32[3];
	m_vector.m128_f32[2] = vz.m128_f32[0]+vz.m128_f32[1]+vz.m128_f32[2]+vz.m128_f32[3];
	m_vector.m128_f32[1] = vy.m128_f32[0]+vy.m128_f32[1]+vy.m128_f32[2]+vy.m128_f32[3];
	m_vector.m128_f32[0] = vx.m128_f32[0]+vx.m128_f32[1]+vx.m128_f32[2]+vx.m128_f32[3];
	
	return *this;
}

#endif