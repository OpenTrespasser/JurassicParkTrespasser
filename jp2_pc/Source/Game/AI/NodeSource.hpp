/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	CNodeSource
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/NodeSource.hpp                                                $
 * 
 * 9     9/19/98 1:42a Agrant
 * added the node_me flag to increase the likelihood of noding important things
 * 
 * 8     9/11/98 12:40p Agrant
 * Revamped terrain knowledge for slope info
 * 
 * 7     9/02/98 5:21p Agrant
 * Simplification support.
 * Better node positioning
 * 
 * 6     8/26/98 3:17a Agrant
 * added the BLOCKED_PATH flag
 * 
 * 5     8/11/98 7:42p Agrant
 * function to allow resetting silhouettes
 * 
 * 4     8/11/98 2:19p Agrant
 * much hacking to keep nodes current with influences
 * 
 * 3     3/12/98 7:54p Agrant
 * New silhouette internals (array instead of vector)
 * 2d pathfinding handles compound physics objects
 * 
 * 2     7/14/97 12:56a Agrant
 * New Base class for all pathfinding obstacles.
 * 
 * 1     7/13/97 8:41p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_NODESOURCE_HPP
#define HEADER_LIB_AI_NODESOURCE_HPP


#include "Silhouette.hpp"
#include "Lib/Std/Set.hpp"
#include "Lib/Sys/Timer.hpp"

class CBrain;

enum ENodeSourceFlag
//  prefix:  ensf
{
	ensfIN_GRAPH,		// True when the NS is represented in the pathfinding graph.
	ensfBLOCKS_PATH,	// True when the NS recently blocked an intersection test.   TEMPORARY.
	ensfIS_PASSABLE,	// True when the NS can be walked through.   NOT TEMPORARY.
	ensfBLOCKED_PATH,	// True if the NS ever blocked a path in its lifetime.  NOT TEMPORARY.
	ensfCAN_SIMPLIFY,	// True when the NS silhouettes can possibly be simplified.  NOT TEMPORARY.
	ensfNODE_ME,		// True if someone has requested that this guy get nodes.
	ensfEND
};

// Duplicate definition found in PhysicsInfoCompound.hpp-  number of submodels allowable for a physics object
#define iMAX_SUBMODELS 10


//*********************************************************************************************
//
class CNodeSource
//
//	Prefix: ns
//
//	The CNodeSource class is a base class for all "things" that an AI might want to use to 
//	construct pathfinding nodes.  Influences and terrain polygons inherit from CNodeSource.
//
//*********************************************************************************************
{

//
//  Variable declarations
//

public:

	CSet<ENodeSourceFlag>  setNodeFlags;	// A bunch of flags for node management.

	TSec				sLastUsedInPath;	// When the nodesource last helped in pathfinding.

	TReal				rNodeSuitability;	// Higher values indicate that the influence is a better

	char				cNodeCount;			// Number of nodes referencing this source.
private:
	//	CSilhouette			silSilhouette;		// The 2d representation of the influence target,
//											// used for pathfinding.
	CMSArray<CSilhouette, iMAX_SUBMODELS>	asilSilhouettes;

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	CNodeSource();

	//*****************************************************************************************
	//
	//	Member functions
	//

		//*********************************************************************************
		//
		void Simplify();
		//
		//	Merges silhouettes if possible.  Uses ensfCAN_SIMPLIFY flag.
		//
		//******************************

		//*********************************************************************************
		//
		void ClearSilhouettes
		(
		)
		//
		//	Clears all silhouettes of the node source.
		//
		//******************************
		{
			asilSilhouettes.uLen = 0;
		}

		//*********************************************************************************
		//
		const CSilhouette* psilGetSilhouette 
		(
			int i_index
		) const
		//
		//	Gets a pointer to the indexed silhouette.
		//
		//******************************
		{
			return &asilSilhouettes[i_index];
		}

		//*********************************************************************************
		//
		CSilhouette* psilGetSilhouette 
		(
			int i_index
		)
		//
		//	Gets a pointer to the indexed silhouette.
		//
		//******************************
		{
			return &asilSilhouettes[i_index];
		}

		//*********************************************************************************
		//
		void MakeSilhouette 
		(
			int i_index
		)
		//
		//	Sets up and clears a silhouette in position "i_index"
		//
		//******************************
		{
			Assert(i_index <= asilSilhouettes.uLen);
			if (i_index == asilSilhouettes.uLen)
			{
				asilSilhouettes.uLen++;
				asilSilhouettes[i_index].Clear();
			}
		}

		//*********************************************************************************
		//
		int iNumSilhouettes
		(
		) const
		//
		//	Gets the number of silhouettes in the NodeSource
		//
		//******************************
		{
			return asilSilhouettes.uLen;
		}

		//*********************************************************************************
		//
		int iNumNodes
		(
		) const;
		//
		//	Gets the number nodes this source provides.
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
		void WasUsedInPath
		(
			TSec s_when		// The time of the use in a path.
		)
		//
		//	Updates the time of last use in a path.
		//
		//******************************
		{
			sLastUsedInPath = s_when;
		}

		//******************************************************************************************
		//
		bool bEdgeIntersects
		(
			const CLineSegment2<>& ls
		) const;
		//
		// Returns true if an edge of the influence's silhouette intersects the given line segment.
		//
		//	Notes:  Modifies setFlags.  Declared const to help out CInfluenceList.
		//
		//**************************************

		//*********************************************************************************
		//
		virtual void ResetTemporaryFlags
		(
		)
		//
		//	Resets the flags which are short-lived.
		//
		//	Notes:
		//		Short-lived flags are flags that are reset each AI cycle, before a new
		//		round of sensory input.  Examples:  eifIS_VISIBLE, eifBLOCKS_PATH
		//
		//******************************
		{
			 setNodeFlags[ensfBLOCKS_PATH]	= false;
		}

		//*********************************************************************************
		//
		virtual void CalculateNodeSuitability
		(
		);
		//
		//	Decided how important the influence is in general.
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
};


//*********************************************************************************************
//
class CTerrainKnowledge : public CNodeSource
//
//	Prefix: tk
//
//	What the animal knows about a particular piece of terrain.
//
//*********************************************************************************************
{
public:
	TReal rRadius;		// Maximum extent of the knowledge.
	CVector2<> v2Center;// Center point of the knowledge.


	CMSArray<CSilhouette, iMAX_SUBMODELS>	asilOriginalSilhouettes;	// The silhouettes before merging
	CMSArray<CDir3<>, iMAX_SUBMODELS>		ad3Normals;					// The normal of each triangle.

	//
	//	Member functions.
	//

};




#endif  // #ifndef HEADER_LIB_AI_NODESOURCE_HPP
