/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Header file to include the 'Windows.h' file without interfering with STL.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/W95/WinInclude.hpp                                                $
 * 
 * 8     1/22/98 2:45p Pkeet
 * Made definition of standard Windows macros conditional on the macros not already having been
 * defined.
 * 
 * 7     12/08/97 6:47p Rwyatt
 * Added LZExand.h below the default windows header.
 * 
 * 6     97/02/05 19:43 Speter
 * Removed WinMix and MIX types.  Defined STRICT type checking.
 * 
 * 5     10/08/96 9:41p Agrant
 * error for WinAlias before WinInclude
 * 
 * 4     10/08/96 9:26p Agrant
 * bool is now a typedef.
 * Pragma the bool reserved word warning off.
 * Error when WinAlias is included before WinInclude to avoid errors where DWORD is a long
 * instead of an int.  Quick fix only....
 * 
 * 
 * 3     96/10/04 17:01 Speter
 * Added crap to work around bool problem.
 * 
 * 2     7/31/96 10:27a Pkeet
 * Added 'WinMix' include.
 * 
 * 1     6/24/96 11:02a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_W95_WININCLUDE_HPP
#define HEADER_LIB_W95_WININCLUDE_HPP


// Check to make sure that WinAlias has not been included before now.
#ifdef HEADER_LIB_WINALIAS_HPP
#error  WinAlias.hpp included before WinInclude.hpp!  Reorder header files.
#endif

#ifndef STRICT
	#define STRICT
#endif // STRICT

#ifndef VC_EXTRA_LEAN
	#define VC_EXTRA_LEAN
#endif // VC_EXTRA_LEAN

#include <Windows.h>
#include "LZExpand.h"				// file decompress header file.

#pragma 

// Windows turns the "'bool' is a reserved word" warning on for us.
// This pragma turns the warning back off.
#pragma warning(disable:4237)

//
// The following macros conflict with STL templates, and therefore should be undefined.
//
#undef min
#undef max


#endif
