/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of support code for reading processor specific registers and performance
 * registers.
 *
 * If VER_TIMING_STATS is not defined then all the code in this file is removed and any error
 * codes that are returned act as if there has been an error.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/PerformanceCount.cpp                                          $
 * 
 * 10    98.02.18 3:45p Mmouni
 * Added descriptions for Pentium II performance counters.
 * 
 * 9     10/02/97 1:20p Rwyatt
 * Added dynamic loading for win95 performance VxD
 * 
 * 8     9/29/97 11:06a Mmouni
 * Added the ability to count clocks on the Pentium.
 * Fixed a problem with toggling the ring 0/3 settings.
 * 
 * 7     9/24/97 3:09a Rwyatt
 * Added a runtime OS detect so it can load the correct performance drivers
 * 
 * 6     9/23/97 11:08p Mmouni
 * Fixed Pentium counter selection.
 * Added full names for counter menu.
 * 
 * 5     9/04/97 11:03a Agrant
 * Final build fix
 * 
 * 4     8/28/97 4:05p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 5     8/26/97 1:34a Rwyatt
 * will now return a string for the current menu selection
 * 
 * 4     8/25/97 5:20p Rwyatt
 * Major change:
 * No uses the VER_PROFILE_STAT compile option so all the stats come and go together.
 * Works with the profile system to allow it to montior any event in the same format as the
 * previous stats.
 * Builds a GDI menu with all the perfornace events for the current processor.
 * Evaluates to no code in a FINAL build
 * 
 * 3     7/23/97 9:55p Rwyatt
 * Now supports Pentium MSRs and the P6/PII counters have been given a tidy. they now use the
 * read/write MSR functions rather than direct IOCTL.
 * 
 * 2     7/22/97 1:29a Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Config.hpp"
#include "PerformanceCount.hpp"


//**********************************************************************************************
// static data
//

struct SProfileText
// prefix pt
{
	char*	strTextName;
	uint32	u4CounterVal;
};



#if VER_TIMING_STATS

// setup data
static HANDLE	hFile		= NULL;
static int32	i4Going		= 0;
static int32	i4Processor	= -1;

// current counter values..
static uint32	u4Counter0	= 0;
static uint32	u4Counter1	= 0;

// by default we read stats for both OS rings..
static bool		bRing0		= true;
static bool		bRing3		= true;

// IO data for IOCTL calls
static DWORD	dwIn[3];
static DWORD	dwOut[2];
static DWORD	dwReturnedLength;     // Number of bytes returned

// menus for options..
static HMENU	hmenu0;
static HMENU	hmenu1;

// number of the last option on each menu
static uint32	u4LastMenu0 = 0xffffffff;
static uint32	u4LastMenu1 = 0xffffffff;


static uint32	u4Menu0Base = 0;
static uint32	u4Menu1Base = 0;


//**********************************************************************************************
static SProfileText	ptEmpty[] =
{
	{NULL, 0xffffffff}				//end of list
};

//**********************************************************************************************
static SProfileText	ptCountersPentium[] =
{
	{"Count Clocks",																0x003f0300 },
	{"Any Segment Register Loaded",													0x000F0300 },
	{"Bank Conflicts",																0x000A0300 },
	{"Branches",																	0x00120300 },
	{"BTB False entries",															0x003A0100 },
	{"BTB Hits",																	0x00130300 },
	{"BTB misprediction on a Not-Taken Branch",										0x003A0200 },
	{"Clocks stalled due to Address Generation Interlock",							0x001F0300 },
	{"Clocks stalled due to Full Write Buffers",									0x00190300 },
	{"Clocks stalled due to Full Write Buffers while executing MMX instructions",	0x003B0100 },
	{"Clocks stalled on MMX instruction write to E or M state line",				0x003B0200 },
	{"Clocks stalled waiting for a data memory read",								0x001A0300 },
	{"Clocks stalled writing an E or M state line",									0x001B0300 },
	{"Clocks while bus cycle active",												0x00180300 },
	{"Code Cache Misses",															0x000E0300 },
	{"Code Cache Reads",															0x000C0300 },
	{"Code TLB Misses",																0x000D0300 },
	{"Cycles Not in HALT State",													0x00300100 },
	{"D1 Starvation and one instruction in FIFO",									0x00330200 },
	{"Data Cache Lines Written Back",												0x00060300 },
	{"Data Cache Snoop Hits",														0x00080300 },
	{"Data Cache Snoops",															0x00070300 },
	{"EMMS Instructions Executed",													0x002D0100 },
	{"Data Cache Reads",															0x00000300 },
	{"Data Read Misses",															0x00030300 },
	{"Data Read or Write Misses",													0x00290300 },
	{"Data Reads or Data Writes",													0x00280300 },
	{"Data TLB Miss",																0x00020300 },
	{"Data Cache Writes",															0x00010300 },
	{"Data Write Miss",																0x00040300 },
	{"Floating Point Stalls",														0x00320100 },
	{"Floating Point Operations",													0x00220300 },
	{"Hardware Interrupts",															0x00270300 },
	{"Instructions Executed",														0x00160300 },
	{"Instructions Executed in the V-Pipe (Pairing)",								0x00170300 },
	{"I/O Read or Write Cycle",														0x001D0300 },
	{"Memory Access in Both Pipes",													0x00090300 },
	{"Misaligned Data Memory Reference",											0x000B0300 },
	{"Misaligned Data Memory Reference on MMX instruction",							0x00360100 },
	{"MMX Instruction Data Writes",													0x00340100 },
	{"MMX Instruction Data Write Misses",											0x00340200 },
	{"MMX Instruction Data Read",													0x00310100 },
	{"MMX Instruction Executed in U-pipe",											0x002B0100 },
	{"MMX Instruction Executed in V-pipe",											0x002B0200 },
	{"MMX Multiply Unit Interlock",													0x00380100 },
	{"MOVD/MOVQ store stall due to previous operation",								0x00380200 },
	{"Non-Cacheable Memory Reads",													0x001E0300 },
	{"Non-Cacheable Memory Writes",													0x002E0200 },
	{"Pipeline flushes",															0x00150300 },
	{"Pipeline flushes due to wrong branch prediction",								0x00350100 },
	{"Pipeline flushes due to wrong branch prediction resolved in WB-stage",		0x00350200 },
	{"Pipeline Stalled Waiting for MMX instruction data memory read",				0x00360200 },
	{"Returns",																		0x00390100 },
	{"Returns Predicted (Correctly and Incorrectly)",								0x00370200 },
	{"Returns Predicted Incorrectly",												0x00370100 },
	{"RSB Overflows",																0x00390200 },
	{"Saturated MMX Instructions Executed",											0x002F0100 },
	{"Saturations Performed",														0x002F0200 },
	{"Taken Branch or BTB Prediction",												0x00140300 },
	{"Taken Branches",																0x00320200 },
	{"Transition from MMX to FP instructions",										0x002D0200 },
	{"Write (Hit) to M- or E-state line",											0x00050300 },

	{NULL, 0xffffffff}				//end of list
};


//**********************************************************************************************
static SProfileText	ptCountersPII[] =
{
	{"BACLR BACLEARS Asserted",												0x20E60300},
	{"RETDC Bogus Branches",												0x20E40300},
	{"BRDEC Branch Instructions Decoded",									0x20E00300},
	{"BRRET Branch Instructions Retired",									0x20C40300},
	{"BRMSR Branch Mispredictions Retired",									0x20C50300},
	{"BTBMS BTB Misses",													0x20E20301},
	{"MASKD Clocks while interrupts masked",								0x20C60300},
	{"MSKPN Clocks while interrupts masked and an interrupt is pending",	0x20C70300},
	{"CLOCK Clockticks",													0x20790300},
	{"DBUSY Cycles Divider Busy",											0x20140200},
	{"CSFET Cycles Instruction Fetch Stalled",								0x20860300},
	{"FTSTL Cycles Instruction Fetch stalled -> pipe",						0x20870300},
	{"L2BBS Cycles L2 Data Bus Busy",										0x20220300},
	{"L2BBT Cycles L2 Data Bus Busy transferring data to CPU",				0x20230300},
	{"DMREF Data Memory References (all)",									0x20430300},
	{"FPDIV Divides",														0x20130200},
	{"BBIFT External Bus Burst Instruction Fetches",						0x20680300},
	{"BURST External Bus Burst Read Operations",							0x20650300},
	{"BUSTB External Bus Burst Transactions",								0x206E0300},
	{"BUSBS External Bus Cycles - DRDY Asserted (busy)",					0x20620300},
	{"BLOCK External Bus Cycles - LOCK signal asserted",					0x20630300},
	{"BRBNR External Bus Cycles While BNR Asserted",						0x20610300},
	{"BRHIT External Bus Cycles While HIT Asserted",						0x207A0300},
	{"BRHTM External Bus Cycles While HITM Asserted",						0x207B0300},
	{"BBRCV External Bus Cycles While Processor receiving data",			0x20640300},
	{"BRSST External Bus Cycles While Snoop Stalled",						0x207E0300},
	{"BRWRA External Bus Cycles While Receive Active",						0x20600300},
	{"BUSDF External Bus Deferred Transactions",							0x206D0300},
	{"BUSIO External Bus I/O Bus Transactions",								0x206C0300},
	{"BINVL External Bus Invalidate Transactions",							0x20690300},
	{"BMALL External Bus Memory Transactions",								0x206F0300},
	{"BPTMO External Bus Partial Memory Transactions",						0x206B0300},
	{"BPRBT External Bus Partial Read Transactions",						0x206A0300},
	{"BRINV External Bus Read for Ownership Transaction",					0x20660300},
	{"BRDCD External Bus Request Outstanding",								0x20600300},
	{"BSALL External Bus Transactions (All)",								0x20700300},
	{"BMLEV External Bus Writeback M-state Evicted",						0x20670300},
	{"FPOPS FP operations retired",											0x20C10100},
	{"FPOPE FP Computational Operations Executed",							0x20100100},
	{"FPEOA FP Except Cases handled by Microcode",							0x20110200},
	{"INSTD Instructions Decoded",											0x20D00300},
	{"INSTR Instructions Retired",											0x20C00300},
	{"INTLB Instructions TLB Misses",										0x20850300},
	{"HWINT Hardware Interrupts Received",									0x20C80300},
	{"DCALO L1 Lines Allocated",											0x2045030F},
	{"DCMAL L1 M-state Data Cache Lines Allocated",							0x20460300},
	{"DCMEV L1 M-state Data Cache Lines Evicted",							0x20470300},
	{"DCOUT L1 Misses outstanding (weighted)",								0x20480300},
	{"L2STR L2 address strobes => address bus utilization",					0x20210300},
	{"L2MCF L2 Cache Instruction Fetch Misses (highly correlated)",			0x20280301},
	{"L2FET L2 Cache Instruction Fetches",									0x2028030F},
	{"L2DRM L2 Cache Read Misses (highly correlated)",						0x20290301},
	{"L2DMR L2 Cache Reads",												0x2029030F},
	{"L2CMS L2 Cache Request Misses (highly correlated)",					0x202E0301},
	{"L2DWM L2 Cache Write Misses (highly correlated)",						0x202A0301},
	{"L2DMW L2 Cache Writes",												0x202A030F},
	{"L2DCR L2 Cache Requests",												0x202E030F},
	{"L2ALO L2 Lines Allocated",											0x20240300},
	{"L2CEV L2 Lines Evicted",												0x20260300},
	{"L2MAL L2 M-state Lines Allocated",									0x20250300},
	{"L2MEV L2 M-state Lines Evicted",										0x20270300},
	{"UOPSR Micro-Ops Retired",												0x20C20300},
	{"UOPS1 Micro-Ops Retired - Low parallelization",						0x20C20300},
	{"MISMM Misaligned Data Memory Reference",								0x20050300},
	{"MMXIE MMX Instructions Executed",										0x20B00300},
	{"MMXIR MMX Instructions Retired",										0x20CE0300},
	{"PORT0 MMX micro-ops executed on Port 0",								0x20B20301},
	{"PORT1 MMX micro-ops executed on Port 1",								0x20B20302},
	{"PORT2 MMX micro-ops executed on Port 2",								0x20B20304},
	{"PORT3 MMX micro-ops executed on Port 3",								0x20B20308},
	{"MMXPA MMX Packed Arithmetic",											0x20B30300},
	{"MMXPL MMX Packed Logical",											0x20B30310},
	{"MMXPM MMX Packed Multiply",											0x20B30301},
	{"MMXPO MMX Packed Operations",											0x20B30304},
	{"MMXPS MMX Packed Shift",												0x20B30302},
	{"MMXUO MMX Unpacked Operations",										0x20B30308},
	{"FMULT Multiplies",													0x20120200},
	{"STRBB Store Buffer Block",											0x20030300},
	{"STBDC Store Buffer Drain Cycles",										0x20040300},
	{"NPRTL Renaming Stalls",												0x20D20300},
	{"RSTAL Resource Related Stalls",										0x20A20300},
	{"SAISE Saturated Arithmetic Instructions Executed",					0x20B10300},
	{"SAISR Saturated Arithmetic Instructions Retired",						0x20CF0300},
	{"SEGLD Segment register loads",										0x20060300},
	{"SRSES Segment Rename Stalls - ES",									0x20D40301},
	{"SRSDS Segment Rename Stalls - DS",									0x20D40302},
	{"SRSFS Segment Rename Stalls - FS",									0x20D40304},
	{"SRSGS Segment Rename Stalls - GS",									0x20D40308},
	{"SRSXS Segment Rename Stalls - ES DS FS GS",							0x20D4030F},
	{"SRNES Segment Renames - ES",											0x20D50300},
	{"SRNDS Segment Renames - DS",											0x20D50302},
	{"SRNFS Segment Renames - FS",											0x20D50304},
	{"SRNGS Segment Renames - GS",											0x20D50308},
	{"SRNXS Segment Renames - ES DS FS GS",									0x20D5030F},
	{"SIMDA SIMD Assists (EMMS Instructions Executed)",						0x20CD0300},
	{"BTAKR Taken Branch Retired",											0x20C90300},
	{"BTAKM Taken Branch Mispredictions",									0x20CA0300},
	{"TSMCD Time Self-Modifiying Code Detected",							0x20520300},
	{"TOIFM Total Instruction Fetch Misses",								0x20810300},
	{"CMREF Total Instruction Fetches",										0x20800300},
	{"FPMMX Transitions from Floating Point to MMX",						0x20CC0301},
	{"MMXFP Transitions from MMX to Floating Point",						0x20CC0300},

	{NULL, 0xffffffff}				//end of list
};



//**********************************************************************************************
static SProfileText	ptCountersP6[] =
{
	{"BACLR BACLEARS Asserted ",											0x10E60300},
	{"RETDC Bogus Branches",												0x10E40300},
	{"BRDEC Branch Instructions Decoded",									0x10E00300},
	{"BRRET Branch Instructions Retired",									0x10C40300},
	{"BRMSR Branch Mispredictions",											0x10C50300},
	{"BTBMS BTB Misses",													0x10E20301},
	{"MASKD Cycles IRQs Masked",											0x10C60300},
	{"MSKPN Cycles IRQs Masked, IRQ pending",								0x10C70300},
	{"CLOCK Clock ticks",													0x10790300},
	{"DBUSY Cycles divider busy",											0x10140200},
	{"CSFET Cycles Instruction fetch stalled",								0x10860300},
	{"FTSTL Cycles Instruction fetch stalled -> pipe",						0x10870300},
	{"L2BBS Cycles L2 Data Bus Busy",										0x10220300},
	{"L2BBT Cycles L2 Data Bus Busy, data to CPU",							0x10230300},
	{"DMREF Data Memory References (all)",									0x10430300},
	{"FPDIV FP divides",													0x10130200},
	{"BBIFT External Bus Burst Instruction Fetches",						0x10680300},
	{"BURST External Bus Burst Read Operations",							0x10650300},
	{"BUSTB External Bus Burst Transactions",								0x106E0300},
	{"BUSBS External Bus Cycles-DRDY Asserted",								0x10620300},
	{"BLOCK External Bus Cycles-LOCK signaled",								0x10630300},
	{"BRBNR External Bus Cycles-BNR Asserted",								0x10610300},
	{"BRHIT External Bus Cycles-HIT Asserted",								0x107A0300},
	{"BRHTM External Bus Cycles-HITM Asserted",								0x107B0300},
	{"BBRCV External Bus Cycles-CPU receiving data",						0x10640300},
	{"BRSST External Bus Cycles-Snoop Stalled",								0x107E0300},
	{"BRWRA External Bus Cycles-Receive Active",							0x10600300},
	{"BUSDF External Bus Deferred Transactions",							0x106D0300},
	{"BUSIO External Bus I/O Bus Transactions",								0x106C0300},
	{"BINVL External Bus Invalidate Transactions",							0x10690300},
	{"BMALL External Bus Memory Transactions",								0x106F0300},
	{"BPTMO External Bus Partial Memory Transactions",						0x106B0300},
	{"BPRBT External Bus Partial Read Transactions",						0x106A0300},
	{"BRINV External Bus Read Ownership Transaction",						0x10660300},
	{"BRDCD External Bus Request Outstanding",								0x10600300},
	{"BSALL External Bus Transactions (All)",								0x10700300},
	{"BMLEV External Bus Writeback M-state Evicted",						0x10670300},
	{"FPOPS FP Operations",													0x10C10100},
	{"FPOPE FP Computational Operations Executed",							0x10100100},
	{"FPEOA FP Except Cases handled by Microcode",							0x10110200},
	{"INSTD Instructions Decoded",											0x10D00300},
	{"INSTR Instructions Retired",											0x10C00300},
	{"INTLB Instructions TLB Misses",										0x10850300},
	{"HWINT Hardware Interrupts Received",									0x10C80300},
	{"DCALO L1 Lines Allocated",											0x1045030F},
	{"DCMAL L1 M-state Data Cache Lines Allocated",							0x10460300},
	{"DCMEV L1 M-state Data Cache Lines Evicted",							0x10470300},
	{"DCOUT L1 Misses outstanding",											0x10480300},
	{"L2STR L2 adr strobes => address bus utilization",						0x10210300},
	{"L2MCF L2 Cache Instruction Fetch Misses",								0x10280301},
	{"L2FET L2 Cache Instruction Fetches",									0x1028030F},
	{"L2DRM L2 Cache Data Read Misses",										0x10290301},
	{"L2DMR L2 Cache Data Reads",											0x1029030F},
	{"L2CMS L2 Cache Request Misses",										0x102E0301},
	{"L2DWM L2 Cache Write Misses",											0x102A0301},
	{"L2DMW L2 Cache Writes",												0x102A030F},
	{"L2DCR L2 Cache Requests",												0x102E030F},
	{"L2ALO L2 Lines Allocated",											0x10240300},
	{"L2CEV L2 Lines Evicted",												0x10260300},
	{"L2MAL L2 M-state Lines Allocated",									0x10250300},
	{"L2MEV L2 M-state Lines Evicted",										0x10270300},
	{"UOPSR uOPs Retired",													0x10C20300},
	{"UOPS1 uOPs Retired - Low parallelization",							0x10C20300},
	{"MISMM Misaligned Memory Reference",									0x10050300},
	{"FMULT Multiplies (all)",												0x10120200},
	{"STRBB Store Buffer Block",											0x10030300},
	{"STBDC Store Buffer Drain Cycles",										0x10040300},
	{"NPRTL Renaming Stalls",												0x10D20300},
	{"RSTAL Resource Related Stalls",										0x10A20300},
	{"SEGLD Segment register loads",										0x10060300},
	{"BTAKR Taken Branch Retired",											0x10C90300},
	{"BTAKM Taken Branch Mispredictions",									0x10CA0300},
	{"TOIFM Total Instruction Fetch Misses",								0x10810300},
	{"CMREF Total Instruction Fetches",										0x10800300},

	{NULL, 0xffffffff}				//end of list
} ;

#endif


//**********************************************************************************************
// Open the device and start everything going.....
//
int iPSInit(void)
{
#if VER_TIMING_STATS
	if (i4Going!=0)
	{
		i4Going ++;
		return PS_INIT_GOING;
	}

	OSVERSIONINFO	OS;
	OS.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&OS);

	if (OS.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// WINDOWS NT
		hFile = CreateFile(
			 "\\\\.\\PERFMON_DWI",
			 GENERIC_READ,
			 0,
			 NULL,
			 OPEN_EXISTING,
			 FILE_ATTRIBUTE_NORMAL,
			 NULL);
	}
	else
	{
		// under windows 95 I had to change the driver name because thay have to be less than
		// eight chars...
		hFile = CreateFile(
			"\\\\.\\DWIPERF.VXD",
			 GENERIC_READ,
			 0,
			 NULL,
			 OPEN_EXISTING,
			 FILE_ATTRIBUTE_NORMAL,
			 NULL);
	}

    if (hFile != ((HANDLE)-1) )
	{
		i4Going=1;

		// get the version of this processor so we know what MSRs to set and the format of them
		// we should check for an Intel device here.
		_asm
		{
			mov			eax,1
			//CPUID [ features ]
			_emit		0x0f 
			_emit		0xa2
			mov			ebx,eax
			shr			ebx,8
			and			ebx,0x0f

			cmp			ebx, 4
			je			short P486
			cmp			ebx, 5
			je			short P586
			// must be a p6 or later..
			mov			ebx, 1					// normal p6
			test		edx, 1<<23
			je			short DONE
			mov			ebx, 2					// PII
			jmp			short DONE

P486:		// oh no....its going to be messy - MSRs do not exist!
			jmp			short ERR
				
P586:
			mov			ebx, 0					// P5
DONE:
			mov			i4Processor, ebx
		}

		// the counters in a Pentium are always enabled, but in a PentiumPro/Pentium II
		// you have to set the bit in counter 0. The select functions in this file always
		// delect the bit so we just have to write to counter 0.
		// Make sure we use a register for the correct processor
		if (i4Processor == 1)
		{
			// we are a pentium pro so we need to write to the counters to enable them
			PSSelectCounter0(P6_CLOCK);
			PSSelectCounter1(P6_CLOCK);
		}

		if (i4Processor == 2)
		{
			// we are a pentium pro so we need to write to the counters to enable them
			PSSelectCounter0(PII_CLOCK);
			PSSelectCounter1(PII_CLOCK);
		}

		return PS_INIT_OK;

ERR:
		i4Going = 0;
		return PS_INIT_FAIL;
	}
	else
	{
		OutputDebugString("Failed to open Perf_DWI driver\n");
		return PS_INIT_FAIL;
	}
#else
	return PS_INIT_FAIL;
#endif
}

//**********************************************************************************************
// Is the perfornace system going??
//
bool bPSGoing()
{
#if VER_TIMING_STATS
	return i4Going>0;
#else
	// if not compiled with performance stats return false
	return false;
#endif
}


//**********************************************************************************************
// close the device driver
//
void PSClose(void)
{
#if VER_TIMING_STATS

	// we have not been opened properly
	if (i4Going == 0)
		return;

	i4Going--;
	if (i4Going <= 0)
	{
		if (hFile)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}
	}
#endif
}

//**********************************************************************************************
// Set the ring3 flag
//
void PSRing3(bool b_ring)
{
#if VER_TIMING_STATS
	// can we set the counter ring
	if ((i4Going == 0) || (i4Processor == -1))
		return;

	bRing3 = b_ring;

	// reset the counters with the new ring flag
	PSSelectCounter0(u4Counter0);
	PSSelectCounter1(u4Counter1);

#endif
}


//**********************************************************************************************
// Set the ring0 flag
//
void PSRing0(bool b_ring)
{
#if VER_TIMING_STATS
	// can we set the counter ring
	if ((i4Going == 0) || (i4Processor == -1))
		return;

	bRing0 = b_ring;

	// reset the counters with the new ring flag
	PSSelectCounter0(u4Counter0);
	PSSelectCounter1(u4Counter1);

#endif
}



//**********************************************************************************************
// return true if we are using ring 3
//
bool bPSRing3()
{
#if VER_TIMING_STATS
	return bRing3;
#else
	return false;
#endif
}


//**********************************************************************************************
// return true if we are using ring 3
//
bool bPSRing0()
{
#if VER_TIMING_STATS
	return bRing0;
#else
	return false;
#endif
}



//**********************************************************************************************
// Select the event to monitor with counter 0
//
void PSSelectCounter0(unsigned int ui_count)
{
#if VER_TIMING_STATS
	BOOL	b_res;

	// has the processor been set?
	if (i4Processor == -1)
		return;

	// is this an operation for this processor
	if ( ((ui_count>>28)&3) != (unsigned int)i4Processor)
	{
		OutputDebugString("Invalid Processor operation\n");
		return;
	}

	// is this a counter 0 operation??
	if ((ui_count & 0x100) != 0x100)
	{
		OutputDebugString("Invalid operation for Event Counter 0\n");
		return;
	}

	if (i4Processor == 0)
	{
		DWORD	dw_low;
		DWORD	dw_high;
		BYTE	u1_event = (ui_count & (0x3F0000))>>16;		//(only 6 bits on pentium)

		// setup the models specifc registers for a Pentium, this is the weird one....
		// There is only 1 event select register (CESR), this is split into two halfs,
		// the top 16bits do couter 1 and the bottom 16 bits do counter 0.
		// Therefore we have to do a read-modify-write if we want to change just half
		// of the register.

		// get the current contents
		bPSReadMSR(0x11,&dw_low,&dw_high);

		// completely clear timer 0 control to stop it
		dw_low &= 0xffff0000;

		// stop the counter
		bPSWriteMSR(0x11,dw_low,dw_high);

		// clear the counter.
		bPSWriteMSR(0x12,0,0);

		if (u1_event == 0x3f)
		{
			// Count clocks.
			bPSWriteMSR(0x11,(dw_low | 0x100 | (bRing3 ? (1<<7) : 0) | (bRing0 ? (1<<6) : 0) ), 0);
		}
		else
		{
			// write the new timer 0 reading only ring 3, leaving timer 1 how it was.
			bPSWriteMSR(0x11,(dw_low | (u1_event) | (bRing3?(1<<7):0) | (bRing0?(1<<6):0) ),0);
		}
	}
	else
	{
		// setup the model specific registers for the P6/PII
		BYTE	u1_event;
		BYTE	u1_mask;

		u1_event= (ui_count & (0xFF0000))>>16;
		u1_mask = (ui_count & 0xFF);

		b_res = bPSWriteMSR(0x186,(u1_event | (bRing3?(1<<16):0) | (bRing0?(1<<17):0) | (1<<22) | (1<<18) | (u1_mask<<8)),0);

		// has the IOCTL call failed?
		if (!b_res)
		{
			OutputDebugString("Set Event Counter 0 failed..\n");
			return;
		}
	}
	u4Counter0 = ui_count;
#endif
}




//**********************************************************************************************
// Select the event to monitor with counter 1
//
void PSSelectCounter1(unsigned int ui_count)
{
#if VER_TIMING_STATS
	BOOL	b_res;

	// has the processor been set?
	if (i4Processor == -1)
		return;

	// is this an operation for this processor
	if ( ((ui_count>>28)&3) != (unsigned int)i4Processor)
	{
		OutputDebugString("Invalid Processor operation\n");
		return;
	}

	// is this a counter 1 operation??
	if ((ui_count & 0x200) != 0x200)
	{
		OutputDebugString("Invalid operation for Event Counter 1\n");
		return;
	}

	if (i4Processor == 0)
	{
		// setup the models specifc registers for a Pentium
		DWORD	dw_low;
		DWORD	dw_high;
		BYTE	u1_event = (ui_count & (0x3F0000))>>16;		//(only 6 bits on pentium)

		// setup the models specifc registers for a Pentium, this is the weird one....
		// There is only 1 event select register (CESR), this is split into two halfs,
		// the top 16bits do couter 1 and the bottom 16 bits do counter 0.
		// Therefore we have to do a read-modify-write if we want to change just half
		// of the register.

		// get the current contents
		bPSReadMSR(0x11,&dw_low,&dw_high);

		// completely clear timer 1 control to stop it
		dw_low &= 0x0000ffff;

		// stop the counter
		bPSWriteMSR(0x11,dw_low,dw_high);

		// clear the counter.
		bPSWriteMSR(0x13,0,0);

		if (u1_event == 0x3f)
		{
			// Count clocks.
			bPSWriteMSR(0x11,dw_low | ((0x100 | (bRing3 ? (1<<7) : 0) | (bRing0 ? (1<<6) : 0)) << 16), 0);
		}
		else
		{
			// write the new timer 1 reading only ring 3, leaving timer 0 how it was.
			bPSWriteMSR(0x11,dw_low | (((u1_event) | (bRing3?(1<<7):0) | (bRing0?(1<<6):0))<<16) ,0);
		}
	}
	else
	{
		// setup the model specific registers for the P6/PII
		BYTE	u1_event;
		BYTE	u1_mask;

		u1_event= (ui_count & (0xFF0000))>>16;
		u1_mask = (ui_count & 0xFF);

		b_res = bPSWriteMSR(0x187,(u1_event | (bRing3?(1<<16):0) | (bRing0?(1<<17):0) | (1<<22) | (1<<18) | (u1_mask<<8)),0);

		// has the IOCTL call failed?
		if (!b_res)
		{
			OutputDebugString("Set Event Counter 1 failed..\n");
			return;
		}
	}

	u4Counter1 = ui_count;

#endif
}


//**********************************************************************************************
// Read model specific register
//
BOOL bPSReadMSR(DWORD dw_reg, DWORD* pdw_low, DWORD* pdw_high)
{
#if VER_TIMING_STATS
	BOOL	b_res;

	if (i4Going==0)
		return FALSE;

	// set the register that we want to read and the driver will put the contents of this buffer
	// into the output buffer
	dwIn[0] = dw_reg;

	b_res = DeviceIoControl
			(
				hFile,						// Handle to device
				(DWORD) IOCTL_READ_MSR,	// IO Control code for Read
				dwIn,						// Buffer to driver.
				4,							// Length of buffer in bytes.
				dwOut,						// Buffer from driver.
				8,							// Length of buffer in bytes.
				&dwReturnedLength,			// Bytes placed in DataBuffer.
				NULL						// NULL means wait till op. completes.
			);

	if (dwReturnedLength != 8)
		b_res = FALSE;

	if (b_res)
	{
		*pdw_low	= dwOut[0];
		*pdw_high	= dwOut[1];
	}

	return b_res;
#else
	return FALSE;
#endif
}


//**********************************************************************************************
// Write model specific register
//
BOOL bPSWriteMSR(DWORD dw_reg, DWORD dw_low, DWORD dw_high)
{
#if VER_TIMING_STATS
	BOOL	b_res;

	if (i4Going==0)
		return FALSE;

	// set the register that we want to read and the driver will put the contents of this buffer
	// into the output buffer
	dwIn[0] = dw_reg;
	dwIn[1] = dw_low;
	dwIn[2] = dw_high;

	b_res = DeviceIoControl
			(
				hFile,						// Handle to device
				(DWORD) IOCTL_WRITE_MSR,	// IO Control code for Read
				dwIn,						// Buffer to driver.
				12,							// Length of buffer in bytes.
				NULL,						// Buffer from driver.
				0,							// Length of buffer in bytes.
				&dwReturnedLength,			// Bytes placed in DataBuffer.
				NULL						// NULL means wait till op. completes.
			);

	return b_res;
#else
	return FALSE;
#endif
}



//**********************************************************************************************
// Boosts the priority of an app. This is useful to prevent task switches.
//
void PSMaximumPriority(HANDLE hinst)
{
#if VER_TIMING_STATS
	// nothing is going to get in here!!!
	if (SetPriorityClass(hinst, REALTIME_PRIORITY_CLASS ) == FALSE)
	{
		OutputDebugString("Failed to boost app priority.\n");
	}
#endif
}



//**********************************************************************************************
// restores the priority of an app so task switchs occur as normal.
//
void PSNormalPriority(HANDLE hinst)
{
#if VER_TIMING_STATS
	SetPriorityClass(hinst, NORMAL_PRIORITY_CLASS);
#endif
}



//**********************************************************************************************
// Get the counter array for the current processor
static SProfileText* pptPSGetCounters()
{
#if VER_TIMING_STATS
	switch (i4Processor)
	{
	case 0:
		return ptCountersPentium;

	case 1:
		return ptCountersP6;

	case 2:
		return ptCountersPII;
	}
	return ptEmpty;
#else
	return NULL;
#endif
}



//**********************************************************************************************
// Returns a popup menu with all MSR options for the current processor on timer 0
//
HMENU PSMakeProcessorMenuCounter0(uint32 u4_res)
{
#if VER_TIMING_STATS
	HMENU			hmenu	= CreatePopupMenu();
	SProfileText*	ppt		= pptPSGetCounters();
	int				i		= 0;
	int				i_split	= GetSystemMetrics(SM_CYSCREEN)/GetSystemMetrics(SM_CYMENU);


	Assert(hmenu);
	Assert(ppt);

	u4Menu0Base = u4_res;

	// go around all the current processor counters
	while (ppt[i].u4CounterVal != 0xffffffff)
	{
		if (ppt[i].u4CounterVal & (1<<8))
		{
			InsertMenu(hmenu,
						0xffffffff,
						MF_BYPOSITION | MF_STRING | 
							((ppt[i].u4CounterVal==u4Counter0)?MF_CHECKED:0 | 
							((i%i_split)==0 && i!=0?MF_MENUBARBREAK:0)),
						u4_res+i,
						ppt[i].strTextName);

			if (ppt[i].u4CounterVal==u4Counter0)
			{
				u4LastMenu0 = i;
			}
		}
		i++;
	}
	
	hmenu0 = hmenu;

	return hmenu;
#else
	return NULL;
#endif
}


//**********************************************************************************************
// Returns a popup menu with all MSR options for the current processor on timer 1
//
HMENU PSMakeProcessorMenuCounter1(uint32 u4_res)
{
#if VER_TIMING_STATS
	HMENU			hmenu	= CreatePopupMenu();
	SProfileText*	ppt		= pptPSGetCounters();
	int				i		= 0;
	int				i_split	= GetSystemMetrics(SM_CYSCREEN)/GetSystemMetrics(SM_CYMENU);

	Assert(hmenu);
	Assert(ppt);

	u4Menu1Base = u4_res;

	// go around all the current processor counters
	while (ppt[i].u4CounterVal != 0xffffffff)
	{
		if (ppt[i].u4CounterVal & (1<<9))
		{
			InsertMenu(hmenu,
						0xffffffff,
						MF_BYPOSITION | MF_STRING | 
							((ppt[i].u4CounterVal==u4Counter1)?MF_CHECKED:0) |
							((i%i_split)==0 && i!=0?MF_MENUBARBREAK:0),
						u4_res+i,
						ppt[i].strTextName);

			if (ppt[i].u4CounterVal==u4Counter1)
			{
				u4LastMenu1 = i;
			}

		}
		i++;
	}

	hmenu1 = hmenu;

	return hmenu;
#else
	return NULL;
#endif
}


//**********************************************************************************************
// Menu 0 has been clicked....
//
void PSMenu0Click(uint32 u4_id)
{
#if VER_TIMING_STATS

	Assert(hmenu0);
	Assert(u4Menu0Base);

	SProfileText*	ppt		= pptPSGetCounters();
	PSSelectCounter0(ppt[u4_id].u4CounterVal);

	// if we have the last option ticked then untick it
	if (u4LastMenu0 != 0xffffffff)
	{
		CheckMenuItem(hmenu0,u4LastMenu0 + u4Menu0Base, MF_BYCOMMAND | MF_UNCHECKED);
	}

	CheckMenuItem(hmenu0,u4_id + u4Menu0Base, MF_BYCOMMAND | MF_CHECKED);

	u4LastMenu0 = u4_id;

#endif
}



//**********************************************************************************************
// Menu 1 has been clicked....
//
void PSMenu1Click(uint32 u4_id)
{
#if VER_TIMING_STATS

	Assert(hmenu1);
	Assert(u4Menu1Base);

	SProfileText*	ppt		= pptPSGetCounters();
	PSSelectCounter1(ppt[u4_id].u4CounterVal);

	// if we have the last option ticked then untick it
	if (u4LastMenu1 != 0xffffffff)
	{
		CheckMenuItem(hmenu1,u4LastMenu1 + u4Menu1Base, MF_BYCOMMAND | MF_UNCHECKED);
	}

	CheckMenuItem(hmenu1,u4_id + u4Menu1Base, MF_BYCOMMAND | MF_CHECKED);

	u4LastMenu1 = u4_id;

#endif
}


//**********************************************************************************************
// Menu 1 has been clicked....
//
const char* strPSGetMenuText(uint32 u4_menu)
{
#if VER_TIMING_STATS

	static char* strEmpty = "CLOCK Clock ticks";

	SProfileText*	ppt		= pptPSGetCounters();

	Assert(u4_menu<=1);

	if (u4_menu == 0)
	{
		if (u4LastMenu0 == 0xffffffff)
		{
			return strEmpty;
		}

		return ppt[u4LastMenu0].strTextName;
	}
	else
	{
		if (u4LastMenu1 == 0xffffffff)
		{
			return strEmpty;
		}

		return ppt[u4LastMenu1].strTextName;
	}
#else
	return "INVALID";
#endif
}