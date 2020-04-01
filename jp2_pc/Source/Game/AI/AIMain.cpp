/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of AIMain.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/AIMain.cpp                                                   $
 * 
 * 130   10/03/98 7:09p Jpaque
 * fixed bug where a dino could get activated twice
 * 
 * 129   10/02/98 10:05p Agrant
 * disable dinos cheat
 * 
 * 128   10/02/98 4:42p Agrant
 * fix dino sleep bug (and some frame rate badness)
 * 
 * 127   9/27/98 11:33p Agrant
 * don't walk through your friends
 * 
 * 126   9/23/98 11:18p Agrant
 * better wakeup-sleep for dinos
 * 
 * 125   9/22/98 3:46p Pkeet
 * make dinos NOT activate during ProcessPending(), to avoid world database munging during
 * hardware rendering
 * 
 * 124   9/19/98 9:05p Agrant
 * turn off dinos physically if they are not thinking
 * 
 * 123   9/17/98 6:46p Jpaque
 * some global AI system safety
 * 
 * 122   9/16/98 9:20p Agrant
 * Make sure to handle dead things properly.
 * Lowered the maximum dino movable mass
 * 
 * 121   9/13/98 8:05p Agrant
 * cancel the visual doubling for dino wake up
 * Handle too many dinos in a more robust way
 * 
 * 120   9/10/98 4:25p Agrant
 * save sleep flag
 * 
 * 119   9/10/98 12:04a Agrant
 * use the sleep flag
 * 
 * 118   9/08/98 7:38p Agrant
 * more likely to wake up dinos with the wake me flag set
 * 
 * 117   9/05/98 3:56p Agrant
 * fixed ai load crash
 * 
 * 116   9/05/98 2:30p Asouth
 * loop variable
 * 
 * 115   9/04/98 8:52p Agrant
 * added some asserts to protect against bad aigraph pointers
 * 
 * 114   9/02/98 5:23p Agrant
 * A new AI heap
 * 
 * 113   9/02/98 3:58p Agrant
 * Now sharing graphs for all dinos in a level.
 * 
 * 112   8/31/98 5:08p Agrant
 * lowered max number of active animals
 * 
 * 111   8/29/98 10:58p Agrant
 * Send physics requests every time we evaluate the AI
 * 
 * 110   8/26/98 10:21p Kmckis
 * @#$^%@#$
 * 
 * 109   8/26/98 3:16a Agrant
 * better AI mass
 * 
 * 108   8/26/98 1:04a Agrant
 * set velocity and speed before running the AI model
 * 
 * 107   8/25/98 10:59a Rvande
 * Fixes for MW build
 * 
 * 106   8/24/98 10:05p Agrant
 * Limit AI to ten "frames" per second
 * 
 * 105   8/23/98 2:24p Agrant
 * more startle functions
 * 
 * 104   8/23/98 3:51a Agrant
 * Disable 3d sound handling until it's tested
 * 
 * 103   8/20/98 11:37p Agrant
 * function for listening to sound in the world
 * 
 * 102   8/20/98 11:09p Agrant
 * tail point accessor function
 * 
 * 101   8/20/98 5:12p Pkeet
 * Removed Z buffering as a flag from Direct3D.
 * 
 * 100   8/17/98 6:50p Agrant
 * use activate/deactive functions for animals brains
 * 
 * 99    8/16/98 3:21p Agrant
 * Much tweaking to simplify activity balancing
 * 
 * 98    8/13/98 6:56p Agrant
 * code to allow dinos to ignore influences that are unattainable
 * 
 * 97    8/13/98 6:12p Mlange
 * Paint message now requires registration of the recipients.
 * 
 * 96    8/13/98 1:49p Mlange
 * The step message now requires registration of its recipients.
 * 
 * 95    8/12/98 5:24p Pkeet
 * Added the 'bStepPending' data member and the 'ProcessPending' member function.
 * 
 * 94    8/10/98 5:23p Rwyatt
 * Removed iTotal and replaced it with a memory counter
 * 
 * 93    7/29/98 3:09p Agrant
 * moved team to CAnimate
 * 
 * 92    7/25/98 8:04p Agrant
 * effects of fatigue now more predictable
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "AIMain.hpp"

#include "Lib/W95/Direct3D.hpp"
#include "MentalState.hpp"

#include "Lib\EntityDBase\WorldDBase.hpp"
#include "Lib\EntityDBase\Query\QAI.hpp"
#include "Lib\EntityDBase\MessageTypes\MsgStep.hpp"
#include "Lib\EntityDBase\MessageTypes\MsgPaint.hpp"

#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/ConIO.hpp"

#include "Activity.hpp"
#include "Synthesizer.hpp"
#include "Brain.hpp"
#include "AIInfo.hpp"
#include "AIGraph.hpp"

#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Lib/Loader/SaveFile.hpp"

#include "Game/DesignDaemon/Player.hpp"

#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/View/LineDraw.hpp"


// The CFeeling rating names.
char *CFeeling::pcRatingNames[] = 
{
	"Fear",
	"Love",
	"Anger",
	"Curiosity",
	"Hunger",
	"Thirst",
	"Fatigue",
	"Pain",
	"Solidity"
};

#define iDEFAULT_MAXNODES 50

// The AI system global.  There can be only one.
CAISystem* gpaiSystem = 0;

CFastHeap fhAI(1 << 24);

// Globals for debugging.
float fAIDebug1 = 0.5;
float fAIDebug2 = 0.5;
float fAIDebug3 = 0.5;



//**********************************************************************************************
//
//	Class CAISystem implementation.
//

	//*********************************************************************************************
	//
	CAISystem::CAISystem(CConsoleBuffer *pcon) : pconDebugConsole(pcon), pinsSelected(0), bStepPending(false)
	{
		AlwaysAssert(gpaiSystem == 0);

		//  Set the name.
		SetInstanceName("AI System");

		// Init the shared synthesizer.
		psynGlobalSynthesizer = new CSynthesizer();  //lint !e1732 !e1733
	
		// Seed the AI random number generator.
		rnd = iAI_RANDOM_SEED;

		// Make sure that the current brain is not masquerading as valid.
		pbrCurrentBrain = 0;

		// Initialize the opinion table.
		InitDefaultOpinions();

		// Default to AI on.
		bActive = true;
		bSleep	= false;
		bBoring = false;

		// Init the thinking interval.		AI 10 times a second!
		sMinInterval = 0.1;

		// Init the timestamps.
		sNow = 0;
		sLastExecution = sNow;

		for (int i = iMAX_ACTIVE_ANIMALS - 1; i >= 0; i--)
		{
			apaniActiveAnimals[i] = 0;

			// Init the AI Graphs.
			apgraphActiveGraphs[i] = new CAIGraph(0, iDEFAULT_MAXNODES);
		}

		// Default values for debugging flags.
		bDrawAllGraphLinks	= false;
		bDrawJumpGraphLinks = false;
		bShow3DInfluences	= false;

//		void TestSilhouette();
//		TestSilhouette();

		// Register this entity with the message types it needs to receive.
		 CMessageStep::RegisterRecipient(this);
		CMessagePaint::RegisterRecipient(this);
	}

	//*********************************************************************************************
	CAISystem::CAISystem()
	{
		Assert(false);

		// Register this entity with the message types it needs to receive.
		 CMessageStep::RegisterRecipient(this);
		CMessagePaint::RegisterRecipient(this);
	};

	//*********************************************************************************************
	CAISystem::~CAISystem()
	{
		CMessagePaint::UnregisterRecipient(this);
		CMessageStep::UnregisterRecipient(this);

		delete psynGlobalSynthesizer;
		pconDebugConsole = 0;

		for (int i = iMAX_ACTIVE_ANIMALS - 1; i >= 0; i--)
		{
			// Delete the AI Graphs.
			delete apgraphActiveGraphs[i];
		}


		// Reset the AI FastHeap.
		fhAI.Reset(0,0);

		gpaiSystem = 0;
	}

	//*********************************************************************************************
	void CAISystem::HandleSensoryInput()
	{
	}

	//*********************************************************************************************
	void CAISystem::AddAnimal(CAnimal *pani)
	{
#if VER_DEBUG
		// Make sure it isn't already in the list.
		std::list<void*>::iterator ipani = lpaniInactiveAnimals.begin();

		bool b_found_it = false;

		for ( ; ipani != lpaniInactiveAnimals.end(); ++ipani)
		{
			Assert(*ipani != pani)
		}
#endif

		lpaniInactiveAnimals.push_back(pani);
	}	
		
	//*********************************************************************************************
	void CAISystem::RemoveAnimal(CAnimal *pani)
	{
		DeactivateAnimal(pani);

		// Make sure it isn't already in the list.
		std::list<void*>::iterator ipani = lpaniInactiveAnimals.begin();

		for ( ; ipani != lpaniInactiveAnimals.end(); ++ipani)
		{
			if (*ipani == pani)
			{
				lpaniInactiveAnimals.erase(ipani);
				return;
			}
		}

		// Should have found it by now!
		Assert(false);
	}	

	//*********************************************************************************************
	void CAISystem::ActivateAnimal(CAnimal *pani)
	{
		int i;
		if (pani->bDead() || bSleep || bBoring)
			return;

#if VER_TEST
		{
			// Make sure that the graph is valid for all active animals.
			for (i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
			{
				if (apaniActiveAnimals[i])
				{
					AlwaysAssert(apaniActiveAnimals[i]->pbrBrain->paigGraph);
				}
			}
		}
#endif

		TReal r_farthest_dino_sqr = (gpPlayer->v3Pos() - pani->v3Pos()).tLenSqr();
		int i_farthest_dino = -1;
		int i_free_index = -1;

		for (i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
		{
			// It's already here!
			if (apaniActiveAnimals[i] == pani)
				return;

			// Is there an animal in this index?
			if (!apaniActiveAnimals[i])
			{
				// No!  Use this array value.
				i_free_index = i;
			}
			else
			{
				// Is this animal far away?
				TReal r_distance_sqr = (gpPlayer->v3Pos() - apaniActiveAnimals[i]->v3Pos()).tLenSqr();
				if (r_distance_sqr > r_farthest_dino_sqr)
				{
					r_farthest_dino_sqr = r_distance_sqr;
					i_farthest_dino = i;
				}
			}
		}

		// Did we find a slot?
		if (i_free_index != -1)
		{
			// Yes!  Activate.
			apaniActiveAnimals[i_free_index] = pani;

			// Remove from inactive list.
			std::list<void*>::iterator ipani = lpaniInactiveAnimals.begin();

			bool b_found_it = false;

			for ( ; ipani != lpaniInactiveAnimals.end(); ++ipani)
			{
				if (*ipani == pani)
				{
					b_found_it = true;
					lpaniInactiveAnimals.erase(ipani);
					break;
				}
			}

			// Activated animal was not in the inactive list!
			Assert(b_found_it);

			// Set up the AI Graph.
			apgraphActiveGraphs[i_free_index]->paniAnimal = pani;
			pani->pbrBrain->paigGraph = apgraphActiveGraphs[i_free_index];
			
			// Wake up the brain.
			pani->pbrBrain->Activate();

#if VER_TEST
			{
				// Make sure that the graph is valid for all active animals.
				for (int i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
				{
					if (apaniActiveAnimals[i])
					{
						AlwaysAssert(apaniActiveAnimals[i]->pbrBrain->paigGraph);
					}
				}
			}

			{
				// Remove from inactive list.
				std::list<void*>::iterator ipani = lpaniInactiveAnimals.begin();
				for ( ; ipani != lpaniInactiveAnimals.end(); ++ipani)
				{
					if (*ipani == pani)
					{
						AlwaysAssert(false);
					}
				}
			}
#endif


			return;
		}
		else
		{
			// Did not find a slot.


			// Did we find someone to boot out?
			if (i_farthest_dino != -1)
			{
#if VER_TEST
				dout << "BOOTING " << apaniActiveAnimals[i_farthest_dino]->strGetInstanceName() << " in favor of " << pani->strGetInstanceName() << "\n";
#endif

				// Yes!  Boot them.
				DeactivateAnimal(apaniActiveAnimals[i_farthest_dino]);

				// Now take that slot.
				ActivateAnimal(pani);

#if VER_TEST
				{
					// Remove from inactive list.
					std::list<void*>::iterator ipani = lpaniInactiveAnimals.begin();
					for ( ; ipani != lpaniInactiveAnimals.end(); ++ipani)
					{
						if (*ipani == pani)
						{
							AlwaysAssert(false);
						}
					}
				}
#endif

			}
			else
			{
				// Cannot boot anyone out.  Just fail to activate.
#if VER_TEST
				dout << "NO ROOM!  Failed to activate " << pani->strGetInstanceName() << "\n";
#endif
			}
		}
	}

	//*********************************************************************************************
	void CAISystem::DeactivateAnimal(CAnimal *pani)
	{

		int i;
		for (i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
		{
			if (apaniActiveAnimals[i] == pani)
			{
				apaniActiveAnimals[i] = 0;
				lpaniInactiveAnimals.push_back(pani);

				// Let the physics system know about this raptor!
				// No, skip this.  Let the general AI cleanup handle it.
//				pani->pphiGetPhysicsInfo()->Deactivate(pani);

				// Set up the AI Graph.
				apgraphActiveGraphs[i]->paniAnimal = 0;
				pani->pbrBrain->paigGraph = 0;

				// And shut down the brain.
				pani->pbrBrain->Deactivate();

#if VER_TEST
				{
					int i;
					for (i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
					{
						AlwaysAssert(apaniActiveAnimals[i] != pani);
					}
				}
#endif
				return;
			}
		}

#if VER_TEST
		// Make sure that the graph is valid for all active animals.
		for (i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
		{
			if (apaniActiveAnimals[i])
			{
				AlwaysAssert(apaniActiveAnimals[i]->pbrBrain->paigGraph);
			}
		}
#endif
	}


	//*********************************************************************************************
	void CAISystem::ActivateAnimals(CVector3<> v3_center, TReal r_radius)
	{
		// square the radius
		r_radius *= r_radius;

		// Loop through inactive animals and save a pointer to them if appropriate.
		std::list<void*> lpani;
		
		// Search inactive list.
		std::list<void*>::iterator ipani = lpaniInactiveAnimals.begin();

		for ( ; ipani != lpaniInactiveAnimals.end(); ++ipani)
		{
			CAnimal* pani = (CAnimal*)*ipani;
			if ((pani->v3Pos() - v3_center).tLenSqr() < r_radius)
			{
				lpani.push_back(*ipani);
			}
		}

		ipani = lpani.begin();
		for ( ; ipani != lpani.end(); ++ipani)
		{
			ActivateAnimal((CAnimal*)*ipani);
		}
	}

	//*********************************************************************************************
	void CAISystem::DeactivateAnimals(CVector3<> v3_center,	TReal r_radius)
	{
		// Square the radius.
		r_radius *= r_radius;

		for (int i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
		{
			if (apaniActiveAnimals[i] && (apaniActiveAnimals[i]->v3Pos() - v3_center).tLenSqr() < r_radius)
			{
				DeactivateAnimal(apaniActiveAnimals[i]);
			}
		}
	}

	//*********************************************************************************************
	void CAISystem::AlertAnimals(CInstance* pins, TReal r_radius)
	{
		CVector3<> v3 = pins->v3Pos();

		for (int i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
		{
			if (apaniActiveAnimals[i] && (apaniActiveAnimals[i]->v3Pos() - v3).tLenSqr() < r_radius * r_radius)
			{
				// Give it a neutral command so that it stops moving.
				apaniActiveAnimals[i]->pbrBrain->pwvWorldView->See(pins, gaiSystem.sNow);;
				return;
			}
		}
	}

	//*********************************************************************************************
	void CAISystem::Handle3DSound(const CVector3<>&	v3_location, float f_master_volume, float f_distance_attenuation)
	{
		// Until sounds are a bit more predictable, do not excite dinos based on sound.
		return;

		Handle3DStartle(v3_location, f_master_volume + 10.0f, f_distance_attenuation);
	
	}

	//*********************************************************************************************
	void CAISystem::Handle3DStartle(const CVector3<>& v3_location, float f_excitement, float f_distance_attenuation)
	{
		// At what distance do we get startled?
		TReal r_radius = 10000.0f;
		if (f_distance_attenuation < 0)
			r_radius = - f_excitement / f_distance_attenuation;

		Handle3DStartle(v3_location, r_radius);
	}

	//*********************************************************************************************
	void CAISystem::Handle3DStartle(const CVector3<>& v3_location, TReal r_radius)
	{
		for (int i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
		{
			if (apaniActiveAnimals[i] && (apaniActiveAnimals[i]->v3Pos() - v3_location).tLenSqr() < r_radius * r_radius)
			{
				apaniActiveAnimals[i]->pbrBrain->Startle(v3_location);
			}
		}
	}

	//*********************************************************************************************
	//
	void CAISystem::ThinkAwake()
	{
		// For each dino, call its brain's Think() function.
		// In the future, this function will take into account a notion of 
		// "sleeping" dinos.
		static int i_last_thinker = -1;

		// Only let one animal think per frame.
		// Start with the animal just after the last thinker.
		for (i_last_thinker++; i_last_thinker < iMAX_ACTIVE_ANIMALS; i_last_thinker++)
		{
			CAnimal* pani = apaniActiveAnimals[i_last_thinker];

			if (pani)
			{
				{
					// Give us a clean slate if we are about to think the selected dino.
					if (pconDebugConsole && pinsSelected == pani)
						pconDebugConsole->ClearScreen();

					// Then actually think.
					pani->pbrBrain->Think();
				}

				return;
			}
		}

		// We found no animals.  Prepare for the animal next frame.
		i_last_thinker = -1;
	}


	//*********************************************************************************************
	void CAISystem::SetCurrentBrain
	(
		CBrain *pbr
	)
	//**************************************
	{
		Assert(pbr);
		
		pbrCurrentBrain = pbr;
		SetCurrentSynthesizer(pbr->psynSynthesizer);

		// And the velocity....
		CAnimal* pani = paniGetCurrentAnimal();
		const CPhysicsInfo* pphi = pani->pphiGetPhysicsInfo();

		// If we are moving quickly
		v3CurrentVelocity	= pphi->p3GetVelocity(pani).v3Pos;
		rCurrentSpeed		= v3CurrentVelocity.tLen();
	}

	//*********************************************************************************************
	void CAISystem::InitDefaultOpinions()
	{
		for (int ear = earEND - 1; ear >= earBEGIN; ear--)
		{
			// Everybody drinks!
			afeelDefaultOpinions[ear][eaiWATER][eptTHIRST] = 1.0f;
		}

		CFeeling feel_carnivore_default;
		feel_carnivore_default[eptFEAR]		= 1.0f;		// Overridden by Danger * (1 - Brave) & TEAM
		feel_carnivore_default[eptLOVE]		= 0.0f;		// Overridden by Team
		feel_carnivore_default[eptANGER]	= 1.0f;		// Overridden by Bravery & TEAM
		feel_carnivore_default[eptCURIOSITY]= 1.0f;		
		feel_carnivore_default[eptHUNGER]	= 1.0f;		// Overridden by TEAM
		feel_carnivore_default[eptTHIRST]	= 0.0f;
		feel_carnivore_default[eptFATIGUE]	= 1.0f;
		feel_carnivore_default[eptPAIN]		= 1.0f;		
		feel_carnivore_default[eptSOLIDITY]	= 0.5f;		// Overrriden by AI Mass

		CFeeling feel_herbivore_default;
		feel_herbivore_default[eptFEAR]		= 1.0f;		// Overridden by Danger * (1 - Brave) & TEAM
		feel_herbivore_default[eptLOVE]		= 0.0f;		// Overridden by Team
		feel_herbivore_default[eptANGER]	= 1.0f;		// Overridden by 1 - FEAR	& TEAM
		feel_herbivore_default[eptCURIOSITY]= 1.0f;		
		feel_herbivore_default[eptHUNGER]	= 0.0f;		
		feel_herbivore_default[eptTHIRST]	= 0.0f;
		feel_herbivore_default[eptFATIGUE]	= 1.0f;
		feel_herbivore_default[eptPAIN]		= 1.0f;		
		feel_herbivore_default[eptSOLIDITY]	= 0.5f;		// Overrriden by AI Mass


		// Carnivores....
		afeelDefaultOpinions[earCARNIVORE][eaiANIMAL]			= feel_carnivore_default;
		afeelDefaultOpinions[earCARNIVORE][eaiHUMAN]			= feel_carnivore_default;
		
		afeelDefaultOpinions[earCARNIVORE][eaiMEAT]				= feel_carnivore_default;
		afeelDefaultOpinions[earCARNIVORE][eaiMEAT][eptANGER]	= 0.0f;
		afeelDefaultOpinions[earCARNIVORE][eaiMEAT][eptPAIN]	= 0.0f;
		afeelDefaultOpinions[earCARNIVORE][eaiMEAT][eptFEAR]	= 0.0f;


		// Herbivores....
		afeelDefaultOpinions[earHERBIVORE][eaiANIMAL]			= feel_herbivore_default;
		afeelDefaultOpinions[earHERBIVORE][eaiHUMAN]			= feel_herbivore_default;
		
		afeelDefaultOpinions[earHERBIVORE][eaiGRASS]			= feel_carnivore_default;
		afeelDefaultOpinions[earHERBIVORE][eaiGRASS][eptANGER]	= 0.0f;
		afeelDefaultOpinions[earHERBIVORE][eaiGRASS][eptPAIN]	= 0.0f;
		afeelDefaultOpinions[earHERBIVORE][eaiGRASS][eptFEAR]	= 0.0f;

		///
		///		Init the default samples.
		///

		TSoundHandle* asndhnd = 0;

		TSoundMaterial tmat = sndhndHashIdentifier("PLACEHOLDER VOCAL");

		// Default the entire array to no sounds.
		for (int i_sample = 0; i_sample < evtEND * edtEND; ++i_sample)
			for (int i_vocal = 0; i_vocal < iNUM_VOCALS; ++i_vocal)
			{
				avsSamples[i_sample][i_vocal].sndhnd	= tmat;
				avsSamples[i_sample][i_vocal].sOpen		= 0;
				avsSamples[i_sample][i_vocal].sClose	= 0;
			}
	}

	//*********************************************************************************************
	void CAISystem::Process(const CMessagePaint& msgpaint)
	{
		if (bShow3DInfluences && pinsSelected && ptCast<CAnimal>(pinsSelected))
		{
			// We have selected an animal.  Draw wireframe boxes around influences.
			CAnimal* pani = ptCast<CAnimal>(pinsSelected);

			// Construct draw object for target raster.
			rptr<CRaster> pras = rptr_this(msgpaint.renContext.pScreenRender->prasScreen);
			rptr<CRasterWin> prasw = rptr_dynamic_cast(CRasterWin, pras);
			if (!prasw)
				return;

			CDraw draw(prasw);

			pani->pbrBrain->Draw3DInfluences(draw, msgpaint.camCurrent);
		}
	}

	//*********************************************************************************************
	void CAISystem::Process( const CMessageStep& msgs)
	{
		// Do nothing if we aren't active.
		if (!bActive)
			return;
	
		if (bSleep || bBoring)
		{
			// Put those dinos to sleep!
			DeactivateAnimals(CVector3<>(0,0,0), 100000.0f);
		}

		// Deactivate the useless dinos.
		int i;
		for (i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
		{
			CAnimal* pani = apaniActiveAnimals[i];
			if (pani)
			{
				// Now see if we can this guy to sleep.  Is he dead or far away?
				if (pani->bReallyDead() || 
					(pani->v3Pos() - gpPlayer->v3Pos()).tLenSqr() > pani->pbrBrain->rSleepDistance * pani->pbrBrain->rSleepDistance)
				{
					DeactivateAnimal(pani);	
					pani->pbrBrain->DebugSay("ZZZ  Sleepy Time!  ZZZ\n");
				}
			}
		}

		// Grab the elapsed time.
		sNow		= msgs.sTotal;

		AlwaysAssert(sLastExecution <= sNow);

		// Cap the AI frame rate.
		if (sNow > sLastExecution + sMinInterval)
		{
			// Flag the AI as having a process step message pending.
			bStepPending = true;

			// If the Z buffer is not in use, process the AI right away.
			if (!d3dDriver.bUseD3D())
				ProcessPending();
		}


		// And send every awake dino's control message.
		for (i = iMAX_ACTIVE_ANIMALS - 1; i > 0; i--)
		{
			if (apaniActiveAnimals[i])
			{
				// Let the physics system know about this raptor!
				apaniActiveAnimals[i]->pphiGetPhysicsInfo()->Activate(apaniActiveAnimals[i]);
				apaniActiveAnimals[i]->pbrBrain->SendPhysicsCommandMessage();
			}
		}

		// Iterate through the inactive list, and wake up the first guy within X distance.
		std::list<void*>::iterator ipani = lpaniInactiveAnimals.begin();

		for ( ; ipani != lpaniInactiveAnimals.end(); ++ipani)
		{
			CAnimal* pani = (CAnimal*) *ipani;

			// Multiplier for more wakeable dinos.
			float f_mul = 1.0f;

			if (!bSleep &&
				!bBoring &&
				!pani->bDead() && 
				(pani->v3Pos() - gpPlayer->v3Pos()).tLenSqr() < pani->pbrBrain->rWakeDistance * pani->pbrBrain->rWakeDistance * f_mul)
			{
			}
			else if (!pani->bDead())
			{
				// It's not dead, and it's not AI awake, so make sure it is asleep.
				pani->PhysicsDeactivate();
			}
		}

		ipani = lpaniInactiveAnimals.begin();
		for ( ; ipani != lpaniInactiveAnimals.end(); ++ipani)
		{
			CAnimal* pani = (CAnimal*) *ipani;

			// Multiplier for more wakeable dinos.
			float f_mul = 1.0f;

			if (!bSleep &&
				!bBoring &&
				!pani->bDead() && 
				(pani->v3Pos() - gpPlayer->v3Pos()).tLenSqr() < pani->pbrBrain->rWakeDistance * pani->pbrBrain->rWakeDistance * f_mul)
			{
				ActivateAnimal(pani);				
#if VER_TEST
				char buffer[256];
				sprintf(buffer, "Yawn.  Waking up at %f meters.\n", (pani->v3Pos() - gpPlayer->v3Pos()).tLen());
				pani->pbrBrain->DebugSay(buffer);
#endif
				return;
			}
			else if (!pani->bDead())
			{
			}

			// Don't be quite so excitable next time.
			pani->pbrBrain->bWakeMe = false;
		}
	}

	//*********************************************************************************************
	void CAISystem::ProcessPending()
	{
		// If no step message is waiting to be processed, do nothing.
		if (!bStepPending)
			return;
		bStepPending = false;

		// For temp AI spike purposes, make sure we have clean stats.
//		proProfile.psAI.Reset();

		// Profile stat timer.
		CCycleTimer ctmr;

		CTimerLocal		tmrAITimer;		// A profile timer for the entire AI cycle.

		TMSec	ms = tmrAITimer.msElapsed();

		// Sensory input to wakeful dinos.
		HandleSensoryInput();
	
		// Let each dino think
		ThinkAwake();

		// Get the time elapsed for profiling.
		msAITimeTaken	= tmrAITimer.msElapsed() - ms;

		// Save the profile stat.
		proProfile.psAI.Add(ctmr());

		// And remember that we processed the AI.
		sLastExecution = sNow;

/*
		static int i_total_ai_executions = 0;
		static int i_total_ai_spikes = 0;
		static TSec s_ai_total = 0;
		static TSec s_worst_spike = 0.0f;
		i_total_ai_executions++;
		s_ai_total += proProfile.psAI.fSeconds();

		if (proProfile.psAI.fSeconds() > .01)
		{
			i_total_ai_spikes++;

			if (proProfile.psAI.fSeconds() > s_worst_spike)
				s_worst_spike = proProfile.psAI.fSeconds();


			extern CProfileStat psSynthesizer;
			extern CProfileStat psPathfinder;
			extern CProfileStat psAnalyzer;
			extern CProfileStat psAnal2;
			extern CProfileStat psSearch;

			dout << "AI TIME!  \tSec: ";
			dout << proProfile.psAI.fSeconds() << "   (worst " << s_worst_spike << ")\n";
			dout << "Synth: " << psSynthesizer.fSeconds() << "\t";
			dout << "Path: " << psPathfinder.fSeconds() << "\n";
			dout << "Analyze: " << psAnalyzer.fSeconds() << "\n";
			dout << "Anal2: " << psAnal2.fSeconds() << "\n";
//			dout << "Vocal: " << psVocal.fSeconds() << "\n";
			dout << "Search: " << psSearch.fSeconds() << "\n";

			dout << "Spike proportion = " << float(i_total_ai_spikes) / float(i_total_ai_executions) << "\n";
			dout << "AI Total = " << s_ai_total << " of " << CMessageStep::sElapsedRealTime << "-- prop: " << s_ai_total / CMessageStep::sElapsedRealTime << "\n";


		}
*/
	}


	void CAISystem::GetOpinion(const CAnimal* pani_self, const CInstance* pins_target, CFeeling *pfeel_return) const
	{
		EArchetype ear_self		= pani_self->pbrBrain->earArchetype;

		const CAIInfo* paii = pins_target->paiiGetAIInfo();

		const CAnimate* panimate_target = pins_target->paniGetOwner();
		const CAnimal*  panimal_target  = ptCast<CAnimal >((CInstance*)panimate_target);


		EAIRefType eai = paii->eaiRefType;

		// Are we on the same team?   And we are not team zero (free-for-all team)
		bool b_same_team = pani_self->iTeam != 0 && panimal_target && pani_self->iTeam == panimal_target->iTeam;

		if (!b_same_team)
		{
			// Only treat non-teammates as meat.

			// Is it a dead animal?
			if (panimate_target && panimate_target->bDead())
			{
				// Yes!  Treat it as just so much meat.
				eai = eaiMEAT;
			}
		}
		
		*pfeel_return = afeelDefaultOpinions[ear_self][eai];

		// This is only the default feeling!  Must adjust it for....
		
		if (b_same_team)
		{
			// We love our teammates.
			(*pfeel_return)[eptLOVE] = 1.0f;

			// We do not fear or eat our teammates.
			(*pfeel_return)[eptFEAR]	= 0.0f;
			(*pfeel_return)[eptHUNGER]	= 0.0f;
			(*pfeel_return)[eptCURIOSITY]	= 0.0f;


			// But we might be angry at them.
			(*pfeel_return)[eptANGER]	= 0.0f;
		}

		// Danger
		(*pfeel_return)[eptFEAR]    *= paii->rtDanger;
		(*pfeel_return)[eptPAIN]    *= paii->rtDanger;

		// Bravery
		(*pfeel_return)[eptANGER]   *= pani_self->pbrBrain->rtBravery;

		// Fatigue always 1.0f.
		(*pfeel_return)[eptFATIGUE] =  1.0f;


		// human prejudice
		if (paii->eaiRefType == eaiHUMAN && pani_self->pbrBrain->bOverrideHumanFeeling)
			(*pfeel_return) = pani_self->pbrBrain->feelHumanFeeling;

		// Reduce solidity to zero if we have a no pathfind object.
		if (!paii->bPathfinding())
			(*pfeel_return)[eptSOLIDITY] = 0;
		else
		{
			if (b_same_team)
			{
				(*pfeel_return)[eptSOLIDITY] = 1.0f;
			}
			else
			{
				(*pfeel_return)[eptSOLIDITY] = paii->rtGetAIMass(pins_target);
			}
		}
	}

	//*********************************************************************************************
	const CVector2<>& CAISystem::v2GetLastDesiredDirection
	(
	) const
	{
		return pbrCurrentBrain->pmsState->v2LastDirection;
	}

	//*********************************************************************************************
	void CAISystem::DebugPrint
	(
		const char *ac_string
	) const
	{
		Assert(pconDebugConsole);

		pconDebugConsole->Print(ac_string);
	}

	//*********************************************************************************************
	const CVector3<> CAISystem::v3GetBodyLocation
	(
	) const
	{
		Assert(pbrCurrentBrain);
		return pbrCurrentBrain->paniOwner->v3Pos();
	}

	//*********************************************************************************************
	const CVector2<> CAISystem::v2GetBodyFacing() const
	{
		CVector2<> v2_facing = CVector3<>(0,1,0) * pbrCurrentBrain->paniOwner->r3Rot();
		AlwaysAssert(!v2_facing.bIsZero());
		v2_facing.Normalise();
		return v2_facing;
	}

	//*********************************************************************************************
	const CVector3<> CAISystem::v3GetHeadLocation
	(
	) const
	{
		Assert(pbrCurrentBrain);
		return pbrCurrentBrain->paniOwner->v3GetHeadPos();
	}

	//*********************************************************************************************
	const CVector3<> CAISystem::v3GetHeadPoint() const
	{
		return v3GetHeadLocation() - v3GetBodyLocation();
	}

	//*********************************************************************************************
	const CVector3<> CAISystem::v3GetTailPoint() const
	{
		return pbrCurrentBrain->paniOwner->v3GetTailPos() - v3GetBodyLocation();
	}

	//*********************************************************************************************
	const CVector2<> CAISystem::v2GetBodyLocation
	(
	) const
	{
		Assert(pbrCurrentBrain);
		return pbrCurrentBrain->paniOwner->v3Pos();
	}

	//*********************************************************************************************
	const CVector2<> CAISystem::v2GetBodyVelocity
	(
	) const
	{
		Assert(pbrCurrentBrain);
		Assert(0);
		return v2Zero;
//			return pbrCurrentBrain->petOwner->v2Velocity;
	}

	//*********************************************************************************************
	const CVector2<> CAISystem::v2GetDestination
	(
	) const
	{
		return pbrCurrentBrain->v2GetDestination();
	}

	//*********************************************************************************************
	CAnimal* CAISystem::paniGetCurrentAnimal
	(
	)
	{
		return pbrCurrentBrain->paniOwner;
	}

// Vocal constants.
//#define iSAMPLES_PER_PAIR	(iNUM_TOTAL_VOCALS / (edtEND * evtEND))
#define iDINO_STRIDE		(evtEND)

	//*********************************************************************************************
	void CAISystem::ProcessAICommand
	(
		CGroffObjectName* pgon,
		CLoadWorld*	pload,
		const CHandle& h,
		CValueTable* pvt,
		const CInfo* pinfo
	)
	{
		SETUP_TEXT_PROCESSING(pvt, pload)
		SETUP_OBJECT_HANDLE(h)
		{
			int i_type = 0;

			// Which sort of AI command is this?
			bFILL_INT(i_type, esType);

			switch (i_type)
			{
				// AI Vocal specification.
				case 0:
				{
					int i_dino = -1;
					int i_vocal = -1;
					bFILL_INT(i_dino, esDinosaur);
					bFILL_INT(i_vocal, esVocal);

					// Verify that we have valid dino index.
					AlwaysAssert(i_dino >= 0 && i_dino < edtEND);
					
					// Verify that we have a valid vocal index.
					AlwaysAssert(i_vocal >= 0 && i_vocal < evtEND);

					// Now get the vocals array.
					TVocalSet* pvs = pvsGetVocalSet(EDinoType(i_dino), EVocalType(i_vocal));

					// Now fill it up again.
					for (int i = 0; i < iNUM_VOCALS; ++i)
					{
						// Clear the hash value.
						(*pvs)[i].sndhnd	= 0;
						(*pvs)[i].sOpen		= 0.0f;
						(*pvs)[i].sClose	= 0.0f;

						const CEasyString* pestr = 0;

						// Did we find a sample?
						if (bFILL_pEASYSTRING(pestr, ESymbol(esA00 + i)))
						{
							// Yes!  Save the hash value.
							(*pvs)[i].sndhnd = sndhndHashIdentifier(pestr->strData());
						}
						else 
						{
							// No!  See if we find an object.
							IF_SETUP_OBJECT_SYMBOL(ESymbol(esA00 + i))
							{
								// Yes!  Get sample, open time, and close time.
								AlwaysVerify(bFILL_pEASYSTRING(pestr, esTActionSample));   // "Sample"
								(*pvs)[i].sndhnd = sndhndHashIdentifier(pestr->strData());
								
								float f;
								if (bFILL_FLOAT(f, esOpen))
								{
									(*pvs)[i].sOpen = f;
								}

								if (bFILL_FLOAT(f, esClose))
								{
									(*pvs)[i].sClose = f;
								}
							}
							END_OBJECT
						}
					}

					break;
				}
				default:
				{
					// Unknown AI system command!
					AlwaysAssert(false);
					break;
	
				}
			}
		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}



	//*********************************************************************************************
	TVocalSet* CAISystem::pvsGetVocalSet
	(
		EDinoType edt,
		EVocalType evt
	)
	{
		return &avsSamples[edt * iDINO_STRIDE + evt];
	}

	//*********************************************************************************************
	int CAISystem::iNumVocalHandles
	(
		EDinoType edt,
		EVocalType evt
	)
	{
		// Slow, but who cares?
		TVocalSet* pvs = pvsGetVocalSet(edt,evt);

		// Look for a zero.
		int i;
		for (i = 0; i < iNUM_VOCALS; ++i)
		{
			if ((*pvs)[i].sndhnd == 0)
			{
				return i;
			}
		}

		// We found all samples valid.  Return the max number, please.
		return i;
	}


	//*****************************************************************************************
	char *CAISystem::pcSave(char * pc) const
	{


		pc = pcSaveT(pc, bSleep);

		// Should save the random number generator.
		pc = rnd.pcSave(pc);

		pc = pcSaveT(pc, sNow);
		pc = pcSaveT(pc, sLastExecution);
		pc = pcSaveT(pc, msAITimeTaken);

		int i;
		for (i = 0; i < iMAX_ACTIVE_ANIMALS; ++i)
		{
			pc = pcSaveInstancePointer(pc, apaniActiveAnimals[i]);
		}
		// Backwards save file compatability.
		AlwaysAssert(iMAX_ACTIVE_ANIMALS <= 10);
		for (i = iMAX_ACTIVE_ANIMALS; i < 10; ++i)
		{
			pc = pcSaveInstancePointer(pc, 0);
		}

		// Now save the inactive animal pointers.
		int i_inactive = lpaniInactiveAnimals.size();
		pc = pcSaveT(pc, i_inactive);

		// Make sure it isn't already in the list.
		std::list<void*>::const_iterator ipani = lpaniInactiveAnimals.begin();
		for ( ; ipani != lpaniInactiveAnimals.end(); ++ipani)
		{
			pc = pcSaveInstancePointer(pc, (CAnimal*)*ipani);

#if VER_DEBUG
			--i_inactive;
			Assert(i_inactive >= 0);
#endif
		}


		return pc;
	}

	//*****************************************************************************************
	const char *CAISystem::pcLoad(const char * pc)
	{
		if (CSaveFile::iCurrentVersion >= 13)
			pc = pcLoadT(pc, &bSleep);

		// Should save the random number generator.
		pc = rnd.pcLoad(pc);

		pc = pcLoadT(pc, &sNow);
		pc = pcLoadT(pc, &sLastExecution);
		pc = pcLoadT(pc, &msAITimeTaken);


		// Deactivate all animals.
		DeactivateAnimals(CVector3<>(0,0,0), 100000.0f);
		
		int i;
		for (i = 0; i < iMAX_ACTIVE_ANIMALS; ++i)
		{
			CInstance * pins = 0;

			pc = pcLoadInstancePointer(pc, &pins);

			if (pins)
			{
				Assert(ptCast<CAnimal>(pins));
				ActivateAnimal((CAnimal*)pins);
			}
		}

		// Backwards save file compatability.
		AlwaysAssert(iMAX_ACTIVE_ANIMALS <= 10);
		for (i = iMAX_ACTIVE_ANIMALS; i < 10; ++i)
		{
			CInstance* pins = 0;
			pc = pcLoadInstancePointer(pc, &pins);
		}


		// Load inactive animals.

		// Get the total number.
		int i_inactive = 0;

		pc = pcLoadT(pc, &i_inactive);

		CInstance* pins = 0;
		for (; i_inactive > 0; --i_inactive)
		{
			pc = pcLoadInstancePointer(pc, &pins);
//			Assert(ptCast<CAnimal>(pins));
//			((CAnimal*)pins)->pbrBrain->paigGraph = 0;
//			lpaniInactiveAnimals.push_back(pins);
		}

		return pc;
	}




//**********************************************************************************************
//
//	Class CAIInfo implementation.
//

	CAIInfo::CAIInfo(EAIRefType eai, bool b_hack_unique)
	: eaiRefType(eai)
	{
		setFlags[eaifUNIQUE] = b_hack_unique;
		setFlags[eaifIGNORE] = false;		
		setFlags[eaifPATHFINDING] = true;

		fAIMass = 2.0f;

		// Most things can be walked upon.
		setFlags[eaifCLIMB] = true;

		MEMLOG_ADD_COUNTER(emlTotalAIInfo,1);
	}

	CAIInfo::CAIInfo
	(
		const CGroffObjectName*		pgon,
		const CHandle&				h_obj,				// Handle to the object in the value table.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	)
	{
		// Snag the RefType.
		int eai = eaiUNKNOWN;

		// Default to intangible, and therefore no AI.
		bool b_AI =			false;

		// Other flags
		bool b_pathfind =	true;
		bool b_climb =		true;

		float f_danger = 0.0f;

		// Default to choosing our own, indicated by the value listed.
		float f_ai_mass = 2.0f;

		SETUP_TEXT_PROCESSING(pvtable, pload);
		SETUP_OBJECT_HANDLE(h_obj)
		{
			// Use tangibility to determine b_AI state, but override if specified.
			bFILL_BOOL(b_AI, esTangible);
			bFILL_BOOL(b_AI, esAI);

			bFILL_BOOL(b_pathfind,	esPathfind);
			b_climb = false;
			bFILL_BOOL(b_climb,		esClimb);

			if (!b_pathfind)
				f_ai_mass = 0.0f;

			bFILL_FLOAT(f_ai_mass, esAIMass);
			bFILL_INT(eai, esAIType);
			bFILL_FLOAT(f_danger, esDanger);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

#if VER_TEST
//		Assert(eai >= 0 && eai < eaiEND);
		if (eai >= eaiEND)
		{
			dout << "Out of bounds AI Type.\n";
			eai = eaiHUMAN;
		}
#endif

		fAIMass = f_ai_mass;
		rtDanger = f_danger;

		//  Fill the variables.
		eaiRefType = EAIRefType(eai);

		setFlags[eaifIGNORE]		= !b_AI;
		setFlags[eaifPATHFINDING]	= b_pathfind;
		setFlags[eaifCLIMB]			= b_climb;

		MEMLOG_ADD_COUNTER(emlTotalAIInfo,1);
	}
	
	
	typedef std::set<CAIInfo, std::less<CAIInfo> > TSAI;
	// This guy is here instead of in the class header to cut down on compile times.
	// Additionally, only the world dbase need know about it (for purging)
	TSAI tsaiAIInfo;	// A set containing all shared AI infos, for instancing.

	//*****************************************************************************************
	const CAIInfo* CAIInfo::paiiFindShared
	(
		const CAIInfo& aii
	)
	{
		// Insert or find, please.
		std::pair<TSAI::iterator, bool> p = tsaiAIInfo.insert(aii);

		// If we found a duplicate, it will do.
		// If we inserted a new one, the new one will do.
		const CAIInfo* pai_ret = &(*p.first);
		return pai_ret;
	}

	//*****************************************************************************************
	const CAIInfo* CAIInfo::paiiFindShared
	(
		const CGroffObjectName*		pgon,
		const CHandle&				h_obj,				// Handle to the object in the value table.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	)
	{
		CAIInfo aii(pgon, h_obj, pvtable, pload);
		return paiiFindShared(aii);
	}

	//*****************************************************************************************
	const CBoundVol* CAIInfo::pbvGetBoundVol(const CInstance* pins) const
	{
		const CPhysicsInfo* pphi = pins->pphiGetPhysicsInfo();
		Assert(pphi);

		// Use the physics volume if tangible, else the render info.
		if (pphi->bIsTangible())
		{
			return pphi->pbvGetBoundVol();
		}
		else
		{
			rptr_const<CRenderType> prdt = pins->prdtGetRenderInfo();

			Assert(prdt);
			return &prdt->bvGet();
		}
	}

	//*************************************************************************************
	CRating CAIInfo::rtGetAIMass(const CInstance* pins) const
	{
		#define fMAX_MASS 20.0f

		if (fAIMass <= 1.0f)
			return fAIMass;
		else
		{
			const CPhysicsInfo* pphi = pins->pphiGetPhysicsInfo();
			// Is it possible to move this thing?
			if (pphi->bIsMovable())
			{
				// Yes.  See if WE can move it.
				Assert(fAIMass >= 0.0f);
				// Pick AI Mass ourselves.
				float f_mass = pphi->fMass(pins);
				if (f_mass > fMAX_MASS)
					return 1;

				f_mass = f_mass / fMAX_MASS;
				return f_mass;
			}
			else
			{
				return 1;
			}
		}
	}

#if VER_TEST
char* astrAITypes[] = {

	"eaiDONTCARE",
	"eaiUNKNOWN",
	"eaiANIMAL",
	"eaiMEAT",
	"eaiGRASS",
	"eaiHUMAN",
	"eaiNODEHINT",
	"eaiWATER",
	"eaiEND"
};


	//*****************************************************************************************
	int CAIInfo::iGetDescription
	(
		char *	pc_buffer,
		int		i_buffer_len
	) const
	{
		if (bIgnore())
		{
			strcat(pc_buffer, "Ignored by AI.\n");
		}
		else
		{
			strcat(pc_buffer, "AI Type: ");
			strcat(pc_buffer, astrAITypes[eaiRefType]);

			if (bPathfinding())
			{
				strcat(pc_buffer, "\n  Used for pathfinding.\n");
				if (bClimb())
				{
					strcat(pc_buffer, "  Can step on surface.\n");
				}
			}
			else
			{
				strcat(pc_buffer, "\n  NOT USED for pathfinding.\n");
			}

			char buffer[256];
			sprintf(buffer, "  Danger: %f\n", rtDanger.fVal);
			strcat(pc_buffer, buffer);
		}



		return strlen(pc_buffer);
	}
#endif

	//*****************************************************************************************
	bool CAIInfo::operator<
	(
		const CAIInfo& aiinfo
	) const
	{
		return 0 > memcmp(this, &aiinfo, sizeof(CAIInfo));
	}

