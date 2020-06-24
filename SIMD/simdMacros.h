/***************************************************************************/
/*                                                                         */
/*  simdMacros.h                                                           */
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




#ifndef __SIMDMACROS_H__
#define __SIMDMACROS_H__


#ifdef WIN32
    /**  the fastcall method is very fast, parameters are directly passed into ecx & edx redisters. */ 
    #define SIMD_CALL   __fastcall
    /**  the naked call is even faster, no frame is constructed, pure assembler is directly inserted. */
    #define SIMD_ENTRY __declspec(naked)
    #define SIMD_API   __cdecl
#else
    #define SIMD_CALL
    #define SIMD_ENTRY
    #define SIMD_API
#endif

/*
 *  Define this macro to compile code without assembler sources.
 *  Of course only 'standard' performance code will be available.
 */
//#define SIMD_NO_ASSEMBLY

/**	No exception compatibility with c++ < 11. */
#if defined(_MSC_VER) && (_MSC_VER < 1900) && !defined(NOEXCEPT)
	/*_NOEXCEPT : depending on core generation, it may be faster to avoid declaring throw()
	 *  With Visual Studio 2013, no penalty for this declaration.
	 */
	#define NOEXCEPT throw() 
#elif defined(LINUX)
	#define NOEXCEPT throw()
#else
	#define NOEXCEPT noexcept
#endif

//	Include intrinsics if available
#ifndef SIMD_NO_ASSEMBLY
	#include <emmintrin.h>
#endif

//
//
//	CPU status
//
//
typedef struct CPUINFO
{
	// CPU informations
	char			features[0x20];
	short			stepping;
	short			model;
	short			eModel;
	short			family;
	int				eFamily;
	unsigned char	type;
	int				brand;
	int				cacheLineFlush;
	int				maxLogicalAPIC;
	int				apicId;
	char			processorBrand[0x60];
	int				serialNumber[4];
	bool			hyperThreading;
	int				numThreads;
	int				numCores;

	typedef enum
	{
		NONE,
		DATA_TLB,
		INSTRUCTION_TLB,
		DATA_CACHE,
		INSTRUCTION_CACHE,
		TRACE_CACHE,
		CACHE
	} CACHE_TYPE;

	typedef struct CACHEDESCRIPTOR
	{
		CACHE_TYPE		type;	// data, instruction, cache
		unsigned int	level;	// none, L1, L2, L3
		unsigned int	size;	// size or page size in KBytes or uops
		unsigned int	ways;	// associative set, 0 means fully
		unsigned int	entries;// TLB entries or line size in bytes
	} CACHE_DESCRIPTOR;

	struct
	{
		int prefetchingSize;	// in bytes
		int	nbDescriptors;
		CACHE_DESCRIPTOR* descriptors;
	} cacheDescriptor;

	typedef struct CACHEPARAMETER
	{
		unsigned int	numAPICIds;
		unsigned int	maxThreads;
		bool			fullAssociative;
		bool			selfInitialized;
		unsigned int	level;
		CACHE_TYPE		type;
		unsigned int	ways;
		unsigned int	partitions;
		unsigned int	lineSize;
		unsigned int	sets;
		bool			inclusiveLowerLevels;
		bool			invdBehavior;
		unsigned int	cacheSize;
	} CACHE_PARAMETER;

	struct
	{
		int nbCacheLevels;
		CACHE_PARAMETER* parameters;
	} cacheParameters;

	struct
	{
		unsigned short	minMonitorSize;
		unsigned short	maxMonitorSize;
		bool			monitorExtensions;
		bool			monitorInterrupts;
		unsigned char	monitorC0States;
		unsigned char	monitorC1States;
		unsigned char	monitorC2States;
		unsigned char	monitorC3States;
		unsigned char	monitorC4States;
		unsigned char	monitorC5States;
		unsigned char	monitorC6States;
		unsigned char	monitorC7States;
	} monitors;

	struct
	{
		bool			digitalTemperatureSensor;
		bool			turboBoost;
		bool			APICTimer;	// Timer always run
		bool			PLN;		// powerLimitNotification
		bool			ECMD;		// clock modulation duty cycle
		bool			PTM;		// package thermal modulation
		bool			HWP;		// hardware HWP base registers
		bool			HWPNotification;
		bool			HWPActivityWindow;
		bool			HWPEnergyPreference;
		bool			HWPPackageLevel;
		bool			HDC;		// hardware HDC base registers
		unsigned char	numInterrupts;
		bool			HWCoordination;
		bool			energyBias;
	} thermal;

	// CPU capabilities
	bool			CPUID;	// has cpuid instruction
	unsigned int	featureFlagEBX;
	unsigned int	featureFlagECX;
	unsigned int	featureFlagEDX;

	typedef enum FEATURE_EDX_t
	{
		FPU,	// FPU on chip
		VME,	// 8086 virtual mode
		DE,		// Debugging extension
		PSE,	// 4Mb page size extension
		TSC,	// time stamp counter
		MSR,	// model specific registers
		PAE,	// physical address greater then 32 bits
		MCE,	// machine check exception
		CMPXCHG8B,
		APIC,	// on chip APIC hardware
		RESERVED,
		SEP,	// fast system calls
		MTRR,	// memory type range registers
		PGE,	// page global enable
		MCA,	// machine check architecture
		CMOV,
		PAT,	// page attribute table
		PSE_36,	// 36 bits page size extension
		PSN,	// processor serial number
		CLFSH,	// cache line flush
		RESERVED2,
		DS,		// debug store
		ACPI,
		MMX,
		FXSR,	// FXSAVE & FXSTOR
		SSE,
		SSE2,
		SS,		// self snoop
		HTT,
		TM,		// thermal monitor
		RESERVED3,
		PBE		// pending break enable
	} FEATURE_EDX;

	typedef enum FEATURE_ECX_t
	{
		SSE3,
		PCLMULDQ,	// PCLMULDQ instruction
		DTES64,		// 64 bits debug store
		MONITOR,	// monitor / mwait
		DS_CPL,		// CPL Qualified debug store
		VMX,		// virtual machine extension
		SMX,		// safer mode extension (trusted mode execution)
		EST,		// enhanced speed step technology
		TM2,		// thermal monitor 2
		SSSE3,
		CNXT_ID,	// L1 context id
		RESERVED4,
		FMA,		// intel cpuid manual says RESERVED5
		CMPXCHG16B,
		XTPR,		// xTPR update control
		PDCM,		// perfom and debug capability
		RESERVED6,
		RESERVED7,
		DCA,		// direct cache access
		SSE41,
		SSE42,
		X2APIC,		// extended apic support
		MOVBE,		// movbe instruction
		POPCNT,		// popcnt instruction
		RESERVED8,
		AES,
		XSAVE,		// xsave / xstor states
		OSXSAVE,	// os enabled extended state management
		AVX,		// intel cpuid manual says RESERVED9
		RESERVED10,
		RESERVED11,
		RESERVED12
	} FEATURE_ECX;

	typedef enum FEATURE_EBX_t
	{
		FSGSBASE,
		IA32_TSC_ADJUST_MSR,
		SGX,
		BMI1,
		HLE,
		AVX2,
		FDP_EXCPTN_ONLY,
		SMEP,
		BMI2,
		REP_MOVSB_STOSB,
		INVPCID,
		RTM,
		RDT_M,
		FPU_CS_DS,
		MPX,
		RDT_A,
		RESERVED13,
		RESERVED14,
		RDSEED,
		ADX,
		SMAP,
		RESERVED15,
		RESERVED16,
		CLFLUSHOPT,
		CLWB,
		IntelProcessorTrace,
		RESERVED17,
		RESERVED18,
		RESERVED19,
		SHA,
		RESERVED20,
		RESERVED21,
	} FEATURE_EBX;

	bool hasFeature(FEATURE_EDX_t f) const NOEXCEPT
	{
		return ((featureFlagEDX >> f) & 1);
	};
	
	bool hasFeature(FEATURE_ECX_t f) const NOEXCEPT
	{
		return ((featureFlagECX >> f) & 1);
	};

	bool hasFeature(FEATURE_EBX_t f) const NOEXCEPT
	{
		return ((featureFlagEBX >> f) & 1);
	};

} CPU_INFO;

const CPU_INFO& SIMD_API getCPUINFO();



#endif	// __SIMDMACROS_H__

