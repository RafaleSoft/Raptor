#ifndef __REGISTERS_H__
#define __REGISTERS_H__

//
//
//	CODE GENERATION
//
//
//	Registers, as specified by the REG field
//	in the ModR/M byte
#define EAX			0
#define ECX			1
#define EDX			2
#define EBX			3
#define ESP			4
#define EBP			5
#define ESI			6
#define EDI			7

//	sse register operands used where parameter reg is required
//	the #define represents the
//	standard intel operand convention: dest_src
//	e.g.: XMM1_XMM0 means XMM1 = dest; XMM0 = src
#define XMM0_XMM0	0xc0
#define XMM0_XMM1	0xc1
#define XMM0_XMM2	0xc2
#define XMM0_XMM3	0xc3
#define XMM0_XMM4	0xc4
#define XMM0_XMM5	0xc5
#define XMM0_XMM6	0xc6
#define XMM0_XMM7	0xc7

#define XMM1_XMM0	0xc8
#define XMM1_XMM1	0xc9
#define XMM1_XMM2	0xca
#define XMM1_XMM3	0xcb
#define XMM1_XMM4	0xcc
#define XMM1_XMM5	0xcd
#define XMM1_XMM6	0xce
#define XMM1_XMM7	0xcf

#define XMM2_XMM0	0xd0
#define XMM2_XMM1	0xd1
#define XMM2_XMM2	0xd2
#define XMM2_XMM3	0xd3
#define XMM2_XMM4	0xd4
#define XMM2_XMM5	0xd5
#define XMM2_XMM6	0xd6
#define XMM2_XMM7	0xd7

#define XMM3_XMM0	0xd8
#define XMM3_XMM1	0xd9
#define XMM3_XMM2	0xda
#define XMM3_XMM3	0xdb
#define XMM3_XMM4	0xdc
#define XMM3_XMM5	0xdd
#define XMM3_XMM6	0xde
#define XMM3_XMM7	0xdf

#define XMM4_XMM0	0xe0
#define XMM4_XMM1	0xe1
#define XMM4_XMM2	0xe2
#define XMM4_XMM3	0xe3
#define XMM4_XMM4	0xe4
#define XMM4_XMM5	0xe5
#define XMM4_XMM6	0xe6
#define XMM4_XMM7	0xe7

#define XMM5_XMM0	0xe8
#define XMM5_XMM1	0xe9
#define XMM5_XMM2	0xea
#define XMM5_XMM3	0xeb
#define XMM5_XMM4	0xec
#define XMM5_XMM5	0xed
#define XMM5_XMM6	0xee
#define XMM5_XMM7	0xef

#define XMM6_XMM0	0xf0
#define XMM6_XMM1	0xf1
#define XMM6_XMM2	0xf2
#define XMM6_XMM3	0xf3
#define XMM6_XMM4	0xf4
#define XMM6_XMM5	0xf5
#define XMM6_XMM6	0xf6
#define XMM6_XMM7	0xf7

#define XMM7_XMM0	0xf8
#define XMM7_XMM1	0xf9
#define XMM7_XMM2	0xfa
#define XMM7_XMM3	0xfb
#define XMM7_XMM4	0xfc
#define XMM7_XMM5	0xfd
#define XMM7_XMM6	0xfe
#define XMM7_XMM7	0xff

//	sse register operands
//	the #define represents the
//	standard intel memory addressing: 
//	reg/mem for load ops
//	mem/reg for store ops
#define XMM0_ECX	0x01
#define XMM1_ECX	0x09
#define XMM2_ECX	0x11
#define XMM3_ECX	0x19
#define XMM4_ECX	0x21
#define XMM5_ECX	0x29
#define XMM6_ECX	0x31
#define XMM7_ECX	0x39

#define XMM0_EDX	0x02
#define XMM1_EDX	0x0a
#define XMM2_EDX	0x12
#define XMM3_EDX	0x1a
#define XMM4_EDX	0x2e
#define XMM5_EDX	0x2a
#define XMM6_EDX	0x3e
#define XMM7_EDX	0x3a

#define XMM0_ESI	0x06
#define XMM1_ESI	0x0e
#define XMM2_ESI	0x16
#define XMM3_ESI	0x1e
#define XMM4_ESI	0x26
#define XMM5_ESI	0x2e
#define XMM6_ESI	0x36
#define XMM7_ESI	0x3e

#define XMM0_EDI	0x07
#define XMM1_EDI	0x0f
#define XMM2_EDI	0x17
#define XMM3_EDI	0x1f
#define XMM4_EDI	0x27
#define XMM5_EDI	0x2f
#define XMM6_EDI	0x37
#define XMM7_EDI	0x3f

//	sse compare flags for op parameter
#define SSE_EQUAL			0
#define SSE_LOWER			1
#define SSE_LOWER_EQUAL		2
#define SSE_QNAN			3
#define SSE_NOT_EQUAL		4
#define SSE_NOT_LOWER		5
#define SSE_NOT_LOWER_EQUAL	6
#define SSE_NOT_QNAN		7

//	unique orders ( non unique are of course possible, but means a table of 256 values )
//	sse order parameter
#define SSE_R4_R3_R2_R1		0xe4	// 11 10 01 00
#define SSE_R4_R3_R1_R2		0xe1	// 11 10 00 01
#define SSE_R4_R2_R3_R1		0xd8	// 11 01 10 00
#define SSE_R4_R2_R1_R3		0xd2	// 11 01 00 10
#define SSE_R4_R1_R3_R2		0xc9	// 11 00 10 01
#define SSE_R4_R1_R2_R3		0xc6	// 11 00 01 10

#define SSE_R3_R4_R2_R1		0xb4	// 10 11 01 00
#define SSE_R3_R4_R1_R2		0xb1	// 10 11 00 01
#define SSE_R3_R2_R4_R1		0x9c	// 10 01 11 00
#define SSE_R3_R2_R1_R4		0x93	// 10 01 00 11
#define SSE_R3_R1_R4_R2		0x8e	// 10 00 11 01
#define SSE_R3_R1_R2_R4		0x87	// 10 00 01 11

#define SSE_R2_R4_R3_R1		0x78	// 01 11 10 00
#define SSE_R2_R4_R1_R3		0x72	// 01 11 00 10
#define SSE_R2_R3_R4_R1		0x6c	// 01 10 11 00
#define SSE_R2_R3_R1_R4		0x63	// 01 10 00 11
#define SSE_R2_R1_R4_R3		0x4e	// 01 00 11 10
#define SSE_R2_R1_R3_R4		0x4b	// 01 00 10 11

#define SSE_R1_R4_R3_R2		0x39	// 00 11 10 01
#define SSE_R1_R4_R2_R3		0x36	// 00 11 01 10
#define SSE_R1_R3_R4_R2		0x2d	// 00 10 11 01
#define SSE_R1_R3_R2_R4		0x27	// 00 10 01 11
#define SSE_R1_R2_R4_R3		0x1e	// 00 01 11 10
#define SSE_R1_R2_R3_R4		0x1b	// 00 01 10 11

//	Instruction emission
#define EMIT(code) __asm _emit code
#define EMIT3(c1,c2,c3) EMIT(c1) EMIT(c2) EMIT(c3)
#define EMIT4(c1,c2,c3,c4) EMIT(c1) EMIT(c2) EMIT(c3) EMIT(c4)


//	instructions prefetch
#define prefetch_all(reg)	EMIT3(0x0f,0x18,0x08+reg)
#define prefetch_l1(reg)	EMIT3(0x0f,0x18,0x10+reg)
#define prefetch_l2(reg)	EMIT3(0x0f,0x18,0x18+reg)
#define prefetch_nt(reg)	EMIT3(0x0f,0x18,reg)


//
//
//	MMX Instructions
//
//
#define mmx_pshufw(reg,order)   EMIT4(0x0f, 0x70, reg, order)


//
//
//	SSE Instructions
//
//
#define sse_addps(reg)			EMIT3(0x0f,0x58,reg)
#define sse_addss(reg)			EMIT4(0xf3,0x0f,0x58,reg)
#define sse_addnps(reg)			EMIT3(0x0f,0x55,reg)
#define sse_andps(reg)			EMIT3(0x0f,0x54,reg)

#define sse_cmpps(reg,op)		EMIT4(0x0f,0xc2,reg,op)
#define sse_cmpeqps(reg)	sse_cmpps(reg,SSE_EQUAL)
#define sse_cmpltps(reg)	sse_cmpps(reg,SSE_LOWER)
#define sse_cmpleps(reg)	sse_cmpps(reg,SSE_LOWER_EQUAL)
#define sse_cmpunordps(reg)	sse_cmpps(reg,SSE_QNAN)
#define sse_cmpneqps(reg)	sse_cmpps(reg,SSE_NOT_EQUAL)
#define sse_cmpgeps(reg)	sse_cmpps(reg,SSE_NOT_LOWER)
#define sse_cmpgtps(reg)	sse_cmpps(reg,SSE_NOT_LOWER_EQUAL)
#define sse_cmpordps(reg)	sse_cmpps(reg,SSE_NOT_QNAN)

#define sse_cmpss(reg,op)		EMIT4(0xf3,0x0f,0xc2,reg) EMIT(op)
#define sse_cmpeqss(reg)	sse_cmpss(reg,SSE_EQUAL)
#define sse_cmpltss(reg)	sse_cmpss(reg,SSE_LOWER)
#define sse_cmpless(reg)	sse_cmpss(reg,SSE_LOWER_EQUAL)
#define sse_cmpunordss(reg)	sse_cmpss(reg,SSE_QNAN)
#define sse_cmpneqss(reg)	sse_cmpss(reg,SSE_NOT_EQUAL)
#define sse_cmpgess(reg)	sse_cmpss(reg,SSE_NOT_LOWER)
#define sse_cmpgtss(reg)	sse_cmpss(reg,SSE_NOT_LOWER_EQUAL)
#define sse_cmpordss(reg)	sse_cmpss(reg,SSE_NOT_QNAN)

#define sse_comiss(reg)			EMIT3(0x0f,0x2f,reg)
#define sse_cvtpi2ps(reg)		EMIT3(0x0f,0x2a,reg)
#define sse_cvtps2pi(reg)		EMIT3(0x0f,0x2d,reg)
#define sse_cvtsi2ss(reg)		EMIT4(0xf3,0x0f,0x2a,reg)
#define sse_cvtss2si(reg)		EMIT4(0xf3,0x0f,0x2d,reg)
#define sse_cvttps2pi(reg)		EMIT3(0x0f,0x2c,reg)
#define sse_cvttss2si(reg)		EMIT4(0xf3,0x0f,0x2c,reg)
#define sse_divps(reg)			EMIT3(0x0f,0x5e,reg)
#define sse_divss(reg)			EMIT4(0xf3,0x0f,0x5e,reg)
#define sse_maxps(reg)			EMIT3(0x0f,0x5f,reg)
#define sse_maxss(reg)			EMIT4(0xf3,0x0f,0x5f,reg)
#define sse_minps(reg)			EMIT3(0x0f,0x5d,reg)
#define sse_minss(reg)			EMIT4(0xf3,0x0f,0x5d,reg)

//	sse_movaps(reg)
#define sse_loadaps(reg)		EMIT3(0x0f,0x28,reg)
#define sse_storeaps(reg)		EMIT3(0x0f,0x29,reg)
#define sse_loadapsofs(reg,ofs)	EMIT4(0x0f,0x28,0x40+reg,ofs)
#define sse_storeapsofs(reg,ofs)	EMIT4(0x0f,0x29,0x40+reg,ofs)

#define sse_movhlps(reg)		EMIT3(0x0f,0x12,reg)
#define sse_movlhps(reg)		EMIT3(0x0f,0x16,reg)

//	sse_movhps(reg)
#define sse_loadhps(reg)		EMIT3(0x0f,0x16,reg)
#define sse_storehps(reg)		EMIT3(0x0f,0x17,reg)
//	sse_movlps(reg)
#define sse_loadlps(reg)		EMIT3(0x0f,0x12,reg)
#define sse_storelps(reg)		EMIT3(0x0f,0x13,reg)

#define sse_movmskps(reg)		EMIT3(0x0f,0x50,reg)
#define sse_movntps(reg)		EMIT3(0x0f,0x2b,reg)
// sse_movss
#define sse_loadss(reg)			EMIT4(0xf3,0x0f,0x10,reg)
#define sse_storess(reg)		EMIT4(0xf3,0x0f,0x11,reg)
// sse_movups
#define sse_loadups(reg)		EMIT3(0x0f,0x10,reg)
#define sse_storeups(reg)		EMIT3(0x0f,0x11,reg)
#define sse_loadupsofs(reg,ofs)	EMIT4(0x0f,0x10,0x40+reg,ofs)
#define sse_storeupsofs(reg,ofs)	EMIT4(0x0f,0x11,0x40+reg,ofs)

#define sse_mulps(reg)			EMIT3(0x0f,0x59,reg)
#define sse_mulss(reg)			EMIT4(0xf3,0x0f,0x59,reg)
#define sse_orps(reg)			EMIT3(0x0f,0x56,reg)
#define sse_rcpps(reg)			EMIT3(0x0f,0x53,reg)
#define sse_rcpss(reg)			EMIT4(0xf3,0x0f,0x53,reg)
#define sse_rsqrtps(reg)		EMIT3(0x0f,0x52,reg)
#define sse_rsqrtss(reg)		EMIT4(0xf3,0x0f,0x52,reg)

#define sse_shufps(reg,order)	EMIT4(0x0f,0xc6,reg,order)
#define sse_sqrtps(reg)			EMIT3(0x0f,0x51,reg)
#define sse_sqrtss(reg)			EMIT4(0xf3,0x0f,0x51,reg)
#define sse_subps(reg)			EMIT3(0x0f,0x5c,reg)
#define sse_subss(reg)			EMIT4(0xf3,0x0f,0x5c,reg)
#define sse_ucomiss(reg)		EMIT3(0x0f,0x2e,reg)
#define sse_unpckhps(reg)		EMIT3(0x0f,0x15,reg)
#define sse_unpcklps(reg)		EMIT3(0x0f,0x14,reg)
#define sse_xorps(reg)			EMIT3(0x0f,0x57,reg)

// parameter must be a const float *v
//	ex : 
//	float *v;
//	float vect[4];
//	sse_loadxmm0(v) => OK
//	sse_loadxmm0(vect) => KO
//	sse_loadxmm0(&vect) => KO
#define sse_loadxmm0(v)	__asm mov edi,v EMIT3(0x0f,0x10,0x07)
#define sse_loadxmm1(v)	__asm mov edi,v EMIT3(0x0f,0x10,0x0f)
#define sse_loadxmm2(v)	__asm mov edi,v EMIT3(0x0f,0x10,0x17)
#define sse_loadxmm3(v)	__asm mov edi,v EMIT3(0x0f,0x10,0x1f)
#define sse_loadxmm4(v)	__asm mov edi,v EMIT3(0x0f,0x10,0x27)
#define sse_loadxmm5(v)	__asm mov edi,v EMIT3(0x0f,0x10,0x2f)
#define sse_loadxmm6(v)	__asm mov edi,v EMIT3(0x0f,0x10,0x37)
#define sse_loadxmm7(v)	__asm mov edi,v EMIT3(0x0f,0x10,0x3f)

#define sse_storexmm0(v) __asm mov edi,v EMIT3(0x0f,0x11,0x07)
#define sse_storexmm1(v) __asm mov edi,v EMIT3(0x0f,0x11,0x0f)
#define sse_storexmm2(v) __asm mov edi,v EMIT3(0x0f,0x11,0x17)
#define sse_storexmm3(v) __asm mov edi,v EMIT3(0x0f,0x11,0x1f)
#define sse_storexmm4(v) __asm mov edi,v EMIT3(0x0f,0x11,0x27)
#define sse_storexmm5(v) __asm mov edi,v EMIT3(0x0f,0x11,0x2f)
#define sse_storexmm6(v) __asm mov edi,v EMIT3(0x0f,0x11,0x37)
#define sse_storexmm7(v) __asm mov edi,v EMIT3(0x0f,0x11,0x3f)


//
//
//	SSE2 Instructions
//
//
#define sse2_addpd(reg)			EMIT4(0x66,0x0f,0x58,reg)
#define sse2_addsd(reg)			EMIT4(0xf2,0x0f,0x58,reg)
#define sse2_andpd(reg)			EMIT4(0x66,0x0f,0x54,reg)
#define sse2_andnpd(reg)		EMIT4(0x66,0x0f,0x55,reg)

#define sse2_cmppd(reg,op)		EMIT4(0x66,0x0f,0xc2,reg) EMIT(op)
#define sse2_cmpeqpd(reg)		sse_cmppd(reg,SSE_EQUAL)
#define sse2_cmpltpd(reg)		sse_cmppd(reg,SSE_LOWER)
#define sse2_cmplepd(reg)		sse_cmppd(reg,SSE_LOWER_EQUAL)
#define sse2_cmpunordpd(reg)	sse_cmppd(reg,SSE_QNAN)
#define sse2_cmpneqpd(reg)		sse_cmppd(reg,SSE_NOT_EQUAL)
#define sse2_cmpgepd(reg)		sse_cmppd(reg,SSE_NOT_LOWER)
#define sse2_cmpgtpd(reg)		sse_cmppd(reg,SSE_NOT_LOWER_EQUAL)
#define sse2_cmpordpd(reg)		sse_cmppd(reg,SSE_NOT_QNAN)

#define sse2_cmpsd(reg,op)		EMIT4(0xf2,0x0f,0xc2,reg) EMIT(op)
#define sse2_cmpeqsd(reg)		sse_cmpsd(reg,SSE_EQUAL)
#define sse2_cmpltsd(reg)		sse_cmpsd(reg,SSE_LOWER)
#define sse2_cmplesd(reg)		sse_cmpsd(reg,SSE_LOWER_EQUAL)
#define sse2_cmpunordsd(reg)	sse_cmpsd(reg,SSE_QNAN)
#define sse2_cmpneqsd(reg)		sse_cmpsd(reg,SSE_NOT_EQUAL)
#define sse2_cmpgesd(reg)		sse_cmpsd(reg,SSE_NOT_LOWER)
#define sse2_cmpgtsd(reg)		sse_cmpsd(reg,SSE_NOT_LOWER_EQUAL)
#define sse2_cmpordsd(reg)		sse_cmpsd(reg,SSE_NOT_QNAN)

#define sse2_comisd(reg)		EMIT4(0x66,0x0f,0x2f,reg)
#define sse2_cvtdq2pd(reg)		EMIT4(0xf3,0x0f,0xe6,reg)
#define sse2_cvtpd2dq(reg)		EMIT4(0xf2,0x0f,0xe6,reg)
#define sse2_cvtpd2pi(reg)		EMIT4(0x66,0x0f,0x2d,reg)
#define sse2_cvtpd2ps(reg)		EMIT4(0x66,0x0f,0x5a,reg)
#define sse2_cvtpi2pd(reg)		EMIT4(0x66,0x0f,0x2a,reg)
#define sse2_cvtps2dq(reg)		EMIT4(0x66,0x0f,0x5b,reg)
#define sse2_cvtps2pd(reg)		EMIT3(0x0f,0x5a,reg)
#define sse2_cvtsd2si(reg)		EMIT4(0xf2,0x0f,0x2d,reg)
#define sse2_cvtsd2ss(reg)		EMIT4(0xf2,0x0f,0x5a,reg)
#define sse2_cvtsi2sd(reg)		EMIT4(0xf2,0x0f,0x2a,reg)
#define sse2_cvtss2sd(reg)		EMIT4(0xf3,0x0f,0x5a,reg)
#define sse2_cvttpd2pi(reg)		EMIT4(0x66,0x0f,0x2c,reg)
#define sse2_cvttpd2dq(reg)		EMIT4(0x66,0x0f,0xe6,reg)
#define sse2_cvttps2dq(reg)		EMIT4(0xf3,0x0f,0x5b,reg)
#define sse2_cvttsd2si(reg)		EMIT4(0xf2,0x0f,0x2c,reg)

#define sse2_divpd(reg)			EMIT4(0x66,0x0f,0x5e,reg)
#define sse2_divsd(reg)			EMIT4(0xf2,0x0f,0x5e,reg)

#define sse2_maxpd(reg)			EMIT4(0x66,0x0f,0x5f,reg)
#define sse2_maxsd(reg)			EMIT4(0xf2,0x0f,0x5f,reg)
#define sse2_minpd(reg)			EMIT4(0x66,0x0f,0x5d,reg)
#define sse2_minsd(reg)			EMIT4(0xf2,0x0f,0x5d,reg)

//#define sse2_movapd(reg)		
#define sse2_loadapd(reg)		EMIT4(0x66,0x0f,0x28,reg)
#define sse2_storeapd(reg)		EMIT4(0x66,0x0f,0x29,reg)
//#define sse2_movdqa(reg)		
#define sse2_loaddqa(reg)		EMIT4(0x66,0x0f,0x6f,reg)
#define sse2_storedqa(reg)		EMIT4(0x66,0x0f,0x7f,reg)
//#define sse2_movdqu(reg)		
#define sse2_loaddqu(reg)		EMIT4(0xf3,0x0f,0x6f,reg)
#define sse2_storedqu(reg)		EMIT4(0xf3,0x0f,0x7f,reg)

#define sse2_movdq2q(reg)		EMIT4(0xf2,0x0f,0xd6,reg)

//#define sse2_movhpd(reg)		
#define sse2_loadhpd(reg)		EMIT4(0x66,0x0f,0x16,reg)
#define sse2_storehpd(reg)		EMIT4(0x66,0x0f,0x17,reg)
//#define sse2_movlpd(reg)		
#define sse2_loadlpd(reg)		EMIT4(0x66,0x0f,0x12,reg)
#define sse2_storelpd(reg)		EMIT4(0x66,0x0f,0x13,reg)

#define sse2_movmskpd(reg)		EMIT4(0x66,0x0f,0x50,reg)
#define sse2_movntdq(reg)		EMIT4(0x66,0x0f,0xe7,reg)
#define sse2_movni(reg)			EMIT2(0x0f,0xc3,reg)
#define sse2_movntpd(reg)		EMIT4(0x66,0x0f,0x2b,reg)
#define sse2_movq2dq(reg)		EMIT4(0xf3,0x0f,0xd6,reg)

//#define sse2_movupd(reg)		
#define sse2_loadupd(reg)		EMIT4(0x66,0x0f,0x10,reg)
#define sse2_storeupd(reg)		EMIT4(0x66,0x0f,0x11,reg)

#define sse2_mulpd(reg)			EMIT4(0x66,0x0f,0x59,reg)
#define sse2_mulsd(reg)			EMIT4(0xf2,0x0f,0x59,reg)
#define sse2_orpd(reg)			EMIT4(0x66,0x0f,0x56,reg)

#define sse2_shufpd(reg,order)	EMIT4(0x66,0x0f,0xc6,reg) EMIT(order)
#define sse2_sqrtpd(reg)		EMIT4(0x66,0x0f,0x51,reg)
#define sse2_sqrtsd(reg)		EMIT4(0xf2,0x0f,0x51,reg)
#define sse2_subpd(reg)			EMIT4(0x66,0x0f,0x5c,reg)
#define sse2_subsd(reg)			EMIT4(0xf2,0x0f,0x5c,reg)

#define sse2_ucomisd(reg)		EMIT4(0x66,0x0f,0x2e,reg)
#define sse2_unpckhpd(reg)		EMIT4(0x66,0x0f,0x15,reg)
#define sse2_unpcklpd(reg)		EMIT4(0x66,0x0f,0x14,reg)

#define sse2_xorpd(reg)			EMIT4(0x66,0x0f,0x57,reg)



#endif

