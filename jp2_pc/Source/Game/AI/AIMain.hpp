/***********************************************************************************************
 *
 * $Source::																				   $
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The AI main functions.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/AIMain.hpp                                                    $
 * 
 * 51    10/02/98 10:01p Agrant
 * added boring bool
 * 
 * 50    9/08/98 7:38p Agrant
 * added sleep flag for entire AI system
 * 
 * 49    9/02/98 5:23p Agrant
 * A new AI heap
 * 
 * 48    9/02/98 3:58p Agrant
 * Now sharing graphs for all dinos in a level.
 * 
 * 47    8/31/98 5:08p Agrant
 * lowered max number of active animals
 * 
 * 46    8/26/98 1:05a Agrant
 * velocity and speed easily accessible
 * 
 * 45    8/25/98 10:42a Rvande
 * Removed redundant scope
 * 
 * 44    8/24/98 10:05p Agrant
 * Limit AI to ten "frames" per second
 * 
 * 43    8/23/98 2:22p Agrant
 * More startle functions
 * 
 * 42    8/20/98 11:37p Agrant
 * function for listening to sound in the world
 * 
 * 41    8/20/98 11:09p Agrant
 * tail point accessor function
 * 
 * 40    8/13/98 1:49p Mlange
 * Moved constructor out of line.
 * 
 * 39    8/12/98 5:24p Pkeet
 * Added the 'bStepPending' data member and the 'ProcessPending' member function.
 * 
 * 38    7/23/98 9:35p Agrant
 * reworked vocals
 * added some activate/deactivate functions
 * 
 * 37    7/21/98 4:52p Agrant
 * added the AlertAnimals function
 * 
 * 36    7/14/98 4:13p Agrant
 * draw ai influences in 3d
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_AIMAIN_HPP
#define HEADER_GAME_AI_AIMAIN_HPP

#include <list>
#include "Lib/Std/Random.hpp"
#include "Lib/EntityDBase/Subsystem.hpp"

#include "Classes.hpp"
#include "Feeling.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Lib/Audio/SoundDefs.hpp"
#include "Lib/Sys/FastHeap.hpp"

#define iAI_RANDOM_SEED 87;
#define iMAX_ACTIVE_ANIMALS 4
#define iNUM_VOCALS 10
//(edtEND * evtEND * 10)



extern float fAIDebug1;
extern float fAIDebug2;
extern float fAIDebug3;
extern CFastHeap fhAI;

class CAnimal;

class CConsoleBuffer;

class CGroffObjectName;
class CLoadWorld;
class CHandle;
class CValueTable;
class CInfo;
class CAIGraph;


struct SVocal {
	TSoundHandle sndhnd;	// what to play
	TSec		 sOpen;		// when to open mouth, after begining of sample
	TSec		 sClose;	// when to close mouth, before end of sample
};

typedef CSArray<SVocal, iNUM_VOCALS> TVocalSet;


//**********************************************************************************************
//
class CAISystem : public CSubsystem
//
//	An AI system,  handling all AI calls and data external to the object system.
// 
//	Notes:
//		An instance of this class should be the only AI global variable.  
//		Ideally, all AI that do not belong a class will be stuck in here, to act as a sort of
//		namespace.	
//
// Prefix: ai
//
//**************************************
{
	//
	// Variable declarations.
	//
public:

	bool		bActive;			// True if the AI system runs on a step message.
	bool		bSleep;				// True if we artificially sleeping everybody via trigger.
	bool		bBoring;			// True if user has set up the no dinos cheat.

	TSec		sMinInterval;		// How often does the AI system run?
	TSec		sLastExecution;		// When did we last run?


	CSynthesizer*	psynGlobalSynthesizer;
							// A global synthesizer, shared by all brains.
	
	CRandom			rnd;	// The global AI random number generator.

	CFeeling		afeelDefaultOpinions[earEND][eaiEND];
							// A table of default feelings describing how each AI archetype feels about
							// each type of object.  This may be replaced in the future.

	CSArray<TVocalSet, edtEND * evtEND> avsSamples;	// The samples to play for the vocalization.  Set in constructor.


	TSec			sNow;	// Current simulation time.

	CConsoleBuffer* pconDebugConsole;

	TMSec			msAITimeTaken;	// The number of seconds required by the AI system in
									// its most recent Step process.

	CInstance*		pinsSelected;	// Mostly for debugging-  which instance are we most interested in?

	CSArray<CAnimal* ,iMAX_ACTIVE_ANIMALS>		apaniActiveAnimals;	// The set of active animals.
	std::list<void*>     lpaniInactiveAnimals;		// List of all inactive animals.  void * to avoid STL stupidity.

	CSArray<CAIGraph*,iMAX_ACTIVE_ANIMALS> 		apgraphActiveGraphs;// The shared AI graphs.

///  Debugging flags.
	bool			bDrawAllGraphLinks;			// True if the entire graph is drawn.
	bool			bDrawJumpGraphLinks;		// True if we ought to draw jump links. (surface transfers)
	bool			bShow3DInfluences;
private:
	// These are horrible globals that allow us to avoid passing the same arguments throughout the AI code.
	// Perhaps I ought to remove them.
	CSynthesizer*	psynCurrentSynthesizer;
						// The synthesizer currently in use by the currently thinking brain.

	CBrain*			pbrCurrentBrain;	// The brain that is currently thinking.
	CVector3<>		v3CurrentVelocity;	// The velocity of the current animal.
	TReal			rCurrentSpeed;		// Magnitude of velocity vector.

	bool            bStepPending;	// Flag set to 'true' if an AI step is still pending.

	//
	// Member function definitions.
	//

public:
	//****************************************************************************************
	//
	//	Constructors and destructor
	//

	CAISystem
	(
		CConsoleBuffer* pcon    // The buffer used for debugging.
	);

	CAISystem();
	
	~CAISystem();


	//****************************************************************************************
	//
	//	Member functions
	//

		//*********************************************************************************************
		//
		void InitDefaultOpinions
		(
		);
		//
		//	Initializes the default opinion table.
		//
		//**************************************

		//*********************************************************************************************
		//
		void HandleSensoryInput
		(
		);
		//
		//	Handles all dinosaur sensory input.
		//
		//**************************************

		//*********************************************************************************************
		//
		void ThinkAwake
		(
		);
		//
		//	Runs the AI system for each wakeful dinosaur.
		//
		//**************************************

		//*********************************************************************************************
		//
		void AddAnimal
		(
			CAnimal* pani	// The animal to add to the activate list.
		);
		//
		//**************************************

		//*********************************************************************************************
		//
		void RemoveAnimal
		(
			CAnimal* pani	// The animal to add to the activate list.
		);
		//
		//**************************************

		//*********************************************************************************************
		//
		void ActivateAnimal
		(
			CAnimal* pani	// The animal to add to the activate list.
		);
		//
		//**************************************

		//*********************************************************************************************
		//
		void DeactivateAnimal
		(
			CAnimal* pani	// The animal to remove from the activate list.
		);
		//
		//**************************************

		//*********************************************************************************************
		//
		void ActivateAnimals
		(
			CVector3<>	v3_center,	// center of wakey sphere
			TReal		r_radius	// radius of wakey sphere
		);
		//
		//**************************************

		//*********************************************************************************************
		//
		void DeactivateAnimals
		(
			CVector3<>	v3_center,	// center of sleepy sphere
			TReal		r_radius	// radius of sleepy sphere
		);
		//
		//**************************************

		//*********************************************************************************************
		//
		void AlertAnimals
		(
			CInstance*			pins,		// The thing to bring to the animals' attention
			TReal				r_radius	// The distance beyond which animals are not notified
		);
		//
		//	Tells all animals within "r_radius" of "pins" to notice "pins"
		//
		//**************************************

		//*********************************************************************************************
		//
		void Handle3DSound
		(
			const CVector3<>&	v3_location,			// Where is the sound?				(world coords)
			float				f_master_volume,		// How loud is it at its source?	(DB)
			float				f_distance_attenuation	// How quickly does it get quiet?	(DB/m)
		);
		//
		//	Tells all animals about a 3D sound.
		//
		//**************************************
				
		//*********************************************************************************************
		//
		void Handle3DStartle
		(
			const CVector3<>&	v3_location,			// Where is the event?				(world coords)
			float				f_excitement,			// How exciting is it at its source?(greater than zero)
			float				f_distance_attenuation	// How quickly does it get quiet?	(excitement fall of per meter)
		);
		//
		//	Tells all animals about an exciting event in 3D space.  They react when the excitement at their
		//	position is greater than zero.
		//
		//**************************************

		//*********************************************************************************************
		//
		void Handle3DStartle
		(
			const CVector3<>&	v3_location,			// Where is the event?				(world coords)
			TReal				r_radius				
		);
		//
		//	Tells all animals within the radius about an exciting event in 3D space.  
		//
		//**************************************

	//
	//	Accessor functions
	//
			
		//*********************************************************************************************
		//
		void ProcessAICommand
		(
			CGroffObjectName* pgon,
			 CLoadWorld*	pload,
			 const ::CHandle& h,
			 CValueTable* pvt,
			 const CInfo* pinfo
		);
		//
		//	Interprets commands passed into the AI system via the groff load process.
		//
		//**************************************

		//*********************************************************************************************
		//
		TVocalSet* pvsGetVocalSet
		(
			EDinoType edt,
			EVocalType evt
		);
		//
		//	Gets a pointer to an array of sound handles.
		//
		//**************************************

		//*********************************************************************************************
		//
		int iNumVocalHandles
		(
			EDinoType edt,
			EVocalType evt
		);
		//
		//	Gets the number of vocals actually specified for a given dino/vocal pair.
		//
		//**************************************

		//*************************************************************************************
		//
		void GetOpinion
		(
			const CAnimal* pani_self,
			const CInstance* pins_target,
			CFeeling*		pfeel_return
		) const;
		//
		//	Gets a feeling representing how the specified archetype usually feels about things
		//	attached to this info.
		//
		//******************************

		//*********************************************************************************************
		//
		CSynthesizer* psynGetCurrentSynthesizer
		(
		)
		//
		//	Gets the synthesizer being used by the current brain.
		//
		//**************************************
		{
			return psynCurrentSynthesizer;
		}

		//*********************************************************************************************
		//
		void SetCurrentSynthesizer
		(
			CSynthesizer* psyn
		)
		//
		//	Sets the synthesizer being used by the current brain.
		//
		//**************************************
		{
			Assert(psyn);
			psynCurrentSynthesizer = psyn;
		}

		//*********************************************************************************************
		//
		CBrain* pbrGetCurrentBrain
		(
		)
		//
		//	Gets the current brain.
		//
		//**************************************
		{
			Assert(pbrCurrentBrain);
			return pbrCurrentBrain;
		}


		//*********************************************************************************************
		//
		void SetCurrentBrain
		(
			CBrain *pbr
		);
		//
		//	Sets the current brain.
		//
		//**************************************

		//*********************************************************************************************
		//
		CAnimal* paniGetCurrentAnimal
		(
		);
		//
		//	Gets the current animal.
		//
		//**************************************

		//*********************************************************************************************
		//
		void ClearCurrentBrain
		(
		)
		//
		//	Clears current brain.
		//
		//**************************************
		{
			pbrCurrentBrain			= 0;
			psynCurrentSynthesizer	= 0;
		}

		//*********************************************************************************************
		//
		const CVector3<> v3GetBodyLocation
		(
		) const;
		//
		//	Gets a 3d vector describing the animal's location.
		//
		//	Notes:
		//		Current brain must be set.
		//
		//**************************************

		//*********************************************************************************************
		//
		const CVector2<> v2GetBodyFacing
		(
		) const;
		//
		//	Gets a 2d vector describing the direction the pelvis faces
		//
		//	Notes:
		//		Current brain must be set.
		//
		//**************************************

		//*********************************************************************************************
		//
		const CVector3<> v3GetBodyVelocity
		(
		) const
		//
		//	Gets a 3d vector describing the velocity of the animal.
		//
		//**************************************
		{
			return v3CurrentVelocity;
		}

		//*********************************************************************************************
		//
		const TReal rGetBodySpeed
		(
		) const
		//
		//	Gets speed of the current animal.
		//
		//**************************************
		{
			return rCurrentSpeed;
		}

		//*********************************************************************************************
		//
		const CVector3<> v3GetHeadLocation
		(
		) const;
		//
		//	Gets a 3d vector describing the animal's head location.
		//
		//	Notes:
		//		Current brain must be set.
		//
		//**************************************

		//*********************************************************************************************
		//
		const CVector3<> v3GetHeadPoint
		(
		) const;
		//
		//	Gets a 3d vector describing the animal's head orientation.
		//
		//	Notes:
		//		Current brain must be set.
		//
		//**************************************

		//*********************************************************************************************
		//
		const CVector3<> v3GetTailPoint
		(
		) const;
		//
		//	Gets a 3d vector describing the animal's tail orientation.
		//
		//	Notes:
		//		Current brain must be set.
		//
		//**************************************

		//*********************************************************************************************
		//
		const CVector2<> v2GetBodyLocation
		(
		) const;
		//
		//	Gets a 2d vector describing the animal's location.
		//
		//	Notes:
		//		Current brain must be set.
		//
		//**************************************

		//*********************************************************************************************
		//
		const CVector2<> v2GetBodyVelocity
		(
		) const;
		//
		//	Gets a 2d vector describing the animal's current velocity
		//
		//	Notes:
		//		Current brain must be set.
		//
		//**************************************

		//*********************************************************************************************
		//
		const CVector2<> v2GetDestination
		(
		) const;
		//
		//	Gets a 2d vector describing the animal's current destination
		//
		//**************************************

		//*********************************************************************************************
		//
		const CVector2<>& v2GetLastDesiredDirection
		(
		) const;
		//
		//	Gets a 2d vector describing the animal's current destination
		//
		//**************************************

		//*********************************************************************************************
		//
		void DebugPrint
		(
			const char *pc_string
		) const;
		//
		//	Prints the string to a debug console.
		//
		//**************************************

		//*********************************************************************************************
		//
		inline bool bIsSelected
		(
			const CInstance *pins
		) const
		//
		//	Returns true if the instance is selected, else false.
		//
		//**************************************
		{
#if VER_DEBUG
			return pinsSelected == pins;
#else
			return false;
#endif
		}

	//****************************************************************************************
	//
	//	Overrides.
	//

		//*********************************************************************************************
		//
		virtual void Process
		(
			const CMessageStep& ms
		);
		//
		//	Runs the AI system for one cycle.
		//
		//**************************************

		//*********************************************************************************************
		//
		void ProcessPending
		(
		);
		//
		//	Processes a step message if one is still pending.
		//
		//**************************************

		//*********************************************************************************************
		//
		virtual void Process(const CMessagePaint& msgpaint);
		//
		//	Draws any AI debugging info.
		//
		//**************************************


		//*****************************************************************************************
		virtual char * pcSave(char *  pc_buffer) const;

		//*****************************************************************************************
		virtual const char * pcLoad(const char *  pc_buffer);



};


// The AI system global.  There can be only one.
extern CAISystem* gpaiSystem;

// A gruesome hack to avoud changing lots of code that might just get changed again later.
#define gaiSystem (*gpaiSystem)

// #ifndef HEADER_GAME_AI_AIMAIN_HPP
#endif
