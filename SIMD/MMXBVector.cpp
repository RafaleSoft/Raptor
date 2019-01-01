// MMXByteVector.cpp: implementation of the CMMXBVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MMXBYTEVECTOR_H__8C6D4565_1B55_4069_9E6A_54484A482CA1__INCLUDED_)
	#include "MMXBVector.h"
#endif

#ifndef SIMD_NO_ASSEMBLY

extern CMMXBVector _unalignedMMXByteVector;

CMMXBVector SIMD_CALL CMMXBVector::operator*  (const CMMXBVector& v2) const
{
	__asm
	{
		mov esi,this
		mov edi,v2
		movd mm0,[esi+4]
		pxor mm2,mm2
		movd mm1,[edi+4]
		punpcklbw mm0,mm2
		punpcklbw mm1,mm2
		pmullw mm0,mm1
		lea edi,_unalignedMMXByteVector;
		packsswb mm0,mm2
		movd [edi+4],mm0
		emms
	}
	return _unalignedMMXByteVector;
};

CMMXBVector SIMD_CALL CMMXBVector::operator+  (const CMMXBVector& v2) const
{
	__asm
	{
		mov esi,this
		mov edi,v2
		movd mm0,[esi+4]
		movd mm1,[edi+4]
		paddsb mm0,mm1
		lea edi,_unalignedMMXByteVector;
		movd [edi+4],mm0
		emms
	}
	return _unalignedMMXByteVector;
};

CMMXBVector SIMD_CALL CMMXBVector::operator-  (const CMMXBVector& v2) const
{
	__asm
	{
		mov esi,this
		mov edi,v2
		movd mm0,[esi+4]
		movd mm1,[edi+4]
		psubsb mm0,mm1
		lea edi,_unalignedMMXByteVector;
		movd [edi+4],mm0
		emms
	}
	return _unalignedMMXByteVector;
};
#endif

