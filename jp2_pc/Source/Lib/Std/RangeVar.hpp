/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Simple range variable type.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/RangeVar.hpp                                                  $
 * 
 * 3     6/24/97 1:49p Mlange
 * Added the SetRange function.
 * 
 * 2     6/10/97 2:19p Mlange
 * Added CRangeVar<>::SetFraction() and CRangeVar::fGetFraction().
 * 
 * 1     6/09/97 10:55p Mlange
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_RANGEVAR_HPP
#define HEADER_LIB_STD_RANGEVAR_HPP

//**********************************************************************************************
//
template<class T> class CRangeVar
//
// A simple range variable type. Contains a single datum that is limited to a specified range.
//
// Prefix: rvar
//
//**************************************
{
	T tData;	// The data.

	T tMax;		// The upper and lower limits. The data is clamped to the range [tMin, tMax].
	T tMin;

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Initialiser constructor.
	CRangeVar(T t_min, T t_max, T t_default)
		: tMin(t_min), tMax(t_max)
	{
		Assert(t_min <= t_max);

		Set(t_default);
	}


	//******************************************************************************************
	//
	// Operators.
	//
	CRangeVar<T>& operator =(T t)
	{
		Set(t);
		return *this;
	}

	operator T() const
	{
		return tData;
	}



	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Set
	(
		T t_new		// The new data for this range variable.
	)
	//
	// Update the contents of this range variable. The given data is clamped to be within range.
	//
	//**************************************
	{
		tData = MinMax(t_new, tMin, tMax);
	}


	//******************************************************************************************
	//
	T tGet() const
	//
	// Returns:
	//		The current value of this range variable.
	//
	//**************************************
	{
		return tData;
	}


	//******************************************************************************************
	//
	void SetFraction
	(
		float f_t		// Fraction [0, 1] of range to set this to.
	)
	//
	// Set the contents of this as a fraction of its range.
	//
	//**************************************
	{
		Set(f_t * float(tGetRange()) + float(tMin));
	}


	//******************************************************************************************
	//
	float fGetFraction() const
	//
	// Returns:
	//		The current value of this as a fraction of its range.
	//
	//**************************************
	{
		return float(tData - tMin) / float(tGetRange());
	}


	//******************************************************************************************
	//
	const T tGetMin() const
	//
	// Returns:
	//		The lower limit of this.
	//
	//**************************************
	{
		return tMin;
	}


	//******************************************************************************************
	//
	const T tGetMax() const
	//
	// Returns:
	//		The upper limit of this.
	//
	//**************************************
	{
		return tMax;
	}


	//******************************************************************************************
	//
	void SetRange
	(
		T t_min, T t_max	// New limits for range.
	)
	//
	// Set the range of this.
	//
	//**************************************
	{
		Assert(t_min <= t_max);

		tMin = t_min;
		tMax = t_max;
	}


	//******************************************************************************************
	//
	const T tGetRange() const
	//
	// Returns:
	//		The range of this, i.e. the upper limit minus the lower limit.
	//
	//**************************************
	{
		return tMax - tMin;
	}

};


#endif

