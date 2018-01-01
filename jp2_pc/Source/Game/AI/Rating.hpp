/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CRating class.  A CRating object is an arithmetic type that operates in the range from
 *		0 to 1 inclusive.  It overloads addition, subtraction, and other stuff in ways that allow
 *		you to add .9 and .9 and get .99.
 *
 * Bugs:
 *
 * To do:
 *
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Rating.hpp                                                    $
 * 
 * 7     8/23/98 3:19a Agrant
 * clamp *= to 1
 * 
 * 6     3/23/98 5:28p Agrant
 * Added Clamp function.
 * Removed the CRating math stuff, as it wasn't really used and made it difficult to handle
 * float/CRating conversions.
 * 
 * 5     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 4     5/09/97 12:13p Agrant
 * Allow rating *= float
 * 
 * 3     10/15/96 9:20p Agrant
 * Synthesizer reworked to be better, stronger, smarter, faster than before.
 * Activity Rate and Act and Register functions now have better defined roles.
 * Added some accessor functions to smooth future changes.
 * 
 * 2     10/10/96 7:18p Agrant
 * Modified code for code specs.
 * 
 * 1     9/19/96 1:41p Agrant
 * initial revision
 *
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_RATING_HPP
#define HEADER_GAME_AI_RATING_HPP

//*********************************************************************************************
//
class CRating
//
//	Prefix: rt
//
//	The CRating class is a floating point number between 0 and 1 inclusive.
//
//	Notes:
//		CRating arithmetic is a bit odd, but maintains the sense of the basic operations but
//		keeps the results in the desired range.  Some of its operations greatly resemble
//		combinations of probabilities.
//
//		Each operation is detailed in its declaration.
//
//		Operations defined:
//			+  (+=)
//			-  (-=)
//			*  (*=)
//		
//*********************************************************************************************
{

//  Variable declarations
public:
	float fVal;		// Value from 0 to 1 inclusive.


//  Member function definitions
public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	CRating
	(
		float f_val		// Initial fVal
	)
	{
		// Call the = operator for floats
		*this = f_val;
	};

	CRating
	(
		double d_val		// Initial fVal
	)
	{
		// Call the = operator for doubles
		*this = d_val;
	};

	CRating
	(
		int i_val		// Initial fVal
	)
	{
		if (i_val == 0)
		{
			fVal = 0.0f;
		}
		else
		{
			fVal = 1.0f;
			Assert(i_val == 1);
		}
	};

	CRating
	(
	)
	: fVal(0.0f)
	{}


		//**********************************************************************************
		void Clamp
		(
			float f
		)
		//
		//	Sets the Rating's value to f, clamping it into the 0-1 range.
		//
		//*************************	
		{
			if (f < 0)
				fVal = 0.0f;
			else if (f > 1)
				fVal = 1.0f;
			else fVal = f;
		}
		

	//******************************************************************************************
	//
	// Operators.
	//

		//**********************************************************************************
		CRating& operator=
		(
			float f
		)
		//
		//	Sets the Rating's value to f.
		//
		//*************************	
		{
			Assert(f >= 0.0f && f <= 1.0f);
			fVal = f;
			return *this;
		}

		CRating& operator=
		(
			double d
		)
		//
		//	Sets the Rating's value to f.
		//
		//*************************	
		{
			Assert(d >= 0.0 && d <= 1.0);
			fVal = (float) d;
			return *this;
		}

		//lint -save -e1529  // check fro assignment of "this" to "this"
		CRating& operator=
		(
			const CRating& rt
		)
		//
		//	Sets the Rating's value to rt.
		//
		//*************************	
		{
			Assert(this != &rt);
			fVal = rt.fVal;
			return *this;
		}  //lint -restore
	
		CRating& operator=
		(
			int i
		)
		{
			Assert(i == 0 || i == 1);
			if (i)
			{
				fVal = 1.0f;
			}
			else
			{
				fVal = 0.0f;
			}
			return *this;
		}

#ifdef USE_RATING_MATH
		//**********************************************************************************
		CRating operator+
		(
			const CRating rt
		) const
		//
		//	Additions follow these constraints:
		//		A + 0 = A
		//		A + 1 = 1
		//		A + B = B + A
		//		(A + B) + C = A + (B + C)
		//		A + B falls between 0 and 1 inclusive.
		//		A + B > A for all B != 0, A != 1
		//		A + B < 1 for all A & B < 1
		//
		//	Returns:
		//	    A valid CRating that is the sum of the left and right addends.
		//
		//	Math:
		//		A + B ~= a - ab + b = a(1-b) + b
		//
		//*************************	
		{
			Assert(fVal >= 0.0f && fVal <= 1.0f);
			return CRating(fVal + rt.fVal - (fVal * rt.fVal));
		}
	
		void operator+=(const CRating rt)
		{
			fVal += rt.fVal - (fVal * rt.fVal);
			Assert(fVal >= 0.0f && fVal <= 1.0f);
		}

		//**********************************************************************************
		CRating operator-
		(
			const CRating rt
		) const
		//
		//	Subtractions follow these constraints:
		//		A - 0 = A
		//		A - 1 = 0
		//		(A - B) - C = (A - C) - B
		//		A - B falls between 0 and 1 inclusive.
		//		A - B < A for all B != 0, A != 0
		//		A - B > 0 for all A != 0, B != 1
		//
		//	Returns:
		//	    A valid CRating that is the difference of the left and right addends.
		//
		//	Math:
		//		A - B ~= a(1-b)
		//
		//*************************	
		{
			Assert(fVal >= 0.0f && fVal <= 1.0f);
			return CRating(fVal - (fVal * rt.fVal));
		}


		void operator-=(const CRating rt)
		{
			fVal -= fVal * rt.fVal;
			Assert(fVal >= 0.0f && fVal <= 1.0f);
		}

		//**********************************************************************************
		CRating operator*
		(
			const CRating rt
		) const
		//
		//	Multiplications follow all normal multiplication constraints.
		//
		//	Returns:
		//	    a valid CRating that is the product of the left and right arguments
		//
		//	Math:
		//		A * B ~= ab
		//
		//*************************	
		{
			return CRating(fVal * rt.fVal);	
		}

		void operator*=(CRating rt)
		{
			fVal *= rt.fVal;
		}


		//**********************************************************************************
		//
		//	Comparison operators.  
		//  These operators duplicate the standard scalar comparisons.
		//
		
//		bool operator==(const CRating rt) const
//		{
//			return fVal == rt.fVal;	  //lint !e777   // okay to test floats for equality here.
//		}

		bool operator>(const CRating rt) const 
		{
			return fVal > rt.fVal;	
		}

		bool operator<(const CRating rt) const
		{
			return fVal < rt.fVal;	
		}

		bool operator>=(const CRating rt) const
		{
			return fVal >= rt.fVal;	
		}

		bool operator<=(const CRating rt) const
		{
			return fVal <= rt.fVal;	
		}
#endif	// USE_RATING_MATH

		operator float () const
		{
			return fVal;
		}

		void operator*=(float f)
		{
			fVal *= f;
			if (fVal > 1.0f)
				fVal = 1.0f;
			Assert(fVal >= 0.0f && fVal <= 1.0f);
		}


};
//  #ifndef HEADER_GAME_AI_RATING_HPP
#endif 
