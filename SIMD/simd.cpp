/***************************************************************************/
/*                                                                         */
/*  simd.cpp                                                               */
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


#ifndef __SIMD_H__
	#include "simd.h"
#endif

#ifndef __REGISTERS_H__
	#include "Registers.h"
#endif

#if defined(WIN32)
	#include <intrin.h>
#elif defined(LINUX)
	#ifndef __arm__
		#include <cpuid.h>
	#else
	#endif
	#include <string.h>
#endif

#ifndef __GENERIC_ALIGNED_VECTOR_IMPL__
	#include "GenericAlignedVector.cxx"
#endif

#ifndef __GENERIC_MATRIX_IMPL__
	#include "GenericMatrix.cxx"
#endif

#if !defined(AFX_SSEFLOATVECTOR_H__9AAB93A4_DB7C_4486_802A_1B4CE4E6CEC4__INCLUDED_)
	#include "SSEFVector.h"
#endif

#if !defined(AFX_SSEFMATRIX_H__7923E89B_7F26_4EE5_8038_15480F2B7985__INCLUDED_)
	#include "SSEFMatrix.h"
#endif

#if !defined(AFX_MMXBYTEVECTOR_H__8C6D4565_1B55_4069_9E6A_54484A482CA1__INCLUDED_)
	#include "MMXBVector.h"
#endif

#if !defined(AFX_MMXWVector_H__EDBB336A_5AF7_4AF0_9000_1F0F0CE7C0C0__INCLUDED_)
	#include "MMXWVector.h"
#endif

#if !defined(AFX_MMXWMATRIX_H__6CB7BBD5_103F_4C28_AA90_88AA8AD9E466__INCLUDED_)
	#include "MMXWMatrix.h"
#endif

//	These global vectors are defined to
//	avoid "sloooow" calls to the constructor
CSSEFVector	_alignedSSEFloatVector;
CSSEFMatrix	_alignedSSEFloatMatrix;
CMMXBVector	_unalignedMMXByteVector;
CMMXWVector	_unalignedMMXShortVector;
CMMXWMatrix	_unalignedMMXShortMatrix;


//!	Processor capbilities.
static bool		cpuScanned = false;
static CPU_INFO	cpuInfo;


//!
//! Define missing __cpuidex function
//! @param regs : int[4], cpuid return in EAX,EBX,ECX & EDX
//! @param func : int, cpuid function passed in EAX
//! @param leaf : int, cpuid subfunction passed in ECX
#ifdef WIN32
	#if defined(_WIN64)
		// function defined in intrin.h
		//__cpuidex(regs,func,leaf);
	#elif _MSC_VER >= 1600
		// function defined in intrin.h
		//__cpuidex(regs,func,leaf);
	#else
		#define __cpuidex(regs,func,leaf) \
				__asm mov ecx,leaf \
				__asm mov eax,func \
				__asm cpuid \
				__asm mov dword ptr [regs], eax \
				__asm mov dword ptr [regs+4], ebx \
				__asm mov dword ptr [regs+8], ecx \
				__asm mov dword ptr [regs+12], edx
	#endif
	#define CPUID(function,regs)  __cpuid(regs,function)
	#define CPUIDEX(function,subfunction,regs)  __cpuidex(regs,function,subfunction)
#elif defined(LINUX)
	#ifndef __arm__
		static __inline int __get_cpuidex(unsigned int __level, unsigned int __subfunction,
										unsigned int *__eax, unsigned int *__ebx,
										unsigned int *__ecx, unsigned int *__edx)
		{
			unsigned int __ext = __level & 0x80000000;

			if (__get_cpuid_max (__ext, 0) < __level)
				return 0;

			__cpuid_count(__level, __subfunction, *__eax, *__ebx, *__ecx, *__edx);
			return 1;
		}
		#define CPUID(level,regs)  __get_cpuid(level,&regs[0],&regs[1],&regs[2],&regs[3])
		#define CPUIDEX(level,subfunction,regs)  __get_cpuidex(level,subfunction,&regs[0],&regs[1],&regs[2],&regs[3])
	#else
		static __inline int __get_cpuid(unsigned int __level, unsigned int __subfunction,
										unsigned int *__eax, unsigned int *__ebx,
										unsigned int *__ecx, unsigned int *__edx)
		{
			if (__eax) *__eax = 0;
			if (__ebx) *__ebx = 0;
			if (__ecx) *__ecx = 0;
			if (__edx) *__edx = 0;
			return 0;
		}
		#define CPUID(level,regs)  __get_cpuid(level, 0, &regs[0],&regs[1],&regs[2],&regs[3])
		#define CPUIDEX(level,subfunction,regs)  __get_cpuid(level, subfunction, &regs[0],&regs[1],&regs[2],&regs[3])
	#endif
#endif

//!
//! Decoding table for cache descriptors
//! (according to intel's cpuid documentation)
const CPU_INFO::CACHE_DESCRIPTOR cacheDescriptors[256] = 
{
	/* 00h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 01h */	{ CPU_INFO::INSTRUCTION_TLB, 0, 4, 4, 32 },
	/* 02h */	{ CPU_INFO::INSTRUCTION_TLB, 0, 4096, 0, 2 },
	/* 03h */	{ CPU_INFO::DATA_TLB, 0, 4, 4, 64 },
	/* 04h */	{ CPU_INFO::DATA_TLB, 0, 4096, 4, 8 },
	/* 05h */	{ CPU_INFO::DATA_TLB, 0, 4096, 4, 32 },
	/* 06h */	{ CPU_INFO::INSTRUCTION_CACHE, 1, 8, 4, 32 },
	/* 07h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 08h */	{ CPU_INFO::INSTRUCTION_CACHE, 1, 16, 4, 32 },
	/* 09h */	{ CPU_INFO::INSTRUCTION_CACHE, 1, 32, 4, 64 },
	/* 0Ah */	{ CPU_INFO::DATA_CACHE, 1, 8, 2, 32 },
	/* 0Bh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 0Ch */	{ CPU_INFO::DATA_CACHE, 1, 16, 4, 32 },
	/* 0Dh */	{ CPU_INFO::DATA_CACHE, 1, 16, 4, 64 },
	/* 0Eh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 0Fh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 10h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 11h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 12h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 13h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 14h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 15h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 16h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 17h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 18h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 19h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 1Ah */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 1Bh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 1Ch */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 1Dh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 1Eh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 1Fh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 20h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 21h */	{ CPU_INFO::CACHE, 2, 256, 8, 64 },
	/* 22h */	{ CPU_INFO::CACHE, 3, 512, 4, 64 },
	/* 23h */	{ CPU_INFO::CACHE, 3, 1024, 8, 64 },
	/* 24h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 25h */	{ CPU_INFO::CACHE, 3, 2048, 8, 64 },
	/* 26h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 27h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 28h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 29h */	{ CPU_INFO::CACHE, 3, 4096, 8, 64 },
	/* 2Ah */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 2Bh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 2Ch */	{ CPU_INFO::DATA_CACHE, 1, 32, 8, 64 },
	/* 2Dh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 2Eh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 2Fh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 30h */	{ CPU_INFO::INSTRUCTION_CACHE, 1, 32, 8, 64 },
	/* 31h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 32h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 33h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 34h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 35h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 36h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 37h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 38h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 39h */	{ CPU_INFO::NONE, 2, 128, 4, 64 },
	/* 3Ah */	{ CPU_INFO::NONE, 2, 192, 6, 64 },
	/* 3Bh */	{ CPU_INFO::NONE, 2, 128, 2, 64 },
	/* 3Ch */	{ CPU_INFO::NONE, 2, 256, 4, 64 },
	/* 3Dh */	{ CPU_INFO::NONE, 2, 384, 6, 64 },
	/* 3Eh */	{ CPU_INFO::NONE, 2, 512, 4, 64 },
	/* 3Fh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 40h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 41h */	{ CPU_INFO::CACHE, 2, 128, 4, 32 },
	/* 42h */	{ CPU_INFO::CACHE, 2, 256, 4, 32 },
	/* 43h */	{ CPU_INFO::CACHE, 2, 512, 4, 32 },
	/* 44h */	{ CPU_INFO::CACHE, 2, 1024, 4, 0 },
	/* 45h */	{ CPU_INFO::CACHE, 2, 2048, 4, 0 },
	/* 46h */	{ CPU_INFO::CACHE, 3, 4096, 4, 0 },
	/* 47h */	{ CPU_INFO::CACHE, 3, 8192, 8, 0 },
	/* 48h */	{ CPU_INFO::CACHE, 2, 3072, 12, 64 },
	/* 49h */	{ CPU_INFO::CACHE, 3, 4096, 16, 64 },	// Xeon MP F:0f M:06
	/* 4Ah */	{ CPU_INFO::CACHE, 3, 6144, 12, 64 },
	/* 4Bh */	{ CPU_INFO::CACHE, 3, 8192, 16, 64 },
	/* 4Ch */	{ CPU_INFO::CACHE, 3, 12288, 12, 64 },
	/* 4Dh */	{ CPU_INFO::CACHE, 3, 16384, 16, 64 },
	/* 4Eh */	{ CPU_INFO::CACHE, 2, 6144, 24, 64 },
	/* 4Fh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 50h */	{ CPU_INFO::INSTRUCTION_TLB, 0, 4, 0, 64 },
	/* 51h */	{ CPU_INFO::INSTRUCTION_TLB, 0, 4, 0, 128 },
	/* 52h */	{ CPU_INFO::INSTRUCTION_TLB, 0, 4, 0, 256 },
	/* 53h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 54h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 55h */	{ CPU_INFO::INSTRUCTION_TLB, 0, 2048, 0, 7 },
	/* 56h */	{ CPU_INFO::DATA_TLB, 1, 4096, 4, 16 },
	/* 57h */	{ CPU_INFO::DATA_TLB, 1, 4, 4, 16 },
	/* 58h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 59h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 5Ah */	{ CPU_INFO::DATA_TLB, 0, 2048, 4, 32 },
	/* 5Bh */	{ CPU_INFO::DATA_TLB, 0, 4, 0, 64 },
	/* 5Ch */	{ CPU_INFO::DATA_TLB, 0, 4, 0, 128 },
	/* 5Dh */	{ CPU_INFO::DATA_TLB, 0, 4, 0, 256 },
	/* 5Eh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 5Fh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 60h */	{ CPU_INFO::DATA_CACHE, 1, 16, 8, 64 },
	/* 61h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 62h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 63h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 64h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 65h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 66h */	{ CPU_INFO::DATA_CACHE, 1, 8, 4, 64 },
	/* 67h */	{ CPU_INFO::DATA_CACHE, 1, 16, 4, 64 },
	/* 68h */	{ CPU_INFO::DATA_CACHE, 1, 32, 4, 64 },
	/* 69h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 6Ah */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 6Bh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 6Ch */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 6Dh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 6Eh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 6Fh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 70h */	{ CPU_INFO::TRACE_CACHE, 0, 12, 8, 0 },
	/* 71h */	{ CPU_INFO::TRACE_CACHE, 0, 16, 8, 0 },
	/* 72h */	{ CPU_INFO::TRACE_CACHE, 0, 32, 8, 0 },
	/* 73h */	{ CPU_INFO::TRACE_CACHE, 0, 64, 8, 0 },
	/* 74h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 75h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 76h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 77h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 78h */	{ CPU_INFO::CACHE, 2, 1024, 4, 64 },
	/* 79h */	{ CPU_INFO::CACHE, 2, 128, 8, 64 },
	/* 7Ah */	{ CPU_INFO::CACHE, 2, 256, 8, 64 },
	/* 7Bh */	{ CPU_INFO::CACHE, 2, 512, 8, 64 },
	/* 7Ch */	{ CPU_INFO::CACHE, 2, 1024, 8, 64 },
	/* 7Dh */	{ CPU_INFO::CACHE, 2, 2048, 8, 64 },
	/* 7Eh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 7Fh */	{ CPU_INFO::CACHE, 2, 512, 2, 64 },
	/* 80h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 81h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 82h */	{ CPU_INFO::CACHE, 2, 256, 8, 32 },
	/* 83h */	{ CPU_INFO::CACHE, 2, 512, 8, 32 },
	/* 84h */	{ CPU_INFO::CACHE, 2, 1024, 8, 32 },
	/* 85h */	{ CPU_INFO::CACHE, 2, 2048, 8, 32 },
	/* 86h */	{ CPU_INFO::CACHE, 2, 512, 4, 64 },
	/* 87h */	{ CPU_INFO::CACHE, 2, 1024, 8, 64 },
	/* 88h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 89h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 8Ah */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 8Bh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 8Ch */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 8Dh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 8Eh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 8Fh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 90h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 91h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 92h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 93h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 94h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 95h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 96h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 97h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 98h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 99h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 9Ah */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 9Bh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 9Ch */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 9Dh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 9Eh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* 9Fh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* A0h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* A1h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* A2h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* A3h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* A4h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* A5h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* A6h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* A7h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* A8h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* A9h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* AAh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* ABh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* ACh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* ADh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* AEh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* AFh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* B0h */	{ CPU_INFO::INSTRUCTION_TLB, 0, 4, 4, 128 },
	/* B1h */	{ CPU_INFO::INSTRUCTION_TLB, 0, 2048, 4, 8 },
	/* B2h */	{ CPU_INFO::INSTRUCTION_TLB, 0, 4, 4, 64 },
	/* B3h */	{ CPU_INFO::DATA_TLB, 0, 4, 4, 128 },
	/* B4h */	{ CPU_INFO::DATA_TLB, 0, 4, 4, 256 },
	/* B5h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* B6h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* B7h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* B8h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* B9h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* BAh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* BBh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* BCh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* BDh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* BEh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* BFh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* C0h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* C1h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* C2h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* C3h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* C4h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* C5h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* C6h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* C7h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* C8h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* C9h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* CAh */	{ CPU_INFO::INSTRUCTION_TLB, 2, 4, 4, 512 },
	/* CBh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* CCh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* CDh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* CEh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* CFh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* D0h */	{ CPU_INFO::CACHE, 3, 512, 4, 64 },
	/* D1h */	{ CPU_INFO::CACHE, 3, 1024, 4, 64 },
	/* D2h */	{ CPU_INFO::CACHE, 3, 2048, 4, 64 },
	/* D3h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* D4h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* D5h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* D6h */	{ CPU_INFO::CACHE, 3, 1024, 8, 64 },
	/* D7h */	{ CPU_INFO::CACHE, 3, 2048, 8, 64 },
	/* D8h */	{ CPU_INFO::CACHE, 3, 4096, 8, 64 },
	/* D9h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* DAh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* DBh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* DCh */	{ CPU_INFO::CACHE, 3, 1536, 12, 64 },
	/* DDh */	{ CPU_INFO::CACHE, 3, 3072, 12, 64 },
	/* DEh */	{ CPU_INFO::CACHE, 3, 6144, 12, 64 },
	/* DFh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* E0h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* E1h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* E2h */	{ CPU_INFO::CACHE, 3, 2048, 16, 64 },
	/* E3h */	{ CPU_INFO::CACHE, 3, 4096, 16, 64 },
	/* E4h */	{ CPU_INFO::CACHE, 3, 8192, 16, 64 },
	/* E5h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* E6h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* E7h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* E8h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* E9h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* EAh */	{ CPU_INFO::CACHE, 3, 12288, 24, 64 },
	/* EBh */	{ CPU_INFO::CACHE, 3, 18432, 24, 64 },
	/* ECh */	{ CPU_INFO::CACHE, 3, 24576, 24, 64 },
	/* EDh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* EEh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* EFh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* F0h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* F1h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* F2h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* F3h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* F4h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* F5h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* F6h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* F7h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* F8h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* F9h */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* FAh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* FBh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* FCh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* FDh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* FEh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
	/* FFh */	{ CPU_INFO::NONE, 0, 0, 0, 0 },
};

const CPU_INFO& SIMD_API getCPUINFO()
{
	if (cpuScanned)
		return cpuInfo;
	else
	{
		cpuScanned = true;
		memset(&cpuInfo,0,sizeof(CPU_INFO));

#if defined(WIN32)
		int cpuidRegs[4];
#elif defined(LINUX)
		unsigned int cpuidRegs[4];
#endif

		CPUID(0,cpuidRegs);

		int nbId = cpuidRegs[0];

		cpuInfo.CPUID = (nbId > 0);

		if (nbId >= 1)
		{
			*((int*)&cpuInfo.features[0]) = cpuidRegs[1];
			*((int*)&cpuInfo.features[4]) = cpuidRegs[3];
			*((int*)&cpuInfo.features[8]) = cpuidRegs[2];

			CPUID(1,cpuidRegs);

			cpuInfo.stepping = (cpuidRegs[0] & 0x0f);
			cpuInfo.model = (cpuidRegs[0] >> 4) & 0x0f;
			cpuInfo.family = (cpuidRegs[0] >> 8) & 0x0f;
			cpuInfo.type = (cpuidRegs[0] >> 12) & 0x03;
			cpuInfo.eModel = (cpuidRegs[0] >> 16) & 0x0f;
			cpuInfo.eFamily = (cpuidRegs[0] >> 20) & 0xff;

			cpuInfo.brand = cpuidRegs[1] & 0xff;
			cpuInfo.cacheLineFlush = ((cpuidRegs[1] >> 8) & 0xff) * 8;
			cpuInfo.maxLogicalAPIC = (cpuidRegs[1] >> 16) & 0xff;
			cpuInfo.apicId = (cpuidRegs[1] >> 24) & 0xff;
			cpuInfo.hyperThreading = ((cpuidRegs[1] >> 16) & 0xff) > 1;
			cpuInfo.featureFlagECX = cpuidRegs[2];
			cpuInfo.featureFlagEDX = cpuidRegs[3];
		}
		if (nbId >= 2)
		{
			CPUID(2,cpuidRegs);

			int nbCacheCalls = (cpuidRegs[0] & 0xff);
			if (cpuInfo.cacheDescriptor.descriptors != 0)
				delete [] cpuInfo.cacheDescriptor.descriptors;
			cpuInfo.cacheDescriptor.descriptors = new CPU_INFO::CACHE_DESCRIPTOR[nbCacheCalls*16];
			cpuInfo.cacheDescriptor.nbDescriptors = 0;
			for (int i=0;i<nbCacheCalls;i++)
			{
				for (int j=0;j<4;j++)
				{
					if (0 == (cpuidRegs[j] & (1 << 31)))
					{
						// cache descriptors
						for (int k=3;k>=0;k--)
						{
							int val = (cpuidRegs[j] >> (k*8)) & 0xff;
							if (val == 0xF0)
								cpuInfo.cacheDescriptor.prefetchingSize = 64;
							else if (val == 0xF1)
								cpuInfo.cacheDescriptor.prefetchingSize = 128;
							else if (val != 0)
							{
								CPU_INFO::CACHE_DESCRIPTOR descriptor = cacheDescriptors[val];
								cpuInfo.cacheDescriptor.descriptors[cpuInfo.cacheDescriptor.nbDescriptors++] = descriptor;
							}
						}
					}
				}

				CPUID(2,cpuidRegs);
			}
		}
		if ((nbId >= 3) && cpuInfo.hasFeature(CPUINFO::PSN))
		{
			CPUID(3,cpuidRegs);

			cpuInfo.serialNumber[0] = cpuidRegs[2];
			cpuInfo.serialNumber[1] = cpuidRegs[3];
		}
		if (nbId >= 4)
		{
			if (cpuInfo.cacheParameters.parameters != 0)
				delete [] cpuInfo.cacheParameters.parameters;
			cpuInfo.cacheParameters.parameters = new CPU_INFO::CACHE_PARAMETER[8];
			cpuInfo.cacheParameters.nbCacheLevels = 0;
			int cacheLevel = 0;

			CPUIDEX(4,cacheLevel,cpuidRegs);
			cacheLevel++;
			
			short cacheType = (cpuidRegs[0] & 0x1f);
			while (cacheType != 0)
			{
				CPU_INFO::CACHE_PARAMETER parameter;
				parameter.numAPICIds = 1 + (cpuidRegs[0] >> 26);
				parameter.maxThreads = 1 + (cpuidRegs[0] >> 14) & 0x0fff;
				parameter.fullAssociative = ((cpuidRegs[0] >> 9) & 1);
				parameter.selfInitialized = ((cpuidRegs[0] >> 8) & 1);
				parameter.level = ((cpuidRegs[0] >> 5) & 0x07);
				parameter.type = (cacheType == 1 ? CPU_INFO::DATA_CACHE :
								 (cacheType == 2 ? CPU_INFO::INSTRUCTION_CACHE : 
								 (cacheType == 3 ? CPU_INFO::CACHE : CPU_INFO::NONE)));
				parameter.ways = 1 + (cpuidRegs[1] >> 22);
				parameter.partitions = 1  + ((cpuidRegs[1] >> 12) & 0x03ff);
				parameter.lineSize = 1 + (cpuidRegs[1] & 0x0fff);
				parameter.sets = cpuidRegs[2];
				parameter.inclusiveLowerLevels = (cpuidRegs[3] >> 1) & 1;
				parameter.invdBehavior = cpuidRegs[3] & 1;

				parameter.cacheSize = parameter.ways * parameter.partitions * parameter.lineSize * parameter.sets;
				cpuInfo.cacheParameters.parameters[cpuInfo.cacheParameters.nbCacheLevels++] = parameter;

				CPUIDEX(4,cacheLevel,cpuidRegs);
				cacheLevel++;
				cacheType = (cpuidRegs[0] & 0x1f);
			}
		}
		if (nbId >= 5)
		{ /* monitor / mwait functions */	}
		if (nbId >= 6)
		{ /* digital thermal sensor and power management functions */ }
		//
		// Processor clock frequency can only be determined in superuser mode.
		//	Using this lib in user space will fail, freq can be queried from
		//	win32 performance counters or roughly extracted from processor brand string
		// 
		if (nbId >= 7)
		{ /* reserved */ }
		if (nbId >= 8)
		{ /* reserved */ }
		if (nbId >= 9)
		{ /* direct cache access parameters */ }
		if (nbId >= 10)
		{ /* architectural performance monitor */ }
		if (nbId >= 11)
		{ 
			/* processor topology */
			int topologyLevel = 0;

			CPUIDEX(11,topologyLevel,cpuidRegs);
			topologyLevel++;
		
			int apicShift = cpuidRegs[0] & 0xff;
			cpuInfo.numThreads = cpuidRegs[1] & 0xffff;

			if ((cpuidRegs[0] > 0) && (cpuidRegs[1] > 0))
			{
				CPUIDEX(11,topologyLevel,cpuidRegs);
				topologyLevel++;

				apicShift = cpuidRegs[0] & 0xff;
				cpuInfo.numCores = cpuidRegs[1] & 0xffff;
			}
		}

		CPUID(0x80000000,cpuidRegs);

		nbId = cpuidRegs[0];
		if (nbId >= 0x80000002)
		{
			CPUID(0x80000002,cpuidRegs);
			memcpy(&cpuInfo.processorBrand[0],cpuidRegs,16);
		}
		if (nbId >= 0x80000003)
		{
			CPUID(0x80000003,cpuidRegs);
			memcpy(&cpuInfo.processorBrand[16],cpuidRegs,16);
		}
		if (nbId >= 0x80000004)
		{
			CPUID(0x80000004,cpuidRegs);
			memcpy(&cpuInfo.processorBrand[32],cpuidRegs,16);
		}
		//!
		//! Old cpuid fetch method for olf compilers/cpus
		//!
		/*
		else
		{
			char _cpuInfo[0x20];
			memset(_cpuInfo,0,0x20);
			memcpy(_cpuInfo,"CPUID reported: no CPUID",24);

			int is_cpuid = 0;
			int regedx = 0;
			int regecx = 0;
			int regeax = 0;

			//! must test before if CPUID can be called
			__asm
			{
				push	eax
				push	ebx
				push	ecx
				push	edx

				pushfd                      ;push original EFLAGS
				pop     eax                 ; get original EFLAGS in eax
				mov     ecx,eax             ;save original EFLAGS in ecx
				or      eax,200000h         ; flip ID bit in EFLAGS
				push    eax                 ;save for EFLAGS
				popfd                       ; copy to EFLAGS
				pushfd                      ;push EFLAGS
				pop     eax                 ; get new EFLAGS value
				xor     eax,ecx
				je      end_get_cpuid       ;if ID bit cannot be changed,
											;CPU=Intel486 without CPUID
											;instruction functionality
				mov		is_cpuid,1

		end_get_cpuid:
				push ecx
				popfd

				test is_cpuid,1
				jz end_cpuid

				mov eax,0
				cpuid	//_emit 0x0F _emit 0xA2	- equivalent to cpuid
				mov dword ptr [_cpuInfo+16],ebx
				mov dword ptr [_cpuInfo+16+4],edx
				mov dword ptr [_cpuInfo+16+8],ecx
				mov dword ptr [_cpuInfo+16+12],0
				mov eax,1
				cpuid
				mov regedx,edx
				mov regecx,ecx
				mov regeax,eax

		end_cpuid:
				pop		edx
				pop		ecx
				pop		ebx
				pop		eax
			}

			// Intel only:
			//if (regeax&0x00001800)
			//	memcpy(&_cpuInfo[28]," Intel reserved\0",16);
			//else if (regeax&0x00001000)
			//	memcpy(&_cpuInfo[28]," Dual Processor\0",16);
			//else if (regeax&0x00000800)
			//	memcpy(&_cpuInfo[28]," Over Drive Processor\0",22);
			//else
			//	memcpy(&_cpuInfo[28]," Original OEM Processor\0",24);

			cpuInfo.CPUID = (is_cpuid == 1);
			cpuInfo.CMPXCHG8B = (0 != (regedx&0x100));
			cpuInfo.CMOV = (0 != (regedx&0x8000));
			cpuInfo.MMX = (0 != (regedx&0x800000));
			cpuInfo.SSE = (0 != (regedx&0x2000000));
			cpuInfo.SSE2 = (0 != (regedx&0x4000000));

			cpuInfo.SSE3 = (0 != (regecx&0x1));
			cpuInfo.SSSE3 = (0 != (regecx&0x200));
			cpuInfo.FMA = (0 != (regecx&0x1000));
			cpuInfo.SSE41 = (0 != (regecx&0x80000));
			cpuInfo.SSE42 = (0 != (regecx&0x100000));
			cpuInfo.AES = (0 != (regecx&0x2000000));
			cpuInfo.AVX = (0 != (regecx&0x10000000));

			memcpy(cpuInfo.features,_cpuInfo,sizeof(_cpuInfo));
		}*/

		return cpuInfo;
	}
}


