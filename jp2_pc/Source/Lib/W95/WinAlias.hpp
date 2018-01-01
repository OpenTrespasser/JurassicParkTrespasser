/****************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:   Aliases for standard Windows types and definitions. This
 *             header is called when <windows.h> is not desired for a module
 *             that calls another module requiring <windows.h>. In essence,
 *             this is a tiny version of <windows.h>.
 *
 * To do:      Update as additional Windows types and handles are needed.
 *
 ****************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/WinAlias.hpp                               $
 * 
 * 19    9/10/98 10:10p Shernd
 * added HRGN
 * 
 * 18    8/26/98 5:41p Asouth
 * Moved RGB out of the _WINDOWS_ ifdef
 * 
 * 17    8/25/98 11:55a Rvande
 * Fixed forward declaration of FILE to comply with MSL's definition
 * 
 * 16    8/05/97 10:34a Mlange
 * Added HPEN styles.
 * 
 * 15    97/05/21 17:28 Speter
 * Added HPEN alias.
 * 
 * 14    5/08/97 4:08p Mlange
 * Added HANDLE definition.
 * 
 * 13    97/02/05 19:43 Speter
 * Removed WinMix and MIX types.  Defined STRICT type checking.
 * 
 * 12    7/31/96 10:29a Pkeet
 * Added 'WinMix' include.
 * 
 * 11    7/18/96 3:17p Pkeet
 * Added 'POINT' and 'MSG' structure definitions.
 * 
 * 10    5/23/96 10:06a Pkeet
 * Added prefix for RECT.
 * 
 * 9     5/23/96 10:05a Pkeet
 * Added 'RECT' definition because Scott was wrong.
 * 
 * 8     96/05/16 18:20 Speter
 * Corrected definitions of LONG and LPARAM.
 * 
 * 7     96/05/16 13:39 Speter
 * Removed struct definitions for RECT, PALETTEENTRY, and RGBQUAD, because no
 * longer needed.
 * 
 * 6     5/08/96 2:02p Pkeet
 * Added HMENU definition.
 * 
 * 5     5/07/96 1:18p Pkeet
 * Added PALETTEENTRY, RGBQUAD and FILE structures.
 * 
 * 4     5/03/96 5:10p Pkeet
 * Remove FAR's.
 * 
 * 3     5/01/96 5:18p Pkeet
 * 
 * 2     96/04/19 17:57 Speter
 * Added some structs, made pointer typedefs standard.
 * 
 * 1     4/17/96 9:51a Pkeet
 * Aliases for common Windows types from windows.h.
 *
 ***************************************************************************/

#ifndef HEADER_LIB_WINALIAS_HPP
#define HEADER_LIB_WINALIAS_HPP

//
// Check to see if <windows.h> has already been included in the calling
// module. If it hasn't, create the minimum number of definitions required.
//


//
// Standard Windows macros.
//

#ifndef RGB
 // 98/08/26, ARS: moved outside of the _WINDOWS_ ifdef
 #define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif

#ifndef _WINDOWS_

//
// HPEN styles.
//
#define PS_SOLID            0
#define PS_DASH             1       /* -------  */
#define PS_DOT              2       /* .......  */
#define PS_DASHDOT          3       /* _._._._  */
#define PS_DASHDOTDOT       4       /* _.._.._  */
#define PS_NULL             5
#define PS_INSIDEFRAME      6
#define PS_USERSTYLE        7
#define PS_ALTERNATE        8


//
// Standard windows types.
//

typedef int			  BOOL;			// A Boolean value.
typedef char*         BSTR;			// A 32-bit character pointer.
typedef unsigned char BYTE;			// An 8-bit unsigned integer.
typedef unsigned int  COLORREF;		// A 32-bit value used as a color value.
typedef unsigned int  DWORD;		// A 32-bit unsigned integer.
typedef void*         HANDLE;		// Generic windows handle.
typedef long          LONG;			// A 32-bit signed integer.
typedef long          LPARAM;		// A 32-bit value passed to a window procedure or callback function.
typedef const char*   LPCSTR;		// A 32-bit pointer to a constant character string.
typedef char*         LPSTR;		// A 32-bit pointer to a character string.
typedef const char*   LPCTSTR;		// A 32-bit pointer to a portable constant character string.
typedef char*         LPTSTR;		// A 32-bit pointer to a portable character string.
typedef void*         LPVOID;		// A 32-bit pointer to an unspecified type.
typedef unsigned int  LRESULT;		// A 32-bit value returned from a window procedure or callback function.
typedef unsigned int  UINT;			// A 32-bit unsigned integer on Win32.
typedef void*         WNDPROC;		// A 32-bit pointer to a window procedure.
typedef unsigned short int WORD;	// A 16-bit unsigned integer.
typedef unsigned int  WPARAM;		// A value passed as to a window procedure or callback function.

//
// Standard Windows handles (as defined when STRICT macro is in effect).
//

typedef struct HDC__*		HDC;		// A Windows device context.
typedef struct HWND__*		HWND;		// A handle to a window.
typedef struct HINSTANCE__*	HINSTANCE;	// A handle to the instance of and application or thread.
typedef struct HMENU__*		HMENU;		// A menu handle.
typedef struct HPEN__*		HPEN;		// A drawing pen handle.
typedef struct HRGN__*      HRGN;       // A region handle

// Windows point structure.
// Prefix: pt
typedef struct tagPOINT
{
   LONG x;
   LONG y;
} POINT;

// Windows message structure.
// Prefix: msg
typedef struct tagMSG
{  
    HWND   hwnd;	   
    UINT   message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD  time;
    POINT  pt;
} MSG;


// Rectangle storage structure.
// Prefix: rect
typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT;

#endif

//
// StdIO.h types.
//

// FILE definition.
#ifndef _FILE_DEFINED
struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };

#ifdef __MWERKS__
 #include <stdio.h>
#else
 typedef struct _iobuf FILE;
#endif

// prefix: fil
#define _FILE_DEFINED
#endif

#endif
