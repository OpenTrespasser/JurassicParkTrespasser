/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *	Implementation of NodeHistory.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/NodeHistory.cpp                                              $
 * 
 * 17    8/25/98 11:07a Rvande
 * Fixed a for ( int i ;.... type issue...
 * 
 * 16    8/23/98 3:34a Agrant
 * Random pathfinding nodes.
 * 
 * 15    8/11/98 2:19p Agrant
 * much hacking to keep nodes current with influences
 * 
 * 14    5/16/98 10:00a Agrant
 * Pathfinding fixes for the AI bounding box vs the physics bounding box
 * Major pathfinding fix in general-  how did it ever work???
 * 
 * 13    1/29/98 4:41p Agrant
 * Change node collision error reporting to a dout instead of an assert.
 * 
 * 12    12/20/97 7:00p Agrant
 * Better include dependencies in AI system
 * 
 * 11    9/11/97 1:54p Agrant
 * Added some debug checking for adding nodes
 * 
 * 10    9/09/97 9:04p Agrant
 * added validation process to nodes
 * removed unused old influence nodes and old nodesource nodes
 * improved behavior of moving influences
 * 
 * 9     7/31/97 4:41p Agrant
 * Pathfinding over objects begins
 * 
 * 8     7/19/97 1:24p Agrant
 * Removed unneeded assert.
 * 
 * 7     7/14/97 12:57a Agrant
 * Added NodeSource histories.
 * Support unknown nodes.
 * 
 * 6     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 5     4/01/97 1:14p Agrant
 * Better influence forgetting code
 * 
 * 4     3/28/97 2:00p Agrant
 * Brain now points to graph, mentalstate, and worldview
 * Pathfinding more efficient
 * 
 * 3     3/26/97 5:32p Agrant
 * SparseArray graph implementation change
 * 
 * 2     2/16/97 5:37p Agrant
 * Even better pathfinding.
 * 
 * 1     2/12/97 7:29p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "NodeHistory.hpp"

#include "AIGraph.hpp"
#include "AIMain.hpp"
#include "Influence.hpp"
#include "WorldView.hpp"
#include "Brain.hpp"

#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/EntityDBase/Animal.hpp"

#include "Lib/Sys/DebugConsole.hpp"


// Rating weights for history tyoes.
// Higher weights tend to get collected sooner.
// Think of this as a time ratio to real time.

#define rNHINFLUENCE_TIME_FACTOR		TReal(1.0)
#define rNHINFLUENCEOLD_TIME_FACTOR		TReal(2.5)
#define rNHUNKNOWN_TIME_FACTOR			TReal(2.5)

//*********************************************************************************************
//
//	Class CNodeHistory implementation.
//

	void CNodeHistory::WasUsed
	(
		CAIGraphNode*	paig,		// The node to update.
		TSec			s_when_used // The time the node was used.
	)
	{
		//  Flag the node as having been used at time s_when.
		paig->sLastUsed = s_when_used;

		// Used to assert when the history didn't have it's own WasUsed function, but that now seems
		// silly.  Hmmmm.
//		Assert(0);
	}

	//*************************************************************************************
	void CNodeHistory::Delete
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	{
		paig->RemoveNode(i_node_index);
	}

	//*************************************************************************************
	void CNodeHistory::Validate
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	{
		Assert(!setFlags[enfVALIDATED]);

		// Check to see if the node lies inside an immovable object.
		CInfluenceList* pinfl = &paig->paniAnimal->pbrBrain->pwvWorldView->inflInfluences;

		CInfluenceList::iterator pinf = pinfl->begin();

		CAIGraphNode* paign = paig->pnNode(i_node_index);

		for (; pinf != pinfl->end(); ++pinf)
		{
			CInstance* pins = (*pinf).pinsTarget;
			const CPhysicsInfo* pphi = pins->pphiGetPhysicsInfo();  // OK physics info usage
			if (!pphi->bIsMovable() && pphi->bContains(paign->v3Pos, pins->pr3Presence()))
			{
				// Node is in an immoveable object.  It's useless.
				setFlags[enfDISCARDABLE] = true;
				setFlags[enfVALIDATED] = true;
				
				return;
			}
		}

#if VER_DEBUG
		int i_collide = 0;
		// Make sure no two nodes occupy the same point in space.
		for (int i = paig->iMaxNodeIndex() - 1; i >= 0; --i)
		{	
			if (i == i_node_index)
				continue;

			CAIGraphNode* paign2 = paig->pnNode(i);			

			if (paign2->bIsValid())
			{
//				Assert(paign->v3Pos != paign2->v3Pos);
				if (paign->v3Pos == paign2->v3Pos)
					++i_collide;
			}
		}
		
		if (i_collide > 0)
		{
			dout << "Colliding nodes: " << i_collide << "\n";
		}
#endif


		
		setFlags[enfVALIDATED] = true;
	}


//*********************************************************************************************
//
//	Class CNHInfluence implementation.
//

	CNHInfluence::CNHInfluence(CInfluence *pinf) : pinfInfluence(pinf)
	{
		Assert(pinf);
	}

	//*********************************************************************************************
	void CNHInfluence::Delete
	(
		CAIGraph* paig, int i_index
	)
	{
//		CInfluence* pinf = pinfInfluence;
		CNodeHistory::Delete(paig, i_index);					
	}


	//*********************************************************************************************
	void CNHInfluence::WasUsed(CAIGraphNode* paign, TSec s_when)
	{
		//  Flag the node as having been used at time s_when.
		paign->sLastUsed = s_when;

		//  Flag the influence as having been used at time s_when for pathfinding.
		pinfInfluence->WasUsedInPath(s_when);
	}

	//*********************************************************************************************
	float CNHInfluence::fRate
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	{
		Assert(pinfInfluence);

		if (!setFlags[enfVALIDATED])
			Validate(paig, i_node_index);

		float f_ret;
		const CAIGraphNode& aign = paig->nNode(i_node_index);

		if (aign.sLastUsed >= pinfInfluence->sLastUsedInPath)
			f_ret = gaiSystem.sNow - aign.sLastUsed;
		else
			f_ret = gaiSystem.sNow - pinfInfluence->sLastUsedInPath;
		f_ret *= rNHINFLUENCE_TIME_FACTOR;			// Multiply by the weight factor.

		
		Assert(f_ret >= 0);
//		Assert(f_ret < 3600);

		return f_ret;
	}

	//*********************************************************************************************
	bool CNHInfluence::bReferences
	(
		const CInstance*	pins
	) const
	{
		return pins == pinfInfluence->pinsTarget;
	}

	//*********************************************************************************************
	CNodeSource* CNHInfluence::pnsReferences()
	{
		return pinfInfluence;
	}

//*********************************************************************************************
//
//	Class CNHInfluenceOld implementation.
//
#ifdef TRACK_OLD_NODES

	CNHInfluenceOld::CNHInfluenceOld(CInfluence *pinf) : pinfInfluence(pinf)
	{
		Assert(pinf);
	}

	//*********************************************************************************************
	void CNHInfluenceOld::Delete(CAIGraph* paig, int i_index)
	{
		// Nothing special here.

		// Delete the offending nodes.
		paig->RemoveNode(i_index);
	}


	//*********************************************************************************************
	void CNHInfluenceOld::WasUsed(CAIGraphNode* paign, TSec s_when)
	{
		//  Flag the node as having been used at time s_when.
		paign->sLastUsed = s_when;
	}

	//*********************************************************************************************
	float CNHInfluenceOld::fRate
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	{
		if (!setFlags[enfVALIDATED])
			Validate(paig, i_node_index);

		float f_ret;
		const CAIGraphNode& aign = paig->nNode(i_node_index);

		f_ret = gaiSystem.sNow - aign.sLastUsed;	// How long has this node been useless?
		f_ret *= rNHINFLUENCEOLD_TIME_FACTOR;		// Multiply by the weight factor.
		
		Assert(f_ret >= 0);
		Assert(f_ret < 3600);

		return f_ret;
	}

	//*********************************************************************************************
	bool CNHInfluenceOld::bReferences
	(
		const CInstance*	pins
	) const
	{
		return pins == pinfInfluence->pinsTarget;
	}

	//*********************************************************************************************
	CNodeSource* CNHInfluence::pnsReferences()
	{
		return pinfInfluence;
	}

#endif  // TRACK_OLD_NODES



//*********************************************************************************************
//
//	Class CNHInfluenceSurface implementation.
//

	CNHInfluenceSurface::CNHInfluenceSurface(CInfluence *pinf) : CNHInfluence(pinf)
	{
	}

	//*********************************************************************************************
	void CNHInfluenceSurface::Delete
	(
		CAIGraph* paig, int i_index
	)
	{
		// We assume that this function is only called when this node is not needed to gain access to the top
		// surface of the influence.

		// Could add a bunch of check code, but it ought to be obvious in the test app.

		// Just do it.
//		paig->RemoveNode(i_index);
		CNHInfluence::Delete(paig, i_index);
	}

	//*********************************************************************************************
	void CNHInfluenceSurface::Validate
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	{
		CNodeHistory::Validate(paig, i_node_index);

		// If discardable, we're done.
		if (setFlags[enfDISCARDABLE])
			return;

		// Is it on the surface?
		if (pinfInfluence->pinsTarget == paig->nNode(i_node_index).pinsSurface)
		{
			// It's on the surface, and therefore okay.
			return;
		}


		// Must check to see if this node connects to any nodes on the surface of the influence.
		// It just needs one to be worthwhile.

		// Loop through all nodes.
		for (int i = 0; i < paig->iMaxNodeIndex(); ++i)
		{
			CNodeHistory* pnh = paig->nNode(i).pnhGetNodeHistory();
			CNHInfluence* pnhis = pnh->pCastNHInfluenceSurface();
			if (pnhis)
			{
				// We've found an influence surface history.
				if (pnhis->pinfInfluence == pinfInfluence && paig->nNode(i).pinsSurface == pnhis->pinfInfluence->pinsTarget)
				{
					// We've found a node based on "pinf" lying on the surface.
					if (paig->bIsSuccessor(i_node_index, i) ||
						paig->bIsSuccessor(i, i_node_index))
					{	
						// It's a good node.
						return;
					}
				}

			}
		}

		// Looped through all nodes, and didn't find a connection.  Nuke it.
//		pinfInfluence = 0;
		setFlags[enfDISCARDABLE] = true;
	}



//*********************************************************************************************
//
//	Class CNHNodeSource implementation.
//

	CNHNodeSource::CNHNodeSource(CNodeSource *pns) : pnsNodeSource(pns)
	{
		Assert(pns);
	}

	//*********************************************************************************************
	void CNHNodeSource::Delete
	(
		CAIGraph* paig, int //i_index
	)
	{
		// This gets a little weird.

		// First, delete all nodes in the graph that are CNHInfluences pointing to the same
		// influence that this one points to.
		// Why?  
		// We can't set the IN_GRAPH flag to false otherwise.
		// And if we don't unset the IN_GRAPH flag, this node can never be re-added.

		// Save the source pointer, because "this" will be invalidated in the middle of
		// the Delete() call.
		CNodeSource* pns = pnsNodeSource;

		int i_first_node_with_source = - 1;

		// Go through all nodes, finding ones that have the same NodeSource.
		int i;
		for (i = 0; i < paig->iMaxNodeIndex(); i++)
		{
			CNodeHistory* pnh = paig->nNode(i).pnhGetNodeHistory();
			CNHNodeSource* pnhns = pnh->pCastNHNodeSource();
			if (pnhns)
			{
				// We've found a NodeSource history.
				if (pnhns->pnsNodeSource == pns)
				{
					// We've found a node based on "pns"
					if (i_first_node_with_source == - 1)
					{	
						// And it's the first one!
						i_first_node_with_source = i;
					}

					// Look at the next node!
					continue;
				}

			}

			// We can only get here if the current node is not the kind we want.
			if (i_first_node_with_source != - 1)
			{
				// Then we have found a node that doesn't match, after a string of nodes
				// that DO match.

				// Delete the offending nodes.
				paig->RemoveNodes(i_first_node_with_source, i - 1);

				// Prepare for another run.
				i_first_node_with_source = - 1;
			}
		}

		// If the influenced nodes are at the end of the graph, then we need to do this again,
		// because we won't have actually removed them yet.
		if (i_first_node_with_source != - 1)
		{
			// Then we have found a node that doesn't match, after a string of nodes
			// that DO match.

			// Delete the offending nodes.
			paig->RemoveNodes(i_first_node_with_source, i - 1);

			// Prepare for another run.
			i_first_node_with_source = - 1;
		}


		// Final Act: let the source know that it is no longer in the graph.
		pns->setNodeFlags[ensfIN_GRAPH] = false;
		AlwaysAssert(pns->cNodeCount == 0);
	}


	//*********************************************************************************************
	void CNHNodeSource::WasUsed(CAIGraphNode* paign, TSec s_when)
	{
		//  Flag the node as having been used at time s_when.
		paign->sLastUsed = s_when;

		//  Flag the influence as having been used at time s_when for pathfinding.
		pnsNodeSource->WasUsedInPath(s_when);
	}

	//*********************************************************************************************
	float CNHNodeSource::fRate
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	{
		if (!setFlags[enfVALIDATED])
			Validate(paig, i_node_index);

		float f_ret;
		const CAIGraphNode& aign = paig->nNode(i_node_index);

		if (aign.sLastUsed >= pnsNodeSource->sLastUsedInPath)
			f_ret = gaiSystem.sNow - aign.sLastUsed;
		else
			f_ret = gaiSystem.sNow - pnsNodeSource->sLastUsedInPath;
		f_ret *= rNHINFLUENCE_TIME_FACTOR;			// Multiply by the weight factor.

		
		Assert(f_ret >= 0);
		Assert(f_ret < 3600);

		return f_ret;
	}

	//*********************************************************************************************
	CNodeSource* CNHNodeSource::pnsReferences()
	{
		return pnsNodeSource;
	}


//*********************************************************************************************
//
//	Class CNHNodeSourceOld implementation.
//
#ifdef TRACK_OLD_NODES

	CNHNodeSourceOld::CNHNodeSourceOld(CNodeSource *pns) : CNHNodeSource(pns)
	{
	}

	//*********************************************************************************************
	void CNHNodeSourceOld::Delete(CAIGraph* paig, int i_index)
	{
		// Nothing special here.

		// Delete the offending nodes.
		paig->RemoveNode(i_index);
	}


	//*********************************************************************************************
	void CNHNodeSourceOld::WasUsed(CAIGraphNode* paign, TSec s_when)
	{
		//  Flag the node as having been used at time s_when.
		paign->sLastUsed = s_when;
	}

	//*********************************************************************************************
	float CNHNodeSourceOld::fRate
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	{
		if (!setFlags[enfVALIDATED])
			Validate(paig, i_node_index);


		float f_ret;
		const CAIGraphNode& aign = paig->nNode(i_node_index);

		f_ret = gaiSystem.sNow - aign.sLastUsed;	// How long has this node been useless?
		f_ret *= rNHINFLUENCEOLD_TIME_FACTOR;		// Multiply by the weight factor.
		
		Assert(f_ret >= 0);
		Assert(f_ret < 3600);

		return f_ret;
	}
#endif // TRACK_OLD_NODES


	//*********************************************************************************************
	float CNHUnknown::fRate
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	{
		if (!setFlags[enfVALIDATED])
			Validate(paig, i_node_index);

		float f_ret;
		const CAIGraphNode& aign = paig->nNode(i_node_index);

		f_ret = gaiSystem.sNow - aign.sLastUsed;	// How long has this node been useless?
		f_ret *= rNHUNKNOWN_TIME_FACTOR;			// Multiply by the weight factor.
		
		Assert(f_ret >= 0);
		Assert(f_ret < 3600);

		return f_ret;
	}



	//*********************************************************************************************
	float CNHRandom::fRate
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	{
		if (!setFlags[enfVALIDATED])
			Validate(paig, i_node_index);

		float f_ret;
		const CAIGraphNode& aign = paig->nNode(i_node_index);

		f_ret = gaiSystem.sNow - aign.sLastUsed;	// How long has this node been useless?
		f_ret *= rNHUNKNOWN_TIME_FACTOR;			// Multiply by the weight factor.
		
		Assert(f_ret >= 0);
		Assert(f_ret < 3600);

		return f_ret;
	}

	//*************************************************************************************
	void CNHRandom::Delete
	(
		CAIGraph*	paig,			// The graph the node is in.
		int			i_node_index	// The index of the node.
	)
	{
		paig->RemoveNode(i_node_index);
	}
