/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *
 *		Debug console class
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/DebugConsole.hpp                                             $
 * 
 * 11    98/09/17 16:40 Speter
 * No more console output in final mode.
 * 
 * 10    3/22/98 5:03p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 9     2/10/98 3:19p Rwyatt
 * Changed function to be static
 * 
 * 8     10/15/97 8:37p Rwyatt
 * New memory checking macros
 * 
 * 7     9/18/97 12:02p Rwyatt
 * Release and final mode fixed
 * 
 * 6     9/17/97 3:35p Rwyatt
 * New ostream class for debug window, dprintf now uses this.
 * 
 * 5     97/06/05 17:01 Speter
 * Added unformatted dprint function.
 * 
 * 4     5/19/97 9:34p Rwyatt
 * Restructured and the second constructor has been removed
 * 
 * 3     5/19/97 7:18p Rwyatt
 * Added wrappers for c runtime library debug heaps.
 * 
 * 2     5/09/97 3:23p Rwyatt
 * initial implementation os simple text debug class
 * 
 * 1     5/01/97 6:41p Rwyatt
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_SYS_DEBUGCONSOLE_HPP
#define HEADER_LIB_SYS_DEBUGCONSOLE_HPP

#if VER_TEST

// Declare the debug console interface.

#include <iostream>
#include <fstream>
#include "crtdbg.h"

#pragma warning(disable:4237)

#ifdef _DEBUG

//void* __cdecl operator new(size_t nSize, const char* lpszFileName, int nLine);
//#define DEBUG_NEW new(__FILE__, __LINE__)
//#define new DEBUG_NEW

#endif


//**********************************************************************************************
// An unformatted print function for the console window.
extern void dprint(const char* str);

//**********************************************************************************************
// a printf function for the console window....
extern void __cdecl dprintf(char* str,...);



//**********************************************************************************************
//
class dbgstreambuf : public std::streambuf
{
public:
	//******************************************************************************************
	dbgstreambuf(); 

	dbgstreambuf(char* str_fname);

	//******************************************************************************************
	~dbgstreambuf();

	//******************************************************************************************
	// underflow and overflow are pure virtual and are the only 2 members that must be
	// implemented.
	//
	int underflow();

	//******************************************************************************************
	// Makes a string out of a char and sends it to the debugger and optionally a file
	//
	int overflow(int ch);

	//******************************************************************************************
	// data members
#if VER_DEBUG_TEXT
	char					buf[1028];			// buffer to accumulate the text
	uint32					u4Off;				// current offset in the above buffer
	bool					bFile;
	std::ofstream			dbgfile;			// ofstream for the debuglog if in joint mode
#endif
};


//**********************************************************************************************
// DO NOT CHANGE THESE.....
#define DEBUG_OUTPUT_NULL			0
#define DEBUG_OUTPUT_FILE			1
#define DEBUG_OUTPUT_CONSOLE		2
#define DEBUG_OUTPUT_BOTH			3


//**********************************************************************************************
// MAKE SURE ONLY 1 OF THESE IS PRESENT
//#define DEBUG_OUT			DEBUG_OUTPUT_NULL
//#define DEBUG_OUT			DEBUG_OUTPUT_FILE
//#define DEBUG_OUT			DEBUG_OUTPUT_CONSOLE
#define DEBUG_OUT			DEBUG_OUTPUT_BOTH		


//**********************************************************************************************
// global dout that is identical to cout but for the console debug window
#if (DEBUG_OUT == DEBUG_OUTPUT_FILE)
extern std::ofstream dout;
#else
extern std::ostream	dout;
#endif

// #if VER_TEST
#else

// Make null versions of the print functions.

inline void dprint(const char*)
{
}

inline void __cdecl dprintf(char*, ...)
{
}

//**********************************************************************************************
//
class nullstream
//
// Declare a dummy debug console interface that prints nothing.
//
//**************************************
{
};


// Define a null generic output operator.

template<class T> inline nullstream& operator<<(nullstream& ns, const T&)
{
	return ns;
}

// The dummy debug stream.
extern nullstream dout;

// #if VER_TEST #else
#endif


//**********************************************************************************************
// Debug macros to check the validity of memory blocks
//
#ifdef _DEBUG
// In debug mode uses Win32 debug functions to do the work...
#define	HEAP_PTR_VALID(x)		_CrtIsValidHeapPointer((void*)(x))
#define HEAP_BLOCK_VALID(x,s)	_CrtIsMemoryBlock((void*)(x), (unsigned int)(s), NULL, NULL, NULL)
#define MEMORY_BLOCK_VALID(x,s)	_CrtIsValidPointer((void*)(x), (unsigned int)(s), 1)
#else
// If not debug then just return true
#define	HEAP_PTR_VALID(x)		(1)
#define HEAP_BLOCK_VALID(x,s)	(1)
#define MEMORY_BLOCK_VALID(x,s)	(1)
#endif


//**********************************************************************************************
// Memory check class to help detect memory leaks..
//
class CMemCheck
{
#if VER_DEBUG_TEXT
#ifdef _DEBUG
	char			strMarker[256];
	_CrtMemState	dbgMemState;
	unsigned long	ulFlags;
#endif
#endif

public:
	//CMemCheck(unsigned long ulFlags = 0);
	CMemCheck(const char* strName = "UNNAMED", unsigned long ulFlags = 0);

	~CMemCheck();

	void HeapStats();

	void HeapDiff();

	void HeapDiffDump();

	void HeapCheck();

	static void HeapAllocBreak(long l_block);
};

//**********************************************************************************************
// Flags to control the above class

//**********************************************************************************************
// destructor is to a difference dump
#define MEM_DIFF_DUMP		0x00000001

//**********************************************************************************************
// destructor is to give stats on the differences
#define MEM_DIFF_STATS		0x00000002

//**********************************************************************************************
// destructor is to give stats on the global heap
#define MEM_GLOBAL_STATS	0x00000004

//**********************************************************************************************
// destructor is to check memory blocks for corruption.
#define MEM_GLOBAL_CHECK	0x00000008


#endif //#ifndef HEADER_LIB_SYS_DEBUGCONSOLE_HPP
