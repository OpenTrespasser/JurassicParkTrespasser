/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Interface header file for all other global header files. This header file includes all
 *		global header files in the appropriate order.
 *
 *		This file must be the FIRST (non-ANSI) #include header file in EVERY source file in the
 *		project. Therefore it is not necessary to #include this file in any other header files.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Gblinc/common.hpp                                                     $
 * 
 * 14    96/10/18 18:22 Speter
 * Added <new.h>.
 * 
 * 
 * 13    10/08/96 9:09p Agrant
 * moved warning pragmas out of common into warnings.hpp
 * included warnings.hpp
 * 
 * 12    9/16/96 7:03p Gstull
 * 
 * 11    96/09/16 11:38 Speter
 * Added include of Ptr.hpp.
 * 
 * 10    96/07/31 15:33 Speter
 * Disabled warning 4786.
 * Moved order of includes.
 * 
 * 9     7/01/96 11:44a Mlange
 * Added include for array.hpp.
 * 
 * 8     96/05/29 17:25 Speter
 * Added InitSys.hpp include.
 * 
 * 7     96/05/17 10:54 Speter
 * Added pragma to turn off warning 4250
 * 
 * 6     5/03/96 11:33a Mlange
 * Changes from code review. Now uses forward slashes to delimit directories in #include paths.
 * Added include for StdLibEx.hpp. Fixed typo in comment.
 * 
 * 5     4/30/96 2:02p Mlange
 * Updated for changes to the coding standards. Removed include for Lib\StdLibEx.hpp.
 * 
 * 4     96/04/19 17:59 Speter
 * Added lib/std/stdlibex.hpp.
 * 
 * 3     4/18/96 4:59p Mlange
 * Updated for changes to the coding standards.
 * 
 * 2     4/16/96 10:32a Mlange
 * Added #include for uassert.hpp.
 * 
 * 1     4/12/96 3:19p Mlange
 * Interface header file for all other global header files.
 *
 **********************************************************************************************/

#ifndef HEADER_GBLINC_COMMON_HPP
#define HEADER_GBLINC_COMMON_HPP

#include "GblInc/Warnings.hpp"
#include <stddef.h>
#include <new.h>

#include "GblInc/BuildVer.hpp"

#include "Lib/Std/UTypes.hpp"
#include "Lib/Std/UAssert.hpp"
#include "Lib/Std/UDefs.hpp"
#include "Lib/Std/StdLibEx.hpp"
#include "Lib/Std/InitSys.hpp"
#include "Lib/Std/Array.hpp"
#include "Lib/Std/Ptr.hpp"


#endif
