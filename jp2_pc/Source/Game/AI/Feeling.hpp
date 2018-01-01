/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CFeeling class for AI library.  CFeeling contains a rating for each state variable
 *		associated with a mental opinion.  
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Feeling.hpp                                                   $
 * 
 * 8     5/08/98 5:34p Agrant
 * more better vocal code
 * 
 * 7     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 6     4/29/97 6:42p Agrant
 * CFeeling now float-based instead of CRating-based.
 * Activities now rate/act based on a list of influences.
 * Better rating functions for activities.
 * Debugging tools for activity processing.
 * 
 * 5     10/10/96 7:18p Agrant
 * Modified code for code specs.
 * 
 * 4     10/02/96 8:11p Agrant
 * Added Fleeing and Hunting.
 * Added Sheep and Wolves to Test App.
 * Added some debugging hooks.
 * 
 * 3     9/30/96 3:05p Agrant
 * modified for enum code spec
 * 
 * 
 * 2     9/26/96 5:58p Agrant
 * basic AI structure done
 * AI system adapted to TReal for world coords
 * AI Test App exhibits very basic herding with two very simple actvities.
 * 
 * 1     9/23/96 3:03p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_FEELING_HPP
#define HEADER_LIB_AI_FEELING_HPP

#include "AITypes.hpp"
#include "Rating.hpp"

//*********************************************************************************************
//
class CFeeling
//
//	Prefix: feel
//
//	An emotional state.
//
//	Notes:
//		The CFeeling class records an emotional feeling in terms of a set of ratings.  The 
//		feeling can be about something, or it can be just an internal state.  The parameters
//		are interpreted slightly differently depending on the context of the feeling.
//
//	Parameters:
//		When used as a self-describing mental state:
//		eptFEAR			// Nervousness, fear, anxiety.
//			1	terror, startled by anything.
//			0	absolutely fearless.
//		eptLOVE			// Love, freindliness, general happiness.
//			1	delirious with joy and affection for all
//			0	no affection for anything
//		eptANGER		// Anger, rage.
//			1	will lash out at anything.  Pissed.  Really, really, hacked off.
//			0	at peace with the world.
//		eptCURIOSITY	// Curiosity, distractablility.
//			1	will investigate any vaguely unknown quantity.  Kills cats.
//			0	singleminded and interested in the slightest.
//		eptHUNGER		// Hunger
//			1	will eat anything it can get its mouth on that has any nutritional value.
//			0	will not eat.
//		eptTHIRST		// Thirst
//			1	will drink til it floats
//			0	will not drink
//		eptFATIGUE		// Fatigue
//			1	Likely to already be asleep.
//			0	Full of pep and vigor.
//		eptPAIN			// Pain currently felt
//			1	In extreme agony.
//			0	Fit as a fiddle.
//		eptSOLIDITY		// How much it respects solid objects.
//			1	Normal animal behavior.
//			0	Normal ghost behavior.
//
//		When used to describe an influence, a feeling indicates which emotions MIGHT apply
//		to the influence.  For example, although a piece of grass might be very tasty, only
//		a somewhat peckish cow would eat it.  Note that a 0 implies that the emotion will
//		NEVER have any effect on an animal's opinion of in influence.  
//		eptFEAR			// How terrifying is it?
//			1	a nightmare come to life
//			0	a security blanket
//		eptLOVE			// How much do I like it?
//			1	the animal's true love
//			0	the neighbor 5 doors down from 20 years ago.
//		eptANGER		// How much do I hate it?
//			1	the thing that killed my parents, cousins, and dog
//			0	the neighbor 5 doors down from 20 years ago.
//		eptCURIOSITY	// How much do I wish to investigate it?
//			1	whatever that thing behind the hill is
//			0	the back of my hand
//		eptHUNGER		// How good is it to drink?
//			1	the tastiest dish ever created
//			0	gravel
//		eptTHIRST		// How good is it to drink?
//			1	the tastiest drink ever created
//			0	gravel
//		eptFATIGUE		// Not really relevent
//		eptPAIN			// How much pain has it caused me?
//			1	Dad's belt
//			0	A feather
//		eptSOLIDITY		// How much it respects solid objects.
//			1	Steel reinforced concrete.
//			0	Air
//
//*********************************************************************************************
{
//
//  Variable declarations
//

public:
	float			afRatings[eptEND];		// Current rating values.

	static char *	pcRatingNames[eptEND];	// Names of the ratings.	
	
//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	CFeeling
	(
	)
	{
		for (int i = eptEND - 1; i >= 0; i--)
		{
			afRatings[i] = 0;
		}
	}

	//lint -save -e1541
	CFeeling
	(
		bool b_init_to_zero
	)
	{
		// Allows us to skil the init step when not needed.
		if (b_init_to_zero)
		{
			for (int i = eptEND - 1; i >= 0; i--)
			{
				afRatings[i] = 0;
			}
		}
	}
	//lint -restore

	//******************************************************************************************
	//
	// Member Functions.
	//

		//*********************************************************************************
		//
		float fDot
		(
			const CFeeling&		feel
		) const
		//
		//	Returns:
		//		The dot product of the two feelings.
		//
		//******************************
		{
			float f = 0;
			for (int i = eptEND - 1; i >= 0; i--)
				f += afRatings[i] * feel.afRatings[i];
			return f;
		}

		//*********************************************************************************
		//
		CRating rtDot
		(
			const CFeeling&		feel
		) const
		//
		//	Returns:
		//		The dot product of the two feelings, as a rating.
		//		Negative products are returned as zero.
		//		Positive ratings are scaled to the 0 to 1 range, 
		//		making the assumption that individual parameters of a feeling are
		//		never larger than one.
		//
		//******************************
		{
			float f = fDot(feel);

			if (f < 0)
			{
				return 0.0f;
			}
			else
			{
				// Max dot product is number of parameters, so normalize with eptEND
				f = f * (1.0f / ((float)eptEND));
				if (f > 1.0f)
					return 1.0f;
				else
					return f;
			}
		}

		//*********************************************************************************
		//
		CFeeling feelUnion
		(
			const CFeeling&		feel
		) const
		//
		//	Returns:
		//		The fuzzy union of the two feelings.  Or, the pairwise maximum.
		//
		//******************************
		{
			CFeeling feel_return;
			for (int i = eptEND - 1; i >= 0; i--)
				feel_return.afRatings[i] = Max(afRatings[i], feel.afRatings[i]);
			return feel_return;
		} 

	//******************************************************************************************
	//
	// Operators.
	//

		//**********************************************************************************
		CFeeling operator+
		(
			const CFeeling& feel
		) const
		//
		//	Pairwise sums all ratings in the feelings and returns the result.
		//
		//	Returns:
		//	    A CFeeling that is the pairwise sum of both addends.
		//
		//******************************
		{
			CFeeling feel_return;

			for (int i = eptEND - 1; i >= 0; i--)
				feel_return.afRatings[i] = afRatings[i] + feel.afRatings[i];
			return feel_return;
		}

		//**********************************************************************************
		CFeeling operator*
		(
			const CFeeling& feel
		) const
		//
		//	Pairwise multiplies all ratings in the feelings and returns the result.
		//
		//	Returns:
		//	    a CFeeling that is the pairwise product of both arguments.
		//
		//******************************

		{
			CFeeling feel_return;

			for (int i = eptEND - 1; i >= 0; i--)
				feel_return.afRatings[i] = afRatings[i] * feel.afRatings[i];
			return feel_return;
		}

		//**********************************************************************************
		CFeeling operator*
		(
			float f
		) const
		//
		//	Multiplies all ratings in the feeling by "f" and returns the result.
		//
		//	Returns:
		//	    A scaled CFeeling.
		//
		//******************************
		{
			CFeeling feel_return;

			for (int i = eptEND - 1; i >= 0; i--)
				feel_return.afRatings[i] = afRatings[i] * f;
			return feel_return;
		}

		// Access the elements, with checking.
		float& operator [](int i)
		{
			Assert(i >= 0 && i < eptEND);
			return afRatings[i];
		}
	
		const float& operator [](int i) const
		{
			Assert(i >= 0 && i < eptEND);
			return afRatings[i];
		}



};


#endif   // #ifndef HEADER_LIB_AI_FEELING_HPP
