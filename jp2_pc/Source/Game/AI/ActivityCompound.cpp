/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *	Implementation of ActivityCompound.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/ActivityCompound.cpp                                         $
 * 
 * 15    9/30/98 8:00p Agrant
 * Dying vocal repairs
 * 
 * 14    9/16/98 9:20p Agrant
 * implemented the single influence Exclusive rate & register
 * 
 * 13    8/25/98 10:52a Rvande
 * Checking in on behalf of Andrew S.  Fixing some for ( int i... type problems.
 * 
 * 12    8/16/98 3:21p Agrant
 * Much tweaking to simplify activity balancing
 * 
 * 11    6/05/98 6:08p Agrant
 * Revised synthesizer to handle random-access acitivities.
 * Also handles exclusive ones (the old method).
 * 
 * 10    12/22/97 6:17p Agrant
 * Changed v2Location to v2Location (in CInfluence)
 * 
 * 9     12/20/97 7:03p Agrant
 * Cleaning up AI includes
 * 
 * 8     12/18/97 7:35p Agrant
 * Better group behavior through the CONTINUE flag
 * 
 * 7     9/15/97 7:25p Agrant
 * Better AI test debug info, timing info
 * 
 * 6     7/31/97 4:39p Agrant
 * #if not #ifdef for VER_DEBUG
 * 
 * 5     7/14/97 12:53a Agrant
 * unsigned/signed collision fixed
 * 
 * 4     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 3     5/18/97 5:04p Agrant
 * Compound rate and register list n longer tries to optimize-
 * we were losing some activities that way!
 * 
 * 2     5/18/97 3:11p Agrant
 * Moved a bunch of virtual functions to .cpp files
 * Fixed a rating bug
 * 
 * 1     5/18/97 2:31p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#include "Common.hpp"

#include "ActivityCompound.hpp"
#include "AIMain.hpp"
#include "Brain.hpp"
#include "Influence.hpp"
#include "Lib/EntityDBase/Animal.hpp"

#include "stdio.h"


//*********************************************************************************************
//
//	Class CActivityCompound implementation.
//

	//*************************************************************************************
	void CActivityCompound::CalculateRatingFeeling
	(
	)
	{
		// Get a union of all sub-activities' rating feelings.  Then, when we check to
		// see if we can skip this compound activity, we get a conservative answer.

		Assert(sapact.uLen > 0);

		feelRatingFeeling = sapact[0]->feelRatingFeeling;

		for (int i = sapact.uLen - 1; i > 0; i--)
		{
			feelRatingFeeling = feelRatingFeeling.feelUnion(sapact[i]->feelRatingFeeling);
			if (sapact[i]->rtThreshold < rtThreshold)
				rtThreshold = sapact[i]->rtThreshold;
		}


	}

	//*************************************************************************************
	CRating CActivityCompound::rtRateAndRegister
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	{
//		CRating rt_importance = rtRate(feel, pinf);

		// Rate self zero if inactive or can't go yet.
		if (!bIsActive() || sCanAct > gaiSystem.sNow)
			return 0;

		if (pinf && gaiSystem.paniGetCurrentAnimal()->bDead())
			return 0;
		
		CRating rt_importance = rtRateWithRatingFeeling(feel);
		rt_importance *= fMultiplier;

		if (rt_importance >= rtThreshold)
		{
			for (int i = sapact.uLen - 1; i >= 0; i--)
			{
				Assert(bValid(sapact[i]));
				(void) sapact[i]->rtRateAndRegister(feel, pinf);
			}
		}
		return rt_importance;
	}

	//*************************************************************************************
	CRating CActivityCompound::rtRateAndRegisterList
	(
		const CFeeling&	feel,		// The emotional state used to evaluate the action.
		CInfluenceList*	pinfl		// All influences known.
	)
	{

		// Just rate and register each sub-activity, thanks.
		CRating rt_importance = 0;

		for (int i = sapact.uLen - 1; i >= 0; i--)
		{
			Assert(bValid(sapact[i]));

			CActivity* pact = 					sapact[i];
			CRating rt = pact->rtRateAndRegisterList(feel, pinfl);

			if (rt > rt_importance)
				rt_importance = rt;
		}

		return rt_importance;
	}

	//*************************************************************************************
	void CActivityCompound::ResetTempFlags
	(
	)
	{
		CActivity::ResetTempFlags();

		for (int i = sapact.uLen - 1; i >= 0; i--)
			{
				Assert(bValid(sapact[i]));
				sapact[i]->ResetTempFlags();
			}
	}



//*********************************************************************************************
//
//	Class CActivityDOSubBrain implementation.
//

	//*********************************************************************************
	//
	CRating CActivityDOSubBrain::rtRate
	(
		const CFeeling&	feel_self,		// How the animal feels inside.
		CInfluence*		pinf			// What influence we are analyzing.
	)
	{
		// Not used!
		Assert(false);
		Assert(pinf);

		/*
		// Calculate square of distance to influence.
		TReal rDistanceFactor	= (gaiSystem.v3GetBodyLocation() -
									   pinf->v3Location).tLenSqr();

		// Calculate a distance scale factor for reducing the importance of distant
		// influences.  This might be slow, and will certainly need tweaking.
		rDistanceFactor				= rINFLUENCE_FALLOFF_DISTANCE / sqrt(rDistanceFactor);

		// Scale the influence down, if the scale factor < 1.
		if (rDistanceFactor < 1)
			feelAdjustedAttitude	= pinf->feelAboutTarget * CRating(rDistanceFactor);
		else 
			feelAdjustedAttitude	= pinf->feelAboutTarget;

		// Calculate final Adjusted Attitude by multiplying.
		feelAdjustedAttitude = feelAdjustedAttitude * feel_self;

		// Return and save rating.
		return rtRateWithRatingFeeling(feelAdjustedAttitude);
		*/
		return 0;
	}
	//******************************



	//*************************************************************************************
	CRating CActivityDOSubBrain::rtRateAndRegister
	(
		const CFeeling&	feel_self,		// How the animal feels inside.
		CInfluence*		pinf			// What influence we are analyzing.
	)
	//
	//	Notes:
	//		Uses an adjusted feeling to determine the sub-activities ratings.
	//
	{
		// Rate AND set feelAdjustedAttitude
		CRating rt_importance = rtRate(feel_self, pinf);

		if (rt_importance >= rtThreshold)
		{
			for (int i = sapact.uLen - 1; i >= 0; i--)
			{
				Assert(bValid(sapact[i]));
				(void) sapact[i]->rtRateAndRegister(feelAdjustedAttitude, pinf);
			}
		}
		return rt_importance;
	}


	
//**********************************************************************************************
//
//	Class CActivityGroup.
//

	//*********************************************************************************
	//
	CRating CActivityGroup::rtRateAndRegister
	(
		const CFeeling&,	//feel_self,		// How the animal feels inside.
		CInfluence*		pinf			// What influence we are analyzing.
	)
	{
		Assert(pinf);

		// We are we calling this?
		Assert(false);
		return 0;
	}
	//******************************


	//*********************************************************************************
	//
	CRating CActivityGroup::rtRateAndRegisterList
	(
		const CFeeling&	feel_self,		// How the animal feels inside.
		CInfluenceList*	pinfl			// What influence we are analyzing.
	)
	{
		Assert(pinfl);

		// First, rate each activity.
		// Saving as float to speed things up a bit.
		float	art[iACTIVITY_COMPOUND_NUM_SUBACTIVITIES];

		float	f_high	= 0;
		float	f_low	= 2;
		float	f_total	= 0;
//		int		i_high	= -1;

		int i;
		for (i = sapact.uLen - 1; i >= 0; i--)
		{
			// Get all the ratings.
			Assert(bValid(sapact[i]));
			art[i] = sapact[i]->rtRateList(feel_self, pinfl);

			// Test me!
//			if (art[i] != 0)
//				art[i] = .1 * i;

			// Find the lowest one.
			if (art[i] < f_low && art[i] > 0)
				f_low = art[i];

			// Find the highest one.
			if (art[i] > f_high)
			{
				f_high = art[i];
//				i_high = i;
			}

			// Keep a running total.
			f_total += art[i];
		}

		Assert(iLastChosen < (int)sapact.uLen);

		// Check on the guy last chosen.  Is he still a good choice?
		if (iLastChosen >= 0 && art[iLastChosen] != 0 &&
			sapact[iLastChosen]->sFlags[eafCONTINUE])  //lint !e771  (yes, art[iLastChosen] has been initialized.)
		{
			sapact[iLastChosen]->RegisterList(f_high, pinfl);
			return f_high;
		}
		else
		{
			iLastChosen = -1;
		}


		// Reject the trivial case, please.
		if (f_high <= 0)
			return 0;

		// Get a random number from 0 to f_high - f_low.
		float f_rand = (float) gaiSystem.rnd(0,f_total);
//		if (f_high != f_low)
//			f_rand = gaiSystem.rnd(0,f_total);
//		else
//		{
//			// Then only one was non-zero.  Take it.
//			sapact[i_high]->RegisterList(f_high, pinfl);
//			return f_high;
//		}

		// Make sure that we have a valid low rating.
		Assert(f_low < 1.5);

		// Now see where this number lands in our rating table.

		for (i = sapact.uLen - 1; i >= 0; i--)
		{
			if (art[i] != 0)
			{
				float f_temp = art[i];

				// We may have a winner!
				if (f_rand < f_temp)
				{
					sapact[i]->RegisterList(f_high, pinfl);
					iLastChosen = i;

#if VER_TEST
					if (f_high > 0.0f)
					{
						char ac[128];
						sprintf(ac, "%s rated \t\t%f\n", sapact[i]->acName, 1024 * f_high);
						gaiSystem.pbrGetCurrentBrain()->MaybeDebugSay(ac);
					}
#endif

					return f_high;
				}
				else
				{
					// We have a loser!
					f_rand -= f_temp;
				}
			}
		}

		// We shouldn't be here!  We should have found a match in the above loop.
		Assert(false);			
		
		return 0;
	}


	//*********************************************************************************
	//
	CRating CActivityExclusive::rtRateAndRegisterList
	(
		const CFeeling&	feel_self,		// How the animal feels inside.
		CInfluenceList*	pinfl			// What influence we are analyzing.
	)
	{
		Assert(pinfl);

		// First, rate each activity.
		// Saving as float to speed things up a bit.
//		float	art[iACTIVITY_COMPOUND_NUM_SUBACTIVITIES];

		CRating rt_high	= 0;
		int		i_high	= -1;

		for (int i = sapact.uLen - 1; i >= 0; i--)
		{
			// Get all the ratings.
			Assert(bValid(sapact[i]));
			CRating rt = sapact[i]->rtRateList(feel_self, pinfl);

			// Find the highest one.
			if (rt > rt_high)
			{
				rt_high = rt;
				i_high = i;
			}
		}

		if (i_high < 0)
			return 0.0f;

		// Register the winner!
		sapact[i_high]->RegisterList(rt_high, pinfl);

#if VER_TEST
		{
			char ac[128];
			sprintf(ac, "%s rated \t\t%f\n", sapact[i_high]->acName, 1024 * rt_high);
			gaiSystem.pbrGetCurrentBrain()->MaybeDebugSay(ac);
		}
#endif

		return rt_high;
	}


	//*********************************************************************************
	CRating CActivityExclusive::rtRateAndRegister
	(
		const CFeeling&	feel_self,		// How the animal feels inside.
		CInfluence*		pinf			// What influence we are analyzing.
	)
	{
		CRating rt_high	= 0;
		int		i_high	= -1;

		for (int i = sapact.uLen - 1; i >= 0; i--)
		{
			// Get all the ratings.
			Assert(bValid(sapact[i]));
			CRating rt = sapact[i]->rtRate(feel_self, pinf);

			// Find the highest one.
			if (rt > rt_high)
			{
				rt_high = rt;
				i_high = i;
			}
		}

		if (i_high < 0)
			return 0.0f;

		// Register the winner!
		sapact[i_high]->Register(rt_high, pinf);

#if VER_TEST
		{
			char ac[128];
			sprintf(ac, "%s rated \t\t%f\n", sapact[i_high]->acName, 1024 * rt_high);
			gaiSystem.pbrGetCurrentBrain()->MaybeDebugSay(ac);
		}
#endif

		return rt_high;
	}
