/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of Synthesizer.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Synthesizer.cpp                                              $
 * 
 * 14    8/25/98 11:12a Rvande
 * Checked in for Andrew Southwick.
 * 
 * 13    6/09/98 12:39p Agrant
 * Continue the continuing activities by classifying them as exclusive in the synthesizer.
 * 
 * 12    6/05/98 6:08p Agrant
 * Revised synthesizer to handle random-access acitivities.
 * Also handles exclusive ones (the old method).
 * 
 * 11    2/23/98 5:07p Agrant
 * AI save/load
 * 
 * 10    2/13/98 1:53p Agrant
 * Rework multipliers, continues, and was used last time data
 * 
 * 9     12/20/97 6:59p Agrant
 * Assert for active activities in synthesizer
 * 
 * 8     7/19/97 1:25p Agrant
 * Dino resource tracking begins
 * 
 * 7     4/29/97 6:42p Agrant
 * CFeeling now float-based instead of CRating-based.
 * Activities now rate/act based on a list of influences.
 * Better rating functions for activities.
 * Debugging tools for activity processing.
 * 
 * 6     10/15/96 9:20p Agrant
 * Synthesizer reworked to be better, stronger, smarter, faster than before.
 * Activity Rate and Act and Register functions now have better defined roles.
 * Added some accessor functions to smooth future changes.
 * 
 * 5     10/10/96 7:18p Agrant
 * Modified code for code specs.
 * 
 * 4     9/27/96 5:43p Agrant
 * Added the objectless sub-brain and created a CWanderActivity
 * 
 * 3     9/26/96 5:58p Agrant
 * basic AI structure done
 * AI system adapted to TReal for world coords
 * AI Test App exhibits very basic herding with two very simple actvities.
 * 
 * 2     9/23/96 2:55p Agrant
 * Some basic functionality
 * 
 * 1     9/20/96 5:06p Agrant
 * Initial revision
 * 
 **********************************************************************************************/

#include "Common.hpp"

#include "Synthesizer.hpp"

#include "Activity.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "AIMain.hpp"
#include "Brain.hpp"


//**********************************************************************************************
//
//	Class CSynthesizer implementation.
//

	//****************************************************************************************
	CSynthesizer::CSynthesizer() : setResourcesAvailable()
	{
		pbrBrain = 0;
	}


	//*********************************************************************************
	void CSynthesizer::Register
	(
		const CRating		rt_importance,	// The importance of the hint.
		CActivity*			pact,			// The activity being registered.
		CInfluence*			pinf			// The direct object of the activity.
	)
	{
		Assert(rt_importance > 0.0f); 

		CActBundle ab(pact, pinf);

		if (pact->bIsExclusive() || pact->sFlags[eafCONTINUE])
		{
			mmapExclusiveActivities.insert( TConstPairAct(rt_importance, ab) );  //lint !e534   // ignoring return value
		}
		else
		{
			mmapRandomActivities.insert( TConstPairAct(rt_importance, ab) );  //lint !e534   // ignoring return value
		}
	}

	//*********************************************************************************
	void CSynthesizer::RegisterHerdHint
	(
		const CRating		rt_importance,	// The importance of the hint.
		const CVector2<>&	v2_direction	// The direction of the hint.
	)
	{
		if (v2_direction.tX == 0 &&
			v2_direction.tY == 0)
			return;

		CVector2<> v2 = v2_direction;
		v2.Normalise(rt_importance.fVal);
		
		// Add to the sum of all movement inputs.
		v2HerdHint += v2;

		// And set the importance.
		if (rt_importance > rtHerdImportance)
			rtHerdImportance = rt_importance;
	}

	//*********************************************************************************
	void CSynthesizer::RegisterFleeHint
	(
		const CRating		rt_importance,	// The importance of the hint.
		const CVector2<>&	v2_direction	// The direction of the hint.
	)
	{
		if (v2_direction.tX == 0 &&
			v2_direction.tY == 0)
			return;

		// Normalise to importance.
		CVector2<> v2 = v2_direction;
		v2.Normalise(rt_importance.fVal);
		
		// Add to the sum of all movement inputs.
		v2FleeHint += v2;

		// And set the importance.
		if (rt_importance > rtFleeImportance)
			rtFleeImportance = rt_importance;
	}

	//*********************************************************************************
	void CSynthesizer::Reset
	(
		CBrain*		pbr
	)
	{
		Assert(pbr);

		// Set the current brain.
		pbrBrain			= pbr;

		// Clear movement variables.
		v2HerdHint			= CVector2<>(0.0f,0.0f);
		rtHerdImportance	= 0;

		v2FleeHint			= v2HerdHint;
		rtFleeImportance	= 0;

		// Clear the map of activities.
		mmapRandomActivities.erase
		(
			mmapRandomActivities.begin(),
			mmapRandomActivities.end()
		);

		mmapExclusiveActivities.erase
		(
			mmapExclusiveActivities.begin(),
			mmapExclusiveActivities.end()
		);

	}

	//*************************************************************************************
	bool CSynthesizer::bAllResourcesUsed
	(
	)
	{
		return !setResourcesAvailable;
	}

	//*********************************************************************************
	//
	void CSynthesizer::Synthesize
	(
	)
	//
	//	Runs the most important activities registered with the synthesizer.
	//
	//	Notes:
	//		Examines all actions registered and picks the most important ones to actually 
	//		run, then calls their Act() functions.  Tracks resources used or blocked 
	//		by actions thus called.
	//
	//******************************
	{
		CRating		rt;
		CActBundle*	pab_bundle;
		CActivity*	pact;		// Temp activity.

		// All resources available.
		setResourcesAvailable = CSet<EDinoResource>(0xffffffff,1);

		// First, grab all exclusive activities, most important ones first.
		TMMapAct::reverse_iterator pab;
		for (pab = mmapExclusiveActivities.rbegin();
			 pab != mmapExclusiveActivities.rend();
			 pab++)
		{
			// Grab the data out of the TMMapAct
			rt				= (*pab).first;
			pab_bundle		= &(*pab).second;
			pact			= pab_bundle->pactActivity;

			// For each activity....
			pact = (*pab).second.pactActivity;
			Assert(pact);
			Assert((uint)pact != 0xcdcdcdcd);
			Assert(pact->bIsActive());

			// Verify that required resources are available.
			if (pact->bResourcesAreAdequate(setResourcesAvailable))
			{
				// Run the activity.  The activity has won out over all others 
				// competing for the same resources.

#if VER_TEST
				char buffer[256];
				sprintf(buffer, "Exclusive:  %s\n", pact->acName);
				pbrBrain->MaybeDebugSay(buffer);
#endif
				// Act on the data.
				pact->Act(rt, pab_bundle->pinfInfluence);

				// Mark the activity as chosen.
				pact->SetWasChosen();
					
				// Update the available resources.
				pact->UseResources(&setResourcesAvailable);

				// If all resources are used up, we're done. 
				if (bAllResourcesUsed())
					break;
			}
		}

		// Done with the exclusives.  No choose from the randoms.
		while (!bAllResourcesUsed())
		{
			// We have some body parts unused.  Use them!

			// Keep a running total of all ratings.
			float f_rating_total = 0.0f;


			// First pass through the list, adding up the total ratings and removing impossible actions.
			TMMapAct::reverse_iterator pab;
			for (pab = mmapRandomActivities.rbegin();
				 pab != mmapRandomActivities.rend();
				 pab++)
			{
				// Grab the data out of the TMMapAct
				rt				= (*pab).first;
				pab_bundle		= &(*pab).second;
				pact			= pab_bundle->pactActivity;

				// For each activity....
				Assert(pact);
				Assert((uint)pact != 0xcdcdcdcd);
				Assert(pact->bIsActive());


				// Do we have resources for this activity?
				if (pact->bResourcesAreAdequate(setResourcesAvailable))
				{
					// Yes!  Log its rating.
					f_rating_total += rt;
				}
			}

			// If total is zero, break.
			if (f_rating_total == 0.0f)
				break;
						
			// Now roll a random number, zero to total rating.
			float f_choose = gaiSystem.rnd(0.0f, f_rating_total);

			// Iterate through list again, until our random gets lower than the current guy's rating.
			for (pab = mmapRandomActivities.rbegin();
				 pab != mmapRandomActivities.rend();
				 pab++)
			{
				// Grab the data out of the TMMapAct
				rt				= (*pab).first;
				pab_bundle		= &(*pab).second;
				pact			= pab_bundle->pactActivity;

				// For each activity....
				Assert(pact);
				Assert((uint)pact != 0xcdcdcdcd);
				Assert(pact->bIsActive());


				// Do we have resources for this activity?
				if (pact->bResourcesAreAdequate(setResourcesAvailable))
				{
					// Yes!  Subtract its rating.
					f_choose -= rt;

					// Is this our chosen one?
					if (f_choose <= 0.0f)
					{
						// Yes!  Go with it.

#if VER_TEST
						char buffer[256];
						sprintf(buffer, "Random:  %s\n", pact->acName);
						pbrBrain->MaybeDebugSay(buffer);
#endif

						// Act on the data.
						pact->Act(rt, pab_bundle->pinfInfluence);

						// Mark the activity as chosen.
						pact->SetWasChosen();
							
						// Update the available resources.
						pact->UseResources(&setResourcesAvailable);

						// If all resources are used up, we're done. 
						if (bAllResourcesUsed())
							break;
					}
				}
			}
		}
	}



	//*****************************************************************************************
	char *CSynthesizer::pcSave(char * pc) const
	{
//		Assert(rtThreshold.fVal >= 0.0f);
//		Assert(fMultiplier >= 0.0f);

//		pc = pcSaveT(pc, feelRatingFeeling);
//		pc = pcSaveT(pc, rtThreshold);
//		pc = pcSaveT(pc, fMultiplier);
//		pc = pcSaveT(pc, sFlags);

		return pc;
	}

	//*****************************************************************************************
	const char *CSynthesizer::pcLoad(const char * pc)
	{
//		pc = pcLoadT(pc, &feelRatingFeeling);
//		pc = pcLoadT(pc, &rtThreshold);
//		pc = pcLoadT(pc, &fMultiplier);
//		pc = pcLoadT(pc, &sFlags);

//		Assert(rtThreshold.fVal >= 0.0f);
//		Assert(fMultiplier >= 0.0f);

		return pc;
	}
