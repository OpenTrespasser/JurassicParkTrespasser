/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CGammaCorrection
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Gamma.hpp                                                    $
 * 
 * 9     98/03/30 11:03 Speter
 * Gamma table now based on CFloatTable.
 * 
 * 8     97/12/11 16:41 Speter
 * CGamma interface changed.  Input now ranges up to rvMAX_WHITE.
 * 
 * 7     10/10/97 1:34p Mmouni
 * Added fGAMMA_TABLE_SIZE_MINUS_ONE constant.
 * 
 * 6     97/10/02 12:08 Speter
 * Changed iRound() to faster iPosRound().
 * 
 * 5     97/06/23 20:21 Speter
 * Now save gamma parameters as member variables, for inspection.   No longer need to include
 * .cpp file.
 * 
 * 4     97/02/13 14:25 Speter
 * Corrected gamma table range problem.
 * 
 * 3     97/01/26 15:51 Speter
 * Fixed comment.
 * 
 * 2     97/01/16 11:56 Speter
 * Added const specifier.
 * 
 * 1     96/12/17 13:23 Speter
 * New module.
 * 
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_GAMMA_HPP
#define HEADER_TEST_GAMMA_HPP

#include "Lib/Renderer/Material.hpp"
#include "Lib/Math/FloatTable.hpp"

//
// Specify the size of the gamma table.  This should be large so that every clut value is
// mapped to sufficiently.
//
#define iGAMMA_TABLE_BITS	9
#define iGAMMA_TABLE_SIZE	(1 << iGAMMA_TABLE_BITS)

//*********************************************************************************************
//
template<class T> class CGammaCorrection: 
	public CFloatTableRound<T, iGAMMA_TABLE_SIZE, 0, static_cast<int>(rvMAX_WHITE)>
//
// A gamma-correction and light-value conversion facility.  
// Converts floating-point lighting values to gamma-corrected values, while scaling and
// converting to type T.
//
// Prefix: gc
//
// Notes:
//		The gamma correction table translates a one byte value to a one byte value
//		representing the corrected value. Correction values are generated from an exponent
//		curve:
//				                                exponent
//				corrected value = original value
//
//**************************************
{
public:
	// Variables for inspection.
	float	fGamma;						// The gamma correction exponent.
										// This converts lighting values to screen values.
	T		tOutputRange;				// The range of the output value
										// (output must be <= tOutputRange).

public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	//*****************************************************************************************
	CGammaCorrection
	(
		T t_output_range = T(1),
		float f_gamma = 1.0
	);

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Set
	(
		T t_output_range = T(1),
		float f_gamma = 1.0
	);
	//
	// Sets up the table using the given values.
	//
	//**********************************
};

#endif
