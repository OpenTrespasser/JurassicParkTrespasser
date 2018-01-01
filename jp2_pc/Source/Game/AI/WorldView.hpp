/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	CWorldView class for AI library.  CMentalState handles an animal's view of the world around
 *	it.	
 *
 * Bugs:
 *
 * To do:
 *		Make the Notice() function find repeats more quickly. 
 *
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/WorldView.hpp                                                 $
 * 
 * 24    9/11/98 12:39p Agrant
 * incremental terrain knowledge rebuilding, rather than wholesale re-querying
 * 
 * 23    8/17/98 6:48p Agrant
 * Improved save functions
 * Made a function to remove all influences
 * 
 * 22    8/11/98 2:17p Agrant
 * moved remove influence implementation to cpp file
 * 
 * 21    5/19/98 9:16p Agrant
 * pathfailure and path obstacle and pathavoider all gone
 * 
 * 20    2/23/98 5:07p Agrant
 * AI save/load
 * 
 * 19    1/13/98 9:34p Agrant
 * Correct hungarian notation
 * 
 * 18    7/14/97 12:54a Agrant
 * Terrain Pathfinding data setup
 * 
 * 17    5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 16    4/29/97 6:42p Agrant
 * CFeeling now float-based instead of CRating-based.
 * Activities now rate/act based on a list of influences.
 * Better rating functions for activities.
 * Debugging tools for activity processing.
 * 
 * 15    4/07/97 4:03p Agrant
 * Closer to True Vision(tm) technology!
 * 
 * 14    3/28/97 2:01p Agrant
 * Brain now points to graph, mentalstate, and worldview
 * Pathfinding more efficient
 * 
 * 13    3/26/97 5:28p Agrant
 * Added number of pathfinding nodes to brain constructor, mentalstate, and worldview.
 * 
 * 12    2/16/97 5:37p Agrant
 * Even better pathfinding.
 * 
 * 11    2/09/97 8:17p Agrant
 * Added pathfinding node graph.
 * Better updates for influences.
 * 
 * 10    1/28/97 6:05p Agrant
 * Intermediate pathfinding revision check in
 * 
 * 9     12/11/96 12:25p Agrant
 * Faster notice function
 * 
 * 8     11/20/96 1:26p Agrant
 * Now using world database queries for perception.
 * Now using archetypes for default personality behaviors.
 * 
 * 7     11/14/96 4:50p Agrant
 * AI subsystem now in tune with the revised object hierarchy
 * 
 * 6     11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 5     10/14/96 5:48p Agrant
 * Changed the influence array into an InfluenceList class to allow changes later.
 * It uses STL syntax.
 * 
 * 4     10/10/96 7:18p Agrant
 * Modified code for code specs.
 * 
 * 3     10/02/96 8:11p Agrant
 * Added Fleeing and Hunting.
 * Added Sheep and Wolves to Test App.
 * Added some debugging hooks.
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

#ifndef HEADER_GAME_AI_WORLDVIEW_HPP
#define HEADER_GAME_AI_WORLDVIEW_HPP

#include "list.h"
#include "Influence.hpp"
#include "Rating.hpp"


class CAIGraph;


enum EQuadSide
{
	eqsBEGIN = 0,
	eqsLEFT = 0,
	eqsRIGHT,
	eqsTOP,
	eqsBOTTOM,
	eqsEND
};





//*********************************************************************************************
//
class CWorldView
//
//	Prefix: wv
//
//	The CWorldView class records a picture of the world as an animal sees it.  The picture 
//  may be inaccurate, but it is all that the animal has to go on.  Individual objects or
//  important events are stored as Influences, which are accessible only through the WordView.
//
//*********************************************************************************************
{



	//*****************************************************************************************
	//
	//	Member Variables
	//
public:
#define				iNUM_INFLUENCES		50		
									// The maximum number of influences in 
									// a WorldView.

	CInfluenceList	inflInfluences;	// A list of influences in the worldview.

//	CAIGraph*		paigGraph;		// A pointer to the animal's connectivity view of the world.


	CAnimal*		paniOwner;		// The dino with this world view.

	TSec			sTimeOfLastTemporaryFlagReset;			
									// When exactly did we reset the temporary flags last time?
		
	list<CTerrainKnowledge>  ltkKnowledge;	
									// Some info about the terrain, as perceived by the animal.

	CVector2<>		v2CenterOfTerrainKnowledge;	
									// The center point of the terrain knowledge.
	CVector2<>		v2TerrainKnowledgeWorldMin;	// The smallest world xy queried to make this knowledge set
	CVector2<>		v2TerrainKnowledgeWorldMax;	// the largest world xy....



	int				iSideMerging;
									// The side of the terrain segment we are trying to merge.

	list<CTerrainKnowledge>::iterator  ptkKnowledgeMerging;	
									// The terrain segment that we are trying to merge.

	
	//*****************************************************************************************
	//
	//	Constructors
	//
public:
	CWorldView
	(
		CAnimal *pani_owner
	);

	CWorldView
	(
	)
	{
		Assert(false);
	}

	~CWorldView();


	//*****************************************************************************************
	//
	//	Member functions
	//
public:

		//*********************************************************************************
		//
		CInfluenceList::iterator iterFindInfluence
		(
			CInstance*	pins_target			// Object to add to world view.
		)
		//
		//	Finds the influence referencing target, if it exists.
		//
		//******************************
		{
			return inflInfluences.iterFindInfluence(pins_target);
		}

		//*********************************************************************************
		//
		void AddInfluence
		(
			CInstance*	pins_target			// Object to add to world view.
		)
		//
		//	Adds the instance to the influence array.  If there are too many influences 
		//	in the array, bumps the least important influence out of the array.
		//
		//******************************
		{
			Assert(0);
			Assert(pins_target);

			// Create an influence.
			CInfluence inf(paniOwner, pins_target);

			// Add the influence.
//			pair<CInstance* const, CInfluence> p(inf.pinsTarget, inf);

			(void)  inflInfluences.insert(inf);
		}

		//*********************************************************************************
		//
		bool bRemoveInfluence
		(
			CInstance*	pins_target			// Target of influence to remove.
		);
		//
		//	Remvoes the instance from the influence array.  
		//
		//	Returns:  true if the target was in the array, false if not found.
		//
		//	Notes:  Only modifies the worldview.
		//
		//******************************
		
		//*********************************************************************************
		//
		void RemoveSomeInfluences
		(
		);
		//
		//	Removes some influences that are not important.
		//
		//	Notes:  Updates the graph if necessary.
		//
		//******************************

		//*********************************************************************************
		//
		void RemoveAllInfluences
		(
		);
		//
		//	Removes all influences.
		//
		//	Notes:  Updates the graph if necessary.
		//
		//******************************

		//*********************************************************************************
		//
		void See
		(
			CInstance*	pins,			// The thing that has been seen.
			TSec		s_current_time	// When it was seen.
		);
		//
		//	Lets the world view know that the dino has actually seen "pins".
		//
		//******************************
		
		//*********************************************************************************
		//
		void Hear
		(
			CInstance*	pins,			// The sound that has been heard.
			TSec		s_current_time	// When it was heard.
		);
		//
		//	Lets the world view know that the dino has actually heard "pins".
		//
		//******************************

		//*********************************************************************************
		//
		const CInfluence* pinfAddOrUpdate
		(
			CInstance*	pins,			// The thing to be added or updated.
			TSec		s_current_time	// When it was updated/added.
		);
		//
		//	Adds pins to the influence list.  Updates if already present.
		//
		//	Returns:
		//		The influence associated with pins if one already exists, else 0.
		//
		//	Notes:
		//		Includes perfect knowledge of "pins" in the update.
		//
		//******************************

		//*********************************************************************************
		//
		const CInfluence* pinfNotice
		(
			CInstance*	pins,			// The thing that has been noticed.
			TSec		s_current_time	// When it was noticed.
		);
		//
		//	Adds pins to the influence list.  Nothing if already present.
		//
		//	Returns:
		//		The influence associated with pins if one already exists, else 0.
		//
		//	Notes:	
		//		Provides perfect knowledge of the added influence.
		//
		//******************************

		//*********************************************************************************
		//
		const CInfluence* pinfInfluenceMoved
		(
			CInstance*	pins,			// The thing that has been moved.
			TSec		s_current_time	// When it was moved.
		);
		//
		//	Updates the data for this object in the influence list, if the object is in the
		//	list.
		//
		//	Returns:
		//		The influence associated with pins if one already exists, else 0.
		//
		//******************************

		//*********************************************************************************
		//
		void MaybeResetTempInfluenceFlags
		(
		);
		//
		//	Resets influence temporary flags if appropriate.
		//
		//******************************

		//*********************************************************************************
		//
		void MaybeMergeTerrain
		(
		);
		//
		//	Tries to merge a few terrain segments to simplify the representation.
		//
		//******************************
		
		//*********************************************************************************
		//
		void ResetMergeTerrain
		(
		);
		//
		//	Initializes the terrain merger.
		//
		//******************************

		//*********************************************************************************
		//
		void ClearTerrainKnowledge();
		//
		//	Clears the terrain knowledge.
		//
		//******************************
		
		//*********************************************************************************
		//
		void ClearOutsideTerrainKnowledge();
		//
		//	Clears any terrain knowledge outside of the official boundaries.
		//
		//******************************

		//*********************************************************************************
		//
		bool bOutsideTerrainKnowledge
		(
			CVector2<> v2_point
		);
		//
		//	True if the point is outside the terrain knowledge extents
		//
		//******************************

#if 0
		//*********************************************************************************
		//
		bool bIsPathValid
		(
			CPath*		ppath,			// The path to be tested.
			CRating		rt_solidity		// The object solidity through which the dino is willing to go
		);
		//
		//	Tests the path to see if the dino can navigate it.
		//
		//	Noyes:
		//		The reason the path failed is placed in the path itself.
		//		If the path is successful, the failure mode is cleared.
		//
		//******************************

		//*********************************************************************************
		//
		bool bIsPathSegmentValid
		(
			CPath*		ppath,			// The path to be tested.
			int			i_seg,			// The segment index in question.
			CRating		rt_solidity		// The object solidity through which the dino is willing to go
		);
		//
		//	Tests the path segment to see if the dino can navigate it.
		//
		//	Notes:
		//		The reason the path failed is placed in the path itself.
		//		If the segment is successful, the failure mode is not cleared.
		//
		//******************************
#endif

		//*****************************************************************************************
		virtual char * pcSave(char *  pc_buffer) const;

		//*****************************************************************************************
		virtual const char * pcLoad(const char *  pc_buffer);
	


};


#endif   // #ifndef HEADER_LIB_AI_WORLDVIEW_HPP
