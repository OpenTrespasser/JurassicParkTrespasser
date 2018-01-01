/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Definition of the Assert macro, a replacement for ANSI assert, and the definition of the
 *		Verify and AlwaysVerify macro.
 *
 *		This header file is automatically included by "Common.hpp".
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/UAssert.hpp                                                   $
 * 
 * 14    98/09/17 16:39 Speter
 * No more AlwaysAssert in final mode.
 * 
 * 13    97/08/01 9:42 Speter
 * Assert() now causes the debugger to break AT the Assert, not in TerminalError.
 * Unfortunately, can't do the same with verify.
 * 
 * 12    97/01/02 16:37 Speter
 * Added AlwaysAssert macro.
 * 
 * 11    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 10    96/07/03 12:58 Speter
 * Moved many files to new directories, changed include statements.
 * 
 * 9     96/05/24 12:41 Speter
 * Changed Verify macros so they can be used in expressions.
 * 
 * 8     5/14/96 10:16a Pkeet
 * Added 'Retry' or 'Cancel' options to TerminalError.
 * 
 * 7     96/05/09 15:31 Speter
 * Ensures parameter for Assert is now considered as an expression.
 * 
 * 6     5/08/96 8:02p Mlange
 * Now removes the definition of ANSI assert.
 * 
 * 5     5/03/96 12:10p Mlange
 * Added definition of the AlwaysVerify macro. Changes from code review. Added parenthesis
 * around expression parameter in Verify macro. Added some comments. Now uses forward slashes to
 * delimit directories in #include paths. Removed redundant void expression in empty definition
 * of the Assert macro.
 * 
 * 4     4/30/96 1:56p Mlange
 * Updated 'to do' field.
 * 
 * 3     4/30/96 1:44p Mlange
 * Added the Verify macro. Updated for changes to the coding standards.
 * 
 * 2     4/18/96 6:01p Mlange
 * Updated for changes to coding standards. Now calls TerminalError instead of (the
 * non-existing) AssertFail.
 * 
 * 1     4/16/96 10:28a Mlange
 * Definition of the Assert macro.
 *
 **********************************************************************************************/

#ifndef HEADER_GBLINC_UASSERT_HPP
#define HEADER_GBLINC_UASSERT_HPP

//
// Remove the definition of ANSI assert. This header file defines replacements that must be used
// instead. Note that, because the Common.hpp header file is always included after any standard
// header files, this is guaranteed to remove any definitions of ANSI assert.
//
#undef assert


//
// The behaviour of the assert and verify macros is modified by the VER_GENERAL_DEBUG constant.
// Make sure it is defined.
//
#ifndef VER_DEBUG
	#error The VER_DEBUG constant has not been defined! Make sure Common.hpp is included!!
#endif
#ifndef VER_TEST
	#error The VER_TEST constant has not been defined! Make sure Common.hpp is included!!
#endif


// Obtain definition of the TerminalError function.
#include "Lib/Sys/Errors.hpp"


//**********************************************************************************************
//
// Macro definitions.
//
// Note that the Assert macros may only be used as a statement and NOT as part of an
// expression. This is deliberate. Also, they are implemented as macros and not as inline
// functions to ensure that the __FILE__ and __LINE__ constants refer to the 'calling' module.
//
// The macros requires the 'TerminalError' function. This function is defined in the error
// handling module.
//

//**********************************************************************************************
//
// void Assert
// (
//	  bool b_expr	// The expression to evaluate.
// )
//
// The 'debug' assert macro; replacement for the ANSI assert().
// IN DEBUG BUILD, THIS DOES NOT RETURN WHEN 'b_expr' EVALUATES TO FALSE!
//
// Notes:
//		This macro is disabled by defining VER_DEBUG constant to be FALSE (see
//		BuildVer.hpp).
//
//		Note the use of a local bool variable to store the result of evaluating the expression.
//		This is required to fool the compiler into not producing 'unreachable code' warnings
//		when the expression evaluates to a constant (e.g. just comparing compile-time
//		constants).
// 
// Example:
//		Assert(pv_memory != 0);
//
//**************************************

// The assert macro is disabled by defining the VER_DEBUG constant to be FALSE.
#if !VER_DEBUG

	// Empty definition of the assert macro.
	#define Assert(ignore)	\
	{						\
	}

#else

	#define Assert(b_expr)													\
	{																		\
		if (!(b_expr) && bTerminalError(ERROR_ASSERTFAIL, true, __FILE__, __LINE__))	\
			DebuggerBreak();																\
	}

// #if !VER_DEBUG
#endif


//**********************************************************************************************
//
// void AlwaysAssert
// (
//	  bool b_expr	// The expression to evaluate.
// )
//
// Performs Assert, in debug and release builds.  Identical to AlwaysVerify,
// for hysterical reasons.
//
//**************************************

#define AlwaysAssert	AlwaysVerify


//**********************************************************************************************
//
// bool Verify
// (
//	  bool b_expr	// The expression to evaluate.
// )
//
// The 'debug' verify macro. Similar to the Assert macro, except that the expression is always
// evaluated, even in the release build.
// IN DEBUG BUILD, THIS DOES NOT RETURN WHEN 'b_expr' EVALUATES TO FALSE!
//
// Notes:
//		Defining VER_DEBUG constant to be FALSE (see BuildVer.hpp) forces this
//		macro to only evaluate the expression, but not the terminate on a 'false' result.
//
// Example:
//		Verify(phndOpenFile("image.bin") != 0);
//
//**************************************

#if !VER_DEBUG

	// Debug definition of the verify macro.
	#define Verify(b_expr)	(b_expr)

#else

	// The actual definition of the verify macro.
	#define Verify(b_expr)	AlwaysVerify(b_expr)

// #if !VER_DEBUG
#endif





//**********************************************************************************************
//
// bool AlwaysVerify
// (
//	  bool b_expr	// The expression to evaluate.
// )
//
// The 'always' verify macro.
// THIS DOES NOT RETURN WHEN 'b_expr' EVALUATES TO FALSE!
//
// Notes:
//		This macro will always evaluate the expression and terminate on a false result, in
//		debug and release builds, but not final build.
//
// Example:
//		AlwaysVerify(pvMemAlloc(10000) != 0);
//
//**************************************

#if !VER_TEST

	// Debug definition of the verify macro.
	#define AlwaysVerify(b_expr)	(b_expr)

#else

	// The actual definition of the verify macro.
	#define AlwaysVerify(b_expr)											\
		( (b_expr) ? true : (TerminalError(ERROR_ASSERTFAIL, true, __FILE__, __LINE__), false) )

// #if !VER_TEST
#endif
	

#endif
