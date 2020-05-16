/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	Processor detect DLL
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/Processor Detect/Detect.cpp                                     $
 * 
 * 3     9/23/98 12:11p Rwyatt
 * 
 * 2     9/23/98 12:11p Rwyatt
 * Added support for K6-3and K7. Detection code is now future proof with respect to new models.
 * 
 * 1     9/01/98 12:31a Rwyatt
 * Added support for IDT WinChip and 3d now instructions are identified by feature and not
 * maker.
 * 
 * 3     10/24/97 1:51a Rwyatt
 * Changed the 3DX/Cedar ID string to make it different from the standard K6.
 * 
 * 2     10/24/97 1:17a Rwyatt
 * Now detects K6 3DX/Cedar extentions
 * 
 * 1     10/24/97 1:16a Rwyatt
 * Initial implementation
 * 
 ***********************************************************************************************/

#include "Windows.h"

#ifdef _BUILD_EXTERNAL_
#include "..\DWITypes.h"
#include "..\Processor.hpp"
#else
#include "Lib/Std/UTypes.hpp"
#include "Lib/Sys/Processor.hpp"
#endif


//*********************************************************************************************
// This ststic is filled and then copied onto the one passed in, this is so we get another
// free register within the code.
//
static CPUInfo	cpu;
static char		strIntel[]	= "GenuineIntel"; 
static char		strAMD[]	= "AuthenticAMD";
static char		strCyrix[]	= "CyrixInstead";
static char		strIDT[]	= "CentaurHauls";

static void SetGenericFlags(void);
static void DetectProcessor(void);
static void DetectFPU(void);
static uint32 u4CPUSpeed(int clocks);
static void MakeName(void);
static uint32 u4QueryCPUIDName(void);


//*********************************************************************************************
//	Start of detection code
//*********************************************************************************************

//*********************************************************************************************
//
bool WINAPI DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{
	return TRUE;
}


//*********************************************************************************************
// Declare the function below as extern "C" so it does not get its name mangled. This is the
// function that will be used in the call to GetProcAddress.
//
extern "C"
{
__declspec(dllexport) bool _cdecl bGetProcessorInfo(CPUInfo* pcpuProcessorInfo);
}


//*********************************************************************************************
//
__declspec(dllexport) bool _cdecl bGetProcessorInfo(CPUInfo* pcpuProcessorInfo)
{  	
	// clear the destination structure.
	memset(&cpu,0,sizeof(CPUInfo));

	// we don't know anything about the chip
	cpu.cpufamProcessorFamily		= cpufamUNKNOWN;
	cpu.cpufamFPUFamily				= cpufamUNKNOWN;
	cpu.cpumanProcessorManufacture	= cpumanUNKNOWN;

	DetectProcessor();	

	if (cpu.cpufamProcessorFamily == cpufamUNKNOWN)
		return false;

	// we need to do some adjusting for the non-Intel processors.

	if (cpu.cpumanProcessorManufacture == cpumanAMD)
	{
		if (cpu.cpufamProcessorFamily == 4)
		{
			// a model 4 AMD is a AM486
			cpu.cpufamProcessorFamily = cpufamAM486;
		}
		else if (cpu.cpufamProcessorFamily == 5)
		{
			// a model 5 AMD is either a K5 or K6, we need to check the stepping
			switch ((cpu.u4Model & (0x0f<<4))>>4)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				cpu.cpufamProcessorFamily = cpufamK5;
				break;
			case 4:
			case 5:
				cpu.cpufamProcessorFamily = cpufamUNKNOWN;
				break;
			case 6:
			case 7:
				cpu.cpufamProcessorFamily = cpufamK6;
				break;
			case 8:
				cpu.cpufamProcessorFamily = cpufamK62;
				break;
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				cpu.cpufamProcessorFamily = cpufamK63;
				break;
			}
		}
		else if (cpu.cpufamProcessorFamily >= 6)
		{
			// Family model 6 on AMD are at least a K7
			cpu.cpufamProcessorFamily = cpufamK7;
		}
	}


	if (cpu.cpumanProcessorManufacture == cpumanCYRIX)
	{
		if (cpu.cpufamProcessorFamily == 5)
		{
			// model 5 Cyrix is the 686
			cpu.cpufamProcessorFamily = cpufam686;
		}
		else if (cpu.cpufamProcessorFamily == 6)
		{
			// model 6 Cyrix is the M2
			cpu.cpufamProcessorFamily = cpufamM2;
		}
		else if (cpu.cpufamProcessorFamily > 6)
		{
			// Future family is at least an M2
			cpu.cpufamProcessorFamily = cpufamM2;
		}
		else
		{
			// If it is cyrix of model 4 or less the we do not know what
			// it is.
			cpu.cpufamProcessorFamily = cpufamUNKNOWN;
		}
	}


	if (cpu.cpumanProcessorManufacture == cpumanIDT)
	{
		if (cpu.cpufamProcessorFamily == 5)
		{
			// a pentium class IDT chip is either a C6 or a WinChip
			if ( (cpu.u4Model & (0x0f<<4))>>4 >= 8 )
			{
				cpu.cpufamProcessorFamily = cpufamWINCHIP2;
			}
			else if ( (cpu.u4Model & (0x0f<<4))>>4 == 4 )
			{
				cpu.cpufamProcessorFamily = cpufamWINCHIPC6;
			}
			else
			{
				cpu.cpufamProcessorFamily = cpufamUNKNOWN;
			}
		}
		else if (cpu.cpufamProcessorFamily > 5)
		{
			// Future winchips muct be compatible with the WinChip2 so return it
			cpu.cpufamProcessorFamily = cpufamWINCHIP2;
		}
		else
		{
			// IDT will never make a processor that has a family type less than 5.
			// If they do then we have no idea what it is or what it is compatible
			// with.
			cpu.cpufamProcessorFamily = cpufamUNKNOWN;
		}

	}

	// now we have identified the processor lets go and look for an FPU
	// if CPUID is supported then this is simple we just chekc the bit and
	// we can assume the FPU version is the same as the processor. The only
	// device that varies is the 80386 which can use the FPU of the 80286.

	if (cpu.u4CPUFlags & CPU_CPUID)
	{
		// if CPUID says that there is a FPU then it must be the same family
		// as the processor
		if (cpu.u4Feature & 1)
		{
			cpu.u4CPUFlags |= CPU_FPUPRESENT;
			cpu.cpufamFPUFamily = cpu.cpufamProcessorFamily;
		}
	}
	else
	{
		// this will detect a FPU on other processors without the CPUID instrucion
		// The 386 is a special case because it can have a 287 attached to it,
		// the DetectFPU function handles this and sets the result so only copy
		// the family type into the FPU type if we are not a 386.

		// this will also set the bit for an FPU present
		DetectFPU();

		if (cpu.u4CPUFlags & CPU_FPUPRESENT)
		{
			// if an FPU is present then set the FPU family
			if (cpu.cpufamProcessorFamily != 3)
			{
				cpu.cpufamFPUFamily = cpu.cpufamProcessorFamily;
			}
		}
	}


	// Now lets derive the cache information in this machine
	if (cpu.cpumanProcessorManufacture == cpumanINTEL)
	{
		// with an intel processor we much check if we can call CPUID with an input of 2
		if (cpu.u4MaxCPUID>=2)
		{
			char	cache[16];
			int		i;

			// now call CPUID with an input of 2 to get the descriptors
			cpu.u4CPUFlags |= CPU_CACHEINFO;

			_asm
			{
				mov		eax,2
				cpuid

				mov		cl,al
				dec		cl
				je		short GOT_INTEL_CACHE
				// ECX is the number of times we need to call CPUID
NEXT_ID:
				mov		eax,2
				cpuid

				dec		cl
				jnz		short NEXT_ID

GOT_INTEL_CACHE:
				// the registers are now set with the results of the CPUID[2]
				mov		DWORD PTR [cpu.au4ProcessorSpecific+0], eax
				mov		DWORD PTR [cpu.au4ProcessorSpecific+4], ebx
				mov		DWORD PTR [cpu.au4ProcessorSpecific+8], ecx
				mov		DWORD PTR [cpu.au4ProcessorSpecific+12], edx
				mov		al,0
				mov		DWORD PTR [cache+0], eax
				mov		DWORD PTR [cache+4], ebx
				mov		DWORD PTR [cache+8], ecx
				mov		DWORD PTR [cache+12], edx
			}

			for (i=0; i<16 ; i++)
			{
				// if the top bit of the cahce descriptors is clear then it is valid
				if ((cache[i] & 0x80) == 0)
				{
					switch (cache[i])
					{
					case 0x06:
						cpu.u4CodeCacheSize		= 8;
						cpu.u4CodeCacheLineSize	= 32;
						cpu.u4CodeCacheAssoc	= 4;
						break;

					case 0x08:
						cpu.u4CodeCacheSize		= 16;
						cpu.u4CodeCacheLineSize	= 32;
						cpu.u4CodeCacheAssoc	= 4;
						break;

					case 0x0A:
						cpu.u4DataCacheSize		= 8;
						cpu.u4DataCacheLineSize	= 32;
						cpu.u4DataCacheAssoc	= 2;
						break;

					case 0x0C:
						cpu.u4DataCacheSize		= 16;
						cpu.u4DataCacheLineSize	= 32;
						cpu.u4DataCacheAssoc	= 2;
						break;

					case 0x40:
						cpu.u4L2CacheSize		= 0;
						cpu.u4L2CacheLineSize	= 0;
						cpu.u4L2CacheAssoc		= 0;
						break;

					case 0x41:
						cpu.u4L2CacheSize		= 128;
						cpu.u4L2CacheLineSize	= 32;
						cpu.u4L2CacheAssoc		= 4;
						break;

					case 0x42:
						cpu.u4L2CacheSize		= 256;
						cpu.u4L2CacheLineSize	= 32;
						cpu.u4L2CacheAssoc		= 4;
						break;

					case 0x43:
						cpu.u4L2CacheSize		= 512;
						cpu.u4L2CacheLineSize	= 32;
						cpu.u4L2CacheAssoc		= 4;
						break;
					}
				}
			}
		}
	}
	else if ((cpu.cpumanProcessorManufacture == cpumanAMD) || (cpu.cpumanProcessorManufacture == cpumanIDT))
	{
		uint32	u4_level2 = 0;

		// AMD and IDT are different to obtain the cache information, both of these manufactures use
		// CPUID operation 0x80000005
		_asm
		{
			mov		eax,0x80000000			// extended AMD CPUID function
			cpuid

			cmp		eax,0x80000005
			jb		short EXIT_AMD_CACHE		// functions upto 0x80000005 must be present

			mov		eax,0x80000005			// extended AMD CPUID function
			cpuid

			mov		DWORD PTR [cpu.au4ProcessorSpecific+56], eax
			mov		DWORD PTR [cpu.au4ProcessorSpecific+60], ebx
			mov		DWORD PTR [cpu.au4ProcessorSpecific+64], ecx
			mov		DWORD PTR [cpu.au4ProcessorSpecific+68], edx

			or		DWORD PTR [cpu.u4CPUFlags],CPU_CACHEINFO

			mov		eax,0x80000000			// extended AMD CPUID function
			cpuid

			cmp		eax,0x80000006
			jb		short EXIT_AMD_CACHE		// functions upto 0x80000006 must be present

			mov		eax,0x80000006			// extended AMD CPUID function for level 2 cache info
			cpuid

			mov		DWORD PTR [cpu.au4ProcessorSpecific+72], eax
			mov		DWORD PTR [cpu.au4ProcessorSpecific+76], ebx
			mov		DWORD PTR [cpu.au4ProcessorSpecific+80], ecx
			mov		DWORD PTR [cpu.au4ProcessorSpecific+84], edx

			// set the local flag saying we got level 2 cache info
			mov		DWORD PTR [u4_level2],0xffffffff
EXIT_AMD_CACHE:
		}

		if (u4_level2!=0) 
		{
			// this device can get the size of the L2 cache
			cpu.u4L2CacheSize = (cpu.au4ProcessorSpecific[20] & 0xffff0000)>>16;
			cpu.u4L2CacheAssoc = (cpu.au4ProcessorSpecific[20] & 0x0000f000)>>12;
			cpu.u4L2CacheLineSize = (cpu.au4ProcessorSpecific[20] & 0xf);
		}
		else
		{
			// this device cannot get the size of the L2 cache because it does
			// not support CPUID 0x80000006
			cpu.u4L2CacheSize = 0xffffffff;
		}

		cpu.u4DataCacheSize = (cpu.au4ProcessorSpecific[16] & 0xff000000)>>24;
		cpu.u4DataCacheAssoc = (cpu.au4ProcessorSpecific[16] & 0x00ff0000)>>16;
		cpu.u4DataCacheLineSize = (cpu.au4ProcessorSpecific[16] & 0xff);

		cpu.u4CodeCacheSize = (cpu.au4ProcessorSpecific[17] & 0xff000000)>>24;
		cpu.u4CodeCacheAssoc = (cpu.au4ProcessorSpecific[17] & 0x00ff0000)>>16;
		cpu.u4CodeCacheLineSize = (cpu.au4ProcessorSpecific[17] & 0xff);
	}
	else
	{
		// unknown processor, cannot get cache info
		cpu.u4CPUFlags &= ~CPU_CACHEINFO;
	}

	// measure the speed of this processor
	cpu.u4CPUSpeed = u4CPUSpeed(0);

	// finish off by doing general feature determination
	switch(cpu.cpumanProcessorManufacture)
	{
	case cpumanINTEL:
		SetGenericFlags();

		// Some Pentium Pro's incorrectly set bit 11 in the feature flags when they could not do the
		// fast system call operation. This code will sort it out.
		if (cpu.u4Feature & (1<<11))
		{
			if ( (cpu.cpufamProcessorFamily == 6) && ( ((cpu.u4Model & (0x0f<<4))>>4) < 3) && ((cpu.u4Model & 0x04) < 3) )
			{
				cpu.u4CPUFlags &= ~CPU_SEP;
			}
			else
			{
				cpu.u4CPUFlags |= CPU_SEP;
			}
		}
		break;


	case cpumanAMD:
		_asm
		{
			mov		eax,0x80000000			// extended CPUID function
			cpuid
			mov		[cpu.au4ProcessorSpecific+0], eax

			cmp		eax,0x80000001
			jb		short EXIT_AMD_EXT		// functions upto 0x80000001 must be present

			mov		eax,0x80000001			// extended AMD CPUID function
			cpuid

			mov		DWORD PTR [cpu.au4ProcessorSpecific+4], edx
		}

		// SEP
		cpu.u4CPUFlags &= ~CPU_SEP;
		if (cpu.au4ProcessorSpecific[1] & (1<<11) )
		{
			cpu.u4CPUFlags |= CPU_SEP;
		}

		// PAT
		cpu.u4CPUFlags &= ~CPU_PAT;
		if (cpu.au4ProcessorSpecific[1] & (1<<16) )
		{
			cpu.u4CPUFlags |= CPU_PAT;
		}

		// 3DX/CEDAR extentions..
		if (cpu.au4ProcessorSpecific[1] & (1<<31) )
		{
			cpu.u4CPUFlags |= CPU_3DNOW;
		}

EXIT_AMD_EXT:
		SetGenericFlags();

		if ((cpu.cpufamProcessorFamily >= cpufamK5) && (cpu.cpufamProcessorFamily < cpufamK7))
		{
			//These flags are reserved on K5 and all K6 processors so the feature cannot be
			//present
			cpu.u4CPUFlags &= ~(CPU_PAE|CPU_MCA);
		}

		if (cpu.cpufamProcessorFamily == cpufamK5)
		{
			// On the K5 model 0, bit 13 (global paging extentions) is reserved and bit 9
			// is used to indicate global page extentions, which is usually used for APIC
			// support.

			// K5 Cannotsupport an APIC
			cpu.u4CPUFlags &= ~CPU_APIC;
			cpu.u4CPUFlags &= ~CPU_GPE;

			if (cpu.u4Feature & (1<<9))
			{
				cpu.u4CPUFlags |= CPU_GPE;
			}
		}
		break;


	default:
		//
		// This is either an IDT WinChip, a cyrix or a completely unknown processor
		//

		if (cpu.u4CPUFlags & CPU_CPUID)
		{
			//
			// We have a CPUID instruction, lets check some extened features that
			// this clone may support.
			//
			_asm
			{
				mov		eax,0x80000000			// extended CPUID function
				cpuid

				mov		[cpu.au4ProcessorSpecific+0], eax
				cmp		eax,0x80000001
				jb		short NO_EXT			// functions upto 0x80000001 must be present


				mov		edx,0
				mov		eax,0x80000001			// extended CPUID function
				cpuid

				cmp		eax,0
				je		short NO_EXT			// the feature word cannot be zero
				mov		DWORD PTR [cpu.au4ProcessorSpecific+4], edx
			}

			// 3DX/CEDAR extentions..
			if (cpu.au4ProcessorSpecific[1] & (1<<31) )
			{
				cpu.u4CPUFlags |= CPU_3DNOW;
			}
NO_EXT:;
		}

		//
		// Parse the flags supported by the normal CPUID feature word
		//
		SetGenericFlags();
		break;
	}	

	// make a friendly string for this processor
	MakeName();

	// copy the final results into the structure passed in.
	memcpy(pcpuProcessorInfo,&cpu,sizeof(CPUInfo));

	return true;
}


//*********************************************************************************************
//
static void SetGenericFlags(void)
{
	//
	// We have no idea who made this chip so lets just check for some general features,
	// if the CPUID instruction is present.
	//
	if (cpu.u4CPUFlags & CPU_CPUID)
	{
		// check for VME
		if (cpu.u4Feature & (1<<1))
		{
			cpu.u4CPUFlags |= CPU_VME;
		}

		// check for extened DEBUG registers
		if (cpu.u4Feature & (1<<2))
		{
			cpu.u4CPUFlags |= CPU_DEBUG;
		}

		// check for PSE
		if (cpu.u4Feature & (1<<3))
		{
			cpu.u4CPUFlags |= CPU_PSE;
		}

		// check for RDTSC
		if (cpu.u4Feature & (1<<4))
		{
			cpu.u4CPUFlags |= CPU_RDTSC;
		}

		// check for MSR
		if (cpu.u4Feature & (1<<5))
		{
			cpu.u4CPUFlags |= CPU_MSR;
		}

		// check for PAE (physical address extentions)
		if (cpu.u4Feature & (1<<6))
		{
			cpu.u4CPUFlags |= CPU_PAE;
		}

		// Machine Check exception
		if (cpu.u4Feature & (1<<7))
		{
			cpu.u4CPUFlags |= CPU_MCE;
		}

		// check for the CMPXCHG8B instruction
		if (cpu.u4Feature & (1<<8))
		{
			cpu.u4CPUFlags |= CPU_CMPXCHG8B;
		}

		// check for an APIC
		if (cpu.u4Feature & (1<<9))
		{
			cpu.u4CPUFlags |= CPU_APIC;
		}

		// check for Fast system calls
		if (cpu.u4Feature & (1<<11))
		{
			cpu.u4CPUFlags |= CPU_SEP;
		}

		// check for memory type range registers
		if (cpu.u4Feature & (1<<12))
		{
			cpu.u4CPUFlags |= CPU_MTRR;
		}

		// check for Global paging extentions
		if (cpu.u4Feature & (1<<13))
		{
			cpu.u4CPUFlags |= CPU_GPE;
		}

		// check for Machine check architecture
		if (cpu.u4Feature & (1<<14))
		{
			cpu.u4CPUFlags |= CPU_MCA;
		}

		// check for CMOV instructions
		if (cpu.u4Feature & (1<<15))
		{
			cpu.u4CPUFlags |= CPU_CMOV;
		}

		// check for Page Attribute Table
		if (cpu.u4Feature & (1<<16))
		{
			cpu.u4CPUFlags |= CPU_PAT;
		}

		// check for MMX
		if (cpu.u4Feature & (1<<23))
		{
			cpu.u4CPUFlags |= CPU_MMX;
		}
	}

	if  ( ((cpu.u4Model>>8) & 0xf) >= 5)
	{
		//Model is 5 or above so we say we can say we execute pentium code.
		cpu.u4CPUFlags |= CPU_PENTIUM;

		if (cpu.u4CPUFlags & CPU_CMOV)
		{
			// We have the Pentium pro conditional instructions so we say we
			// can execute Pentium pro code.
			cpu.u4CPUFlags |= CPU_PENTIUMPRO;
		}
	}
}



//*********************************************************************************************
//
static void DetectProcessor(void)
{
	_asm
	{
		//	CHECK 8086 : Bits 12-15 are always set on the 8086 processor
		pushad

		pushf			       					// save EFLAGS
		pop  		bx		       				// store EFLAGS in BX
		mov  		ax,0fffh	       			// clear bits 12-15
		and  		ax,bx		       			// in EFLAGS
		push		ax		       				// store new EFLAGS value on stack
		popf			       					// replace current EFLAGS value
		pushf			       					// set new eflags
		pop  		ax		       				// store new eflags in AX
		and			ax,0f000h	       			// if bits 12-15 are set then its a 8086
		cmp  		ax,0f000h	       			// test if its an 8086
		mov  		cpu.cpufamProcessorFamily,cpufam8086
		je   		short END_CPUID				// if CPU is 8086/8088 check for 8087 FPU


		// Check for 80286 
		// Bits 12-15 are always clear on the 80286 processor

		pushf									// Keep a copy of the flags
		pushf
		pop			cx
		mov			bx, cx
		or			cx, 0f000h					// Try to set bits 12-15
	    push		cx							// Save new FLAGS value on stack
		popf									// Replace current FLAGS value
		pushf									// Get new FLAGS
		pop			ax							// Store new FLAGS in AX
		popf									// restore original flags to prevent task switch
        and			ax, 0f000h					// If bits 12-15 are clear
        mov			cpu.cpufamProcessorFamily,cpufam80286
		je			short END_CPUID


		//	Intel386 CPU check
		//	The AC bit, bit #18, is a new bit introduced in the EFLAGS
		//	register on the Intel386 DX CPU to generate alignment faults.
		//	This bit can not be set on the Intel386 CPU

		pushfd
		pop			eax							// get original EFLAGS
		mov			ecx,eax						// save origninal EFLAGS
		xor			eax,40000h					// flip AC bit in EFLAGS
		push		eax							// save for EFLAGS
		popfd									// copy to EFLAGS
		pushfd									// push EFLAGS
		pop			eax							// get new EFLAGS value
		xor			eax,ecx						// cant toggle AC bit, CPU=Intel386
		mov  		cpu.cpufamProcessorFamily,cpufam80386
		je			short END_CPUID


		//	Intel486 DX CPU, Intel487 SX MCP, and Intel486 SX CPU checking
		//	Checking for ability to set/clear ID flag (bit 21) in EFLAGS
		//	which differentiates between a Pentium CPU or other processor
		//	with the ability to use the CPUID instruction. If this bit
		//	cannot be set, CPU=Intel486.

        pushfd									// Get original EFLAGS
		pop			eax
		mov 		ecx, eax
        xor			eax, 200000h				// Flip ID bit in EFLAGS
        push		eax							// Save new EFLAGS value on stack        									
        popfd									// Replace current EFLAGS value
        pushfd									// Get new EFLAGS
        pop			eax							// Store new EFLAGS in EAX
        xor			eax, ecx					// Can not toggle ID bit,
        jz			short END_CPUID				// Processor=80486


		// Otherwise, execute CPUID instruction
		// AMD devices K5 and K6 will all get to this stage, earlier devices will not but will
		// recognized as intel devices

		or			cpu.u4CPUFlags,CPU_CPUID	// set the bit for CPUID instruction
		xor    		eax,eax			      		// set up for CPUID instruction

		cpuid

		mov			cpu.u4MaxCPUID, eax			// maximum input function for CPUID

		mov			DWORD PTR [cpu.strManufactureID],ebx
		mov			DWORD PTR [cpu.strManufactureID+4],edx
		mov			DWORD PTR [cpu.strManufactureID+8],ecx

		mov    		esi,OFFSET [cpu.strManufactureID]
		mov    		edi,OFFSET strIntel
		mov    		ecx,12

		// compare for intel
		repe		cmpsb
		cmp    		ecx,0		       			// must be GenuineIntel if ecx = 0
		jne			short NOT_INTEL

		mov			cpu.cpumanProcessorManufacture, cpumanINTEL
		jmp			short CPU_FEATURE

NOT_INTEL:
		mov    		esi,OFFSET [cpu.strManufactureID]
		mov    		edi,OFFSET strAMD
		mov    		ecx,12
		// compare for amd
		repe		cmpsb
		cmp    		ecx,0		       			// must be AuthenticAMD if ecx = 0
		jne			short NOT_AMD

		mov			cpu.cpumanProcessorManufacture, cpumanAMD
		jmp			short CPU_FEATURE

NOT_AMD:
		mov    		esi,OFFSET [cpu.strManufactureID]
		mov    		edi,OFFSET strCyrix
		mov    		ecx,12
		// compare for cyrix
		repe		cmpsb
		cmp    		ecx,0		       			// must be cyrix if ecx = 0
		jne			short NOT_CYRIX

		mov			cpu.cpumanProcessorManufacture, cpumanCYRIX
NOT_CYRIX:
		mov    		esi,OFFSET [cpu.strManufactureID]
		mov    		edi,OFFSET strIDT
		mov    		ecx,12
		// compare for Win Chip
		repe		cmpsb
		cmp    		ecx,0		       			// must be cyrix if ecx = 0
		jne			short NOT_IDT

		mov			cpu.cpumanProcessorManufacture, cpumanIDT
NOT_IDT:

CPU_FEATURE:
		// feature determination
		mov			eax,1
		cpuid

		// store the results of the instruction
		mov			cpu.u4Model, eax
		mov			cpu.u4Feature, edx
		
		// get the model number
		mov			ebx,eax
		shr			ebx,8
		and			ebx,0x0f

		// not the processor just the instruction family, for Intel these are the same
		// but for the other devices they are not and this needs to be adjusted.
		mov  		cpu.cpufamProcessorFamily, ebx

END_CPUID:
		popad
	}
}





//*********************************************************************************************
// Co-processor cheching begins here for the 8086,
// Intel 286, and Intel 386 CPUS, The algoritm is to 
// detemine whether or not the floating-point status 
// and control words can be written to.
// If they are not, no coprocessor exists.  If the status
// and control words can be written to, the correct processor
// is then determined depending on the processor id.  Coprocessor
// checks are first performed for an 8086, Intel 286 and an
// Intel486 DX CPU.  If the coprocessor id is still undetermined, the
// system must contain an Intel386 CPU.  The Intel386 CPU may work with
// either an Intel287 or an Intel387 math coprocessor.  The infinity of the
// coprocessor must be checked to determine the correct coprocessor id.
//
static void DetectFPU(void)
{
	WORD	fp_status;

	_asm
	{
		pushad									// check for 8087, Intel287, or
												// Intel387 math coprocessor
		fninit									// reset FP status word
		mov    		[fp_status],5a5ah			// initialise temp word to non-zero value
		fnstsw		[fp_status]					// save FP status word
		mov			ax,[fp_status]				// check FP status word
		cmp   		al,0						// see if correct status with written
		jne    		short END_CHECK_FPU

		fnstcw		[fp_status]					// save FP control word
		mov			ax,[fp_status]				// check FP control word
		and			ax,103fh					// see if seleced parts look OK
		cmp			ax,3fh						// check that 1s & 0s correctly read
		jne			END_CHECK_FPU
		or			cpu.u4CPUFlags, CPU_FPUPRESENT	// set the bit for an FPU present

		//
		//	Intel287 and Intel387 math coprocessor check for the Intel386 CPU
		//
		cmp    		cpu.cpufamProcessorFamily,3
		jne    		short END_CHECK_FPU
		fld1									// must use default control from FNINIT
		fldz									// form infinity
		fdiv									// 8087 and Intel287 MCP says +inf = -inf
		fld	st									// form negative infinity
		fchs									// Intel387 MCP says +inf <>-inf
		fcompp									// see if they are the same and remove them
		fstsw		[fp_status]					// look at the status from FCOMPP
		mov			ax,[fp_status]
		mov			cpu.cpufamFPUFamily,2		// store Intel287
		sahf									// see if infinities matched
		jz			short END_CHECK_FPU			// jump if 8087 or Intel287 MCP is present
		mov			cpu.cpufamFPUFamily,3		// store Intel387 MCP for fpu IntelFPUType

END_CHECK_FPU:
		popad
	}
}



//*********************************************************************************************
// cycles*10 to execute a BSF instruciton on each processor
//
static uint32 IntelProcessorCycles[] = 
{
	000,  000,  000, 1150, 470, 430, 
	33,  33,  33, 33,  33, 33,
};

static uint32 AMDProcessorCycles[] = 
{
	470,430,302
};

static uint32 CyrixProcessorCycles[] = 
{
	330,330
};





//*********************************************************************************************
//
#define MAXCLOCKS		150		// Maximum number of cycles per BSF instruction
#define ITERATIONS		4000	// Number of times to repeat BSF instruction in samplings.
#define INITIAL_DELAY	3		// Number of ticks to wait through before starting test sampling.
#define SAMPLING_DELAY	60		// Number of ticks to allow to elapse during sampling.
#define TOLERANCE		1		// Number of MHz to allow samplings to deviate from average of samplings.
#define MAX_TRIES		40		// Maximum number of samplings to allow before giving up and returning current average
#define	SAMPLINGS		10		// Number of BSF sequence samplings to make.


//*********************************************************************************************
//
static uint32 u4CPUSpeed(int clocks) 
{
	
	uint32	ticks;					// Microseconds elapsed during test
	uint32	cycles;					// Clock cycles elapsed during test
	uint32	stamp0, stamp1;			// Time Stamp Variable for beginning and end of test											
	LARGE_INTEGER t0,t1;			// Variables for High-Resolution Performance Counter reads
	
	uint32 freq  =0;				// Most current frequ. calculation
	uint32 freq2 =0;				// 2nd most current frequ. calc.
	uint32 freq3 =0;				// 3rd most current frequ. calc.

	uint32 total;					// Sum of previous three frequency calculations

	// Specifies whether the user manually entered the number of cycles for the BSF instruction.
	int manual=0;

	// Number of times a calculation has been made on this call to cpuspeed
	int tries=0;
					
	LARGE_INTEGER count_freq;		// High Resolution Performance Counter frequency
		  
	// Check for manual BSF instruction clock count
	if (clocks == 0) 
	{
		switch (cpu.cpumanProcessorManufacture)
		{
		case cpumanINTEL:
			cycles = ITERATIONS * IntelProcessorCycles[cpu.cpufamProcessorFamily];
			break;

		case cpumanAMD:
			cycles = ITERATIONS * AMDProcessorCycles[cpu.cpufamProcessorFamily - cpufamAM486];
			break;

		case cpumanCYRIX:
			cycles = ITERATIONS * CyrixProcessorCycles[cpu.cpufamProcessorFamily - cpufam686];
			break;
		}
	}
	else if (0 < clocks && clocks <= MAXCLOCKS)  
	{
		// Toggle manual control flag. Note that this mode will not work properly with processors
		// which can process multiple BSF instructions at a time. For example, manual mode
		// will not work on a  PentiumPro(R)
		cycles = ITERATIONS * clocks;
		manual = 1;			
	}
	else 
	{	
		return 0;
	} 
  	

	// Checks whether the high-resolution counter exists and returns an error if it does not exist.
	if ( !QueryPerformanceFrequency ( &count_freq ) )
	{
		return 0;
	}

	if ( ( cpu.u4Feature & 0x00000010 ) && !(manual) ) 
	{						
		// On processors supporting the Read Time Stamp opcode, compare elapsed time on the
		// High-Resolution Counter with elapsed cycles on the Time Stamp Register.
	    do 
		{
			// This do loop runs up to 20 times or until the average of the previous  three calculated
			// frequencies is within 1 MHz of each of the individual calculated frequencies. 
			// This resampling increases the accuracy of the results since outside factors could affect
			// this calculation			
			tries++;		// Increment number of times sampled on this call to cpuspeed
			freq3 = freq2;	// Shift frequencies back to make
			freq2 = freq;	//   room for new frequency measurement

			//Get high-resolution performance counter time
    		QueryPerformanceCounter(&t0); 
			
			t1.LowPart = t0.LowPart;		// Set Initial time
	  		t1.HighPart = t0.HighPart;


			// Loop until 50 ticks have passed since last read of hi-res counter.
			// This accounts for overhead later.
    		while ( (uint32)t1.LowPart - (uint32)t0.LowPart<50) 
			{	  
				QueryPerformanceCounter(&t1);
			}
			
   			_asm
        	{
				rdtsc
        		mov stamp0, EAX
   			}

			t0.LowPart = t1.LowPart;		// Reset Initial 
			t0.HighPart = t1.HighPart;		//   Time

    		// Loop until 1000 ticks have passed since last read of hi-res counter. 
			// This allows for elapsed time for sampling.
    		while ((uint32)t1.LowPart-(uint32)t0.LowPart<1000 ) 
			{				
    			QueryPerformanceCounter(&t1);
   			}
			
		
			_asm
	        {
				rdtsc
    	    	mov		stamp1, EAX
        	}

        	cycles = stamp1 - stamp0;
    		ticks = (uint32) t1.LowPart - (uint32) t0.LowPart;	
	
			// Note that some seemingly arbitrary mulitplies and divides are done below.
			// This is to maintain a high level of precision without truncating the most 
			// significant data. According to what value ITERATIIONS is set to, these 
			// multiplies and divides might need to be shifted for optimal precision.

			ticks = ticks * 100000;				
			ticks = ticks / ( count_freq.LowPart/10 );		
			
			if ( ticks%count_freq.LowPart > count_freq.LowPart/2 )
			{				
				ticks++;			// Round up if necessary
			}
			
			freq = cycles/ticks;	// Cycles / us  = MHz
        										
          	if ( cycles%ticks > ticks/2 )
			{
          		freq++;				// Round up if necessary
			}
          		
			total = ( freq + freq2 + freq3 );
				
		} while (	(tries < 3 ) || (tries < 20) && ((abs(3 * static_cast<int64>(freq) -total) > 3*TOLERANCE ) ||
			(abs(3 * static_cast<int64>(freq2)-total) > 3*TOLERANCE ) || (abs(3 * static_cast<int64>(freq3)-total) > 3*TOLERANCE )));
		
		if ( total / 3  !=  ( total + 1 ) / 3 )
		{
			total ++; 				// Round up if necessary
		}
		freq = total / 3;
    }
	else if ( cpu.cpufamProcessorFamily >= 3 ) 
	{		
		// If processor does not support time stamp reading, but is at least a 386 or above, 
		// utilize method of timing a loop of BSF instructions which take a known number of 
		// cycles to run on i386(tm), i486(tm), and Pentium(R) processors.
						
		int i;						// Temporary Variable
		uint32 current = 0;         
		uint32 lowest  = 0xffffffff;

		for ( i = 0; i < SAMPLINGS; i++ ) 
		{ 
			QueryPerformanceCounter(&t0);	// Get start time
			_asm 
			{	
				mov eax, 80000000h	
				mov bx, ITERATIONS		
LOOP1:			bsf ecx,eax
   				dec	bx
				jnz	LOOP1
			}
							 
			QueryPerformanceCounter(&t1);	// Get end time

			current = (uint32) t1.LowPart - (uint32) t0.LowPart;	
			if ( current < lowest )	
				lowest = current;	
		}
		ticks = lowest;

		ticks = ticks * 100000;				
		ticks = ticks / ( count_freq.LowPart/10 );		
		if ( ticks%count_freq.LowPart > count_freq.LowPart/2 )
		{
			ticks++;				// Round up if necessary
		}
			
		freq = cycles/ticks;		// Cycles / us  = MHz
        										
	    if ( cycles%ticks > ticks/2 )
    		freq++;					// Round up if necessary		

		freq/=10;
	}
	else
	{
		return 0;
	}

	return freq;
}


 
//*********************************************************************************************
// Make the correct textual name for the processor
// AMD has some instructions to get this from the device itself!!
//
void MakeName(void)
{
	uint32	u4_name;

	if (cpu.u4CPUFlags & CPU_CPUID)
	{
		switch (cpu.cpumanProcessorManufacture)
		{
		case cpumanINTEL:
			switch ((cpu.u4Model>>4) & 0xff)
			{
			case 0x040:
			case 0x041:
				strcat(&cpu.strProcessor[0],"Intel 486 DX");
				break;

			case 0x042:
				strcat(&cpu.strProcessor[0],"Intel 486 SX");
				break;

			case 0x043:
				strcat(&cpu.strProcessor[0],"Intel 486 DX2");
				break;

			case 0x044:
				strcat(&cpu.strProcessor[0],"Intel 486 SL");
				break;

			case 0x045:
				strcat(&cpu.strProcessor[0],"Intel 486 SX2");
				break;

			case 0x047:
				strcat(&cpu.strProcessor[0],"Intel 486 DX2 Write-Back Enhanced");
				break;

			case 0x048:
				strcat(&cpu.strProcessor[0],"Intel 486 DX4");
				break;

			case 0x050:
			case 0x051:
			case 0x052:
			case 0x053:
				strcat(&cpu.strProcessor[0],"Intel Pentium Processor");
				break;

			case 0x054:
			case 0x055:
			case 0x056:
			case 0x057:
			case 0x058:
			case 0x059:
			case 0x05A:
			case 0x05B:
			case 0x05C:
			case 0x05D:
			case 0x05E:
			case 0x05F:
				strcat(&cpu.strProcessor[0],"Intel Pentium Processor with MMX Technology");
				break;

			case 0x060:
			case 0x061:
			case 0x062:
				strcat(&cpu.strProcessor[0],"Intel Pentium Pro Processor");
				break;

			case 0x063:
			case 0x064:
			case 0x065:
			case 0x066:
			case 0x067:
			case 0x068:
			case 0x069:
			case 0x06A:
			case 0x06B:
			case 0x06C:
			case 0x06D:
			case 0x06E:
			case 0x06F:
				strcat(&cpu.strProcessor[0],"Intel Pentium II Processor with MMX Technology");
				break;
			}
			break;


		case cpumanAMD:
			u4_name = u4QueryCPUIDName();

			if (u4_name == 0)
			{
				// we failed to get a name from the CPUID instruction..
				switch (cpu.cpufamProcessorFamily)
				{
				case cpufamAM486:
					strcpy(&cpu.strProcessor[0],"AMD-AM486");
					break;

				case cpufamK5:
					strcpy(&cpu.strProcessor[0],"AMD-K5");
					break;

				case cpufamK6:
					strcpy(&cpu.strProcessor[0],"AMD-K6");
					break;

				}

				if (cpu.u4Feature & (1<<23))
				{
					strcat(&cpu.strProcessor[0]," with multimedia extentions");
				}
			}
			break;


		case cpumanIDT:
			u4_name = u4QueryCPUIDName();

			if (u4_name == 0)
			{
				// we failed to get a name from the CPUID instruction..
				switch (cpu.cpufamProcessorFamily)
				{
				case cpufamWINCHIPC6:
					strcpy(&cpu.strProcessor[0],"IDT WinChip C6");
					break;

				case cpufamWINCHIP2:
					strcpy(&cpu.strProcessor[0],"IDT WinChip2");
					break;

				}
			}

			break;


		case cpumanCYRIX:
			if (cpu.cpufamProcessorFamily == 5)
			{
				strcat(&cpu.strProcessor[0],"Cyrix 686");
			}
			else if (cpu.cpufamProcessorFamily == 6)
			{
				strcat(&cpu.strProcessor[0],"Cyrix M2");
			}
			break;


		default:
			// We do not know what processor is present but we can look for
			// the extended CPUID functions that give the name on some
			// processors. 
			u4_name = u4QueryCPUIDName();

			if (u4_name == 0)
			{
				// Processor does not support the exteneded CPUID so we do
				// not know what it is
				strcpy(&cpu.strProcessor[0],"Cannot Determine");
			}
			else
			{
				// Processor returned something for the extended CPUID functions
				// so we will return that with an unsure comment.
				strcat(&cpu.strProcessor[0]," (Uncertain)");
			}
			break;
		}

		// check for an overdrive
		if ((cpu.u4Model & 0x1000) == 0x1000 )
		{
			strcat(&cpu.strProcessor[0]," [OverDrive]");
		}

		if ((cpu.u4Model & 0x2000) == 0x2000 )
		{
			strcat(&cpu.strProcessor[0]," [Slave]");
		}
	}
	else
	{
		// NO CPUID - we need to just do the best we can with whta we have
		switch (cpu.cpufamProcessorFamily)
		{
		case	cpufam8086:
			strcpy(&cpu.strProcessor[0],"8086");
			if (cpu.u4CPUFlags & CPU_FPUPRESENT)
			{
				strcat(&cpu.strProcessor[0]," with an 8087");
			}
			break;

		case	cpufam80286:
			strcpy(&cpu.strProcessor[0],"80286");
			if (cpu.u4CPUFlags & CPU_FPUPRESENT)
			{
				strcat(&cpu.strProcessor[0]," with an 80287");
			}
			break;

		case	cpufam80386:
			strcpy(&cpu.strProcessor[0],"80386");
			if (cpu.u4CPUFlags & CPU_FPUPRESENT)
			{
				if (cpu.cpufamFPUFamily == 2)
				{
					strcat(&cpu.strProcessor[0]," with an 80287");
				}
				else
				{
					strcat(&cpu.strProcessor[0]," with an 80387");
				}
			}
			break;

		case	cpufam80486:
			strcpy(&cpu.strProcessor[0],"80386");
			if (cpu.u4CPUFlags & CPU_FPUPRESENT)
			{
				strcat(&cpu.strProcessor[0]," DX");
			}
			else
			{
				strcat(&cpu.strProcessor[0]," SX");
			}
			break;

		default:
			strcat(&cpu.strProcessor[0],"Cannot Determine");
			break;
		}
	}
}



//*********************************************************************************************
// Look for the CPUID name functions and use if they are present.
//
uint32 u4QueryCPUIDName(void)
{
	uint32 u4_name;

	// No CPUID instruction then there are no extended names.
	if ((cpu.u4CPUFlags & CPU_CPUID) == 0)
		return 0;

	_asm
	{
		mov		eax,0x80000000			// extended CPUID function,AMD and IDT support it

		//CPUID
		_emit	0x0f 
		_emit	0xa2

		cmp		eax,0x80000004
		jb		short EXIT_AMD_NAME		// functions upto 0x80000004 must be present

		mov		eax,0x80000002			// extended CPUID function
		//CPUID
		_emit	0x0f 
		_emit	0xa2
		
		mov		DWORD PTR [cpu.strProcessor+0],eax
		mov		DWORD PTR [cpu.au4ProcessorSpecific+8], eax
		mov		DWORD PTR [cpu.strProcessor+4],ebx
		mov		DWORD PTR [cpu.au4ProcessorSpecific+12], ebx
		mov		DWORD PTR [cpu.strProcessor+8],ecx
		mov		DWORD PTR [cpu.au4ProcessorSpecific+16], ecx
		mov		DWORD PTR [cpu.strProcessor+12],edx
		mov		DWORD PTR [cpu.au4ProcessorSpecific+20], edx

		mov		eax,0x80000003			// extended CPUID function
		//CPUID
		_emit	0x0f 
		_emit	0xa2
		
		mov		DWORD PTR [cpu.strProcessor+16],eax
		mov		DWORD PTR [cpu.au4ProcessorSpecific+24], eax
		mov		DWORD PTR [cpu.strProcessor+20],ebx
		mov		DWORD PTR [cpu.au4ProcessorSpecific+28], ebx
		mov		DWORD PTR [cpu.strProcessor+24],ecx
		mov		DWORD PTR [cpu.au4ProcessorSpecific+32], ecx
		mov		DWORD PTR [cpu.strProcessor+28],edx
		mov		DWORD PTR [cpu.au4ProcessorSpecific+36], edx

		mov		eax,0x80000004			// extended CPUID function
		//CPUID
		_emit	0x0f 
		_emit	0xa2
		
		mov		DWORD PTR [cpu.strProcessor+32],eax
		mov		DWORD PTR [cpu.au4ProcessorSpecific+40], eax
		mov		DWORD PTR [cpu.strProcessor+36],ebx
		mov		DWORD PTR [cpu.au4ProcessorSpecific+44], ebx
		mov		DWORD PTR [cpu.strProcessor+40],ecx
		mov		DWORD PTR [cpu.au4ProcessorSpecific+48], ecx
		mov		DWORD PTR [cpu.strProcessor+44],edx
		mov		DWORD PTR [cpu.au4ProcessorSpecific+52], edx

		mov		u4_name, 1			// we have got the name
		jmp		short DONE
EXIT_AMD_NAME:
		mov		u4_name, 0			// there is no name function, must get make the name
DONE:
	}

	return u4_name;
}