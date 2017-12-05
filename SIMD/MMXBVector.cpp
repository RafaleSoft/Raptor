// MMXByteVector.cpp: implementation of the CMMXBVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MMXBYTEVECTOR_H__8C6D4565_1B55_4069_9E6A_54484A482CA1__INCLUDED_)
	#include "MMXBVector.h"
#endif

extern CMMXBVector _unalignedMMXByteVector;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
CMMXBVector SIMD_CALL operator^  (const CMMXBVector& v1, const CMMXBVector& v2)
{
	__asm
	{
		mov esi,v1
		mov edi,v2
		movd mm0,[esi+4]
		pxor mm2,mm2
		movd mm1,[edi+4]
		punpcklbw mm0,mm2
		punpcklbw mm1,mm2
		_emit 0x0F	//pshufw mm3,mm0,0xC9
		_emit 0x70
		_emit 0xD8  // ???
		_emit 0xC9	// shuf y z x h
		_emit 0x0F	//pshufw mm4,mm1,0x87
		_emit 0x70
		_emit 0xE1	// ???
		_emit 0xD2	// shuf z' x' y' h'
		pmullw mm3,mm4
		_emit 0x0F	//pshufw mm4,mm0,0x87
		_emit 0x70
		_emit 0xE0	// ???
		_emit 0xD2	// shuf z x y h
		lea esi,_unalignedMMXByteVector;
		_emit 0x0F	//pshufw mm2,mm1,0xC9
		_emit 0x70
		_emit 0xD1	// ???
		_emit 0xC9	// shuf y' z' x' h'
		pmullw mm4,mm2
		psubw mm3,mm4
		movd [esi+4],mm3
		emms
	}
	return _unalignedMMXByteVector;
};
*/

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

