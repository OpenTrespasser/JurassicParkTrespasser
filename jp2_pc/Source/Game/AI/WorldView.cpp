/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *	Implementation of WorldView.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/WorldView.cpp                                                $
 * 
 * 38    9/17/98 6:46p Jpaque
 * only use non-zero graph
 * 
 * 37    9/11/98 12:39p Agrant
 * incremental terrain knowledge rebuilding, rather than wholesale re-querying
 * 
 * 36    9/04/98 7:26p Agrant
 * more pain from the shared graphs
 * 
 * 35    9/03/98 5:15p Agrant
 * allow null ai graph
 * 
 * 34    9/02/98 5:26p Agrant
 * revamped knowledge merging function
 * 
 * 33    8/26/98 3:17a Agrant
 * using AIMass function
 * 
 * 32    8/25/98 11:00a Rvande
 * Fixed const-ness of an iterator
 * 
 * 31    8/18/98 8:25a Shernd
 * Fixed Debug compilation error @ 290
 * 
 * 30    8/17/98 6:48p Agrant
 * Improved save functions
 * Made a function to remove all influences
 * 
 * 29    8/11/98 2:18p Agrant
 * fixed horrible node garbage collection bug
 * 
 * 28    7/25/98 8:26p Agrant
 * fix load game bug
 * 
 * 27    7/25/98 8:03p Agrant
 * remember boi boundary boxes, not animals
 * 
 * 26    7/11/98 8:08p Agrant
 * More lenient influence counts, shown in AI console
 * 
 * 25    7/09/98 9:25p Agrant
 * Better influence removal
 * 
 * 24    7/08/98 11:26p Agrant
 * better use of worldview see function
 * 
 * 23    6/18/98 4:48p Agrant
 * Take longer to forget animates, please
 * 
 * 22    6/11/98 2:58p Agrant
 * safer load function, clearing influence list
 * 
 * 21    6/10/98 8:44p Agrant
 * Ignore wasteful influences
 * 
 * 20    5/19/98 9:16p Agrant
 * pathfailure and path obstacle and pathavoider all gone
 * 
 * 19    5/16/98 10:00a Agrant
 * Pathfinding fixes for the AI bounding box vs the physics bounding box
 * Major pathfinding fix in general-  how did it ever work???
 * 
 * 18    5/10/98 6:27p Agrant
 * AI's now can ignore selected tangible objects.
 * misc. changes
 * 
 * 17    3/12/98 7:54p Agrant
 * New silhouette internals (array instead of vector)
 * 2d pathfinding handles compound physics objects
 * 
 * 16    2/23/98 5:07p Agrant
 * AI save/load
 * 
 * 15    12/20/97 7:00p Agrant
 * Better include dependencies in AI system
 * 
 * 14    97/09/29 16:34 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.  
 * 
 * 13    9/09/97 9:01p Agrant
 * influences forgotten based on a brain parameter
 * 
 * 12    7/14/97 12:54a Agrant
 * Terrain Pathfinding data setup
 * 
 * 11    5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 10    4/29/97 6:42p Agrant
 * CFeeling now float-based instead of CRating-based.
 * Activities now rate/act based on a list of influences.
 * Better rating functions for activities.
 * Debugging tools for activity processing.
 * 
 * 9     4/07/97 4:03p Agrant
 * Closer to True Vision(tm) technology!
 * 
 * 8     4/01/97 1:14p Agrant
 * Better influence forgetting code
 * 
 * 7     3/28/97 2:01p Agrant
 * Brain now points to graph, mentalstate, and worldview
 * Pathfinding more efficient
 * 
 * 6     3/26/97 5:28p Agrant
 * Added number of pathfinding nodes to brain constructor, mentalstate, and worldview.
 * 
 * 5     2/16/97 5:37p Agrant
 * Even better pathfinding.
 * 
 * 4     2/10/97 4:15p Agrant
 * Intermediate pathfinding check-in
 * 
 * 3     2/09/97 8:17p Agrant
 * Added pathfinding node graph.
 * Better updates for influences.
 * 
 * 2     1/30/97 2:39p Agrant
 * Silhouettes added and pathfinder revision.
 * 
 * 1     1/28/97 6:41p Agrant
 * pathfinding files
 * 
 **********************************************************************************************/

#include "Common.hpp"

#include "WorldView.hpp"

#include "Pathfinder.hpp"
#include "Lib/Renderer/Geomtypes.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/EntityDBase/Animal.hpp"
#include "Lib/Loader/SaveFile.hpp"


#include "AIMain.hpp"
#include "AIInfo.hpp"
#include "AIGraph.hpp"
#include "Brain.hpp"

#include "Lib\Sys\DebugConsole.hpp"


//*********************************************************************************************
//
//	Class CWorldView implementation.
//


CWorldView::CWorldView(CAnimal *pani_owner) : inflInfluences()
{
	paniOwner = pani_owner;

	// Reset these guys soon!
	sTimeOfLastTemporaryFlagReset = 0;

	// Make sure these are off in the boonies.
	v2CenterOfTerrainKnowledge = CVector2<>(-10000, -10000);
	v2TerrainKnowledgeWorldMin = CVector2<>(-10000, -10000);
	v2TerrainKnowledgeWorldMax = CVector2<>(-10000, -10000);
}
	
CWorldView::~CWorldView()
{
}


//*********************************************************************************
const CInfluence* CWorldView::pinfNotice
(
	CInstance*	pins,			// The thing that has been noticed.
	TSec		s_current_time	// When it was noticed.
)
{
	// See if it's already here.
	CInfluenceList::iterator i = inflInfluences.iterFindInfluence(pins);

	if (i == inflInfluences.end())
	{
		// Not there yet!
		// Insert the real influence.
		CInfluence inf_real(paniOwner, pins);

		inf_real.UpdateForMove(paniOwner, s_current_time);
		std::pair<CInfluenceList::iterator, bool> p = inflInfluences.insert(inf_real);
		Assert(p.second);

		return 0;
	}
	else
	{
		// Influence already present.
		// Don't do anything to it.

		// Update timestamp.
		// Casting away const because we are not modifying sensitive data.
//		((CInfluence*)&(*i))->UpdateForMove(paniOwner, s_current_time);

		return &(*i);
	}

}

//*********************************************************************************
const CInfluence* CWorldView::pinfInfluenceMoved
(
	CInstance*	pins,			// The thing that has been moved.
	TSec		s_current_time	// When it was noticed.
)
{
	// See if it's already here.
	CInfluenceList::iterator i = inflInfluences.iterFindInfluence(pins);

	if (i == inflInfluences.end())
	{
		// Not there yet!
		return 0;
	}
	else
	{
		// Influence already present.

		// Update timestamp.
		// Casting away const because we are not modifying sensitive data.
		((CInfluence*)&(*i))->UpdateForMove(paniOwner, s_current_time);

		return &(*i);
	}
}


#if 0
//*********************************************************************************
bool CWorldView::bIsPathValid
(
	CPath*		ppath,			// The path to be tested.
	CRating		rt_solidity		// The object solidity through which the dino is willing to go
)
{
	for (int i = 0; i < ppath->size(); i++)
	{
		// Is the segment valid?
		if (!bIsPathSegmentValid(ppath, i, rt_solidity))
			// No!  Failure already set, so just return.
			return FALSE;
	}

	// Clear the failure mode.
	ppath->SetFailure(0);

	// Let the caller know we succeeded.
	return TRUE;
}

//*********************************************************************************
bool CWorldView::bIsPathSegmentValid
(
	CPath*		ppath,			// The path to be tested.
	int			i_seg,			// The segment to be tested.
	CRating		rt_solidity		// The object solidity through which the dino is willing to go
)
{


	TReal r_dino_height	= gaiSystem.pbrGetCurrentBrain()->rHeight;
	TReal r_dino_width	= gaiSystem.pbrGetCurrentBrain()->rWidth;


	CVector3<> v3_segment = ((*ppath)[i_seg + 1] - (*ppath)[i_seg]);

	// Box with the right shape and size.
	CBoundVolBox bvb_segment(r_dino_width, v3_segment.tLen(), r_dino_height);

	// Rotation which takes the box to the right place.
	// Same as rotation taking vector down Y axis to vector down segment.
	CRotate3<>	 r3(CDir3<>(0,1,0), v3_segment);

	// The placement is the above rotation and a translation to halfway down the segment.
	CPresence3<> pr3Path(r3, 1, (*ppath)[0] + v3_segment / 2);



	foreach(inflInfluences)
	{
		const CInfluence* pinf = &inflInfluences.tGet();

		// does our path segment intersect this object?
		if (pinf->feelAboutTarget[eptSOLIDITY] > rt_solidity && 
			bvb_segment.bIntersects(*pinf->pinsTarget->paiiGetAIInfo()->pbvGetBoundVol(pinf->pinsTarget), 
									&pr3Path, 
									&pinf->pinsTarget->pr3Presence()))
		{
			ppath->SetFailure(new CFailureObstacle(i_seg, (CInfluence*)pinf));
			return FALSE;
		}
	}
	return TRUE;
}
#endif 


//*********************************************************************************
bool CWorldView::bRemoveInfluence
(
	CInstance*	pins_target			// Target of influence to remove.
)
{
	CInfluenceList::iterator iter = iterFindInfluence(pins_target);

	if (iter != inflInfluences.end())
	{
		if (paniOwner->pbrBrain->paigGraph)
			paniOwner->pbrBrain->paigGraph->ClearReferences(pins_target);
		Assert(!(*iter).setNodeFlags[ensfIN_GRAPH]);
		inflInfluences.erase(iter);
		return true;
	}
	else 
		return false;
}

//*********************************************************************************
void CWorldView::RemoveAllInfluences()
{
	CAIGraph* paig = paniOwner->pbrBrain->paigGraph;

	// If we have no AI graph, skip clearing it.
	if (paig)
	{
		// Ditch each influence.
		CInfluenceList::iterator it_inf = inflInfluences.begin();
		for( ; it_inf != inflInfluences.end(); ++it_inf)
		{
			// Clean up the graph for each influence.
			paig->ClearReferences((*it_inf).pinsTarget);
			Assert(!(*it_inf).setNodeFlags[ensfIN_GRAPH]);
		}
	}

	// And remove the influences.
	inflInfluences.erase(inflInfluences.begin(), inflInfluences.end());
}



//*********************************************************************************
void CWorldView::RemoveSomeInfluences()
{

	TReal r_lowest;

	if (iNUM_INFLUENCES / 2 < inflInfluences.size())
		r_lowest = 1000000;
	else
		r_lowest = .1;    // Don't NEED to remove any influences, but if they are
							// unimportant enough, go ahead.

	// If we have a bunch of influences, remove one.
	size_t discardable = 0;

	CInstance* lowestInstance = nullptr;
	std::list<CInstance*> todelete;
	
	for (const auto& entry : inflInfluences)
	{
		if (gaiSystem.sNow - entry.sLastSeen > paniOwner->pbrBrain->sForgetInfluence)
			entry.setFlags[eifIS_DISCARDABLE] = true;
		else if (entry.rImportance < r_lowest)
		{
			r_lowest = entry.rImportance;
			lowestInstance = entry.pinsTarget;
		}

		if (entry.setFlags[eifIS_DISCARDABLE]) 
		{
			todelete.push_back(entry.pinsTarget);
			discardable++;
		}
	}

	if (lowestInstance) 
	{
		//Influence set is based on instance pointers, find result is guaranteed to be the correct one
		iterFindInfluence(lowestInstance)->setFlags[eifIS_DISCARDABLE] = true;
		todelete.push_back(lowestInstance);
	}
	
	if (discardable > 10)
		for (auto* entry : todelete)
			bRemoveInfluence(entry);
}

//*********************************************************************************
void CWorldView::See(CInstance *pins, TSec s_when)
{
	if (!pins)
		return;

	// Never look at animates, only their bounding boxes!
	if (ptCast<CAnimate>(pins))
		return;

	if (pins->paiiGetAIInfo() && pins->paiiGetAIInfo()->eaiRefType != eaiDONTCARE)
	{
		// Is it a boundary box?
		if (ptCast<CBoundaryBox>(pins))
		{
			// Make sure we aren't looking at ourselves!
			int i;
			for (i = 0; i < ebbEND; ++i)
			{
				if (pins == paniOwner->apbbBoundaryBoxes[i])
				{
					return;
				}
			}
		}

		(void) pinfAddOrUpdate(pins, s_when);
	}
}

//*********************************************************************************
const CInfluence* CWorldView::pinfAddOrUpdate
(
	CInstance*	pins,			// The thing that has been noticed.
	TSec		s_current_time	// When it was noticed.
)
{
	const CAIInfo*	paii = pins->paiiGetAIInfo();
	Assert(paii);
	// Are we small and uninteresting?
	if (paii->bIgnore() || 
		(paii->rtGetAIMass(pins) < paniOwner->pbrBrain->rMoveableMass && paii->eaiRefType == eaiUNKNOWN))
		// Yes!  Don't add us!
		return 0;

	// See if it's already here.
	CInfluenceList::iterator i = inflInfluences.iterFindInfluence(pins);

	if (i == inflInfluences.end())
	{
		// Not there yet!
		// Insert the real influence.
		CInfluence inf_real(paniOwner, pins);

//		inf_real.UpdateForMove(paniOwner, s_current_time);

		// Update timestamp.
		inf_real.sLastSeen = s_current_time;
		
		// Casting away const because we are not modifying sensitive data.
		inf_real.UpdatePositionalData(paniOwner);

		// Insert the influence.
		std::pair<CInfluenceList::iterator, bool> p = inflInfluences.insert(inf_real);

		AlwaysAssert(!inf_real.setNodeFlags[ensfIN_GRAPH]);

		AlwaysAssert(!(*p.first).setNodeFlags[ensfIN_GRAPH]);

#if VER_TEST
		if (inflInfluences.size() > iNUM_INFLUENCES)
		{
			char buffer[128];
			sprintf(buffer, "Too complicated!  %d influences!\n", inflInfluences.size());
			paniOwner->pbrBrain->DebugSay(buffer);
		}
#endif

		// Verify insert.
		Assert(p.second);

		return 0;
	}
	else
	{
		// Influence already present.

		// Update timestamp.
		((CInfluence*)&(*i))->sLastSeen = s_current_time;

		// Is it animate?
		if (ptCast<CBoundaryBox>(((CInfluence*)&(*i))->pinsTarget))
		{
			// Yes!  Remember it for longer, please.
			((CInfluence*)&(*i))->sLastSeen += 15.0f;
		}

		// Casting away const because we are not modifying sensitive data.
		((CInfluence*)&(*i))->UpdatePositionalData(paniOwner);

		return &(*i);
	}

}

//*********************************************************************************
void CWorldView::MaybeResetTempInfluenceFlags()   // Somewhat misnamed-  also resets terrain knowledge temp flags.
{
	if (gaiSystem.sNow > sTimeOfLastTemporaryFlagReset + 1)
	{
		// Reset each influence.
		CInfluenceList::iterator it_inf = inflInfluences.begin();
		for( ; it_inf != inflInfluences.end(); ++it_inf)
		{
			CInfluence* pinf = (CInfluence*)&*it_inf;
			pinf->ResetTemporaryFlags();
		}

		std::list<CTerrainKnowledge>::iterator ptk;
		for (ptk = ltkKnowledge.begin(); ptk != ltkKnowledge.end(); ++ptk)
		{
			CTerrainKnowledge* ptk_temp = &(*ptk);
			ptk_temp->ResetTemporaryFlags();
		}

		sTimeOfLastTemporaryFlagReset = gaiSystem.sNow;
	}
}

//*********************************************************************************
void CWorldView::MaybeMergeTerrain()
{
	// Is there anything left to merge?
	while(ptkKnowledgeMerging != ltkKnowledge.end())
	{
		// Yes! Try to merge.
		CTerrainKnowledge* ptk = &(*ptkKnowledgeMerging);

		if (ptk->setNodeFlags[ensfCAN_SIMPLIFY])
		{
			ptk->Simplify();			
		}
		else
		{
			++ptkKnowledgeMerging;
		}
	}
}

//*********************************************************************************
void CWorldView::ResetMergeTerrain()
{
  	ptkKnowledgeMerging = ltkKnowledge.begin();
	iSideMerging = eqsBEGIN;
}

//*********************************************************************************
void CWorldView::ClearTerrainKnowledge()
{
	std::list<CTerrainKnowledge>::iterator ptk;

	if (paniOwner->pbrBrain->paigGraph)
	{
		for (ptk = ltkKnowledge.begin(); ptk != ltkKnowledge.end(); ++ptk)
		{
			// Change any dependent nodes to Old nodes.
			paniOwner->pbrBrain->paigGraph->ClearReferences(&(*ptk));
		}
	}

	ltkKnowledge.erase(ltkKnowledge.begin(),ltkKnowledge.end());
	ptkKnowledgeMerging = ltkKnowledge.end();
}

//*********************************************************************************
void CWorldView::ClearOutsideTerrainKnowledge()
{
	std::list<CTerrainKnowledge>::iterator ptk;
	std::list<CTerrainKnowledge>::iterator ptk_last;

	ptk = ltkKnowledge.begin();
	ptk = ltkKnowledge.end();

	while(ptk != ltkKnowledge.end())
	{
		if (bOutsideTerrainKnowledge((*ptk).v2Center))
		{	
			// Clean this guy up.
			if (paniOwner->pbrBrain->paigGraph)
				paniOwner->pbrBrain->paigGraph->ClearReferences(&(*ptk));

			// Erase it.
			ltkKnowledge.erase(ptk);

			if (ptk_last == ltkKnowledge.end())
				ptk = ltkKnowledge.begin();
			else
				ptk = ptk_last;
		}
		else
		{
			ptk_last = ptk;
		}
	}

	ptkKnowledgeMerging = ltkKnowledge.begin();
}

//*********************************************************************************
bool CWorldView::bOutsideTerrainKnowledge(CVector2<> v2)
{
	if (v2.tX > v2TerrainKnowledgeWorldMax.tX ||
		v2.tY > v2TerrainKnowledgeWorldMax.tY)
		return true;

	if (v2.tX < v2TerrainKnowledgeWorldMin.tX ||
		v2.tY < v2TerrainKnowledgeWorldMin.tY)
		return true;

	return false;
}

//*****************************************************************************************
char *CWorldView::pcSave(char * pc) const
{
	// Save data about which influences are present.
	int i_num_influences = inflInfluences.size();
	pc = pcSaveT(pc, i_num_influences);

	// Save each influence.
	CInfluenceList::const_iterator it_inf = inflInfluences.begin();
	for( ; it_inf != inflInfluences.end(); ++it_inf)
	{
		CInfluence* pinf = (CInfluence*)&*it_inf;
		pc = pinf->pcSave(pc);
	}

	return pc;
}

//*****************************************************************************************
const char *CWorldView::pcLoad(const char * pc)
{
	// Kill the old influences.
	RemoveAllInfluences();

	extern int iAnimalVersion;
	if (iAnimalVersion >= 6)
	{
		// Save data about which influences are present.
		int i_num_influences = 0;
		pc = pcLoadT(pc, &i_num_influences);

		// Load each influence.
		CInfluenceList::iterator it_inf = inflInfluences.begin();
		for(--i_num_influences ; i_num_influences >= 0; --i_num_influences)
		{
			CInfluence inf;
			pc = inf.pcLoad(pc);
			inflInfluences.insert(inf);
		}
	}

	// Make sure we don't get lost.
	sTimeOfLastTemporaryFlagReset = 0;

	return pc;
}
