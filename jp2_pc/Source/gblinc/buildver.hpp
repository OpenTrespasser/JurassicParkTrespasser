/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		This is a project-specific, project-wide header file which defines all of the switches
 *		that control conditional compilation within the project.
 *		These are collected together into manageable bundles called 'build modes'.
 *
 *		Programmers must choose which build mode to use by defining the following command-line
 *		macro when compiling:
 *
 *			BUILDVER_MODE = MODE_????
 *
 *		(where MODE_???? is defined below.)
 *
 *		Also defines TRUE and FALSE for use in pre-processor conditional expressions.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/gblinc/buildver.hpp                                          $
 * 
 * 23    10/01/98 1:21a Asouth
 * CodeWarrior will do bad things with recursive functions and huge inline depths
 * 
 * 22    98.05.17 8:02p Mmouni
 * Default is now 16-bit bump maps.
 * 
 * 21    5/15/98 6:26p Mlange
 * Added build version switch for multiresolution water.
 * 
 * 20    3/17/98 1:40p Agrant
 * turns that VER_FINAL == !VER_TEST, so I'm taking it back out again.
 * 
 * 19    3/12/98 11:10p Agrant
 * Added a VER_FINAL flag.
 * 
 * 18    98/02/04 14:45 Speter
 * Added VER_TEST switch.
 * 
 * 17    1/19/98 7:28p Pkeet
 * Added the 'iBUMPMAP_RESOLUTION' macro.
 * 
 * 16    9/17/97 3:36p Rwyatt
 * New version flag to control debug text output
 * 
 * 15    9/01/97 7:39p Rwyatt
 * Clamping is now disabled on the compile switch as it has been put on to a run time
 * switch
 * 
 * 14    8/15/97 12:46a Rwyatt
 * New build flags to control texture clamping
 * 
 * 13    7/16/97 9:37p Rwyatt
 * Set inlining depth globally to 255. This seems to help a little bit.
 * 
 * 12    97/03/13 15:46 Speter
 * Changed MODE_OPTIMISE to MODE_RELEASE, to match configuration name.
 * 
 * 11    97/02/25 12:51p Pkeet
 * Added a switch for including and using Direct3D, and set the switch to 'false.'
 * 
 * 10    1/09/97 5:11p Pkeet
 * Changed 'VER_ASM_PRIMITIVES' to 'VER_ASM.' Added the 'MODE_DEBUG_NOASM' setting.
 * 
 * 9     96/12/05 17:34 Speter
 * Added MODE_OPTIMISE definition, changed flags to be other than 0.
 * 
 * 8     96/10/30 3:42p Mlange
 * Added VER_CLIP3D_CHECKS.
 * 
 * 7     96/10/30 12:18 Speter
 * Added new BUILDMODE_OPTIMISE mode.
 * Changed some comments.
 * 
 * 6     96/10/22 11:58a Mlange
 * Added the VER_LOG_MESSAGES, VER_TIMING_STATS and VER_ASM_PRIMITIVES switches.
 * 
 * 5     96/09/25 19:49 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types,
 * and prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 4     4/30/96 1:58p Mlange
 * Updated for changes to the coding standards.
 * 
 * 3     4/18/96 4:59p Mlange
 * Updated for changes to the coding standards.
 * 
 * 2     4/16/96 10:31a Mlange
 * Added definitions for the TRUE and FALSE pre-processor constants.
 * 
 * 1     4/12/96 3:20p Mlange
 * Contains switches to control the conditional compilation of the entire project.
 *
 **********************************************************************************************/

#ifndef HEADER_GBLINC_BUILDVER_HPP
#define HEADER_GBLINC_BUILDVER_HPP

//
// Pre-processor constants.
//

// Define true and false for use in pre-processor conditional expressions.
#define FALSE	0
#define TRUE	1

//
// Force the inline depth to be maximum.
// This can be done globally outside of any build mode because in debug
// mode this will be completely ignored. To acknowledge this pragma you
// must inline with any suitable and not just __inline.
//
#ifndef __MWERKS__
 // MWERKS actually obeys this command, and hence it's bad for the compiler
 #pragma inline_depth(255)
#endif

//
// BUILDVER_MODE selects which bundle of switches to use for the current build.
// Allowed options are defined below.
//
#ifndef BUILDVER_MODE
	#error BUILDVER_MODE not defined - please select a project mode.
#endif



//
// Build mode names.
//
// These symbols collect together 'bundles' of individual code control
// switches.
//
// Programmers may add new modes at any time for their own use, with these restrictions:
//
//		+ All build mode name symbols are preceded by 'MODE_'.
//
//		+ It is a good idea to include your user_id in the name of the mode, so
//		  that other programmers know not to interfere with it.
//
//		+ Temporary or unused modes should be deleted as soon as possible, to
//		  avoid clutter.
//
//		+ Of course, each 'bundle' (build mode) must have a unique ID.
//


// Standard build modes.  Do not use 0, as that detects whether BUILDVER_MODE is undefined.
#define MODE_FINAL		1
#define MODE_RELEASE	2
#define MODE_DEBUG		3





// Project-specific build modes (add project-specific modes here).



//
// Build mode definitions.
//
// These are the 'bundles' of code switches for each of the build modes listed above.
//
// Note that:
//
//		+ Every valid mode must contain a FULL list of switches.
//
//		+ Each switch must be either TRUE or FALSE.
//
// Programmers can add new switches to the modes at any time, with these restrictions:
//
//		+ All switches are preceded by 'VER_'.
//
// 		+ New switches must be added to ALL modes at the same time.
//
//		+ New switches must be given a benign (i.e. 'off') setting for all
//		  modes other than a 'private' mode, until the code they control is
//		  fully debugged.
//
//		+ Again, it is a good idea to include your user_id in the name of the
//		  switch, unless it controls something quite generic or is likely to
//		  become permanent.
//
//		+ Again, temporary or unused switches should be deleted as soon as
//		  possible, to avoid clutter.
//


//
// MODE_FINAL
//
// This is the primary group of code control switches. These should generate the final,
// shippable executable; i.e. no test or diagnostic code, no debug info, maximum optimisations
// enabled, etc.
//
#if (BUILDVER_MODE == MODE_FINAL)

	// Controls debug behaviour on a gross level, and the assert macros, Assert() and Verify().
	#define VER_DEBUG			FALSE

	// Controls code used only for testing purposes, such as debug text, special displays, etc.
	#define VER_TEST			FALSE

	// Enables or disables debugging text to console window and/or file
	// This also controls the memory check classes
	#define VER_DEBUG_TEXT		FALSE

	// Enables or disables the entity database message logging.
	#define VER_LOG_MESSAGES	FALSE

	// Enables or disables performance timing.
	#define VER_TIMING_STATS	FALSE

	// Enables or disables the assembly code versions.
	#define VER_ASM				TRUE

	// Enables or disables additional assertion checks in the clipping module.
	#define VER_CLIP3D_CHECKS	FALSE

	// Enables or disables inclusion of Direct3D.
	#define VER_ADD_DIRECT3D	FALSE

	// Clamp UV texture co-ords for all tilable primitives
	#define VER_CLAMP_UV_TILE	FALSE

	// Clamp UV texture co-ords for all 16 bit primitives
	#define VER_CLAMP_UV_16BIT	TRUE

	// Enable/disable multi-resolution water.
	#define VER_MULTI_RES_WATER	FALSE

//
// MODE_RELEASE
//
// This is similar to MODE_FINAL, except that some minimal-overhead debug and timing features
// are enabled.  The compiler should generate maximum optimisation *with* debug info.  
// Asserts and logs are disabled, but timings are enabled.
//
#elif (BUILDVER_MODE == MODE_RELEASE)

	#define VER_DEBUG			FALSE
	#define VER_TEST			TRUE
	#define VER_DEBUG_TEXT		TRUE
	#define VER_LOG_MESSAGES	FALSE
	#define VER_TIMING_STATS	TRUE
	#define VER_ASM				TRUE
	#define VER_CLIP3D_CHECKS	FALSE
	#define VER_ADD_DIRECT3D	FALSE
	#define VER_CLAMP_UV_TILE	FALSE
	#define VER_CLAMP_UV_16BIT	TRUE
	#define VER_MULTI_RES_WATER	FALSE


//
// MODE_DEBUG
//
// This is another 'standard' mode; the default configuration for testing and debugging.
// This should be the same as MODE_FINAL, with the exception that all generic testing
// debugging aids are enabled.
//
#elif (BUILDVER_MODE == MODE_DEBUG)

	#define VER_DEBUG			TRUE
	#define VER_TEST			TRUE
	#define VER_DEBUG_TEXT		TRUE
	#define VER_LOG_MESSAGES	TRUE
	#define VER_TIMING_STATS	TRUE
	#define VER_ASM				TRUE
	#define VER_CLIP3D_CHECKS	TRUE
	#define VER_ADD_DIRECT3D	FALSE
	#define VER_CLAMP_UV_TILE	FALSE
	#define VER_CLAMP_UV_16BIT	TRUE
	#define VER_MULTI_RES_WATER	FALSE

//
// MODE_DEBUG_NOASM
//
// This mode is the same as MODE_DEBUG except no assembly is used.
//
#elif (BUILDVER_MODE == MODE_DEBUG_NOASM)

	#define VER_DEBUG			TRUE
	#define VER_TEST			TRUE
	#define VER_DEBUG_TEXT		TRUE
	#define VER_LOG_MESSAGES	TRUE
	#define VER_TIMING_STATS	TRUE
	#define VER_ASM				FALSE
	#define VER_CLIP3D_CHECKS	TRUE
	#define VER_ADD_DIRECT3D	FALSE
	#define VER_CLAMP_UV_TILE	FALSE
	#define VER_CLAMP_UV_16BIT	TRUE
	#define VER_MULTI_RES_WATER	FALSE


//
// Add further modes here as required...
//
//
// For example:
//
// #elif (BUILDVER_MODE == MODE_userid_DEBUG)
// 
// --------------------------------------------------------------------------
//
// MODE_userid_DEBUG
//
//	#define VER_DEBUG	TRUE
//	#define VER_userid_UNTESTED_CODE	TRUE
//	#define VER_userid_CHECK_PARAMS		TRUE
// 
//




// Trap the use of an unsupported (e.g. deleted) mode.
#else

	#error BUILDVER_MODE does not match a defined project mode!

#endif


//
// A special case for bumpmapping. This will be removed once tools are in place to use 16 bit
// bumpmap texels.
//

// Resolution of the bumpmap.
#define iBUMPMAP_RESOLUTION (16)
//#define iBUMPMAP_RESOLUTION (32)



#endif
