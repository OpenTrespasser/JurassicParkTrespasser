/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CRandom, a fast pseudo random number generator.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Random.hpp                                                    $
 * 
 * 6     98/08/19 2:30 Speter
 * Added a static randMain member.
 * 
 * 5     6/13/98 9:18p Agrant
 * Allow both arguments of Random to be the same.
 * 
 * 4     98/01/15 14:19 Speter
 * Moved several functions, and <stdlib.h>, to new .cpp file.  Changed default seed value, for
 * reproducible behaviour.
 * 
 * 3     11/11/97 2:55p Agrant
 * Save/Load functions
 * 
 * 2     7/24/96 3:36p Mlange
 * Removed overloaded conversion operators and replaced them with more overloaded function call
 * operators, including ones for returning a random number within some range. Removed the
 * dNext() function and made u4Next() private.
 * 
 * 1     7/23/96 7:09p Mlange
 * Class for generating random numbers.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_RANDOM_HPP
#define HEADER_LIB_STD_RANDOM_HPP

//**********************************************************************************************
//
// Constants for CRandom.
//
#define iRANDOM_TABLE_SIZE	55
#define iRANDOM_INDEX_DIFF	31



//**********************************************************************************************
//
class CRandom
//
// Pseudo-random number generator.
//
// Prefix: rnd
//
// Notes:
//		Class to generate a sequence of pseudo-random numbers using the 'additive number
//		generator' method. For more information see: Seminumerical algorithms, second edition,
//		page 26-28.
//
//		Each instance of this class provides an independant sequence of random numbers. This
//		way each module, function, etc. can declare its own instance of this class to mimimise
//		interdependancies with other modules and functions.
//
//		Generally, it is not a good idea to declare an instance of this class as an automatic
//		object.
//
//**************************************
{
    uint32 u4RandomTable[iRANDOM_TABLE_SIZE];	// The table of random numbers.

	int iIndexJ, iIndexK;						// The current indices into the table.

public:

	//
	// A static variable usable for random randomness.
	//
	static CRandom randMain;

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// The default constructor.
	CRandom()
	{
		Seed();
	}

	// Construct with a seed.
	CRandom(uint32 u4_seed)
	{
		Seed(u4_seed);
	}


	//******************************************************************************************
	//
	// Assignment operators.
	//

	//
	// The assignment operator is overloaded to provide a convienent interface to seed the
	// random number generator.
	//
	void operator =(uint32 u4_seed)
	{
		Seed(u4_seed);
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Seed
	(
		uint32 u4_seed = 1
	);
	//
	// Seed the random number sequence.
	//
	// Note: srand() is damn weird.  Only if you pass 1 does it reinitialise to a consistent state.
	// Any other argument causes it to *randomly* reinitialise.  Since Seed() calls srand(),
	// let's use 1 as the default.
	//
	//**************************************

	//******************************************************************************************
	//
	uint32 operator()()
	//
	// Get the next random number in the sequence.
	//
	// Returns:
	//		A pseudo-random unsigned 32 bit integer.
	//
	//**************************************
	{
		return u4Next();
	}


	//******************************************************************************************
	//
	uint32 operator()
	(
		uint32 u4_lo, uint32 u4_hi	// The half-open interval for the random number.
	)
	//
	// Get the next random number in the sequence, to within the specified range.
	//
	// Returns:
	//		A pseudo-random unsigned 32 bit integer, within the range [u4_lo, u4_hi).
	//
	//**************************************
	{
		Assert(u4_lo < u4_hi);
		return u4_lo + (u4Next() % (u4_hi - u4_lo));
	}


	//******************************************************************************************
	//
	double operator()
	(
		double d_lo, double d_hi	// The closed interval for the random number.
	)
	//
	// Get the next random number in the sequence, as a floating point double in the specified
	// range.
	//
	// Returns:
	//		A pseudo-random floating point number in the range [d_lo, d_hi].
	//
	//**************************************
	{
		Assert(d_lo <= d_hi);

		// Obtain a random number in the range [0,1].
		double d_0_1 = (double)u4Next() / UTypeMax(uint32);

		return d_lo + (d_0_1 * (d_hi - d_lo));
	}


	//*****************************************************************************************
	virtual char* pcSave(char* pc) const;

	//*****************************************************************************************
	virtual const char* pcLoad(const char* pc);

private:
	//******************************************************************************************
	//
	uint32 u4Next();
	//
	// Get the next random number in the sequence.
	//
	// Returns:
	//		A pseudo-random unsigned 32 bit integer.
	//
	//**************************************

};

#endif
