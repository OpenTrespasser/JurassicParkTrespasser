/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CShear3<TReal>.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform5/Shear.hpp                                             $
 * 
 * 6     97-04-24 18:40 Speter
 * Folded new changes from 4.2 version into this 5.0 specific version.
 * 
 * 5     97/03/24 14:54 Speter
 * Removed constructors of CDirs and CRotates from ASCII chars; use d3ZAxis etc. constants.
 * When we need optimisation for axes, we'll use derived classes which don't need a switch
 * statement.
 * 
 * 4     96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 3     96/07/08 12:17 Speter
 * Removed commented-out code.
 * 
 * 2     96/06/26 22:07 Speter
 * Added a bunch of comments, prettied things up nicely.
 * 
 * 1     96/06/26 14:03 Speter
 * First version.  Disabled as separate class, just made it an alias to CMatrix3.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_SHEAR_HPP
#define HEADER_LIB_TRANSFORM_SHEAR_HPP

#include "Matrix.hpp"

#define CShear3		CMatrix3

//
// CShear3 is implemented as an alias to CMatrix3, because given the frequency that shear will
// probably be used, creating a separate optimal class is too much trouble.  (Because of compiler
// template bugs, it's tedious to ensure that each transform class operates with all the others,
// even if you make CShear3 a derived class of CMatrix3.)
//
// You can pretend that CShear3 is a separate class, however, and use the following constructor:
//
/*
	CShear3
	(
		char c_axis_const,			// Axis to shear with respect to ('x', 'y', or 'z').
		TR t_1, TR t_2				// Amount to shear other axes (in x-y-z-x order).
	);
*/
//
// Of course, this is actually a constructor of the CMatrix3 class.  Normally, CMatrix3 should
// know nothing about shears, but this is a small kludge for convenience.
//


#endif