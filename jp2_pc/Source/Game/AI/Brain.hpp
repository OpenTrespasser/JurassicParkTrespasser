/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CBrain class for AI library.  CBrain handles all processing and data
 *		for an individual dino.
 *		
 *
 * Bugs:
 *
 * To do:
 *		Get pathfind start/end data from the command event instead of the synthesizer direct.
 *		Set up a file or resource system for loading/saving custom brains.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Brain.hpp                                                     $
 * 
 * 71    9/11/98 12:42p Agrant
 * differentiate between slopes too steep to climb and too steep to descend
 * 
 * 70    9/10/98 12:01p Agrant
 * moved physics commands to cpp file
 * no move commands after death
 * only send jump once
 * 
 * 69    9/08/98 7:37p Agrant
 * delay visual wakeup by using a flag
 * 
 * 68    9/05/98 4:38p Agrant
 * flag to enable/disable terrain pathfinding
 * 
 * 67    9/02/98 5:22p Agrant
 * query function to find out if we are vocalizing
 * 
 * 66    8/26/98 7:13p Agrant
 * Disable tail wag commands
 * 
 * 65    8/26/98 11:15a Agrant
 * jump flags
 * 
 * 64    8/22/98 7:15p Agrant
 * added unstartle
 * 
 * 63    8/20/98 11:38p Agrant
 * added capacity for being startled to dinos
 * 
 * 62    8/17/98 6:49p Agrant
 * activate/deactivate functions
 * 
 * 61    8/14/98 11:35a Agrant
 * record a bool telling us if we have anything to celebrate
 * 
 * 60    8/13/98 6:56p Agrant
 * code to allow dinos to ignore influences that are unattainable
 * 
 * 59    7/29/98 3:08p Agrant
 * moved team to CAnimate
 * 
 * 58    7/23/98 9:35p Agrant
 * allow data control of mouth open/close during vocals
 * 
 * 57    7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 56    7/14/98 4:12p Agrant
 * brains can draw AI influences in 3d
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_BRAIN_HPP
#define HEADER_LIB_AI_BRAIN_HPP


#include "Lib\Sys\Textout.hpp"
//#include "MentalState.hpp"
//#include "Activity.hpp"
#include "ActivityEnum.hpp"
#include "Graph.hpp"
//#include "Lib/EntityDBase/Animal.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPhysicsReq.hpp"
#include "AITypes.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Feeling.hpp"

class CAnimal;
class CCamera;
class CDraw;
class CPlane;
class CMentalState;
class CWorldView;
class CAIGraph;
class CMessageMove;
class CMessageDelete;
class CInfluence;
class CInfluenceList;
class CActivity;
class CActivityCompound;
class CActivityDOSubBrain;
class CPerception;
class CSynthesizer;
namespace NMultiResolution {

class CTriangleQuery;
};

//*********************************************************************************************
//
class CBrain
//
//	Prefix: brain
//
//	Handles everything necessary for a single animal.
//
//	Notes:
//		The CBrain class is an object associated with a dinosaur.  It does all of the 
//		thinking for that dinosaur, and records all state necessary for that decision
//		making process.
//	
//		Eventually, the brain will also handle some optimizations, such as sleeping AI's.
//
//*********************************************************************************************
{

//  Variable declarations
public:


	//
	//  Parts of the brain.
	//
	CMentalState*			pmsState;	// All state for this 
										// dinosaur.
										// The MentalState is unique to this 
										// dinosaur.

	CWorldView*				pwvWorldView;// The dino's image of the world.

	CAIGraph*				paigGraph;	// The pathfinding graph used by this dino.
	
	CActivityCompound *		pcactSelf;	// The Activity that handles all 
										// actions not 
										// requiring a direct object.

	CSArray<CActivity*,eatEND>  sapactActivities;	// A master list of all activities in the AI model.

 	CActivityDOSubBrain *   apasbSubBrain[esbtEND];	
										// An array of DOActivities that deal 
										// with specific types of objects.  
										// Most will point to the generic
										// direct object Actvity.  
										// Activities are indexed by
										// the type of the object with 
										// which they deal.

 	CPerception *			apmodPerceptions[epctEND];	
										// An array of all perceptions that 
										// influence the animal.
										// Indexed by perceptin type, and used
										// only when a
										// particular modifier needs to fire.

	CSynthesizer*			psynSynthesizer;
										// The synthesizer used by this brain.

	bool					bLastPathSucceeded;
										// True when the last pathfinding attempt worked.									
	TSec					sLastSuccessfulPath;
										// The timestamp of the last successful pathfinding attempt.


	//
	//  State variable.  Evil little things that are here because the high concept AI can't handle some details.
	//

	////  WAYPOINTS-
	//  Basically, if the dino hangs out at the same waypoint for too long, he starts making some AI decisions 
	//  to change the way he's acting.
	CVector3<>				v3LastWayPoint;
										// Location of the last "waypoint"
	TSec					sLastWayPoint;
										// Timestamp of placing last waypoint.
	TReal					rWayPointDistance;
										// Distance between waypoints.
	TSec					sPatience;
										// Time that we are willing to wait between waypoints.

	bool					bTriumph;	// True when the animal has killed, but not yet expressed its joy.
	bool					bStartled;	// True when the animal has been startled, but not yet expressed its startlement.
	TSec					sAllowStartle;  // Time when we can be startled again.
	CVector3<>				v3Startle;	// The world position of the cause of our consternation.

	bool					bJumpEnabled;	// True if the dino, in general, can jump.  (Mirrors activity Jump)
	bool					bCanJump;		// True if the dino can jump RIGHT NOW!

	//
	//	The animal.
	//

	CAnimal*				paniOwner;	//	Reference to the parent animal.



	//
	//	Data.
	//

	TSec					sLastTime;	// When did we last think?
	TSec					sElapsedTime;  // How long has it been since we last thought?  (valid only in a Think() call)
	
	CMessagePhysicsReq		msgprPhysicsControl;
										// The message used by the brain to communicate with
										// the physics system.

	char					acName[64];	// The animal's name.

	EArchetype				earArchetype;
										// The animal's general personality type, used to get 
										// appropriate defaults.

	CRating					rtBravery;	// How brave am I?   0-1.
	bool					bOverrideHumanFeeling;  // true when we have a particular notion about humans
	CFeeling				feelHumanFeeling;		// The particular way we feel about humans
	CFeeling				feelDamageFeeling;		// The particular way we react to damage

	// Physical stats
	TReal					rWidth;		// The animal's conception of its own girth.
	TReal					rHeight;	// The animal's conception of its own tallness.
	TReal					rHeadReach;	// How far from the pelvis the head can extend.
	TReal					rTailReach;	// How far from the pelvis the tail can extend.
	TReal					rClawReach;	// How far from the pelvis a foreclaw can extend.

	TReal					rJumpDistanceSqr;	// Horizontal jump distance squared.
	TReal					rJumpDistance;		// Horizontal jump distance.
	TReal					rJumpUp;			// High jump.
	TReal					rJumpDown;			// Down jump.  (drop down)

	TReal					rWalkOver;		// Can step over things this small
	TReal					rWalkUnder;		// Can walk under things this high
	TReal					rJumpOver;		// Can jump things this small
	TReal					rCrouchUnder;	// Can crouch under things this high
	TReal					rMoveableMass;	// Can move things this light

	TReal					rMinDownHillNormalZ;	// Dino can go down slopes with normal.tZ greater than this
	TReal					rMinUpHillNormalZ;		// Dino can go  up  slopes with normal.tZ greater than this

	EDinoType				edtDinoType;	// What kind of dino am I, physically?

	//
	//	Vocalization data.
	//
	int						iVocalizingActivity;	// Index of the activity that owns this vocalization, -1 if none.
	TSec					sSampleLength;	//  The length of the sample being played, in seconds.
	TSec					sSampleStart;	//  When we started the sound.
	TSec					sOpenMouth;		//  When we should open the mouth
	TSec					sCloseMouth;	//  When we should close the mouth
	uint32					u4AudioID;	// The ID of the vocalization currently being made by the dino.
										//  Zero if none.

	//
	//  Together, these variables determine the dino's pathfinding cleverness.
	//

	TReal					rNodesPerSec;
										// The number of nodes a dino is allowed to add to its
										// pathfinding graph each second.
	int						iMaxNodes;	// The maximum number of nodes allowed in the pathfinding graph.
	TSec					sWhenToLearn;
										// The next moment the animal is allowed to learn new nodes.
	TSec					sTimeToForget;
										// The amount of time required to forget a node
										// with a time rating of 1.  HIgher time ratings
										// are forgotten sooner.

	TReal					rAnimateSensoryRange;
	TReal					rObjectSensoryRange;
	TReal					rTerrainSensoryRange;
										// The distance at which the animal senses things.

	bool					bUseTerrainPathfinding;	// True if the animal cares about terrain.

	int						iMaxAStarSteps;	// The number of AStar trials before we give up.
	TReal					rMaxPathLength;
										// The path distance beyond which the animal gives up on pathfinding.

	TSec					sForgetInfluence;
										// How long before an influence is forgotten.

	bool					bWakeMe;		// True when we should try harder to wake him up.
	TReal					rWakeDistance;	// How far away to wake the dino up?
	TReal					rSleepDistance;	// How far away to put the dino sleep?

	// Test data!
	CGraphPath			gpTestPath;		// The last path found by a pathfinder.



	//  Member function definitions
public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//


	CBrain()
	{
		Assert(false);
	};
	
	
	CBrain
	(
		CAnimal*		pet_owner,
		EArchetype		ear,
		const char*		pc_name
	);

	// The basic CInstance constructor for loading from a GROFF file.
	CBrain
	(
		CAnimal*				pani_owner,	// The animal that has this brain.
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);


	~CBrain();

	//*****************************************************************************************
	//
	//	Member functions.
	//
	

		//*********************************************************************************
		//
		void InitDefaults
		(
		);
		//
		//	Sets up default data common to all CBrain constructors.
		//
		//******************************

		//*********************************************************************************
		//
		void InitDependents
		(
		);
		//
		//	Sets up data common to all CBrain constructors that depends on variable parameters.
		//
		//******************************

		//*********************************************************************************
		//
		void CreateActivities
		(
		);
		//
		//	Sets up the master activity list.
		//
		//	Side effects:
		//		Allocates memory.
		//
		//******************************

		//*********************************************************************************
		//
		void InitDOSubBrains
		(
		);
		//
		//	Sets up the DOSubBrains.
		//
		//	Side effects:
		//		Allocates memory.
		//
		//******************************
	
		//*********************************************************************************
		//
		void InitSelfSubBrain
		(
		);
		//
		//	Sets up the self-evaluating SubBrain.
		//
		//	Side effects:
		//		Allocates memory.
		//
		//******************************

		//****************************************************************************************
		//
		int iGetActivityIndex
		(
			const CActivity* pact
		) const;
		//
		//	Searches the activity array for pact and returns its index.
		//
		//******************************

		//*********************************************************************************
		//
		void SetArchetype
		(
			EArchetype ear
		);
		//
		//	Sets the activity set to the default for the given archetype.
		//
		//******************************
		
		//*********************************************************************************
		//
		void Activate
		(
		);
		//
		//	Sets up the brain for actual thinking.
		//
		//******************************

		//*********************************************************************************
		//
		void Deactivate
		(
		);
		//
		//	Prepares the brain for sleeping.
		//
		//******************************

		//*********************************************************************************
		//
		void Think
		(
		);
		//
		//	Causes the brain to ponder its options and direct its animal object.
		//
		//
		//	Side effects:
		//		Sends events to change animal object behavior.
		//		Modifies mstate.
		//
		//	Globals:
		//		World database queries.
		//
		//******************************


		//****************************************************************************************
		//
		void LookAtTerrain();
		//
		//	Causes the brain to examine the terrain around it for significant events.
		//
		//******************************

		//****************************************************************************************
		//
		void Look
		(
		);
		//
		//	Causes the brain to examine the world around it for significant events.
		//
		//******************************
	
		//****************************************************************************************
		//	
		void Analyze
		(
		);
		//
		//  Analyzes the situation and proposes actions to the synthesizer.
		//
		//	Notes:
		//		Causes the brain to use its sub-brains to determine all possible courses of action
		//		and rate their validity.  Puts commands into the command queue.
		//
		//******************************
	
		//****************************************************************************************
		//	
		void AnalyzeInfluence
		(
			CInfluence* pinf
		);
		//
		//  Analyzes influence and proposes actions to the synthesizer.
		//
		//	Notes:
		//		Causes the brain to use its sub-brains to determine all possible courses of action
		//		and rate their validity.  Puts commands into the command queue.
		//
		//******************************
	
		//****************************************************************************************
		//
		void Synthesize
		(
		);
		//
		//	Synthesizes all subcommands into a coherent whole.
		//
		//	Notes:
		//		Causes the brain to combine all commands issued in the Analyze function into a 
		//		coherent set of commands.  Passes the finalized commands on to the Physics system.
		//
		//	Side effects:
		//		Issues events.
		//
		//******************************

		//****************************************************************************************
		//
		void Pathfind
		(
		);
		//
		//	Make sure that obstacles are avoided as appropriate.
		//
		//	Notes:
		//		Uses data internal to Synthesizer and Mental State.
		//
		//******************************

		//****************************************************************************************
		//
		bool bTooSteepUpHill
		(
			const CPlane& pl
		);
		//
		//	Returns true if the plane is too steep to stand on.
		//
		//******************************

		//****************************************************************************************
		//
		bool bTooSteepUpHill
		(
			const NMultiResolution::CTriangleQuery* ptriquery
		);
		//
		//	Returns true if the triangle query is too steep to stand on.
		//
		//******************************

		//****************************************************************************************
		//
		bool bTooSteepDownHill
		(
			const CPlane& pl
		);
		//
		//	Returns true if the plane is too steep to stand on.
		//
		//******************************

		//****************************************************************************************
		//
		bool bTooSteepDownHill
		(
			const NMultiResolution::CTriangleQuery* ptriquery
		);
		//
		//	Returns true if the triangle query is too steep to stand on.
		//
		//******************************

		//****************************************************************************************
		//
		bool bFrustrated
		(
		) const;
		//
		//	Returns true if the dino is having a hard time.
		//
		//******************************

		//****************************************************************************************
		//
		bool bIsVocalizing
		(
		) const;
		//
		//	Returns true if the dino is currently making a sound.
		//
		//******************************

		//****************************************************************************************
		//
		void Startle
		(
			const CVector3<>& v3_startle_location
		);
		//
		//	Try to startle the animal.  Only fails if we've been startled too recently.
		//
		//******************************

		//****************************************************************************************
		//
		void UnStartle
		(
			TSec s_calm_time		// Time until we can become startled again,
		);
		//
		//******************************

		//****************************************************************************************
		//
		void SetWayPoint
		(
		);
		//
		//	Sets up a new waypoint to gauge frustration.
		//
		//******************************


		//*****************************************************************************************
	//
	//	Physics interface functions.
	//

		//*********************************************************************************
		//
		void TellPhysicsMoveTo
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CVector2<>&	v2_location,	// Target location.
			const CRating		rt_speed		// Speed- 1 max, 0 zero.
		);
		//
		//	Stuffs the physics message to tell the animal body to try to go 
		//  to "v2_location" with an urgency of "rt_importance."
		//
		//******************************

		//*********************************************************************************
		//
		void TellPhysicsOpenMouth
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CRating		rt_open_amount,	// Degree to which the mouth should be open.
			const CRating		rt_speed		// Speed- 1 max, 0 zero.
		);
		//
		//	Stuffs the physics message to tell the animal body to try to go 
		//  to "v2_location" with an urgency of "rt_importance."
		//
		//******************************

		//*********************************************************************************
		//
		void TellPhysicsMoveHeadTo
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CVector3<>&	v3_location,	// Target location.
			const CRating		rt_speed		// Speed- 1 max, 0 zero.
		);
		//
		//	Stuffs the physics message to tell the animal head to try to go 
		//  to "v3_location" with an urgency of "rt_importance."
		//
		//******************************

		//*********************************************************************************
		//
		void TellPhysicsCockHead
		(
			const CRating		rt_importance,	// Urgency of the order.
			float				f_angle,		// Target angle in radians.
			const CRating		rt_speed		// Speed- 1 max, 0 zero.
		);
		//
		//	Stuffs the physics message to tell the animal head to try tilt
		//  to "f_angle" with an urgency of "rt_importance."
		//
		//******************************
		
		//*********************************************************************************
		//
		void TellPhysicsPointHead
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CVector3<>&	v3_location,	// Target location.
			const CRating		rt_speed		// Speed- 1 max, 0 zero.
		);
		//
		//	Stuffs the physics message to tell the animal head to try to go 
		//  to "v3_location" with an urgency of "rt_importance."
		//
		//******************************
		
		//*********************************************************************************
		//
		void TellPhysicsWagTail
		(
			const CRating		rt_importance,	// Urgency of the order.
			float				f_amplitude,		// Target angle in radians.
			float				f_frequency		// Speed- 1 max, 0 zero.
		);
		//
		//	Stuffs the physics message to tell the animal head to try tilt
		//  to "f_angle" with an urgency of "rt_importance."
		//
		//******************************
		
		//*********************************************************************************
		//
		void TellPhysicsCrouch
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CRating		rt_speed
		);
		//
		//******************************
		
		//*********************************************************************************
		//
		void TellPhysicsJump
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CRating		rt_speed
		);
		//
		//******************************
		
		//*********************************************************************************
		//
		void SendPhysicsCommandMessage
		(
		)
		//
		//	Sends the entire command composed thus far by the synthesizer.
		//
		//	To do:
		//		Sends a physics event.  No other functions do this.
		//
		//******************************
		{
			msgprPhysicsControl.Dispatch();

			// HACK HACK HACK-  only send a jump request once.
			msgprPhysicsControl.subJump.Reset();
		}

		//*********************************************************************************
		//
		const CVector2<> v2GetLocation
		(
		) const;
		//
		//	Returns world coordinate position of animal associated with brain.
		//
		//******************************

		//*********************************************************************************
		//
		const CVector2<>	v2GetDestination
		(
		) const;
		//
		//	Returns world coordinate position of the destination last decided upon by the
		//	animal associated with brain.
		//
		//******************************

//
//		Test & Debug Member Functions
//

#if VER_TEST
		//*****************************************************************************************
		//
		virtual int iGetDescription(char *buffer, int i_buffer_length);
		//
		//  A human-readable string describing the instance.
		//
		//	Returns:
		//		The number of bytes used of buffer.
		//
		//**************************
#endif
		//****************************************************************************************
		//
		void DrawInfluences
		(
		);
		//
		//	Draws the influences in the world view for the user.
		//
		//******************************

		//****************************************************************************************
		//
		void Draw3DInfluences
		(
			CDraw& draw, 
			CCamera& cam
		);
		//
		//	Draws the influences in the world view for the user.
		//
		//******************************

		//****************************************************************************************
		//
		void DrawGraph
		(
		);
		//
		//	Draws the pathfinding graph.
		//
		//******************************

		//****************************************************************************************
		//
		void DrawDebug
		(
		);
		//
		//	Draws any active debugging info associated with the brain.
		//
		//******************************

		//****************************************************************************************
		//
		void DrawDestination
		(
		);
		//
		//	Draws a line from the brain's animal to its current destination.
		//
		//******************************

		//****************************************************************************************
		//
		void DebugSay
		(
			const char*  ac		// what to say
		);
		//
		//	Causes the animal to print "ac" preceded by its name.
		//
		//******************************

		//****************************************************************************************
		//
		void MaybeDebugSay
		(
			const char*  ac		// what to say
		);
		//
		//	Causes the animal to print "ac" preceded by its name.
		//
		//******************************

		//*****************************************************************************************
		virtual char * pcSave(char *  pc_buffer) const;

		//*****************************************************************************************
		virtual const char * pcLoad(const char *  pc_buffer);


	//*****************************************************************************************
	//
	//	Accessor functions.  These functions exist to set and get member variables.
	//

		//****************************************************************************************
		//
		void AddActivity 
		(
			const ESubBrainType	esbt,		// Add to this sub-brain.
			CActivity*			pact		// Add this activity.
		);
		//
		//	Adds the specified activity to the specified sub-brain.
		//
		//	Side effects:
		//		Modifies sub-brain.
		//		Allocates a new sub-brain if the required sub-brain does not exist.
		//
		//******************************


		//****************************************************************************************
		//
		void AddObjectlessActivity
		(
			CActivity*		pact		// Add this activity.
		);
		//
		//	Adds the specified activity to the objectless sub-brain.
		//
		//	Side effects:
		//		Modifies sub-brain.
		//		Allocates a objectless sub-brain if it does not exist.
		//
		//******************************

		//****************************************************************************************
		//
		CInfluenceList* inflGetInfluences
		(
		);
		//
		//	Gets the influences affecting this Brain's animal.
		//
		//	Returns:
		//		An array of influences.
		//
		//******************************

		//****************************************************************************************
		//
		//   Message processing from the parent animal.
		//
		void HandleMessage
		(
			const CMessageCollision& msgcoll
		);

		void HandleMessage
		(
			const CMessageMove& msgmv
		);

		void HandleMessage
		(
			const CMessageDelete& msgdel
		);

		void HandleMessage
		(
			const CMessageDeath& msgdeath
		);

		//****************************************************************************************
		//
		void HandleDamage(float f_damage, const CInstance* pins_aggressor = 0);
		//
		//	Updates brain for taking hit points of damage.
		//
		//********************************

};


#endif   // #ifndef HEADER_LIB_AI_BRAIN_HPP
