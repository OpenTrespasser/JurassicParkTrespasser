/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CSynthesizer class for AI library.  CSynthesizer combines all sub-brain inputs into 
 *		a form the game can actually use.
 *		
 *
 * Bugs:
 *
 * To do:
 *		Synthesize:		
 *			Set up a resources-used table for all actions
 *			Let the dino do multiple things at once.
 *		TellPhysicsMove:
			Set up a real method for max speeds, replacing the constant.
 *		Misc:
 *			Create a notion of MAX SPEED.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Synthesizer.hpp                                               $
 * 
 * 16    8/25/98 11:42a Agrant
 * made CRating const in the activity maps.  Seems to make STL happier under MS.
 * 
 * 15    8/25/98 10:54a Rvande
 * Added const pair type needed for multimap
 * 
 * 14    6/05/98 6:08p Agrant
 * Revised synthesizer to handle random-access acitivities.
 * Also handles exclusive ones (the old method).
 * 
 * 13    2/23/98 5:07p Agrant
 * AI save/load
 * 
 * 12    12/17/97 9:27p Agrant
 * Reduced synthesizer dependency
 * 
 * 11    5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 10    12/10/96 7:41p Agrant
 * World DBase changes.
 * 
 * 9     10/23/96 7:38p Agrant
 * more debugging info
 * first pass at the tree avoiding pathfinder
 * 
 * 8     10/15/96 9:20p Agrant
 * Synthesizer reworked to be better, stronger, smarter, faster than before.
 * Activity Rate and Act and Register functions now have better defined roles.
 * Added some accessor functions to smooth future changes.
 * 
 * 7     10/10/96 7:18p Agrant
 * Modified code for code specs.
 * 
 * 6     10/02/96 8:11p Agrant
 * Added Fleeing and Hunting.
 * Added Sheep and Wolves to Test App.
 * Added some debugging hooks.
 * 
 * 5     9/30/96 3:06p Agrant
 * modified for enum code spec
 * 
 * 
 * 4     9/27/96 5:43p Agrant
 * Added the objectless sub-brain and created a CWanderActivity
 * 
 * 3     9/26/96 5:58p Agrant
 * basic AI structure done
 * AI system adapted to TReal for world coords
 * AI Test App exhibits very basic herding with two very simple actvities.
 * 
 * 2     9/23/96 2:56p Agrant
 * Basic implementation
 * 
 * 1     9/19/96 1:41p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_SYNTHESIZER_HPP
#define HEADER_GAME_AI_SYNTHESIZER_HPP


//#include "Brain.hpp"
#include "Influence.hpp"
//#include <multimap.h>
#include <map>

#include "Lib\Std\Set.hpp"


class CBrain;

//**********************************************************************************************
//
//	Helper functions.
//


	
class CActBundle
//
//	Prefix: ab
//
//	Bundles together all data needed to store an activity state for later use.
//
//	Notes:
//		There is no need to copy an entire Activity every time an animal wants to 
//		register an Activity.  This class saves the data that might be lost in later
//		analysis by the brain, but does not waste time and space copying redundant data
//		in the activity that will not change.
//
//		The CSynthesizer class handles all manipulation of this class.  It could be
//		a struct.
//
//*********************************************************************************************
{
//
//	Variable declarations.
//
public:
	CActivity*	pactActivity;	// The base activity whose state is being saved.
	CInfluence*	pinfInfluence;	// The influence to which the activity pertains, 0 if none.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	CActBundle
	(
		CActivity*	pact,
		CInfluence* pinf
	) : pactActivity(pact), pinfInfluence(pinf)
	{
		Assert(bValid(pact));
	}

	CActBundle
	(
	)
	{
		Assert(false);
	}
};



//*********************************************************************************************
//
class CSynthesizer
//
//	Prefix: synth
//
//	Resolves conflicting desires within an animal's brain.
//
//	Notes:
//		The CSynthesizer class is an object associated with a brain.  It coordinates conflicting
//		desires and issues commands to the physics system.  All actions are sent to the synth for
//		dispatching.
//
//		Hints are commands issued during the rating phase of an animal's thought processes.
//		Later, when the actual victorious Activities are run, they may or may not take the hints
//		into account, as the individual activity sees fit.  Currently, there are two types of
//		hints:
//			Direction in which to flee.
//			Direction in which to herd.
//		These hints are each expressed as a weighted vector sum.
//
//		An activity is usually registered with the synthesizer immediately after it has been
//		rated, and it is only registered if above its threshold.  The synthesizer remembers
//		which activity has been registered, its rating at that time, and the influence (if any)
//		associated with the activity.  This data allows the activity to be used for other
//		influences before the synthesizer calls upon it to Act().
//
//*********************************************************************************************
{


//
//  Variable declarations
//
public:

	CVector2<>		v2FleeHint;	
						// Weighted sum of all flee hint vectors.
	CRating			rtFleeImportance;
						// How much do I want to follow the flee hints?

	CVector2<>		v2HerdHint;	
						// Weighted sum of all herd hint vectors.
	CRating			rtHerdImportance;
						// How much do I want to follow the herd hints?

	CBrain*			pbrBrain;	
						// The brain currently using this synthesizer.

private:
	typedef std::multimap<const CRating, CActBundle, std::less<CRating> >	TMMapAct;
	typedef std::pair<const CRating, CActBundle>						TPairAct;
	typedef std::pair<const CRating, CActBundle>					TConstPairAct;

	TMMapAct				mmapRandomActivities;
						// The set of all registered activities,
						// sorted by importance.  We choose one of these randomly, weighted by importance.

	TMMapAct				mmapExclusiveActivities;
						// The set of all registered, exclusive activities,
						// sorted by importance.  We choose the most important of these, until we fall below the first
						//  normally registered one.

	CSet<EDinoResource>		setResourcesAvailable;
						// A set of dinosaur resources.  The resource is 
						// "available" if setResourcesAvailable[resource]
						// is true.
						// Elements are indexed with EDinoResource.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	CSynthesizer();

	~CSynthesizer()
	{
		pbrBrain = 0;
	};

	//*****************************************************************************************
	//
	//	Member functions.
	//
	
		//*********************************************************************************
		//
		void Register
		(
			const CRating		rt_importance,	// The importance of the hint.
			CActivity*			pact,			// The activity being registered.
			CInfluence*			pinf			// The direct object of the activity.
		);
		//
		//	Registers an activity with the synthesizer.
		//
		//	Notes:
		//		When Synthesize() is run, the most important registered activities are run.
		//
		//******************************

		//*********************************************************************************
		//
		void RegisterHerdHint
		(
			const CRating		rt_importance,	// The importance of the hint.
			const CVector2<>&	v2_direction	// The direction of the hint.
		);
		//
		//	Adds a hint about how to herd to the synthesizer.
		//
		//	Notes:
		//		Hints may or may not be ignored by whatever Activity wins the movement
		//		controls.
		//
		//******************************
		

		//*********************************************************************************
		//
		void RegisterFleeHint
		(
			const CRating		rt_importance,	// The importance of the hint.
			const CVector2<>&	v2_direction	// The direction of the hint.
		);
		//
		//	Adds a hint about how to flee to the synthesizer.
		//
		//	Notes:
		//		Hints may or may not be ignored by whatever Activity wins the movement
		//		controls.
		//
		//******************************
	

		//*********************************************************************************
		//
		void Reset
		(
			CBrain*		pbr
		);
		//
		//	Clears the synthesizer to prepare it for a new set of commands.
		//
		//******************************
		
		//*************************************************************************************
		//
		bool bAllResourcesUsed
		(
		);
		//
		//	Are there any resources left?
		//
		//	Returns:
		//		true if there are no resources available.
		//
		//******************************
		


		//*********************************************************************************
		//
		void Synthesize
		(
		);
		//
		//	Causes the synthesizer to send requests to the physics system to have
		//	the object associated with the current brain perform the synthesized commands.
		//
		//  Notes:
		//		Eventually, will perform the following actions:	
		//		For each resource...
		//			Verify that the other needed resources are available.
		//			Remove the primitive from the other resources that have it so we don't send 
		//			it twice.
		//			Send the primitive to physics.	
		//	
		//		For now, we use only the head resource, so most of this stuff is not
		//		used.
		//
		//
		//******************************


		//*****************************************************************************************
		virtual char * pcSave(char *  pc_buffer) const;

		//*****************************************************************************************
		virtual const char * pcLoad(const char *  pc_buffer);


};

// #ifndef HEADER_GAME_AI_SYNTHESIZER_HPP
#endif
