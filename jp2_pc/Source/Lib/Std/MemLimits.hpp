/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Definition of global memory functions.
 *		These functions are  implemented in mem.cpp, this header file is seperate from mem.hpp
 *		because this header requires windows headers which cannot be included from mem.hpp
 *
 * To do:
 *
 * Notes:
 *		Lib/W95/WinInclude.hpp needs to be included before this file, it cannot be included
 *		within this file because it will cause compiler errors due to order dependencies.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/MemLimits.hpp                                                 $
 * 
 * 2     12/21/97 10:57p Rwyatt
 * Implementation of SetupSystemPageSize has moved to the cpp file to save including windows
 * headers in this file.
 * 
 * 1     12/04/97 3:20p Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_MEM_LIMITS_HPP
#define HEADER_LIB_STD_MEM_LIMITS_HPP


//******************************************************************************************
//
// Global Variables..
//
//
extern uint32 gu4SystemPageSize;
//
//**************************************


//**********************************************************************************************
// setup the global above with the system page size
//
void SetupSystemPageSize();
//
//*************************************


//**********************************************************************************************
// return the size of a system memory page
//
inline uint32 u4SystemPageSize
(
)
//*************************************
{
	// Page size is not a power of two!!, this is going to cause big trouble elsewhere.
	Assert(bPowerOfTwo(gu4SystemPageSize));
	return gu4SystemPageSize;
}




//**********************************************************************************************
//
uint32 u4TotalPhysicalMemory();
//
// Returns the total amount of physical memory (RAM) in the machine
//
//**************************************



//**********************************************************************************************
//
uint32 u4FreePhysicalMemory();
//
// Returns the total amount of free (usable) physical memory (RAM) in the machine
//
//**************************************



//**********************************************************************************************
//
uint32 u4FreePagefileMemory();
//
// Returns the amount of free space in the system page file
//
//**************************************



//**********************************************************************************************
//
uint32 u4FreeVirtualMemory();
//
// Returns the amount of free space in the apps virtual address space (2GB to start with)
//
//**************************************


//**********************************************************************************************
//
#define HEAP_BLOCK_COMMITED				0
#define HEAP_BLOCK_PART_COMMITED		1
#define HEAP_BLOCK_NOT_COMMITED			2
#define HEAP_BLOCK_FAIL					0xffffffff
//
// return values for memory status function..
//
//**************************************


//**********************************************************************************************
//
uint32	u4HeapBlockStatus(void* pv_adr,uint32 u4_length,void** ppv_base,	uint32*	pu4_region_size);
//
// return the virtual status of the block passed in
//
//**************************************

#endif