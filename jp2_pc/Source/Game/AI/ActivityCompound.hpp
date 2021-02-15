/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CActivityCompound base classes for AI library.  
 *
 * Bugs:
 *
 * To do:
 *		If the activity registered by the group activity is chosen, it is put forth next
 *		cycle if possible.
 *
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/ActivityCompound.hpp                                          $
 * 
 * 12    6/05/98 6:08p Agrant
 * Revised synthesizer to handle random-access acitivities.
 * Also handles exclusive ones (the old method).
 * 
 * 11    5/19/98 9:17p Agrant
 * Expanded the array of subactivities
 * 
 * 10    98/02/10 13:02 Speter
 * Changed original CSArray to CMSArray.
 * 
 * 9     1/07/98 5:14p Agrant
 * Allow more sub-activities in a group
 * 
 * 8     12/20/97 7:03p Agrant
 * Cleaning up AI includes
 * 
 * 7     7/14/97 12:53a Agrant
 * Init iLastChosen correctly
 * 
 * 6     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 5     5/18/97 3:11p Agrant
 * Moved a bunch of virtual functions to .cpp files
 * Fixed a rating bug
 * 
 * 4     5/11/97 12:53p Agrant
 * virtualized some functions
 * made group activities do good things.
 * 
 * 3     5/06/97 7:54p Agrant
 * delete those pointers
 * 
 * 2     5/04/97 9:30p Agrant
 * Reworked the way activities are handled-  each now operates on an
 * InfluenceList.
 * 
 * 1     4/30/97 1:59p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_ACTIVITYCOMPOUND_HPP
#define HEADER_LIB_AI_ACTIVITYCOMPOUND_HPP

#include "Activity.hpp"

//*********************************************************************************************
//
class CActivityCompound : public CActivity
//
//	Prefix: actc
//
//	A group of activities that can be treated as a single activity.
// 
//	Notes:
//		The CActivityCompound class is a collection of activities grouped together.  It handles
//		optimizations and helps logically group activities.
//
//*********************************************************************************************
{

//
//	Variable declarations.
//
#define iACTIVITY_COMPOUND_NUM_SUBACTIVITIES 20
protected:
	CMSArray<CActivity*, iACTIVITY_COMPOUND_NUM_SUBACTIVITIES>		sapact;				
				// An array of pointers to the sub-activities in the 
				// ActivityCompound.  
//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CActivityCompound
	(
	) : CActivity("COMPOUND")
	{
		Activate(true);
	};

	CActivityCompound
	(
		char *ac_name
	) : CActivity(ac_name)
	{
		Activate(true);
	};

	~CActivityCompound
	(
	)
	{
		for (int i = sapact.uLen - 1; i >= 0; i--)
		{
			delete sapact[i];
		}
	};


	//*****************************************************************************************
	//
	//	Member functions.
	//

	public:

		//*************************************************************************************
		//
		void AddActivity
		(
			CActivity*		pact
		)
		//
		//	Adds a sub-activity.
		//
		{
			sapact << pact;

			// THis is slow, but who cares?
			CalculateRatingFeeling();
		}
		//******************************


	//*****************************************************************************************
	//
	//	Overrides.
	//

		//*************************************************************************************
		//
		virtual void Act
		(
			CRating,			//rt_importance,	// The importance of the activity.
			CInfluence*		//pinf			// The direct object of the action.
		) override
		//
		//	
		//	Notes:  
		//		Does nothing.
		//
		{
			// It is meaningless for a compound activity to "Act()"
			Assert(false);
		}
		//******************************

		//*************************************************************************************
		//
		void Register
		(
			CRating,		//rt_importance,	// The importance of the action.
			CInfluence*		//pinf			// The direct object of the action.
		)
		//
		//	A compound activity cannot register itself, as it cannot Act.
		//
		//
		//******************************
		{
			Assert(false);
		}


		//*************************************************************************************
		//
		void CalculateRatingFeeling
		(
		);
		//
		//	Calculates a rating feeling based on the sub-activities.
		//
		//
		//******************************

		//*************************************************************************************
		//
		virtual void ResetTempFlags
		(
		) override;
		//
		//******************************

		//*************************************************************************************
		//
		virtual CRating rtRateAndRegister
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluence*		pinf		// The direct object of the action.
		) override;
		//
		//	Rates and Registers the activity with the synthesizer.
		//
		//	Notes:
		//		Compound activities rate and register their sub-activities, but only
		//		rate themselves.
		//
		//******************************

		//*************************************************************************************
		//
		virtual CRating rtRateAndRegisterList
		(
			const CFeeling&	feel,		// The emotional state used to evaluate the action.
			CInfluenceList*	pinfl		// All influences known.
		) override;
		//
		//	Rates and Registers the activity with the synthesizer.
		//
		//	Notes:
		//		If the rating is below the threshold value, the activity will not 
		//		bother registering itself.
		//
		//******************************

};



//*********************************************************************************************
//
class CActivityDOSubBrain : public CActivityCompound
//
//	Prefix: asb
//
//	The CDOSubBrainActivity is an ActivityCompound that handles all Actvitiies relevant to a
//	particular Influence.
//
//*********************************************************************************************
{
//
//  Member variables.
//
public:
	CFeeling	feelAdjustedAttitude;

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CActivityDOSubBrain
	(
	) : CActivityCompound("DOSUBBRAIN")
	{};

	~CActivityDOSubBrain
	(
	)
	{};


	//*****************************************************************************************
	//
	//	Overrides.
	//

	public:
		//*************************************************************************************
		//
		virtual CRating rtRate
		(
			const CFeeling&	feel_self,		// How the animal feels inside.
			CInfluence*		pinf			// What influence we are analyzing.
		) override;
		//
		//	Notes:
		//		The CActivityDOSubBrain performs a bunch of internal processing on the 
		//		influence and the feeling to create a new feeling which accurately 
		//		represents the animal's attitude toward the influence.
		//
		//	Side Effects:
		//		Sets feelAdjustedAttitude
		//

		//*************************************************************************************
		//
		virtual CRating rtRateAndRegister
		(
			const CFeeling&	feel_self,		// How the animal feels inside.
			CInfluence*		pinf			// What influence we are analyzing.
		) override;
		//
		//	Notes:
		//		Uses an adjusted feeling to determine the sub-activities ratings.
		//
		
};


//*********************************************************************************************
//
class CActivityGroup : public CActivityCompound
//
//	Prefix: ag
//
//	A compound activity in which the sub-activities are treated different ways of expressing
//		the same action.
// 
//	Notes:
//		The group activity registers only one of its sub-activities at a time.
//		It registers that activity as strongly as the strongest would have been registered.
//		It selects which activity to register based loosely on the relative rating
//		strengths of the sub-activities.
//
//		A weak sub-activity has a better chance of expressing itself than it would by
//		itself.
//
//*********************************************************************************************
{

//
//  Member variable definitions.
//
	int		iLastChosen;			// The sub-activity last chosen by the group.
									// -1 if none.


public:
	


//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CActivityGroup
	(
	) : CActivityCompound("GROUP")
	{
		iLastChosen = -1;
	};

	~CActivityGroup

	(
	)
	{};


	//*****************************************************************************************
	//
	//	Member functions.
	//

	public:


	//*****************************************************************************************
	//
	//	Overrides.
	//

		//*************************************************************************************
		//
		virtual CRating rtRateAndRegister
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluence*		pinf		// The direct object of the action.
		) override;
		//
		//******************************

		//*************************************************************************************
		//
		virtual CRating rtRateAndRegisterList
		(
			const CFeeling&	feel,		// The emotional state used to evaluate the action.
			CInfluenceList*	pinfl		// All influences known.
		) override;
		//
		//******************************
};


//*********************************************************************************************
//
class CActivityExclusive : public CActivityCompound
//
//	Prefix: ae
//
//	A compound activity in which only the highest rated sub-activity is chosen.
// 
//	Notes:
//		This allows us to get past some of the limitations of the random access synthesizer.
//
//*********************************************************************************************
{

//
//  Member variable definitions.
//

public:
	


//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CActivityExclusive
	(
	) : CActivityCompound("EXCLUSIVE")
	{
	};

	~CActivityExclusive

	(
	)
	{};


	//*****************************************************************************************
	//
	//	Member functions.
	//

	public:


	//*****************************************************************************************
	//
	//	Overrides.
	//

		//*************************************************************************************
		//
		virtual CRating rtRateAndRegister
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluence*		pinf		// The direct object of the action.
		) override;
		//
		//******************************

		//*************************************************************************************
		//
		virtual CRating rtRateAndRegisterList
		(
			const CFeeling&	feel,		// The emotional state used to evaluate the action.
			CInfluenceList*	pinfl		// All influences known.
		) override;
		//
		//******************************
};



// #ifndef HEADER_LIB_AI_ACTIVITYCOMPOUND_HPP
#endif
