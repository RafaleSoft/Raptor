/***************************************************************************/
/*                                                                         */
/*  MMXWVector.cpp                                                         */
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



#if !defined(AFX_MMXWVector_H__EDBB336A_5AF7_4AF0_9000_1F0F0CE7C0C0__INCLUDED_)
	#include "MMXWVector.h"
#endif

#ifndef SIMD_NO_ASSEMBLY

extern CMMXWVector _unalignedMMXShortVector;

CMMXWVector& SIMD_CALL CMMXWVector::operator^  (const CMMXWVector& v2) const NOEXCEPT
{
	__asm
	{
		mov esi,this
		mov edi,v2
		_emit 0x0F	//pshufw mm0,[esi+4],0xC9 : 11 00 10 01
		_emit 0x70
		_emit 0x46
		_emit 0x04
		_emit 0xC9	// shuf y z x h
		_emit 0x0F	//pshufw mm1,[edi+4],0xD2 : 11 01 00 10
		_emit 0x70
		_emit 0x4F
		_emit 0x04
		_emit 0xD2	// shuf z' x' y' h'
		pmullw mm0,mm1
		_emit 0x0F	//pshufw mm2,[esi+4],0xD2 : 11 01 00 10
		_emit 0x70
		_emit 0x56
		_emit 0x04
		_emit 0xD2	// shuf z x y h
		lea esi,_unalignedMMXShortVector;
		_emit 0x0F	//pshufw mm3,[edi+4],0xC9 : 11 00 10 01
		_emit 0x70
		_emit 0x5F
		_emit 0x04
		_emit 0xC9	// shuf y' z' x' h'
		pmullw mm2,mm3
		psubw mm0,mm2
		movq [esi+4],mm0
		emms
	}
	return _unalignedMMXShortVector;
};

CMMXWVector& SIMD_CALL CMMXWVector::operator*  (const CMMXWVector& v2) const NOEXCEPT
{
	__asm
	{
		mov esi,this
		mov edi,v2
		movq mm0,[esi+4]
		movq mm1,[edi+4]
		pmullw mm0,mm1
		lea edi,_unalignedMMXShortVector;
		movq [edi+4],mm0
		emms
	}
	return _unalignedMMXShortVector;
};

CMMXWVector& SIMD_CALL CMMXWVector::operator*  (const CMMXWMatrix& m) const NOEXCEPT
{
	int t[2];
	_asm
	{
		mov edi,m
		mov esi,this
		add edi,4
		add esi,4
		movq mm1,[edi]
		movq mm0,[esi]
		add edi,8
		pmaddwd mm1,mm0
		lea edx,t
		lea ebx,_unalignedMMXShortVector
		movq [edx],mm1
		add ebx,4
		movq mm2,[edi]
		mov eax,[edx]
		pmaddwd mm2,mm0
		add edi,8
		add eax,[edx+4]
		movq mm1,[edi]
		mov [ebx],ax	// v[0] ok
		movq [edx],mm2
		add edi,8
		pmaddwd mm1,mm0
		mov eax,[edx]
		add ebx,2
		movq mm2,[edi]
		add eax,[edx+4]
		movq [edx],mm1
		mov [ebx],ax	// v[1] ok
		pmaddwd mm2,mm0
		mov eax,[edx]
		add ebx,2
		add eax,[edx+4]
		movq [edx],mm2
		mov [ebx],ax	// v[2] ok
		mov eax,[edx]
		add eax,[edx+4]
		add ebx,2
		mov [ebx],ax	// v[3] ok
		emms
	}
	return _unalignedMMXShortVector;
};

CMMXWVector& SIMD_CALL operator*  (const CMMXWVector& v, const CMMXWMatrix& m)
{
	_asm
	{
		mov esi,m
		mov edi,v
		add edi,4
		add esi,4
			
		movq mm4,[esi]
		movq mm5,[esi+8]
		movq mm6,[esi+16]
		movq mm7,[esi+24]

		movq mm0,[edi]
		movq mm1,[edi]
		punpcklwd mm0,[edi]
		punpckhwd mm1,[edi]
		movq mm2,mm0
		movq mm3,mm1
		punpcklwd mm0,mm2	// mm0 = v[0]
		punpckhwd mm2,mm2	// mm2 = v[1]
		punpcklwd mm1,mm3	// mm1 = v[2]
		punpckhwd mm3,mm3	// mm3 = v[3]

		lea edi,_unalignedMMXShortVector
		pmullw mm0,mm4
		pmullw mm2,mm5
		paddsw mm0,mm2
		pmullw mm1,mm6
		pmullw mm3,mm7
		paddsw mm1,mm3
		paddsw mm0,mm1
		movq [edi+4],mm0

		emms
	}
	return _unalignedMMXShortVector;
}

CMMXWVector& SIMD_CALL CMMXWVector::operator+  (const CMMXWVector& v2) const NOEXCEPT
{
	__asm
	{
		mov esi,this
		mov edi,v2
		movq mm0,[esi+4]
		movq mm1,[edi+4]
		paddsw mm0,mm1
		lea edi,_unalignedMMXShortVector;
		movq [edi+4],mm0
		emms
	}
	return _unalignedMMXShortVector;
};

CMMXWVector& SIMD_CALL CMMXWVector::operator-  (const CMMXWVector& v2) const NOEXCEPT
{
	__asm
	{
		mov esi,this
		mov edi,v2
		movq mm0,[esi+4]
		movq mm1,[edi+4]
		psubsw mm0,mm1
		lea edi,_unalignedMMXShortVector;
		movq [edi+4],mm0
		emms
	}
	return _unalignedMMXShortVector;
};

CMMXWVector& SIMD_CALL CMMXWVector::operator*= (const CMMXWMatrix& m) NOEXCEPT
{
	int t[2];
	_asm
	{
		mov edi,m
		mov esi,this
		add edi,4
		add esi,4
		movq mm1,[edi]
		movq mm0,[esi]
		add edi,8
		pmaddwd mm1,mm0
		lea edx,t
		movq [edx],mm1
		movq mm2,[edi]
		mov eax,[edx]
		pmaddwd mm2,mm0
		add edi,8
		add eax,[edx+4]
		movq mm1,[edi]
		mov [esi],ax	// v[0] ok
		movq [edx],mm2
		add edi,8
		pmaddwd mm1,mm0
		mov eax,[edx]
		add esi,2
		movq mm2,[edi]
		add eax,[edx+4]
		movq [edx],mm1
		mov [esi],ax	// v[1] ok
		pmaddwd mm2,mm0
		mov eax,[edx]
		add esi,2
		add eax,[edx+4]
		movq [edx],mm2
		mov [esi],ax	// v[2] ok
		mov eax,[edx]
		add eax,[edx+4]
		add esi,2
		mov [esi],ax	// v[3] ok
		emms
	}
	return *this;
}

#endif	// SIMD_NO_ASSEMBLY

