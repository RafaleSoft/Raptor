/***************************************************************************/
/*                                                                         */
/*  Matrix4f.h                                                             */
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



#ifndef __MATRIX_4F_H__
#define __MATRIX_4F_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __GENERIC_MATRIX_H__
	#include "GenericMatrix.h"	// for linear systems
#endif

#pragma pack(push,16)
class CVector4f;
class CMatrix4f : public CGenericMatrix<float,4>
{
#define M_MATRIX ((__m128*)&m_matrix[0])
#define MATRIX(m) ((__m128*)&m.m_matrix[0])
public:
	CMatrix4f(void) NOEXCEPT { };
	CMatrix4f(const float m[16])
	{
		M_MATRIX[0] = _mm_load_ps(&m[0]);
		M_MATRIX[1] = _mm_load_ps(&m[4]);
		M_MATRIX[2] = _mm_load_ps(&m[8]);
		M_MATRIX[3] = _mm_load_ps(&m[12]);
	};
	CMatrix4f& SIMD_CALL operator=( const float m[16] ) NOEXCEPT
	{
		M_MATRIX[0] = _mm_load_ps(&m[0]);
		M_MATRIX[1] = _mm_load_ps(&m[4]);
		M_MATRIX[2] = _mm_load_ps(&m[8]);
		M_MATRIX[3] = _mm_load_ps(&m[12]);
		return *this;
	};
	virtual ~CMatrix4f(void) {};

	// Scalar operations
	CVector4f SIMD_CALL operator*  (const CVector4f& v) const;


protected:
	friend class CVector4f;
	//__m128 m_matrix[4];
};


#pragma pack(pop)
#endif // __MATRIX_4F_H__
