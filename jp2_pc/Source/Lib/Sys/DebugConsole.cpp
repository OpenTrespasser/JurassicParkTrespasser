/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Repaly.hpp and replay support code
 *
 * To Do:
 *		
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/DebugConsole.cpp                                            $
 * 
 * 12    98/09/17 16:40 Speter
 * No more console output in final mode.
 * 
 * 11    8/28/98 11:57a Asouth
 * #ifdef for differences in STL versions
 * 
 * 10    2/10/98 3:20p Rwyatt
 * Changed AllocBreakSet
 * 
 * 9     9/17/97 10:39p Rwyatt
 * Release and final build did not work
 * 
 * 8     9/17/97 3:35p Rwyatt
 * Uses a derivation of iostream to output to the debug window. This means that the debug
 * window uses a syntax the same as cout and its output can be redirected to a file. Log files
 * can also be redirected to the debug window.
 * The global construction problem has been solved by using a #Pragma init_seg
 * 
 * 7     7/23/97 8:37p Rwyatt
 * No writes all debug window output to a log file. 
 * NOTE: This has introduced a memory leak but it is only in the debug build
 * 
 * 6     97/06/05 17:02 Speter
 * Added unformatted dprint function.
 * 
 * 5     5/20/97 1:30p Rwyatt
 * Put #ifdef around the HeapAllocBreak function
 * 
 * 4     5/19/97 9:34p Rwyatt
 * Restructured and the second constructor has been removed
 * 
 * 3     5/19/97 7:16p Rwyatt
 * Added wrappers for c runtime library debug heaps.
 * 
 * 2     5/01/97 6:44p Rwyatt
 * Added initial printf style function and made a global class so all files can output to the
 * windows console
 * 
 * 1     5/01/97 6:41p Rwyatt
 * 
 *********************************************************************************************/

//
// Includes.
//

#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "DebugConsole.hpp"
#include "stdio.h"
#include "Lib/Sys/Textout.hpp"

#if VER_TEST

//**********************************************************************************************
// globals that are put into the lib init section so they are constructed before any global
// constructors. This is required because global constructors can use dout.
#pragma warning(disable:4073)
#pragma init_seg(lib)

#ifndef DEBUG_OUT
#error Must define debug redirection in DebugConsole.cpp
#endif

#if (DEBUG_OUT == DEBUG_OUTPUT_NULL)

ostream					dout(NULL);

#elif (DEBUG_OUT == DEBUG_OUTPUT_FILE)

ofstream				dout("DebugLog.txt", ios::out|ios::trunc);

#elif (DEBUG_OUT == DEBUG_OUTPUT_CONSOLE)

static	dbgstreambuf	debug_stream;
ostream					dout(&debug_stream);

#elif (DEBUG_OUT == DEBUG_OUTPUT_BOTH)

static	dbgstreambuf	debug_stream("DebugLog.txt");
ostream					dout(&debug_stream);

#endif


//**********************************************************************************************
// Implementation of dbgstreambuf class which is derived from the C++ iostream classes....
// Naming is all lower case to be in line with outher iostream classes
//**********************************************************************************************

	//******************************************************************************************
	// constructor that just copies to debug console
	//
#ifdef __MWERKS__
	dbgstreambuf::dbgstreambuf() : streambuf()
#else
	dbgstreambuf::dbgstreambuf() : streambuf(NULL,0)
#endif
	{
#if VER_DEBUG_TEXT
		u4Off = 0;
		bFile = false;
#endif
	}

	//******************************************************************************************
	// constructor that copies to debug console and to a log file on disk..
	//
#ifdef __MWERKS__
	dbgstreambuf::dbgstreambuf(char* str_name) : streambuf()
#else
	dbgstreambuf::dbgstreambuf(char* str_name) : streambuf(NULL,0)
#endif
	{
#if VER_DEBUG_TEXT
		u4Off = 0;
		dbgfile.open(str_name,ios::out|ios::trunc);

		// set the bfile flag on the state of the open file
		bFile = dbgfile.is_open();
#endif
	}


	//******************************************************************************************
	//
	dbgstreambuf::~dbgstreambuf()
	{
#if VER_DEBUG_TEXT
		if (bFile)
		{
			dbgfile.close();
		}
#endif
	}

	//******************************************************************************************
	// underflow and overflow are pure virtual and are the only 2 members that must be
	// implemented.
	//
	int dbgstreambuf::underflow()
	{
		return EOF;
	}

	//******************************************************************************************
	// Makes a string out of a char and sends it to the debugger and optionally a file
	//
	int dbgstreambuf::overflow(int ch)
	{
#if VER_DEBUG_TEXT
		if (ch == EOF)
			return ch;

		buf[u4Off] = (char)ch;	
		u4Off++;
		if ((ch == '\n') || (u4Off>=1020))
		{
			buf[u4Off] = 0;
			OutputDebugString(buf);

			if (bFile)
			{
				// write the same string to the file
				dbgfile << buf;
			}

			u4Off = 0;
		}
#endif
		return ch;
	}


//**********************************************************************************************
//
void dprint(const char* str)
{
#if VER_DEBUG_TEXT
	dout << str;
#endif
}

//**********************************************************************************************
void __cdecl dprintf(char* str,...)
{
#if VER_DEBUG_TEXT

	char	buf[256];

	va_list argptr;
	va_start (argptr,str);
	vsprintf (buf,str,argptr);
	va_end (argptr);

	dout << buf;

#endif
}

// #if VER_TEST
#else

// Simply declare the dummy debug stream.
nullstream dout;

// #if VER_TEST #else
#endif

//*********************************************************************************************
// constructor takes a snap shot of memory
CMemCheck::CMemCheck(const char* str_marker,unsigned long ul_flags)
{
// only provide an implementation in debug modes.
#ifdef _DEBUG
#if VER_DEBUG_TEXT
	dout << "----------------\n";
	dout << "TAKING HEAP SNAPSHOT - MARKER :" << str_marker << '\n' ;
	dout << "----------------\n";

	// take a snap shot of the current heap
	_CrtMemCheckpoint(&dbgMemState);
	strcpy(strMarker,str_marker);

	ulFlags = ul_flags;
#endif // VER_DEBUG_TEXT
#endif
}



//*********************************************************************************************
//
void CMemCheck::HeapStats()
{
#ifdef _DEBUG
#if VER_DEBUG_TEXT
	dout << "----------------\n";
	dout << "GLOBAL HEAP STATS AT MARKER : "<< strMarker << '\n';
	_CrtMemDumpStatistics(&dbgMemState);
	dout << "----------------\n";
#endif
#endif
}



//*********************************************************************************************
//
void CMemCheck::HeapDiff()
{
#ifdef _DEBUG
#if VER_DEBUG_TEXT	
	_CrtMemState	dbgCurrentState,dbgDifference;

	_CrtMemCheckpoint(&dbgCurrentState);

	dout << "----------------\n";
	dout << "HEAP DIFFERENCES SINCE MARKER : " << strMarker << '\n';
	_CrtMemDifference(&dbgDifference,&dbgMemState,&dbgCurrentState);
	_CrtMemDumpStatistics(&dbgDifference);
	dout << "----------------\n";
#endif
#endif
}


//*********************************************************************************************
//
void CMemCheck::HeapDiffDump()
{
#ifdef _DEBUG
#if VER_DEBUG_TEXT	
	dout << "----------------\n";
	dout << "HEAP DUMP SINCE MARKER : " << strMarker << '\n';
	_CrtMemDumpAllObjectsSince(&dbgMemState);
	dout << "----------------\n";
#endif
#endif
}


//*********************************************************************************************
//
void CMemCheck::HeapAllocBreak(long l_block)
{
#ifdef _DEBUG
#if VER_DEBUG_TEXT
	_CrtSetBreakAlloc(l_block);
#endif
#endif
}



//*********************************************************************************************
//
void CMemCheck::HeapCheck()
{
#if VER_DEBUG_TEXT
#ifdef _DEBUG
	dout << "----------------\n";
	dout << "HEAP INTEGRITY CHECK\n";
	_CrtCheckMemory();
	dout << "----------------\n";
#endif
#endif
}



//*********************************************************************************************
//
CMemCheck::~CMemCheck()
{
#if VER_DEBUG_TEXT
#ifdef _DEBUG
	// check the global heap??
	if (ulFlags & MEM_GLOBAL_CHECK)
	{
		HeapCheck();
	}

	// difference stats from when constructed??
	if (ulFlags & MEM_DIFF_STATS)
	{
		HeapDiff();
	}

	// difference dump from when constructed??
	if (ulFlags & MEM_DIFF_DUMP)
	{
		HeapDiffDump();
	}

	// stats on the global heap???
	if (ulFlags & MEM_GLOBAL_STATS)
	{
		HeapStats();
	}
#endif
#endif
}
