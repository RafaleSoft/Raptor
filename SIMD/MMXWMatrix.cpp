/***************************************************************************/
/*                                                                         */
/*  MMXWMatrix.cpp                                                         */
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



#if !defined(AFX_MMXWMATRIX_H__6CB7BBD5_103F_4C28_AA90_88AA8AD9E466__INCLUDED_)
	#include "MMXWMatrix.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#if (!defined(SIMD_NO_ASSEMBLY) && !defined(_WIN64))

CMMXWMatrix&	SIMD_CALL CMMXWMatrix::operator*= (const CMMXWMatrix& m)
{
	__asm
	{
		mov esi,m
		mov edi,this		
		add esi,4
		add edi,4
			
		movq mm4,[esi]
		movq mm5,[esi+8]
		movq mm6,[esi+16]
		movq mm7,[esi+24]
		mov ecx,4
row:
		movq mm0,[edi]
		movq mm1,[edi]
		punpcklwd mm0,[edi]
		punpckhwd mm1,[edi]
		movq mm2,mm0
		movq mm3,mm1
		punpcklwd mm0,mm2	// mm0 = m[0]
		punpckhwd mm2,mm2	// mm2 = m[1]
		punpcklwd mm1,mm3	// mm1 = m[2]
		punpckhwd mm3,mm3	// mm3 = m[3]

		pmullw mm0,mm4
		pmullw mm2,mm5
		paddsw mm0,mm2
		pmullw mm1,mm6
		pmullw mm3,mm7
		paddsw mm1,mm3
		paddsw mm0,mm1
		movq [edi],mm0

		add edi,8
		dec ecx
		jnz row

		emms
	}
	return *this;
}

#endif	// SIMD_NO_ASSEMBLY