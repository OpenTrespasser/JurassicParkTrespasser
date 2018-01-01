/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of support code for reading processor specific registers and performance
 * registers.
 *
 * WinInclude.hpp must be included before this file...
 *
 * This file uses the windows types because it is included from within the driver so do not
 * change it!
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/PerformanceCount.hpp                                          $
 * 
 * 3     8/28/97 4:05p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 4     8/26/97 2:00a Rwyatt
 * Will return a string for the current menu selection
 * 
 * 3     8/25/97 5:20p Rwyatt
 * Major change:
 * No uses the VER_PROFILE_STAT compile option so all the stats come and go together.
 * Works with the profile system to allow it to montior any event in the same format as the
 * previous stats.
 * Builds a GDI menu with all the perfornace events for the current processor.
 * Evaluates to no code in a FINAL build
 * 
 * 2     7/22/97 1:29a Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#ifndef PERFORMANCE_COUNTER_HEADER
#define PERFORMANCE_COUNTER_HEADER

#include "common.hpp"
#include "Config.hpp"
#include "winioctl.h"


//**********************************************************************************************
//**********************************************************************************************
//  DO NOT CHANGE ANY CONSTANT VALUES IN THIS FILE BECAUSE THE DRIVER WILL NEED TO BE REBUILT.
//**********************************************************************************************
//**********************************************************************************************


//**********************************************************************************************
// Define the IOCTL codes we will use.  The IOCTL code contains a command
// identifier, plus other information about the device, the type of access
// with which the file must have been opened, and the type of buffering.

// Device type           -- in the "User Defined" range."
#define DEVICE_FILE_TYPE 40000


// The IOCTL function codes from 0x800 to 0xFFF are for customer use.

#define IOCTL_WRITE_MSR \
    CTL_CODE( DEVICE_FILE_TYPE, 0x900, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_READ_MSR \
    CTL_CODE( DEVICE_FILE_TYPE, 0x901, METHOD_BUFFERED, FILE_READ_ACCESS )
//**********************************************************************************************




//**********************************************************************************************
// PENTIUM PERFORMANCE COUNTERS.
//**********************************************************************************************
#define	P5_ANYSG	0x000F0300		//Any Segment Register Loaded
#define	P5_BANKS	0x000A0300		//Bank Conflicts
#define	P5_BRANC	0x00120300		//Branches
#define	P5_MMXBF	0x003A0100		//BTB False entries
#define	P5_BTBHT	0x00130300		//BTB Hits
#define	P5_MMXBM	0x003A0200		//BTB misprediction on a Not-Taken Branch
#define	P5_PSLDA	0x001F0300		//Clocks stalled due to Address Generation Interlock
#define	P5_PSDWR	0x00190300		//Clocks stalled due to writes backed up
#define	P5_PXDWR	0x003B0100		//Clocks stalled due to full write buffers while executing MMX instructions
#define	P5_PXZWR	0x003B0200		//Clocks stalled on MMX instruction write to E or M state line
#define	P5_PSWDR	0x001A0300		//Clocks stalled waiting for Data Memory Read
#define	P5_NCLSW	0x001B0300		//Clocks stalled writing an E or M state line
#define	P5_CLOCL	0x00180300		//Clocks while bus cycle active
#define	P5_COCMS	0x000E0300		//Code Cache Misses
#define	P5_COCRD	0x000C0300		//Code Cache Reads
#define	P5_COTLB	0x000D0300		//Code TLB Misses
#define	P5_MMXHS	0x00300100		//Cycles Not in HALT State
#define	P5_MMXD1	0x00330200		//D1 Starvation and one instruction in FIFO
#define	P5_DCLWB	0x00060300		//Data Cache Lines Written Back
#define	P5_DCSHT	0x00080300		//Data Cache Snoop Hits
#define	P5_DCSNP	0x00070300		//Data Cache Snoops
#define	P5_MMXB1	0x002D0100		//EMMS Instructions Executed
#define	P5_DTCRD	0x00000300		//Data Cache Reads
#define	P5_DTRMS	0x00030300		//Data Read Misses
#define	P5_DTRWM	0x00290300		//Data Read or Write Misses
#define	P5_DTRWR	0x00280300		//Data Reads or Data Writes
#define	P5_DTTLB	0x00020300		//Data TLB Miss
#define	P5_DWRIT	0x00010300		//Data Cache Writes
#define	P5_DWRMS	0x00040300		//Data Write Miss
#define	P5_MMXFP	0x00320100		//Floating Point Stalls
#define	P5_FLOPS	0x00220300		//Floating Point Operations
#define	P5_HWINT	0x00270300		//Hardware Interrupts
#define	P5_INSTR	0x00160300		//Instructions Executed
#define	P5_INSTV	0x00170300		//Instructions Executed in the V-Pipe (Pairing)
#define	P5_IORWC	0x001D0300		//I/O Read or Write Cycle
#define	P5_MAIBP	0x00090300		//Memory Access in Both Pipes
#define	P5_MISAL	0x000B0300		//Misaligned Data Memory Reference
#define	P5_MMXF1	0x00360100		//Misaligned Data Memory Reference on MMX instruction
#define	P5_MMXE1	0x00340100		//MMX Instruction Data Writes
#define	P5_MMXE2	0x00340200		//MMX Instruction Data Write Misses
#define	P5_MMXD2	0x00310100		//MMX Instruction Data Read
#define	P5_MMXA1	0x002B0100		//MMX Instruction Executed in U-pipe
#define	P5_MMXA2	0x002B0200		//MMX Instruction Executed in V-pipe
#define	P5_MMXG1	0x00380100		//MMX Multiply Unit Interlock
#define	P5_MMXG2	0x00380200		//MOVD/MOVQ store stall due to previous operation
#define	P5_NOCMR	0x001E0300		//Non-Cacheable Memory Reads
#define	P5_NOCMW	0x002E0200		//Non-Cacheable Memory Writes
#define	P5_PFLSH	0x00150300		//Pipeline flushes
#define	P5_MMXWB	0x00350100		//Pipeline flushes due to wrong branch prediction
#define	P5_MMXWJ	0x00350200		//Pipeline flushes due to wrong branch prediction resolved in WB-stage
#define	P5_MMXF2	0x00360200		//Pipeline Stalled Waiting for MMX instruction data memory read
#define	P5_MMXRT	0x00390100		//Returns
#define	P5_MMXRP	0x00370200		//Returns Predicted (Correctly and Incorrectly)
#define	P5_MMXRI	0x00370100		//Returns Predicted Incorrectly
#define	P5_MMXRO	0x00390200		//RSB Overflows
#define	P5_MMXC1	0x002F0100		//Saturated MMX Instructions Executed
#define	P5_MMXC2	0x002F0200		//Saturations Performed
#define	P5_TBRAN	0x00140300		//Taken Branch or BTB Prediction
#define	P5_MMXTB	0x00320200		//Taken Branches
#define	P5_MMXB2	0x002D0200		//Transition from MMX to FP instructions
#define	P5_WHLCL	0x00050300		//Write (Hit) to M- or E-state line





//**********************************************************************************************
// PENTIUM PRO NO MMX PERFORMANCE COUNTERS.
//**********************************************************************************************
#define	P6_BACLR	0x10E60300		//BACLEARS Asserted (Testing)
#define	P6_RETDC	0x10E40300		//Bogus Branches
#define	P6_BRDEC	0x10E00300		//Branch Instructions Decoded
#define	P6_BRRET	0x10C40300		//Branch Instructions Retired
#define	P6_BRMSR	0x10C50300		//Branch Mispredictions Retired
#define	P6_BTBMS	0x10E20301		//BTB Misses
#define	P6_MASKD	0x10C60300		//Clocks while interrupts masked
#define	P6_MSKPN	0x10C70300		//Clocks while interrupts masked and an interrupt is pending
#define	P6_CLOCK	0x10790300		//Clockticks
#define	P6_DBUSY	0x10140200		//Cycles Divider Busy  (COUNTER 1 ONLY)
#define	P6_CSFET	0x10860300		//Cycles Instruction Fetch Stalled
#define	P6_FTSTL	0x10870300		//Cycles Instruction Fetch stalled -> pipe
#define	P6_L2BBS	0x10220300		//Cycles L2 Data Bus Busy
#define	P6_L2BBT	0x10230300		//Cycles L2 Data Bus Busy transferring data to CPU
#define	P6_DMREF	0x10430300		//Data Memory References (all)
#define	P6_FPDIV	0x10130200		//Divides					(COUNTER 1 ONLY)
#define	P6_BBIFT	0x10680300		//External Bus Burst Instruction Fetches
#define	P6_BURST	0x10650300		//External Bus Burst Read Operations
#define	P6_BUSTB	0x106E0300		//External Bus Burst Transactions
#define	P6_BUSBS	0x10620300		//External Bus Cycles - DRDY Asserted (busy)
#define	P6_BLOCK	0x10630300		//External Bus Cycles - LOCK signal asserted
#define	P6_BRBNR	0x10610300		//External Bus Cycles While BNR Asserted
#define	P6_BRHIT	0x107A0300		//External Bus Cycles While HIT Asserted
#define	P6_BRHTM	0x107B0300		//External Bus Cycles While HITM Asserted
#define	P6_BBRCV	0x10640300		//External Bus Cycles While Processor receiving data
#define	P6_BRSST	0x107E0300		//External Bus Cycles While Snoop Stalled
#define	P6_BRWRA	0x10600300		//External Bus Cycles While Receive Active
#define	P6_BUSDF	0x106D0300		//External Bus Deferred Transactions
#define	P6_BUSIO	0x106C0300		//External Bus I/O Bus Transactions
#define	P6_BINVL	0x10690300		//External Bus Invalidate Transactions
#define	P6_BMALL	0x106F0300		//External Bus Memory Transactions
#define	P6_BPTMO	0x106B0300		//External Bus Partial Memory Transactions
#define	P6_BPRBT	0x106A0300		//External Bus Partial Read Transactions
#define	P6_BRINV	0x10660300		//External Bus Read for Ownership Transaction
#define	P6_BRDCD	0x10600300		//External Bus Request Outstanding
#define	P6_BSALL	0x10700300		//External Bus Transactions (All)
#define	P6_BMLEV	0x10670300		//External Bus Writeback M-state Evicted
#define	P6_FPOPS	0x10C10100		//FP operations retired			(COUNTER 0 ONLY)
#define	P6_FPOPE	0x10100100		//FP Computational Operations Executed (COUNTER 0 ONLY)
#define	P6_FPEOA	0x10110200		//FP Except Cases handled by Microcode (COUNTER 1 ONLY)
#define	P6_INSTD	0x10D00300		//Instructions Decoded
#define	P6_INSTR	0x10C00300		//Instructions Retired
#define	P6_INTLB	0x10850300		//Instructions TLB Misses
#define	P6_HWINT	0x10C80300		//Hardware Interrupts Received
#define	P6_DCALO	0x1045030F		//L1 Lines Allocated
#define	P6_DCMAL	0x10460300		//L1 M-state Data Cache Lines Allocated
#define	P6_DCMEV	0x10470300		//L1 M-state Data Cache Lines Evicted
#define	P6_DCOUT	0x10480300		//L1 Misses outstanding (weighted)
#define	P6_L2STR	0x10210300		//L2 address strobes => address bus utilization
#define	P6_L2MCF	0x10280301		//L2 Cache Instruction Fetch Misses (highly correlated)
#define	P6_L2FET	0x1028030F		//L2 Cache Instruction Fetches
#define	P6_L2DRM	0x10290301		//L2 Cache Data Read Misses (highly correlated)
#define	P6_L2DMR	0x1029030F		//L2 Cache Data Reads
#define	P6_L2CMS	0x102E0301		//L2 Cache Request Misses (highly correlated)
#define	P6_L2DWM	0x102A0301		//L2 Cache Write Misses (highly correlated)
#define	P6_L2DMW	0x102A030F		//L2 Cache Writes
#define	P6_L2DCR	0x102E030F		//L2 Cache Requests
#define	P6_L2ALO	0x10240300		//L2 Lines Allocated
#define	P6_L2CEV	0x10260300		//L2 Lines Evicted
#define	P6_L2MAL	0x10250300		//L2 M-state Lines Allocated
#define	P6_L2MEV	0x10270300		//L2 M-state Lines Evicted
#define	P6_UOPSR	0x10C20300		//Micro-Ops Retired
#define	P6_UOPS1	0x10C20300		//Micro-Ops Retired - Low parallelization
#define	P6_MISMM	0x10050300		//Misaligned Data Memory Reference
#define	P6_FMULT	0x10120200		//Multiplies		(COUNTER 1 ONLY)
#define	P6_STRBB	0x10030300		//Store Buffer Block
#define	P6_STBDC	0x10040300		//Store Buffer Drain Cycles
#define	P6_NPRTL	0x10D20300		//Renaming Stalls
#define	P6_RSTAL	0x10A20300		//Resource Related Stalls
#define	P6_SEGLD	0x10060300		//Segment register loads
#define	P6_BTAKR	0x10C90300		//Taken Branch Retired
#define	P6_BTAKM	0x10CA0300		//Taken Branch Mispredictions
#define	P6_TOIFM	0x10810300		//Total Instruction Fetch Misses
#define	P6_CMREF	0x10800300		//Total Instruction Fetches




//**********************************************************************************************
// PENTIUM II / PENTIUM PRO WITH MMX PERFORMANCE COUNTERS.
//**********************************************************************************************
#define	PII_BACLR	0x20E60300		//BACLEARS Asserted (Testing)
#define	PII_RETDC	0x20E40300		//Bogus Branches
#define	PII_BRDEC	0x20E00300		//Branch Instructions Decoded
#define	PII_BRRET	0x20C40300		//Branch Instructions Retired
#define	PII_BRMSR	0x20C50300		//Branch Mispredictions Retired
#define	PII_BTBMS	0x20E20301		//BTB Misses
#define	PII_MASKD	0x20C60300		//Clocks while interrupts masked
#define	PII_MSKPN	0x20C70300		//Clocks while interrupts masked and an interrupt is pending
#define	PII_CLOCK	0x20790300		//Clockticks
#define	PII_DBUSY	0x20140200		//Cycles Divider Busy  (COUNTER 1 ONLY)
#define	PII_CSFET	0x20860300		//Cycles Instruction Fetch Stalled
#define	PII_FTSTL	0x20870300		//Cycles Instruction Fetch stalled -> pipe
#define	PII_L2BBS	0x20220300		//Cycles L2 Data Bus Busy
#define	PII_L2BBT	0x20230300		//Cycles L2 Data Bus Busy transferring data to CPU
#define	PII_DMREF	0x20430300		//Data Memory References (all)
#define	PII_FPDIV	0x20130200		//Divides					(COUNTER 1 ONLY)
#define	PII_BBIFT	0x20680300		//External Bus Burst Instruction Fetches
#define	PII_BURST	0x20650300		//External Bus Burst Read Operations
#define	PII_BUSTB	0x206E0300		//External Bus Burst Transactions
#define	PII_BUSBS	0x20620300		//External Bus Cycles - DRDY Asserted (busy)
#define	PII_BLOCK	0x20630300		//External Bus Cycles - LOCK signal asserted
#define	PII_BRBNR	0x20610300		//External Bus Cycles While BNR Asserted
#define	PII_BRHIT	0x207A0300		//External Bus Cycles While HIT Asserted
#define	PII_BRHTM	0x207B0300		//External Bus Cycles While HITM Asserted
#define	PII_BBRCV	0x20640300		//External Bus Cycles While Processor receiving data
#define	PII_BRSST	0x207E0300		//External Bus Cycles While Snoop Stalled
#define	PII_BRWRA	0x20600300		//External Bus Cycles While Receive Active
#define	PII_BUSDF	0x206D0300		//External Bus Deferred Transactions
#define	PII_BUSIO	0x206C0300		//External Bus I/O Bus Transactions
#define	PII_BINVL	0x20690300		//External Bus Invalidate Transactions
#define	PII_BMALL	0x206F0300		//External Bus Memory Transactions
#define	PII_BPTMO	0x206B0300		//External Bus Partial Memory Transactions
#define	PII_BPRBT	0x206A0300		//External Bus Partial Read Transactions
#define	PII_BRINV	0x20660300		//External Bus Read for Ownership Transaction
#define	PII_BRDCD	0x20600300		//External Bus Request Outstanding
#define	PII_BSALL	0x20700300		//External Bus Transactions (All)
#define	PII_BMLEV	0x20670300		//External Bus Writeback M-state Evicted
#define	PII_FPOPS	0x20C10100		//FP operations retired			(COUNTER 0 ONLY)
#define	PII_FPOPE	0x20100100		//FP Computational Operations Executed (COUNTER 0 ONLY)
#define	PII_FPEOA	0x20110200		//FP Except Cases handled by Microcode (COUNTER 1 ONLY)
#define	PII_INSTD	0x20D00300		//Instructions Decoded
#define	PII_INSTR	0x20C00300		//Instructions Retired
#define	PII_INTLB	0x20850300		//Instructions TLB Misses
#define	PII_HWINT	0x20C80300		//Hardware Interrupts Received
#define	PII_DCALO	0x2045030F		//L1 Lines Allocated
#define	PII_DCMAL	0x20460300		//L1 M-state Data Cache Lines Allocated
#define	PII_DCMEV	0x20470300		//L1 M-state Data Cache Lines Evicted
#define	PII_DCOUT	0x20480300		//L1 Misses outstanding (weighted)
#define	PII_L2STR	0x20210300		//L2 address strobes => address bus utilization
#define	PII_L2MCF	0x20280301		//L2 Cache Instruction Fetch Misses (highly correlated)
#define	PII_L2FET	0x2028030F		//L2 Cache Instruction Fetches
#define	PII_L2DRM	0x20290301		//L2 Cache Read Misses (highly correlated)
#define	PII_L2DMR	0x2029030F		//L2 Cache Reads
#define	PII_L2CMS	0x202E0301		//L2 Cache Request Misses (highly correlated)
#define	PII_L2DWM	0x202A0301		//L2 Cache Write Misses (highly correlated)
#define	PII_L2DMW	0x202A030F		//L2 Cache Writes
#define	PII_L2DCR	0x202E030F		//L2 Cache Requests
#define	PII_L2ALO	0x20240300		//L2 Lines Allocated
#define	PII_L2CEV	0x20260300		//L2 Lines Evicted
#define	PII_L2MAL	0x20250300		//L2 M-state Lines Allocated
#define	PII_L2MEV	0x20270300		//L2 M-state Lines Evicted
#define	PII_UOPSR	0x20C20300		//Micro-Ops Retired
#define	PII_UOPS1	0x20C20300		//Micro-Ops Retired - Low parallelization
#define	PII_MISMM	0x20050300		//Misaligned Data Memory Reference
#define	PII_MMXIE	0x20B00300		//MMX Instructions Executed
#define	PII_MMXIR	0x20CE0300		//MMX Instructions Retired
#define	PII_PORT0	0x20B20301		//MMX micro-ops executed on Port 0
#define	PII_PORT1	0x20B20302		//MMX micro-ops executed on Port 1
#define	PII_PORT2	0x20B20304		//MMX micro-ops executed on Port 2
#define	PII_PORT3	0x20B20308		//MMX micro-ops executed on Port 3
#define	PII_MMXPA	0x20B30300		//MMX Packed Arithmetic
#define	PII_MMXPL	0x20B30310		//MMX Packed Logical
#define	PII_MMXPM	0x20B30301		//MMX Packed Multiply
#define	PII_MMXPO	0x20B30304		//MMX Packed Operations
#define	PII_MMXPS	0x20B30302		//MMX Packed Shift
#define	PII_MMXUO	0x20B30308		//MMX Unpacked Operations
#define	PII_FMULT	0x20120200		//Multiplies		(COUNTER 1 ONLY)
#define	PII_STRBB	0x20030300		//Store Buffer Block
#define	PII_STBDC	0x20040300		//Store Buffer Drain Cycles
#define	PII_NPRTL	0x20D20300		//Renaming Stalls
#define	PII_RSTAL	0x20A20300		//Resource Related Stalls
#define	PII_SAISE	0x20B10300		//Saturated Arithmetic Instructions Executed
#define	PII_SAISR	0x20CF0300		//Saturated Arithmetic Instructions Retired
#define	PII_SEGLD	0x20060300		//Segment register loads
#define	PII_SRSES	0x20D40301		//Segment Rename Stalls - ES
#define	PII_SRSDS	0x20D40302		//Segment Rename Stalls - DS
#define	PII_SRSFS	0x20D40304		//Segment Rename Stalls - FS
#define	PII_SRSGS	0x20D40308		//Segment Rename Stalls - GS
#define	PII_SRSXS	0x20D4030F		//Segment Rename Stalls - ES DS FS GS
#define	PII_SRNES	0x20D50300		//Segment Renames - ES
#define	PII_SRNDS	0x20D50302		//Segment Renames - DS
#define	PII_SRNFS	0x20D50304		//Segment Renames - FS
#define	PII_SRNGS	0x20D50308		//Segment Renames - GS
#define	PII_SRNXS	0x20D5030F		//Segment Renames - ES DS FS GS
#define	PII_SIMDA	0x20CD0300		//SIMD Assists (EMMS Instructions Executed)
#define	PII_BTAKR	0x20C90300		//Taken Branch Retired
#define	PII_BTAKM	0x20CA0300		//Taken Branch Mispredictions
#define	PII_TSMCD	0x20520300		//Time Self-Modifiying Code Detected
#define	PII_TOIFM	0x20810300		//Total Instruction Fetch Misses
#define	PII_CMREF	0x20800300		//Total Instruction Fetches
#define	PII_FPMMX	0x20CC0301		//Transitions from Floating Point to MMX
#define	PII_MMXFP	0x20CC0300		//Transitions from MMX to Floating Point


//**********************************************************************************************
// file specific variables for counter 0
#if VER_TIMING_STATS

	static unsigned __int64 __u8CountStart0;
	static unsigned __int64 __u8CountStop0;

	#define STARTCOUNTER_0	_asm mov	ecx, 0 \
							_asm _emit	0x0f \
							_asm _emit	0x33 \
							_asm mov DWORD PTR [__u8CountStart0], eax \
							_asm mov DWORD PTR [__u8CountStart0+4], edx

	#define STOPCOUNTER_0	_asm mov	ecx, 0 \
							_asm _emit	0x0f \
							_asm _emit	0x33 \
							_asm mov DWORD PTR [__u8CountStop0], eax \
							_asm mov DWORD PTR [__u8CountStop0+4], edx

	#define u4COUNTER_0 ((DWORD)(__u8CountStop0 - __u8CountStart0))
	#define u8COUNTER_0 (__u8CountStop0 - __u8CountStart0)

#else

	#define STARTCOUNTER_0
	#define STOPCOUNTER_0
	#define u4COUNTER_0 (DWORD)(0)
	#define u8COUNTER_0 (__int64)(0)

#endif


//**********************************************************************************************
// file specific variables for counter 1
#if VER_TIMING_STATS

	static unsigned __int64 __u8CountStart1;
	static unsigned __int64 __u8CountStop1;

	#define STARTCOUNTER_1	_asm mov	ecx, 1 \
							_asm _emit	0x0f \
							_asm _emit	0x33 \
							_asm mov DWORD PTR [__u8CountStart1], eax \
							_asm mov DWORD PTR [__u8CountStart1+4], edx

	#define STOPCOUNTER_1	_asm mov	ecx, 1 \
							_asm _emit	0x0f \
							_asm _emit	0x33 \
							_asm mov DWORD PTR [__u8CountStop1], eax \
							_asm mov DWORD PTR [__u8CountStop1+4], edx

	#define u4COUNTER_1 ((DWORD)(__u8CountStop1 - __u8CountStart1))
	#define u8COUNTER_1 (__u8CountStop1 - __u8CountStart1)

#else

	#define STARTCOUNTER_1
	#define STOPCOUNTER_1
	#define u4COUNTER_1 (DWORD)(0)
	#define u8COUNTER_1 (__int64)(0)

#endif


//**********************************************************************************************
// Init return codes
//
#define PS_INIT_FAIL	0x00000000		// failed to start
#define PS_INIT_OK		0x00000001		// started OK
#define PS_INIT_GOING	0x00000002		// already started


//**********************************************************************************************
// Init function
//
int iPSInit(void);

//**********************************************************************************************
// Close function
//
void PSClose(void);


//**********************************************************************************************
// check if active function
//
bool bPSGoing();


//**********************************************************************************************
// Set the ring3 flag
//
void PSRing3(bool b_ring);


//**********************************************************************************************
// return true if we are using ring 3
//
bool bPSRing3();


//**********************************************************************************************
// Set the ring0 flag
//
void PSRing0(bool b_ring);


//**********************************************************************************************
// return true if we are using ring 0
//
bool bPSRing0();




//**********************************************************************************************
// Set the performance event for counter 0
//
void PSSelectCounter0(unsigned int ui_count);


//**********************************************************************************************
// Set the performance event for counter 1
//
void PSSelectCounter1(unsigned int ui_count);


//**********************************************************************************************
// Boosts the priority of an app. This is useful to prevent task switches.
//
void PSMaximumPriority(HANDLE hinst);


//**********************************************************************************************
// restores the priority of an app so task switchs occur as normal.
//
void PSNormalPriority(HANDLE hinst);


//**********************************************************************************************
// Read model specific register
//
BOOL bPSReadMSR(DWORD dw_reg, DWORD* pdw_low, DWORD* pdw_high);


//**********************************************************************************************
// Write model specific register
//
BOOL bPSWriteMSR(DWORD dw_reg, DWORD dw_low, DWORD dw_high);


//**********************************************************************************************
// Returns a popup menu with all MSR options for the current processor on timer 0
//
HMENU PSMakeProcessorMenuCounter0(uint32 u4_res);


//**********************************************************************************************
// Returns a popup menu with all MSR options for the current processor on timer 1
//
HMENU PSMakeProcessorMenuCounter1(uint32 u4_res);


//**********************************************************************************************
// Menu 0 has been clicked....
//
void PSMenu0Click(uint32 u4_id);


//**********************************************************************************************
// Menu 1 has been clicked....
//
void PSMenu1Click(uint32 u4_id);


//**********************************************************************************************
// Text name for the current counter..
//
const char* strPSGetMenuText(uint32 u4_menu);

#endif //PERFORMANCE_COUNTER_HEADER

