/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		MFC and subproject includes.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/StdAfx.h                                                      $
 * 
 * 8     98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 7     1/22/98 2:46p Pkeet
 * Reduced the number of includes.
 * 
 * 6     96/12/31 16:43 Speter
 * Added common files to header.  Speeds up compiles using precompiled headers.
 * 
 * 5     9/16/96 8:18p Gstull
 * Added project relative path to the include statements.
 * 
 * 4     9/13/96 2:57p Pkeet
 * Removed unnecessary includes.
 * 
 * 3     8/20/96 4:48p Pkeet
 * Added fogging switch and dialog.
 * 
 * 2     8/09/96 3:07p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_STDAFX_HPP
#define HEADER_GUIAPP_STDAFX_HPP


//
// Macros.
//

// Make sure this constant hasn't already been defined.
#ifndef VC_EXTRALEAN
// Exclude rarely-used stuff from Windows headers.
#define VC_EXTRALEAN		
#endif

//
// Includes.
//

#if (_MSC_VER >= 1100)
// MFC defines some comparison operators that differ from STL templated operators only
// by calling convertion resulting in this warning.
#pragma warning(disable: 4666)
#endif

// MFC includes.
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#undef min
#undef max

// Project includes.
#include "resource.h"
#include "Common.hpp"

// GUIApp helper functions.
#include "GUITools.hpp"

#endif

