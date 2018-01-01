/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		memset functions for various integer sizes.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Mem.hpp                                                       $
 * 
 * 8     12/04/97 4:10p Rwyatt
 * Thw Win32 memory functions have been moved to their own header file to help solve header file
 * dependencies. The new header file is MemLimits.hpp but the code is implemented in mem.cpp, as
 * before.
 * 
 * 7     10/03/97 5:16p Rwyatt
 * Moved global memory function definitions to here from FastHeap.hpp
 * Added a global variable that contains the system page size..
 * 
 * 6     97/06/12 7:03p Pkeet
 * Added a C++ version of 'MemCopy32.'
 * 
 * 5     12/20/96 4:28p Cwalla
 * P5 fpuclear
 * 
 * 4     12/13/96 9:15a Cwalla
 * Removed inline, as it didn`t work...
 * 
 * 3     12/13/96 8:35a Cwalla
 * Made memclr32 an inline.
 * 
 * 2     12/05/96 3:03p Cwalla
 * Added memclr32 function.
 * 
 * 1     96/05/23 16:51 Speter
 * New memset functions.
 * 
 **********************************************************************************************/


#ifndef HEADER_LIB_STD_MEM_HPP
#define HEADER_LIB_STD_MEM_HPP


//******************************************************************************************
//
void memset8
(
	uint8* pu1_addr,					// Destination address.
	uint8 u1_val,						// Value to write.
	uint u_count						// How many values to write.
);
//
// Fill u_count bytes starting at pu1_addr with u1_val.
//
//**************************************


//******************************************************************************************
//
void memset16
(
	uint16* pu2_addr,					// Destination address.
	uint16 u2_val,						// Value to write.
	uint u_count						// How many values to write.
);
//
// Fill u_count shorts starting at pu2_addr with u2_val.
//
//**************************************

//******************************************************************************************
//
void memset32
(
	uint32* pu4_addr,					// Destination address.
	uint32 u4_val,						// Value to write.
	uint u_count						// How many values to write.
);
//
// Fill u_count longs starting at pu4_addr with u4_val.
//
//**************************************

//******************************************************************************************
//

void memclr
(
	void* pu4_addr,						// Destination address.
	uint u_count						// How many values to write.
);
//
// Fill u_count longs starting at pu4_addr with zero.
//
//**************************************

//**********************************************************************************************
//
void MemCopy32
(
	void* pv_dest,		// Destination address.
	void* pv_source,	// Source address.
	int   i_num_bytes	// Number of bytes.
);
//
// Copies the number of bytes from the source address to the destination address.
//
//**************************************

#endif
