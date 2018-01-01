/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Warning header file to turn off bad warnings.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GblInc/Warnings.hpp                                                   $
 * 
 * 10    98/01/19 20:21 Speter
 * Turned off EMMS warning for 3DX builds.
 * 
 * 9     10/22/97 9:35p Agrant
 * Removed pragma 4146, negation of unsigned value
 * 
 * 8     10/21/97 8:24p Gstull
 * Disable warning 4146, warning C4146: unary minus operator applied to unsigned type, result
 * still unsigned.    This causes many warnings in VC4.1.
 * 
 * 7     97/06/23 20:33 Speter
 * Added 4660.
 * 
 * 6     6/05/97 3:58p Gstull
 * Added pragma to disable signed/unsigned comparison warnings.
 * 
 * 5     97-05-02 12:05 Speter
 * Disabled warning 4284.
 * 
 * 4     97/01/30 21:10 Speter
 * Moved pragmas to enable level 4 warnings to bottom of file.  Added, but commented, pragma to
 * warn on unexpanded inlines.
 * 
 * 3     1/08/97 7:13p Pkeet
 * Disabled warnings for bool and double to float conversions.
 * 
 * 2     12/20/96 4:56p Agrant
 * Enabled warning for unreferenced locals.
 * 
 * 1     10/08/96 9:09p Agrant
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_GBLINC_WARNINGS_HPP
#define HEADER_GBLINC_WARNINGS_HPP

//
// Pragmas to turn off certain annoying warnings.
//

// Disable message for VC4.1 warning C4244: 'initializing' : conversion from 'const int' to 'float', possible loss of data
#pragma warning(disable: 4244)

// Disable message for VC4.1 warning C4146: unary minus operator applied to unsigned type, result still unsigned
//#pragma warning(disable: 4146)

// "inherits via dominance".
#pragma warning(disable: 4250)

// The following warning appears frequently when you use STL:
// "identifier was truncated to 255 characters in the debug information".
#pragma warning(disable: 4786)

// Turn off the bool as a reserved word warning.
#pragma warning(disable: 4237)

// Warning for forcing bool.
#pragma warning(disable: 4800)

// Warning for double to float conversion.
#pragma warning(disable: 4305)

// Return type for 'operator ->' is not a UDT or reference to a UDT.  Will produce errors if applied using infix notation.
#pragma warning(disable: 4284)

// template-class specialization 'CMatrix3<float>' is already instantiated
// Occurs with most explicit template instantiations.  If you instantiate these classes, VC4.2 complains.  
// If you don't, they don't link.  Lovely.
#pragma warning(disable: 4660)

// Functions contains no EMMS instruction (our 3DX code does not).
#pragma warning(disable: 4799)

// Warning for signed/unsigned comparisons in C++ 4.1: 4018: '<' : signed/unsigned mismatch
#if _MSC_VER < 1020
#pragma warning(disable: 4018)
#endif
// 
// Pragmas to ENABLE certain level-4 warnings we want to see.
// These pragmas make the warnings show up in level-3.
//

// Warn for unreferenced local variables.
#pragma warning(3: 4101)

// 'Function not expanded'
// #pragma warning(3: 4710)

// #ifndef HEADER_GBLINC_WARNINGS_HPP
#endif
