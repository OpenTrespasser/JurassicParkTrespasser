/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Contains simple, useful global definitions and macros etc.
 *
 *		This header file is automatically included by "Common.hpp".
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/UDefs.hpp                                                     $
 * 
 * 9     98.09.19 12:37a Mmouni
 * Added #define for forceinline.
 * 
 * 8     97/11/24 16:48 Speter
 * Added "that" macro.
 * 
 * 7     5/03/96 12:11p Mlange
 * Added some comments.
 * 
 * 6     5/03/96 11:31a Mlange
 * Minor changes for coding standards.
 * 
 * 5     4/30/96 3:02p Mlange
 * Updated for changes to the coding standards.
 * 
 * 4     96/04/30 14:02 Speter
 * Added global overloaded "placement" operator new.
 * 
 * 3     4/18/96 5:10p Mlange
 * Updated for new coding standards. Moved existing definitions to lib\std\stdlibex.hpp. Added
 * several useful macros.
 * 
 * 2     4/16/96 3:03p Mlange
 * Moved definitions for the pre-processor TRUE and FALSE constants to buildver.hpp.
 * Added the Swap function.
 * 
 * 1     4/12/96 3:22p Mlange
 * Contains functions and constants for use in the entire project.
 *
 **********************************************************************************************/

#ifndef HEADER_GBLINC_UDEFS_HPP
#define HEADER_GBLINC_UDEFS_HPP



// 
// Macro to help avoid warning messages for unreferenced parameters.
//
// Example:
//		int iFoo(int i_y)
//		{
//			Unreferenced(i_y);
//	
//			return -1;
//		}
//
#define	Unreferenced(x)		(x) = (x)


//
// Macro for accessing the implicit return variable in MSVC.
// Example:
//		class CVector;
//		CVector v3Negate(const CVector& vc)
//		{
//			// Directly assign members of the return variable.  This will be faster than
//			//  return CVector(-vc.X, -vc.Y, -vc.Z);
//			that->X = -vc.X;
//			that->Y = -vc.Y;
//			that->Z = -vc.Z;
//
//			// This return statement is necessary for compiler consistency, but should
//			// generate no code.
//			return *that;
//		}
//
#define that __$ReturnUdt


//
// Macro to force a function to be inline (in MSVC 6.0)
// Used just like the inline keyword.
//
#if _MSC_VER >= 1200

#define forceinline __forceinline

#else

#define forceinline inline

#endif


//
// Some macros for converting constants into strings and for concatenating constants.
// The macros are essentialy defined twice, this is to ensure that the constants which
// the macros operate on are evaluated by the pre-processor first. Always use the second
// definition.
//

// Macro to convert constant to a string - use 'prep_conv_str' only!
#define prep_iconv_str(s)  # s
#define prep_conv_str(s)   prep_iconv_str(s)

// Macro to concatenate two constants - use 'prep_concat' and 'prep_concat3' only!
#define prep_iconcat(a, b)      a ## b
#define prep_concat(a, b)       prep_iconcat(a, b)
#define prep_concat3(a, b, c)   prep_concat(prep_concat(a, b), c)

// Macro to concatenate two constants and convert the result to a string.
#define prep_concat_str(a, b)   prep_conv_str(prep_concat(a, b))




#endif
