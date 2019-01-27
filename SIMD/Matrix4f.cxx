/***************************************************************************/
/*                                                                         */
/*  Matrix4f.cxx                                                           */
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


#ifndef __MATRIX_4F_IMPL__
#define __MATRIX_4F_IMPL__

#ifndef __MATRIX_4F_H__
	#include "Matrix4f.h"
#endif


__inline CVector4f SIMD_CALL CMatrix4f::operator*(const CVector4f& v) const
{
	__m128 row0 = _mm_mul_ps(VECTOR(v),M_MATRIX[0]);
	__m128 row1 = _mm_mul_ps(VECTOR(v),M_MATRIX[1]);
	__m128 row2 = _mm_mul_ps(VECTOR(v),M_MATRIX[2]);
	__m128 row3 = _mm_mul_ps(VECTOR(v),M_MATRIX[3]);

	CVector4f res(
		row0.m128_f32[0]+row0.m128_f32[1]+row0.m128_f32[2]+row0.m128_f32[3],
		row1.m128_f32[0]+row1.m128_f32[1]+row1.m128_f32[2]+row1.m128_f32[3],
		row2.m128_f32[0]+row2.m128_f32[1]+row2.m128_f32[2]+row2.m128_f32[3],
		row3.m128_f32[0]+row3.m128_f32[1]+row3.m128_f32[2]+row3.m128_f32[3]);
				
	return res;
}

#endif	// __MATRIX_4F_IMPL__