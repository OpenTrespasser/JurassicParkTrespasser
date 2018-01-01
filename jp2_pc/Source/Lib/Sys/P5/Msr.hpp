/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Functions for reading the Pentium model specific registers.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/P5/Msr.hpp                                                    $
 * 
 * 4     10/04/98 5:50p Mmouni
 * Fixed cycle timers for metrowerks build.
 * 
 * 3     8/15/98 6:02p Mmouni
 * Changed to use rdpmc, rdtsc explicitly in VC6. Also added references to eax, edx when
 * emitting instructions.
 * 
 * 2     8/28/97 4:03p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 2     8/25/97 5:18p Rwyatt
 * Now selects between RDTSC and RDPMC based on a control variable. This is to enable access to
 * the performance counters.
 * 
 * 1     6/19/97 1:53p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_SYS_P5_MSR_HPP
#define HEADER_LIB_SYS_P5_MSR_HPP

extern uint32 gu4MSRProfileTimerSelect;
extern bool	gbUseRDTSC;


// Prefix: cy
// Count of processor cycles.
typedef int64 TCycles;


// No return value.
#pragma warning(disable: 4035)

//*****************************************************************************************
//
inline TCycles cyReadCycles()
//
// Returns:
//		The contents of the cycle register in the pentium/pentium pro.
//
// Notes:
//		The MSVC calling convention expects 64-bit return values in EDX:EAX.
//		This is exactly where the following instructions put it.
//
//**************************************
{
#if defined(__MWERKS__)
	TCycles cy_ret;
#endif
	__asm
	{
		cmp [gbUseRDTSC], 0
		jnz	short USE_RDTSC
		// use RDPMC
		mov	ecx,gu4MSRProfileTimerSelect

#if _MSC_VER >= 1200
		rdpmc
#else
		xor	eax,eax		// Make sure compiler knows these are used.
		xor edx,edx
		_emit 0x0f;
		_emit 0x33;
#endif
		
		jmp short DONE
USE_RDTSC:
		// use RDTSC

#if _MSC_VER >= 1200
		rdtsc
#else
		xor	eax,eax		// Make sure compiler knows these are used.
		xor edx,edx
		_emit 0x0f;
		_emit 0x31;
#endif

DONE:
#if defined(__MWERKS__)
		mov	dword ptr[cy_ret],eax
		mov	dword ptr[cy_ret+4],edx
#endif
	}

#if defined(__MWERKS__)
	return cy_ret;
#endif
}


#pragma warning(3: 4035)


#endif