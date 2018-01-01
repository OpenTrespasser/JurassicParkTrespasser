/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *		Implementation of NodeSource.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/NodeSource.cpp                                               $
 * 
 * 13    9/23/98 10:32p Agrant
 * Make Node Source rating function larger, just so we can subtract the distance from it and
 * still have a positive value.
 * 
 * 12    9/19/98 1:42a Agrant
 * added the node_me flag to increase the likelihood of noding important things
 * 
 * 11    9/02/98 5:21p Agrant
 * Simplification support.
 * Better node positioning
 * 
 * 10    8/26/98 3:17a Agrant
 * added the BLOCKED_PATH flag
 * 
 * 9     8/13/98 6:54p Agrant
 * add all silhouettes to the graph, not just the first
 * 
 * 8     8/11/98 2:19p Agrant
 * much hacking to keep nodes current with influences
 * 
 * 7     7/20/98 12:27p Agrant
 * width fixes
 * 
 * 6     7/14/98 4:11p Agrant
 * make sure we have an old "last used" timestamp
 * 
 * 5     6/18/98 4:48p Agrant
 * Removed wacky assert
 * 
 * 4     3/12/98 7:54p Agrant
 * New silhouette internals (array instead of vector)
 * 2d pathfinding handles compound physics objects
 * 
 * 3     12/20/97 7:00p Agrant
 * Better include dependencies in AI system
 * 
 * 2     7/14/97 12:56a Agrant
 * New Base class for all pathfinding obstacles.
 * 
 * 1     7/13/97 8:41p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "NodeSource.hpp"
#include "AIMain.hpp"
#include "AIGraph.hpp"
#include "Brain.hpp"
#include "Lib/Types/FixedP.hpp"

 
//*********************************************************************************************
//
//	Class CNodeSource
//

	//*********************************************************************************
	CNodeSource::CNodeSource()
	{
		ResetTemporaryFlags();	
		sLastUsedInPath = -1000;
 
		cNodeCount	= 0;
		Assert(setNodeFlags[ensfIN_GRAPH] == false);
	}

	//*********************************************************************************
	int CNodeSource::iNumNodes
	(
	) const
	{
		int i_total = 0;
		// Check each silhouette.
		for (int i = iNumSilhouettes() - 1; i >= 0; --i)
		{
			// Does this silhouette block the line?
			i_total += psilGetSilhouette(i)->size();
		}
	
		return i_total;
	}


	//*********************************************************************************
	bool CNodeSource::bEdgeIntersects
	(
		const CLineSegment2<>& ls
	) const
	{
		// Check each silhouette.
		for (int i = iNumSilhouettes() - 1; i >= 0; --i)
		{
			// Does this silhouette block the line?
			if (psilGetSilhouette(i)->bEdgeIntersects(ls))
			{
				// Yes!  Return true.
				CNodeSource* pns = (CNodeSource*) this;
				pns->setNodeFlags[ensfBLOCKS_PATH] = true;
				pns->setNodeFlags[ensfBLOCKED_PATH] = true;
				return true;
			}
		}
	
		// No silhouette blocks the line.
		return false;
	}

	//*********************************************************************************
	void CNodeSource::CalculateNodeSuitability()
	{
		Assert(0);
		rNodeSuitability = 0;
	}

	//*********************************************************************************
	TReal CNodeSource::rRateNodeSuitability() const
	{
		if (setNodeFlags[ensfIS_PASSABLE])
			return 0.0f;

		TReal r_suits = rNodeSuitability;

		r_suits *= gaiSystem.rnd() & 31;
		r_suits *= (1.0f / 32.0f);

		if (setNodeFlags[ensfBLOCKS_PATH])
			r_suits += 100;
		if (setNodeFlags[ensfNODE_ME])
			r_suits += 100;

		// Because we subtract distance from this in our influence calculations, scaling it up 
		// somewhat will make it far less likely that we'll get a negative final rating.
		r_suits *= 10.0f;

		return r_suits;
	}




	//*********************************************************************************
	int CNodeSource::iAddToGraph(CBrain* pbr)
	{
		// Default behavior is too add the silhouette points as nodes.
		Assert(!setNodeFlags[ensfIN_GRAPH]);

		// Slop depends on dino size.
		TReal rSlop = pbr->rWidth * 0.5f;

		int i_node_count = 0;

		for (int i_sil = iNumSilhouettes() - 1; i_sil >= 0; --i_sil)
		{

			CSilhouette& silSilhouette = asilSilhouettes[i_sil];

			// Must at least be a triangle!
			Assert(silSilhouette.size() > 2);


			int i;	// A loop iterator.
			CVector2<> v2_center = CVector2<>(0,0);

			// Find a point that is likely to be in the middle of the silhouette.
			for (i = silSilhouette.size() - 1; i >= 0; --i)
			{
				v2_center += silSilhouette[i];
			}
			v2_center /= silSilhouette.size();

			for (i = silSilhouette.size() - 1; i >= 0; i--)
			{
				CVector3<> v2(silSilhouette[i]);

				// Add some slop.  Push away from the center.
				v2 = v2 - v2_center;
				TReal r_length = v2.tLen();
				if (bFuzzyEquals(r_length, 0))
					continue;

				r_length += rSlop;
				v2.Normalise(r_length);
				v2 += v2_center;

				CAIGraphNode aign(CVector3<>(v2), gaiSystem.sNow);

				aign.SetHistoryNodeSource(this);

				pbr->paigGraph->AddNode(aign);
				i_node_count++;
			}
		}

		// We're now in the graph!
		setNodeFlags[ensfIN_GRAPH] = true;

		// How many nodes added?
		return i_node_count;
	}


	void CNodeSource::Simplify()
	{
		if (!setNodeFlags[ensfCAN_SIMPLIFY])
			return;

		int i_sil = iNumSilhouettes() - 1;

		// Go through silhouettes in turn, trying to simplify.
		while(i_sil >= 0)
		{
			// Are we trying to simplify silhouette zero?
			if (i_sil == 0)
			{
				for (int i = iNumSilhouettes() - 1; i >= 0; --i)
				{
					psilGetSilhouette(i)->RemoveColinearPoints();
				}

				// Yes!  We're done!
				setNodeFlags[ensfCAN_SIMPLIFY] = false;
				return;
			}

			AlwaysAssert(i_sil > 0);

			// Try to merge i_sil with i_sil - 1
			CSilhouette sil;
			bool b_merged = sil.bMerge(psilGetSilhouette(i_sil - 1), psilGetSilhouette(i_sil));

			// Did we succeed?
			if (b_merged)
			{
				int i_index = i_sil;

				// Yes!  Collapse the silhouette array cuz we just lost i_sil
				(*psilGetSilhouette(i_index - 1)) = sil;

				for ( ; i_index < iNumSilhouettes() - 1; ++i_index)
				{
					(*psilGetSilhouette(i_index)) = (*psilGetSilhouette(i_index+1));
				}

				// And then drop off the last guy.
				asilSilhouettes.uLen -= 1;

				// And stop simplifying for now.
				return;
			}
			else
			{
				// No.  Try next silhouette.
				--i_sil;
			}
		}
	}