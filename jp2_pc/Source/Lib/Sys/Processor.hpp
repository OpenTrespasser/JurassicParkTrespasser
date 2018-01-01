/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	Interface header file for the Procesor detection DLL. This code is used from within the C
 *  DLL so this headef ile must be able to be included from a C or C++ file. Please take care
 *  to maintain this if you modify this header file.
 *
 * Bugs:
 *
 * To do:
 *
 * NOTE:
 *
 *	If the processor is identified as a 8086, 80286, 28386 or a 80486 without cpuid then only 
 *	limited infomration can be obtained about the device. In this case the only members that
 *	are valid are the ProcessorFamily, FPUFamily and FPU present bit.
 *
 *	Devices that support CPUID are completely identified along with all the features (to within
 *	the limits of the CPUID instruction).
 *
 *	If the processor detection function returns false you cannot rely on the information present
 *	within the structure, although data upto the point of failure will be valid.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/Processor.hpp                                                 $
 * 
 * 4     9/23/98 12:11p Rwyatt
 * Added support for K6-3and K7. Detection code is not future proof with respect to new models.
 * 
 * 3     9/01/98 12:31a Rwyatt
 * Added support for IDT WinChip processors
 * 
 * 2     7/08/97 5:21p Rwyatt
 * Interface header to processor.DLL 
 * 
 * 1     7/08/97 5:17p Rwyatt
 * Interface header to Processor.DLL
 * 
 **********************************************************************************************/

#ifndef _h_processor
#define _h_processor


//*********************************************************************************************
// Enum of manufactures
//
typedef enum ECPUMan
// prefix: cpumake
{
	cpumanINTEL			= 0,			// GenuineIntel
	cpumanAMD,							// AuthenticAMD
	cpumanCYRIX,						// CyrixInstead
	cpumanIDT,							// CentaurHauls
	cpumanUNKNOWN		= 0xffffffff,	// either before CPUID or unknown processor type
} ECPUMan;




//*********************************************************************************************
//
typedef enum ECPUFamily
// prefix: cpufam
{
	// Intel and generic/unidentified processors start at 0
	cpufam8086			= 0,			// 8086 and 80286 are here for completeness because 
	cpufam80286			= 2,			// Win32 will not run on these devices.
	cpufam80386			= 3,
	cpufam80486			= 4,
	cpufamPENTIUM		= 5,			// PENTIUM
	cpufamPENTIUMPRO	= 6,			// PENTIUMPRO


	// AMD starts at 100
	cpufamAM486			= 104,
	cpufamK5			= 105,
	cpufamK6			= 106,
	cpufamK62			= 107,
	cpufamK63			= 108,
	cpufamK7			= 109,

	// CYRIX starts at 200
	cpufam686			= 205,			// Cyrix 686
	cpufamM2			= 206,			// Cyrix M2

	// IDT starts at 300
	cpufamWINCHIPC6		= 305,
	cpufamWINCHIP2		= 306,

	cpufamUNKNOWN		= 0xffffffff,	// ?? what is it??
} ECPUFamily;



//*********************************************************************************************
//
#define CPU_CPUID		0x00000001		//set if the processor supports CPUID instruction
#define CPU_FPUPRESENT	0x00000002		//set if the processor as an FPU
#define CPU_CACHEINFO	0x00000004		//set if the processor can give cache info
#define CPU_MMX			0x00000008		//set is MMX is present
#define CPU_CMOV		0x00000010		//set if CMOV instructions are present
#define CPU_APIC		0x00000020		//set if APIC is present
#define CPU_RDTSC		0x00000040		//set if RDTSC and CR4 enable are supported
#define CPU_CMPXCHG8B	0x00000080		//set if the CMPXCHG8B instruction is supported
#define CPU_DEBUG		0x00000200		//set if debug extensions are present
#define CPU_VME			0x00000400		//set if virtual mode extensions are present
#define CPU_PSE			0x00000800		//set if page size extensions are present
#define CPU_MSR			0x00001000		//set if Model Specific Registers are present
#define CPU_PAE			0x00002000		//set if Physical Address Extentions are present
#define CPU_SEP			0x00004000		//set if Fast system calls are present
#define CPU_GPE			0x00008000		//set if global paging extentions are present
#define CPU_MCA			0x00010000		//set if machine check architecture is present
#define CPU_PAT			0x00020000		//set if page attribute table feature is present
#define CPU_MCE			0x00040000		//set if machine check exceptions are present
#define CPU_MTRR		0x00080000		//set if memory type range registers are present

#define CPU_PENTIUM		0x10000000		//set if this processor will run pentium code
#define CPU_PENTIUMPRO	0x20000000		//set if this processor will run pentium pro code
#define CPU_3DNOW		0x40000000		//set if this processor will run 3DNow code.


//*********************************************************************************************
// This structure is filled in by the Processor DLL
//
typedef struct CPUInfo
// prefix: cpu
{
	// normalized propeties which should be used with application code
	ECPUFamily	cpufamProcessorFamily;				// family of processor
	ECPUFamily	cpufamFPUFamily;					// family of floating point processor
	ECPUMan		cpumanProcessorManufacture;			// manuafact. of processor
	uint32		u4CPUFlags;							// general flags
	uint32		u4CPUSpeed;							// Speed in MHZ of the processor
	char		strProcessor[96];					// a friendly processor name string

	// cache information: -1 in an element means unknown or undetermined
	// cache data is only valid if the CACHEINFO bit in CPUFlags is set.
	uint32		u4L2CacheSize;						// size of L2 cahce in K or -1
	uint32		u4L2CacheLineSize;					// size of a L2 cache line or -1
	uint32		u4L2CacheAssoc;						// Set associative of L2 cahce or -1
	uint32		u4DataCacheSize;					// size of data cache in K or -1
	uint32		u4CodeCacheSize;					// sizw of instruction cache in K or -1
	uint32		u4DataCacheLineSize;				// cache line length of data cache in bytes or -1
	uint32		u4CodeCacheLineSize;				// cache line length of code cache in bytes or -1
	uint32		u4DataCacheAssoc;					// Set associative of data cahce or -1
	uint32		u4CodeCacheAssoc;					// Set associative of code cahce or -1

	// actual information returned from CPUID for advanced processor specific detection
	// standard information.
	char		strManufactureID[16];
	uint32		u4MaxCPUID;							// max function for CPUID
	uint32		u4Model;							// CPUID function 1, EAX
	uint32		u4Feature;							// CPUID function 1, EDX
	uint32		au4ProcessorSpecific[32];			// 128 bytes of processor specific data

	// processor specific results from model specific CPUID functions.
	// INTEL:
	//		+0	eax CPUID[2]
	//		+4	ebx CPUID[2]
	//		+8	ecx CPUID[2]
	//		+12	edx CPUID[2]
	//
	// AMD:
	//		+0	eax CPUID[0x80000000] - range of extended functions to CPUID
	//		+4  edx CPUID[0x80000001] - extended features
	//
	//		48 byte text string returned for processor name CPUID[0x80000002] - [0x80000004]
	//		+8  eax CPUID[0x80000002]
	//		+12 ebx CPUID[0x80000002]
	//		+16 ecx CPUID[0x80000002]
	//		+20 edx CPUID[0x80000002]
	//		+24 eax CPUID[0x80000003]
	//		+28 ebx CPUID[0x80000003]
	//		+32 ecx CPUID[0x80000003]
	//		+36 edx CPUID[0x80000003]
	//		+40 eax CPUID[0x80000004]
	//		+44 ebx CPUID[0x80000004]
	//		+48 ecx CPUID[0x80000004]
	//		+52 edx CPUID[0x80000004]
	//
	//		+56 Used only by K7 (TLB 2M/4M pages): eax CPUID[0x80000005]
	//		+60 TLB Cache features: ebx CPUID[0x80000005]
	//		+64 L1 Data Cache features: ecx CPUID[0x80000005]
	//		+68 L1 Code Cache features: edx CPUID[0x80000005]
	//
	//		+72 K7 and above L2 TLB, reserved on others: eax CPUID[0x80000006]
	//		+76 K7 and above L2 TLB, reserved on others: ebx CPUID[0x80000006]
	//		+80 K6-3 and above L2 unified cache        : ecx CPUID[0x80000006]
	//		+84 K7 and below reserved                  : edx CPUID[0x80000006]
	//
	// CYRIX:
	//		All the features below are not supported in current versions of Cyrix
	//		processors. The comming processors that support 3D Now will have to have
	//		these features otherwise the new instructions will not be detected.
	//
	//		+0	eax CPUID[0x80000000] - range of extended functions to CPUID
	//		+4  edx CPUID[0x80000001] - extended features
	//
	//
	//		48 byte text string returned for processor name CPUID[0x80000002] - [0x80000004]
	//		+8  eax CPUID[0x80000002]
	//		+12 ebx CPUID[0x80000002]
	//		+16 ecx CPUID[0x80000002]
	//		+20 edx CPUID[0x80000002]
	//		+24 eax CPUID[0x80000003]
	//		+28 ebx CPUID[0x80000003]
	//		+32 ecx CPUID[0x80000003]
	//		+36 edx CPUID[0x80000003]
	//		+40 eax CPUID[0x80000004]
	//		+44 ebx CPUID[0x80000004]
	//		+48 ecx CPUID[0x80000004]
	//		+52 edx CPUID[0x80000004]

	//
	// IDT (WINCHIP):					// AS AMD
	//		+0	eax CPUID[0x80000000] - range of extended functions to CPUID
	//		+4  edx CPUID[0x80000001] - extended features
	//
	//		48 byte text string returned for processor name CPUID[0x80000002] - [0x80000004]
	//		+8  eax CPUID[0x80000002]
	//		+12 ebx CPUID[0x80000002]
	//		+16 ecx CPUID[0x80000002]
	//		+20 edx CPUID[0x80000002]
	//		+24 eax CPUID[0x80000003]
	//		+28 ebx CPUID[0x80000003]
	//		+32 ecx CPUID[0x80000003]
	//		+36 edx CPUID[0x80000003]
	//		+40 eax CPUID[0x80000004]
	//		+44 ebx CPUID[0x80000004]
	//		+48 ecx CPUID[0x80000004]
	//		+52 edx CPUID[0x80000004]
	//
	//		+56 reserved: eax CPUID[0x80000005]
	//		+60 TLB Cache features: ebx CPUID[0x80000005]
	//		+64 L1 Data Cache features: ecx CPUID[0x80000005]
	//		+68 L1 Code Cache features: edx CPUID[0x80000005]
	//


} CPUInfo;



//*********************************************************************************************
// Calling prototype
//
typedef bool (_cdecl *CPUFUNC)(CPUInfo* pcpuProcessorInfo);

#endif // ifdef _h_processor