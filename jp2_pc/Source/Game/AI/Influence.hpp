/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	CInfluence class for AI library.
 *	CInfluenceList class for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *		Change Influence constructor such that it loads data from file rather than assigning
 *		arbitrary constants.  Remove all constants from code.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Influence.hpp                                                 $
 * 
 * 40    9/12/98 1:08a Agrant
 * The new living flag for influences
 * 
 * 39    9/08/98 7:41p Agrant
 * better facing away implementation
 * 
 * 38    9/08/98 4:43p Agrant
 * facing away query
 * 
 * 37    8/25/98 10:52a Rvande
 * Const-conversion cast added for MW build
 * 
 * 36    8/23/98 3:37a Agrant
 * allow default constructor
 * 
 * 35    8/20/98 11:06p Agrant
 * ignore depends somewhat on individual influences rather than just general frustration
 * 
 * 34    8/17/98 6:49p Agrant
 * save functions
 * 
 * 33    8/15/98 6:04p Mmouni
 * Fixed uninitialized const member.
 * 
 * 32    8/13/98 6:56p Agrant
 * code to allow dinos to ignore influences that are unattainable
 * 
 * 31    7/14/98 9:43p Agrant
 * many pathfinding improvements
 * 
 * 30    7/09/98 9:26p Agrant
 * added IS_DISCARDABLE flag
 * removed CContainer from influence list 
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_INFLUENCE_HPP
#define HEADER_LIB_AI_INFLUENCE_HPP

#include "NodeSource.hpp"

#include "Lib\Transform\Vector.hpp"
//#include "Lib/EntityDBase/Animal.hpp"
#include "Lib/Sys/Timer.hpp"

#include <set>
#include "Lib/EntityDBase/Container.hpp"

#include "Classes.hpp"
#include "Feeling.hpp"
//#include "PathObstacle.hpp"
#include "Silhouette.hpp"
#include "Lib/Std/Set.hpp"

class CInstance;
class CAnimal;

enum EInfluenceFlag
//	Prefix: eif
//  Indexes the flags found in an influence.
{
	eifIS_KNOWN,	// True when the influence target is known in detail by the dino.
	eifIS_VISIBLE,	// True when the influence can be seen by the dino.		TEMPORARY.
	eifIS_DISCARDABLE, // True when the influence should be eliminated.
	eifIGNORE,		// true when the animal should ignore this influence for rating purposes.
	eifFACING_AWAY,	// true when target is facing away from animal, else false.
	eifLIVING,		// true when target is alive or part of a living thing.

	eifEND
};

class CAIGraphNode;
class CBrain;
template<class DISTANCE> class CLineSegment2;


// The distance beyond which influence have less, well, influence.
#define rINFLUENCE_FALLOFF_DISTANCE 10.0


//*********************************************************************************************
//
class CInfluence : public CNodeSource
//
//	Prefix: inf
//
//	The CInfluence class is a conceptual tool used by an animal's Brain to track important 
//  events, objects, creatures, and perhaps even memories in the world.
//
//	Notes:
//		One created and placed in an influence list, pinsTarget MAY NOT BE MODIFIED!
//		(*pinsTarget) may be modified, but the pointer value itself may not be.
//		This is because the list uses '<' which is defined by the target pointer value.
//
//
//*********************************************************************************************
{

//
//  Variable declarations
//

public:
	CVector3<>			v3Location;			// The location of the influence in world coords.
	CVector3<>			v3ToTarget;			// Vector from beholder to target.
	TReal				rDistanceTo;		// The distance between target and animal.
											// Set by the brain before rating and acting begin.
	TReal				rDistanceToInverted;// 1 / rDistanceTo
	TReal				rMinZ;				// The lowest the object drops, relative to the ground.
	TReal				rMaxZ;				// The highest the object rises, relative to the ground.

	TSec				sLastSeen;			// The time at which the influence was last accurately
											// pinpointed.
	
	CFeeling			feelAboutTarget;	// The emotional baggage associated with the influence.
	
	CInstance* const	pinsTarget;			// The object associated with this influence
											// or 0 if none.

	CSet<EInfluenceFlag>setFlags;			// A bunch of flags for an influence.

//	CFeeling			feelDBGWeighted;	// The emotional baggage associated with the influence,

//	CMSArray<char,16>	sacDebugString;		// Holds debug info to be displayed with object.

	TReal				rImportance;		// Higher values indicate influences that ought not be forgotten.

	TSec				sIgnoreTimeOut;		// When are we allowed to toggle the ignore bit?
											// While ignoring, stop ignoring at the time out.
											// While not ignoring, you can only start at the time out.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

//	CInfluence
//	(
//	)
//	{}

	CInfluence
	(
		CAnimal*	pani_self,		// The object noting the influence.
		CInstance*	pins_target		// The target associated with the influence.
	);
	//	Notes:
	//		Creates the correct opinion feeling for this target/observer pair.
	//


	// This constructor only used to construct fake CInfluences used to determine if the target is
	// already present in an InfluenceList
	CInfluence
	(
		CInstance*	pins_target		// The target associated with the influence.
	) : pinsTarget(pins_target)
	{
	}
	
	CInfluence
	(
	) : pinsTarget(0)
	{
		// Allow this for the load function!
	}

	//lint -save -e1540
	~CInfluence
	(
	)
	{
	};
	//lint -restore

	//*****************************************************************************************
	//
	//	Member functions
	//

		//*********************************************************************************
		//
		bool bFacingAway
		(
		) const;
		//
		//	True when the influence is facing away from the animal.
		//
		//******************************

		//*********************************************************************************
		//
		bool bLiving
		(
		) const
		//
		//	True when the influence is alive.
		//
		//******************************
		{
			return setFlags[eifLIVING];
		}

		//*********************************************************************************
		//
		CVector3<> v3ClosestPointTo
		(
			const CVector3<>&	v3_origin
		) const;
		//
		//	returns point on the border of "this" that is closest to v3_origin.
		//
		//******************************
			
		//*********************************************************************************
		//
		void UpdateTo
		(
			const CVector3<>&	v3_observer_location	// Where is the observer?
		);
		//
		//	Updates rDistanceToTarget and v2ToTarget.
		//
		//******************************

		//*********************************************************************************
		//
		void UpdatePositionalData
		(
			CAnimal* pani		// Animal to which this influence belongs.
		);
		//
		//	Updates all positional data for the influence.
		//
		//******************************

		//*********************************************************************************
		//
		void UpdateForMove
		(
			CAnimal* pani,		// Animal to which this influence belongs.
			TSec	 s_when		// When the move happened.
		);
		//
		//	Updates the influence for movement of the target.
		//
		//	Notes:  Does not modify list sorting data.
		//
		//******************************
		
		//*********************************************************************************
		//
		virtual int iAddToGraph
		(
			CBrain* pbr	// Brain to which this influence belongs.
		);
		//
		//	Adds nodes to the pathfinding graph based on shape of the target.
		//
		//******************************

		//*********************************************************************************
		//
		int iAddNodeLine
		(
			CVector3<> v3_start,		// Endpoint
			CVector3<> v3_stop,			// Endpoint
			TReal r_max_step,			// Max distance between nodes
			CBrain* pbr,				// The brain of the animal 
			const CAIGraphNode& aign,	// The node to copy and use for submission.
			bool b_endpoints = true		// True if we add the endpoints.
		);
		//
		// Adds nodes in a line to the pathfinding graph.
		//
		//	Notes:
		//		Includes the endpoints.
		//		No node is more than r_max_step from its neighbors.
		//
		//******************************

		//*********************************************************************************
		//
		void CalculateImportance
		(
		);
		//
		//	Decided how important the influence is (rImportance).
		//
		//******************************

		//*********************************************************************************
		//
		virtual void CalculateNodeSuitability
		(
		);
		//
		//	Decides how important the influence is for node construction (pathfinding).
		//
		//******************************

		//*********************************************************************************
		//
		virtual TReal rRateNodeSuitability
		(
		) const;
		//
		//	Decided how important the influence is right now (based on rNodeSuitability).
		//
		//******************************

		//******************************************************************************************
		//
		void AddDebug
		(
			char //c
		)
		//
		//	Adds the character to the debug string.
		//
		//******************************
		{
			Assert(false);
//			sacDebugString << c;
		}

		//*********************************************************************************
		//
		void ResetTemporaryFlags
		(
		);
		//
		//	Resets the flags which are short-lived.
		//
		//	Notes:
		//		Short-lived flags are flags that are reset each AI cycle, before a new
		//		round of sensory input.  Examples:  eifIS_VISIBLE, eifBLOCKS_PATH
		//
		//******************************

		//*********************************************************************************
		//
		void Ignore
		(
			bool b_ignore
		);
		//
		//	Sets the IGNORE flag.
		//
		//******************************

		//*********************************************************************************
		//
		bool bBlocksLine
		(
			CLineSegment2<> *pls, 
			CBrain* pbr
		) const;
		//
		//	Resets the flags which are short-lived.
		//
		//	Notes:
		//		Short-lived flags are flags that are reset each AI cycle, before a new
		//		round of sensory input.  Examples:  eifIS_VISIBLE, eifBLOCKS_PATH
		//
		//******************************

		//*****************************************************************************************
		virtual char * pcSave(char *  pc_buffer) const;

		//*****************************************************************************************
		virtual const char * pcLoad(const char *  pc_buffer);
	

		operator< (const CInfluence& inf) const
		{
			return pinsTarget < inf.pinsTarget;
		}


		//  HACK HACK HACK
		//  This is much of a badness.
		//  STL insists that an item in a map be const, for very good reason.
		//	However, since the less than function for CInfluence is dependent only
		//  on pinsTarget, and pinsTarget cannot change over the life of the influence,
		//  I am going to circumvent STL by making it really easy to convert const
		//  influences to non-const influences.  It's okay, really!

//		operator CInfluence&() const
//		{
//			return (CInfluence)*this;
//		}

};



//*********************************************************************************************
//
class CInfluenceList : public std::set<CInfluence, std::less<CInfluence> >
//
//	Prefix: infl
//
//	The CInfluenceList class is group of influences.
//
//	Notes:
//		The CInfluenceList class exists as a wrapper for an STL container class.
//		We want a wrapper to shield the rest of the AI system from changes in the
//		wrapper.
//
//		The CInfluenceList class can be used in pretty much the same way any STL class might
//		be used.  The following STL thingies are "approved" and are not likely to change
//		even if the base STL class changes:
//			begin()						// first 
//			end()						// last
//			iterator					// temp variable
//			push_back( CInfluence& );	// add to list
//			erase( iterator );			// remove from list
//
//		The wrapper may someday be called upon to:
//			Optimize out unimportant influences.
//			Keep the number of influences below a certain value.
//
{

//
//  Variable declarations
//

public:

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	//*****************************************************************************************
	//
	//	Member functions
	//

	
		//*********************************************************************************
		//
		iterator iterFindInfluence
		(
			CInstance*	pins_target			// Object to add to world view.
		) const
		//
		//	Returns the influence referencing the target, or end() if not found.
		//
		//******************************
		{
			// Make a fake CInfluence for lookup purposes.
			CInfluence inf(pins_target);

			// See if it's already here.
			return (const_cast<CInfluenceList*>(this))->find(inf);
		}

};





// #ifndef HEADER_LIB_AI_INFLUENCE_HPP
#endif
