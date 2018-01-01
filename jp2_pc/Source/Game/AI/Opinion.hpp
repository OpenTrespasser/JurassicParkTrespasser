/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	COpinion class for AI library.  COpinion contains a rating for each state variable
 *  associated with a mental opinion.  
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Opinion.hpp                                                   $
 * 
 * 1     9/19/96 1:41p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_OPINION_HPP
#define HEADER_LIB_AI_OPINION_HPP

#include "AITypes.hpp"

#include "Rating.hpp"

//*********************************************************************************************
//
class COpinion
//
//	Prefix: opi
//
//	The COpinion class records an emotional opinion in terms of a set of ratings.
//
//	Example:
//		none
//
//*********************************************************************************************
{
//
//  Variable declarations
//

public:
	CRating	art[eptNUM_PARAMETERS];		//  current rating values

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	COpinion
	(
	)
	{
	}

	//******************************************************************************************
	//
	// Member Functions.
	//

		//*********************************************************************************
		//
		CRating Dot
		(
			const COpinion&		opi
		)
		//
		//	Returns:
		//		The dot product of the two opinions in CRating math.
		//
		{
			CRating		rt = 0;
			for (int i = eptNUM_PARAMETERS - 1; i >= 0; i--)
				rt += art[i] * opi.art[i];
			return rt;
		}
		//******************************

		//*********************************************************************************
		//
		COpinion Union
		(
			const COpinion&		opi
		)
		//
		//	Returns:
		//		The fuzzy union of the two opinions.  Or, the pairwise maximum.
		//
		{
			COpinion opi_return;
			for (int i = eptNUM_PARAMETERS - 1; i >= 0; i--)
				opi_return.art[i] = art[i] > opi.art[i] ? art[i] : opi.art[i];
			return opi_return;
		}
		//******************************

	//******************************************************************************************
	//
	// Operators.
	//

		//**********************************************************************************
		COpinion operator+
		(
			const COpinion& opi
		)
		//
		//	Pairwise sums all ratings in the opinions and returns the result.
		//
		//	Returns:
		//	    a COpinion that is the pairwise sum of both addends.
		//
		{
			COpinion opi_return;

			for (int i = eptNUM_PARAMETERS - 1; i >= 0; i--)
				opi_return.art[i] = art[i] + opi.art[i];
			return opi_return;
		}

		//**********************************************************************************
		COpinion operator*
		(
			const COpinion& opi
		)
		//
		//	Pairwise multiplies all ratings in the opinions and returns the result.
		//
		//	Returns:
		//	    a COpinion that is the pairwise product of both arguments.
		//
		{
			COpinion opi_return;

			for (int i = eptNUM_PARAMETERS - 1; i >= 0; i--)
				opi_return.art[i] = art[i] * opi.art[i];
			return opi_return;
		}

};


#endif   // #ifndef HEADER_LIB_AI_OPINION_HPP
