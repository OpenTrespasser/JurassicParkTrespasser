/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *			Assembler instruction macros for instructions that the compiler does not have
 *			instructions for.
 *
 * Notes:
 *			The macros in this file will only assemble in the build version of the processor
 *			the instructions belong to. For example, cmoveq will give an error if not a Pentium
 *			Pro or Pentium II,
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/gblinc/AsmSupport.hpp                                                 $
 * 
 * 6     98.09.16 9:41p Mmouni
 * Now only defines rdpmc, rdmsr, rdtsc if the compiler version is earlier than 6.0.
 * 
 * 5     9/07/98 12:05a Asouth
 * Added #ifdef for MW support of K6 instructions in inline asm
 * 
 * 4     97.10.27 1:12p Mmouni
 * Added support for AMD K6-3D.
 * 
 * 3     9/30/97 9:44p Mmouni
 * Added support for simulating Pentium Pro specific instructions on a Pentium.
 * 
 * 2     8/15/97 12:46a Rwyatt
 * Removed a bug from fcmovbe macro
 * 
 * 1     8/14/97 2:59p Rwyatt
 * Initial implementation of just P6 instuctions
 * 
 **********************************************************************************************/

#ifndef HEADER_GBINC_ASMSUPPORT_HPP
#define HEADER_GBINC_ASMSUPPORT_HPP

#include "Config.hpp"


// Define to simulate Pentium Pro extensions.
#define SIMULATE_EXTENSIONS (0)


// address/register size control
#define regsize	_asm _emit 0x66
#define REGSIZE	_asm _emit 0x66


#define adrsize	_asm _emit 0x67
#define ADRSIZE	_asm _emit 0x67


//**********************************************************************************************
// Define the instructions for a Pentium Pro and make any Pentium/AMD specific instructions
// give a compile error
//
#if (TARGET_PROCESSOR == PROCESSOR_PENTIUMPRO)


#if (SIMULATE_EXTENSIONS)

//**********************************************************************************************
// Use real register names for simulated instructions.
//
#define _EAX eax
#define _ECX ecx
#define _EDX edx
#define _EBX ebx
#define _ESP esp
#define _EBP ebp
#define _ESI esi
#define _EDI edi
#define _eax eax
#define _ecx ecx
#define _edx edx
#define _ebx ebx
#define _esp esp
#define _ebp ebp
#define _esi esi
#define _edi edi

// Floating point registers
#define	_ST0 ST(0)
#define	_ST1 ST(1)
#define	_ST2 ST(2)
#define	_ST3 ST(3)
#define	_ST4 ST(4)
#define	_ST5 ST(5)
#define	_ST6 ST(6)
#define	_ST7 ST(7)
#define	_st0 ST(0)
#define	_st1 ST(1)
#define	_st2 ST(2)
#define	_st3 ST(3)
#define	_st4 ST(4)
#define	_st5 ST(5)
#define	_st6 ST(6)
#define	_st7 ST(7)


//**********************************************************************************************
// Simulate CMOV instructions
#define cmova(dst,src)\
	_asm jna $+8 \
	_asm mov dst,src

#define cmovae(dst,src)\
	_asm jnae $+8 \
	_asm mov dst,src

#define cmovb(dst,src)\
	_asm jnb $+8 \
	_asm mov dst,src

#define cmovbe(dst,src)\
	_asm jnbe $+8 \
	_asm mov dst,src

#define cmovc(dst,src)\
	_asm jnc $+8 \
	_asm mov dst,src

#define cmove(dst,src)\
	_asm jne $+8 \
	_asm mov dst,src

#define cmovg(dst,src)\
	_asm jng $+8 \
	_asm mov dst,src

#define cmovge(dst,src)\
	_asm jnge $+8 \
	_asm mov dst,src

#define cmovl(dst,src)\
	_asm jnl $+8 \
	_asm mov dst,src

#define cmovle(dst,src)\
	_asm jnle $+8 \
	_asm mov dst,src

#define cmovna(dst,src)\
	_asm ja $+8 \
	_asm mov dst,src

#define cmovnae(dst,src)\
	_asm jae $+8 \
	_asm mov dst,src

#define cmovnb(dst,src)\
	_asm jb $+8 \
	_asm mov dst,src

#define cmovnbe(dst,src)\
	_asm jbe $+8 \
	_asm mov dst,src

#define cmovnc(dst,src)\
	_asm jc $+8 \
	_asm mov dst,src

#define cmovne(dst,src)\
	_asm je $+8 \
	_asm mov dst,src

#define cmovng(dst,src)\
	_asm jg $+8 \
	_asm mov dst,src

#define cmovnge(dst,src)\
	_asm jge $+8 \
	_asm mov dst,src

#define cmovnl(dst,src)\
	_asm jl $+8 \
	_asm mov dst,src

#define cmovnle(dst,src)\
	_asm jle $+8 \
	_asm mov dst,src

#define cmovno(dst,src)\
	_asm jo $+8 \
	_asm mov dst,src

#define cmovnp(dst,src)\
	_asm jp $+8 \
	_asm mov dst,src

#define cmovns(dst,src)\
	_asm js $+8 \
	_asm mov dst,src

#define cmovnz(dst,src)\
	_asm jz $+8 \
	_asm mov dst,src

#define cmovo(dst,src)\
	_asm jno $+8 \
	_asm mov dst,src

#define cmovp(dst,src)\
	_asm jnp $+8 \
	_asm mov dst,src

#define cmovpe(dst,src)\
	_asm jpo $+8 \
	_asm mov dst,src

#define cmovpo(dst,src)\
	_asm jpe $+8 \
	_asm mov dst,src

#define cmovs(dst,src)\
	_asm jns $+8 \
	_asm mov dst,src

#define cmovz(dst,src)\
	_asm jnz $+8 \
	_asm mov dst,src


//**********************************************************************************************
// Simulate floating point conditional moves
#define fcmovb(x)\
	_asm jnb $+10 \
	_asm fld x \
	_asm fstp st(1)

#define fcmove(x)\
	_asm jne $+10 \
	_asm fld x \
	_asm fstp st(1)

#define fcmovbe(x)\
	_asm jnbe $+10 \
	_asm fld x \
	_asm fstp st(1)

#define fcmovu(x)\
	_asm jnp $+10 \
	_asm fld x \
	_asm fstp st(1)

#define fcmovnb(x)\
	_asm jb $+10 \
	_asm fld x \
	_asm fstp st(1)

#define fcmovne(x)\
	_asm je $+10 \
	_asm fld x \
	_asm fstp st(1)

#define fcmovnbe(x)\
	_asm jbe $+10 \
	_asm fld x \
	_asm fstp st(1)

#define fcmovnu(x)\
	_asm jp $+10 \
	_asm fld x \
	_asm fstp st(1)


//**********************************************************************************************
// Simulate floating point compare setting integer flags
#define fcomi(x)\
	_asm fcom x \
	_asm push eax \
	_asm fstsw ax \
	_asm sahf \
	_asm pop eax

#define fcomip(x)\
	_asm fcomp x \
	_asm push eax \
	_asm fstsw ax \
	_asm sahf \
	_asm pop eax

#define fucomi(x)\
	_asm fucom x \
	_asm push eax \
	_asm fstsw ax \
	_asm sahf \
	_asm pop eax

#define fucomip(x)\
	_asm fucomp x \
	_asm push eax \
	_asm fstsw ax \
	_asm sahf \
	_asm pop eax


#else // (SIMULATE_EXTENSIONS)


//**********************************************************************************************
// Our macro registers have to be given underscores to prevent us macro-ing away the real
// register names, this also applies to a macro called st(x) or ST(x)
//
#define _EAX 0x00
#define _ECX 0x01
#define _EDX 0x02
#define _EBX 0x03
#define _ESP 0x04
#define _EBP 0x05
#define _ESI 0x06
#define _EDI 0x07
#define _eax 0x00
#define _ecx 0x01
#define _edx 0x02
#define _ebx 0x03
#define _esp 0x04
#define _ebp 0x05
#define _esi 0x06
#define _edi 0x07

// Floating point registers
#define	_ST0 0x00
#define	_ST1 0x01
#define	_ST2 0x02
#define	_ST3 0x03
#define	_ST4 0x04
#define	_ST5 0x05
#define	_ST6 0x06
#define	_ST7 0x07
#define	_st0 0x00
#define	_st1 0x01
#define	_st2 0x02
#define	_st3 0x03
#define	_st4 0x04
#define	_st5 0x05
#define	_st6 0x06
#define	_st7 0x07


//**********************************************************************************************
// CMOV instructions
#define cmova(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x47 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovae(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x43 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovb(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x42 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovbe(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x46 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovc(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x42 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmove(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x44 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovg(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4F \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovge(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4D \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovl(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4C \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovle(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4E \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovna(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x46 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovnae(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x42 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovnb(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x43 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovnbe(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x47 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovnc(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x43 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovne(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x45 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovng(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4E \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovnge(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4C \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovnl(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4D \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovnle(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4F \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovno(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x41 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovnp(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4B \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovns(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x49 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovnz(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x45 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovo(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x40 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovp(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4A \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovpe(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4A \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovpo(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4B \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovs(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x48 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovz(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x44 \
	_asm _emit 0xC0 | (dst<<3) | (src)


//**********************************************************************************************
// Floating point conditional moves
#define fcmovb(x)\
	_asm _emit 0xDA\
	_asm _emit 0xC0+(x)

#define fcmove(x)\
	_asm _emit 0xDA\
	_asm _emit 0xC8+(x)

#define fcmovbe(x)\
	_asm _emit 0xDA\
	_asm _emit 0xD0+(x)

#define fcmovu(x)\
	_asm _emit 0xDA\
	_asm _emit 0xD8+(x)

#define fcmovnb(x)\
	_asm _emit 0xDB\
	_asm _emit 0xC0+(x)

#define fcmovne(x)\
	_asm _emit 0xDB\
	_asm _emit 0xC8+(x)

#define fcmovnbe(x)\
	_asm _emit 0xDB\
	_asm _emit 0xD0+(x)

#define fcmovnu(x)\
	_asm _emit 0xDB\
	_asm _emit 0xD8+(x)


//**********************************************************************************************
// Floating point compare setting interger flags
#define fcomi(x)\
	_asm _emit 0xDB\
	_asm _emit 0xF0+(x)

#define fcomip(x)\
	_asm _emit 0xDF\
	_asm _emit 0xF0+(x)

#define fucomi(x)\
	_asm _emit 0xDB\
	_asm _emit 0xE8+(x)

#define fucomip(x)\
	_asm _emit 0xDF\
	_asm _emit 0xE8+(x)


#if _MSC_VER < 1200

//**********************************************************************************************
// Misc additional instructions
#define rdpmc \
	_asm _emit 0x0f\
	_asm _emit 0x33

#define rdmsr \
	_asm _emit 0x0f\
	_asm _emit 0x32

#define rdtsc \
	_asm _emit 0x0f\
	_asm _emit 0x31


#endif

//**********************************************************************************************
// Upper case versions of the above macros...
#define CMOVA(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x47 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVAE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x43 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVB(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x42 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVBE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x46 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVC(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x42 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x44 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVG(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4F \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVGE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4D \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define cmovl(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4C \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVLE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4E \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNA(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x46 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNAE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x42 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNB(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x43 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNBE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x47 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNC(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x43 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x45 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNG(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4E \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNGE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4C \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNL(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4D \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNLE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4F \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNO(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x41 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNP(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4B \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNS(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x49 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVNZ(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x45 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVO(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x40 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVP(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4A \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVPE(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4A \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVPO(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x4B \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVS(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x48 \
	_asm _emit 0xC0 | (dst<<3) | (src)

#define CMOVZ(dst,src)\
	_asm _emit 0x0f \
	_asm _emit 0x44 \
	_asm _emit 0xC0 | (dst<<3) | (src)


//**********************************************************************************************
// Floating point conditional moves
#define FCMOVB(x)\
	_asm _emit 0xDA\
	_asm _emit 0xC0+(x)

#define FCMOVE(x)\
	_asm _emit 0xDA\
	_asm _emit 0xC8+(x)

#define FCMOVBE(x)\
	_asm _emit 0xDA\
	_asm _emit 0xDO+(x)

#define FCMOVU(x)\
	_asm _emit 0xDA\
	_asm _emit 0xD8+(x)

#define FCMOVNB(x)\
	_asm _emit 0xDB\
	_asm _emit 0xC0+(x)

#define FCMOVNE(x)\
	_asm _emit 0xDB\
	_asm _emit 0xC8+(x)

#define FCMOVNBE(x)\
	_asm _emit 0xDB\
	_asm _emit 0xDO+(x)

#define FCMOVNU(x)\
	_asm _emit 0xDB\
	_asm _emit 0xD8+(x)


//**********************************************************************************************
// Floating point compare setting interger flags
#define FCOMI(x)\
	_asm _emit 0xDB\
	_asm _emit 0xF0+(x)

#define FCOMIP(x)\
	_asm _emit 0xDF\
	_asm _emit 0xF0+(x)

#define FUCOMI(x)\
	_asm _emit 0xDB\
	_asm _emit 0xE8+(x)

#define FUCOMIP(x)\
	_asm _emit 0xDF\
	_asm _emit 0xE8+(x)


#endif // (SIMULATE_EXTENSIONS)


#if _MSC_VER < 1200

//**********************************************************************************************
// Misc additional instructions
#define RDPMC \
	_asm _emit 0x0f\
	_asm _emit 0x33

#define RDMSR \
	_asm _emit 0x0f\
	_asm _emit 0x32

#define RDTSC \
	_asm _emit 0x0f\
	_asm _emit 0x31

#endif

#endif //(TARGET_PROCESSOR == PROCESSOR_PENTIUMPRO)



//**********************************************************************************************
// Define the instructions for a Pentium and make any Pentium Pro/AMD specific instructions
// give a compile error
//
#if (TARGET_PROCESSOR == PROCESSOR_PENTIUM)


//**********************************************************************************************
// Misc instructions

#if _MSC_VER < 1200

// MMX Only
#define rdpmc \
	_asm _emit 0x0f\
	_asm _emit 0x33

#define rdmsr \
	_asm _emit 0x0f\
	_asm _emit 0x32

#define rdtsc \
	_asm _emit 0x0f\
	_asm _emit 0x31

// MMX Only
#define RDPMC \
	_asm _emit 0x0f\
	_asm _emit 0x33

#define RDMSR \
	_asm _emit 0x0f\
	_asm _emit 0x32

#define RDTSC \
	_asm _emit 0x0f\
	_asm _emit 0x31

#endif

#endif  //(TARGET_PROCESSOR == PROCESSOR_PENTIUM)


//**********************************************************************************************
// Define the instructions for a AMD K6-3D and make Pentium/Pentium Pro specific instructions
// give a compile error
//
#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)


//**********************************************************************************************
// Misc instructions

#define rdtsc \
	_asm _emit 0x0f\
	_asm _emit 0x31

#define RDTSC \
	_asm _emit 0x0f\
	_asm _emit 0x31


//**********************************************************************************************
// AMD-K6 Model 8 extensions.

#ifdef __MWERKS__
 #include "3DX-mw.hpp"
#else
 // the Metrowerks compiler supports AMD K6-2 3DNow! instructions natively
 #include "3DX.hpp"
#endif


#endif  //(TARGET_PROCESSOR == PROCESSOR_K6_3D)


#endif
