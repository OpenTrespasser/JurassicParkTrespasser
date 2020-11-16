/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of Brain.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Brain.cpp                                                    $
 * 
 * 163   10/02/98 6:10p Agrant
 * faster move message handling
 * 
 * 162   9/26/98 9:12p Agrant
 * turn optimizing on for this file
 * 
 * 161   9/23/98 10:31p Agrant
 * Red line colour for 2d influence debugging
 * 
 * 160   9/22/98 1:07p Agrant
 * handle savegames better (resetting important variables)
 * 
 * 159   9/20/98 7:25p Agrant
 * use flag for drawing 2d influence solidity
 * 
 * 158   9/20/98 4:24p Jpaque
 * don't crash when you have too many triangles in a single quad node
 * 
 * 157   9/19/98 9:01p Agrant
 * include rear back
 * 
 * 156   9/19/98 1:43a Agrant
 * waste less activity memory
 * 
 * 155   9/16/98 9:19p Agrant
 * Handle most emotions in an exclusive block
 * 
 * 154   9/14/98 10:17p Agrant
 * draw influences based on flags rather than solidity
 * 
 * 153   9/13/98 8:04p Agrant
 * reduce wake and sleep distances
 * 
 * 152   9/11/98 12:42p Agrant
 * differentiate between slopes too steep to climb and too steep to descend
 * 
 * 151   9/10/98 4:13p Agrant
 * Completely new brain save function
 * 
 * 150   9/10/98 12:01p Agrant
 * moved physics commands to cpp file
 * no move commands after death
 * only send jump once
 * 
 * 149   9/08/98 7:39p Agrant
 * init wake me flag to false
 * 
 * 148   9/08/98 4:43p Agrant
 * dont load useless and damaging data
 * 
 * 147   9/06/98 4:58p Agrant
 * oops.  Better max node count defaults
 * 
 * 146   98/09/05 23:26 Speter
 * Added safety in test code.
 * 
 * 145   9/05/98 8:28p Agrant
 * before synthesizing, reset damage 
 * 
 * 144   9/05/98 4:38p Agrant
 * flag to enable/disable terrain pathfinding
 * 
 * 143   9/02/98 7:56p Agrant
 * No longer handling ai graph cleanup
 * 
 * 142   9/02/98 5:23p Agrant
 * pathfinding improved
 * vocal query
 * activities now in a heap
 * 
 * 141   9/02/98 3:58p Agrant
 * Now sharing graphs for all dinos in a level.
 * 
 * 140   8/29/98 11:00p Agrant
 * Don't send physics requests-  AI System will do that for you.
 * 
 * 139   8/29/98 6:15p Rwycko
 * don't check for player in the groff file
 * 
 * 138   8/26/98 2:40p Rvande
 * Changed some double float constants to float.
 * 
 * 137   8/26/98 11:14a Agrant
 * jumping and reworked temp flags
 * 
 * 136   8/25/98 10:34a Rvande
 * Fix for-loop scope bug
 * 
 * 135   8/24/98 10:04p Agrant
 * reduce number of pathfinding nodes
 * 
 * 134   8/22/98 7:15p Agrant
 * added unstartle
 * 
 * 133   8/21/98 12:43a Agrant
 * sniff air implementation
 * 
 * 132   8/20/98 11:38p Agrant
 * added capacity for being startled to dinos
 * 
 * 131   8/20/98 11:09p Agrant
 * removed many activities, better tail reach default
 * 
 * 130   8/17/98 6:49p Agrant
 * activate/deactivate functions
 * 
 * 129   8/17/98 3:56p Agross
 * removed bogus assert
 * 
 * 128   8/16/98 3:21p Agrant
 * Much tweaking to simplify activity balancing
 * 
 * 127   8/14/98 4:04p Agrant
 * fixed null pointer bug
 * 
 * 126   8/14/98 11:36a Agrant
 * when you kill something, remember that it's time to celebrate
 * 
 * 125   8/13/98 6:56p Agrant
 * code to allow dinos to ignore influences that are unattainable
 * 
 * 124   8/12/98 1:56p Agrant
 * maybe fix scene file oops
 * 
 * 123   8/11/98 7:43p Agrant
 * pathfind around terrain triangles instead of quads
 * 
 * 122   8/11/98 2:21p Agrant
 * added lots of asserts.  
 * 
 * 121   7/29/98 9:18p Agrant
 * draw compound influences correctly in 3d
 * 
 * 120   7/29/98 3:08p Agrant
 * moved team, changed activity set
 * 
 * 119   7/25/98 8:03p Agrant
 * fixed broken fatigue emotion
 * 
 * 118   7/24/98 6:28p Agrant
 * make stay near and stay away work independent of load order
 * make mental state in the default part of brain construction
 * 
 * 117   7/23/98 10:17p Agrant
 * Only send physics request message if we aren't Really Dead
 * 
 * 116   7/22/98 10:02p Agrant
 * Reduce dino slope threshhold to 30 degrees
 * 
 * 115   7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 114   7/20/98 12:27p Agrant
 * made getout objectless
 * 
 * 113   7/14/98 9:43p Agrant
 * many pathfinding improvements
 * 
 * 112   7/14/98 4:12p Agrant
 * brains can draw AI influences in 3d
 * 
 * 111   7/09/98 9:26p Agrant
 * only add influences when we have room
 * 
 * 110   7/09/98 4:24p Agrant
 * dinos now really notice player at animate distance
 * 
 * 109   7/08/98 11:26p Agrant
 * better use of worldview see function
 * 
 * 108   7/07/98 8:58p Agrant
 * Different sensory distance for animates
 * 
 * 107   6/30/98 9:19p Agrant
 * better description for brain
 * default team zero
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Brain.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"

#include <string.h>

#include "Lib/EntityDBase/Query/QAI.hpp"
#include "Lib/EntityDBase/Query/QPhysics.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgAudio.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgDelete.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/Physics/InfoCompound.hpp"


#include "AIMain.hpp"
#include "AIGraph.hpp"
#include "Synthesizer.hpp"
#include "Influence.hpp"
#include "MentalState.hpp"
#include "Silhouette.hpp"
#include "WorldView.hpp"
#include "ActivityCompound.hpp"

//#include "Test/AI/TestAnimal.hpp"

#include "Game/AI/MoveActivities.hpp"
#include "Game/AI/ActivityAttack.hpp"
#include "Game/AI/ActivityVocal.hpp"
#include "Game/AI/TestActivities.hpp"
#include "Game/AI/HeadActivities.hpp"
#include "Game/AI/EmotionActivities.hpp"
#include "Game/AI/AIInfo.hpp"

#include "Lib/EntityDBase/Animal.hpp"

#include "Lib/Sys/ConIO.hpp"
#include "Lib/Std/Hash.hpp"

#include "Lib/GROFF/VtParse.hpp"
#include "Lib/Loader/SaveFile.hpp"

#include "Game/DesignDaemon/Player.hpp"

#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/View/LineDraw.hpp"
#include "Lib/Renderer/Camera.hpp"


#if VER_DEBUG
#include "Test/AI/AITestShell.hpp"
#include "Test/AI/AITest.hpp"
#include "Test/Ai/airesource.h"
#endif

CProfileStat psSynthesizer("Synthesizer", &proProfile.psAI);
CProfileStat psAnalyzer("Analyzer", &proProfile.psAI);
CProfileStat psPathfinder("Pathfinder", &psSynthesizer);
CProfileStat psVocal("Vocal", &psSynthesizer);

// For easier debugging.
//#pragma optimize("g", off)

// Version of animal in the save file.
extern int iAnimalVersion;

#define bXY_EQUAL(v3A, v3B) (v3A.tX == v3B.tX && v3A.tY == v3B.tY)
#define PI 3.14159265f

//**********************************************************************************************
//
//	Class CBrain implementation.
//

	//****************************************************************************************
	CBrain::CBrain(CAnimal* pani_owner, EArchetype ear, const char* pc_name = "Generic")
		: earArchetype(ear)
	{
		Assert(pani_owner);

		// Set object's pointer to this.
		paniOwner			= pani_owner;
		paniOwner->pbrBrain	= this;

		// Set name.
		strcpy(acName, pc_name);

		// Drudge init work.
		InitDefaults();
		InitDependents();
	}

	//****************************************************************************************
	CBrain::CBrain
	(
		CAnimal*				pani_owner,
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	)
	{
		Assert(pani_owner);

		// Setup owner.
		paniOwner = pani_owner;

		// Set name.
		strcpy(acName, pgon->strObjectName);

		// Defaults.
		InitDefaults();

		TReal r_max_up_angle = 33.0f;
		TReal r_max_down_angle = 45.0f;

		// Now process text props to get something better than default behavior.
		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_object)
		{
			// Here's where the magic happens!!!

			// Search for each prop in the value table.  
			int i;
			for (i = 0; i < eatEND; ++i)
			{
				bool b_active = false;

				bFILL_BOOL(b_active, ESymbol(esActBASE + i));

				// Activate if true, else deactivate.
				sapactActivities[i]->Activate(b_active);

				// Now look for each rating.
				for (int i_param = 0; i_param < eptEND; i_param++)
				{
					bFILL_FLOAT(sapactActivities[i]->feelRatingFeeling[i_param], ESymbol(i_param + esActRATINGS_BASE + i * eptEND));
				}

			}


			// Load in the feeling.
			float f;
			for (i = 0; i < eptEND; ++i)
			{
				if (bFILL_FLOAT(f, ESymbol(esParameter00 + i)))
				{
					Assert(f >= 0.0f && f <= 1.0f);
					pmsState->feelEmotions[i] = f;
				}

				if (bFILL_FLOAT(f, ESymbol(esHumanParameter00 + i)))
				{
					Assert(f >= 0.0f && f <= 1.0f);
					feelHumanFeeling[i] = f;
					bOverrideHumanFeeling = true;
				}

				if (bFILL_FLOAT(f, ESymbol(esDamageParameter00 + i)))
				{
//					Assert(f >= 0.0f && f <= 1.0f);
					feelDamageFeeling[i] = f;
				}

			}

			// Load archetype!
			int ear = earCARNIVORE;
			bFILL_INT(ear, esArchetype);
			Assert(ear >= 0);
			Assert(ear < earEND);
			earArchetype = EArchetype(ear);

			float f_bravery = 0.5;
			bFILL_FLOAT(f_bravery, esBravery);
			rtBravery = f_bravery;

			// Load the physical dino type.
			int edt = edtRAPTOR;
			bFILL_INT(edt, esDinosaur);
			Assert(edt >= 0);
			Assert(edt < edtEND);
			edtDinoType = EDinoType(edt);

			// Self-image

			bFILL_FLOAT(rWidth, esWidth);
			bFILL_FLOAT(rHeadReach, esHeadReach			);
			bFILL_FLOAT(rTailReach, esTailReach			);
			bFILL_FLOAT(rClawReach, esClawReach			);
			bFILL_FLOAT(rJumpDistance, esJumpDistance		);
			bFILL_FLOAT(rJumpUp, esJumpUp			);
			bFILL_FLOAT(rJumpDown, esJumpDown			);

			bFILL_FLOAT(rWalkOver, esWalkOver			);
			bFILL_FLOAT(rWalkUnder, esWalkUnder			);
			// HACK HACK HACK  Walk Over now relative to pelvis, so it must be negative
			if (rWalkOver > 0.0f)
			{
				dout << pgon->strObjectName << " has positive WalkOver.  Should be negative.\n";
				rWalkOver = - 2 * rWalkOver;
			}


			bFILL_FLOAT(rJumpOver, esJumpOver			);
			bFILL_FLOAT(rCrouchUnder, esCrouchUnder		);
			bFILL_FLOAT(rMoveableMass, esMoveableMass		);
			Assert(rMoveableMass <= 1.0f);
			Assert(rMoveableMass >= 0.0f);

			bFILL_FLOAT(r_max_up_angle, esUpAngle);
			bFILL_FLOAT(r_max_down_angle, esDownAngle);

			rMinUpHillNormalZ = cos(r_max_up_angle * PI / 180);
			rMinDownHillNormalZ = cos(r_max_down_angle * PI / 180);

			// Smarts
			bFILL_FLOAT(rAnimateSensoryRange,		esSenseAnimates);
			bFILL_FLOAT(rObjectSensoryRange,		esSenseObjects);
			bFILL_FLOAT(rTerrainSensoryRange, 		esSenseTerrain);
			bFILL_BOOL (bUseTerrainPathfinding, 	esTerrainPathfinding);

			bFILL_FLOAT(rWakeDistance,			esWakeUp);
			bFILL_FLOAT(rSleepDistance,			esSleep);

			bFILL_FLOAT(rNodesPerSec, esNodesPerSec		);
			bFILL_INT(iMaxNodes, esMaxNodes			);
			bFILL_FLOAT(sTimeToForget, esTimeToForgetNode	);
			bFILL_FLOAT(sForgetInfluence, esTimeToForgetInfluence			);
			bFILL_FLOAT(rMaxPathLength, esMaxPathLength		);
			bFILL_INT(iMaxAStarSteps, esMaxAStarSteps);


			// Activities
			const CEasyString* pestr = 0;
			CActivityMoveCommandStayNear* pasn = (CActivityMoveCommandStayNear*) sapactActivities[(int)eatSTAY_NEAR];
			bFILL_FLOAT(pasn->rCloseEnough,	esStayNearOK);
			bFILL_FLOAT(pasn->rTooFarAway,	esStayNearMax);
			if (bFILL_pEASYSTRING(pestr, esStayNearTarget))
			{
				// Set the target.
				pasn->u4Target = u4Hash(pestr->strData());
#if VER_TEST
				if (pasn->u4Target != u4Hash("Player") && !pload->goiInfo.pgonFindObject(pestr->strData()))
				{
					char str_buffer[256];
					sprintf(str_buffer, 
							"%s\n\nMissing StayNearTarget:\n%s needs %s", 
							__FILE__, 
							pgon->strObjectName,
							pestr->strData());

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}
#endif
			}
			
			CActivityMoveCommandStayAway* pasa = (CActivityMoveCommandStayAway*) sapactActivities[(int)eatSTAY_AWAY];
			bFILL_FLOAT(pasa->rFarEnough,	esStayAwayOK);
			bFILL_FLOAT(pasa->rTooClose,		esStayAwayMin);
			if (bFILL_pEASYSTRING(pestr, esStayAwayTarget))
			{
				// Set the target.
				pasa->u4Target = u4Hash(pestr->strData());
#if VER_TEST
				if (pasa->u4Target != u4Hash("Player") && !pload->goiInfo.pgonFindObject(pestr->strData()))
				{
					char str_buffer[256];
					sprintf(str_buffer, 
							"%s\n\nMissing StayAwayTarget:\n%s needs %s", 
							__FILE__, 
							pgon->strObjectName,
							pestr->strData());

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}
#endif
			}


			CActivityBite* pab = (CActivityBite*) sapactActivities[(int)eatBITE];
			bFILL_FLOAT(pab->rLockDistance, esBiteTargetDistance);

			pab = (CActivityBite*) sapactActivities[(int)eatFEINT];
			bFILL_FLOAT(pab->rLockDistance, esFeintTargetDistance);

		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		// Now init the stuff that depends on the above.
		InitDependents();
	}


	

	//****************************************************************************************
	CBrain::~CBrain()
	{
		delete pmsState;
		delete pwvWorldView;

		// Since the AI system allocates the graphs, we don't delete this here.
//		delete paigGraph;

		// Since activities are allocated on a fastheap, we do not delete them.
		for (int i = esbtEND - 1; i >= 0; i--)
			apasbSubBrain[i] = 0;		
		pcactSelf = 0;

		psynSynthesizer = 0;
		paniOwner = 0;
	}

	
	//****************************************************************************************
	void CBrain::InitDefaults()
	{

		// The mental state.
		pmsState = new CMentalState();

		// Setup the AI model.
		CreateActivities();

		// We last thought when created.
		sLastTime = gaiSystem.sNow;


		//  Waypoints.
		rWayPointDistance	= 0.5f;
		sPatience			= 8.0f;
		v3LastWayPoint		= CVector3<>(10000,10000,10000);
		sLastWayPoint		= gaiSystem.sNow;

		bTriumph	= false;
		sAllowStartle = 0;
		bStartled	= false;

		bJumpEnabled = true;
		bCanJump	= true;

		//
		//	Personal data.  Defaults for now, will be set by properties later.
		//
		int iTeam = 0;

		// Set physical stats.  Later, these will depend upon scale and species.
		rWidth		= 1;
		rHeight		= 1;
		rHeadReach	= 2.0f;
		rTailReach	= 4.0f;
		rClawReach	= 1;

		rJumpDistance = 3.0f;
		rJumpDistanceSqr = rJumpDistance * rJumpDistance;
		rJumpUp = 1.0f;
		rJumpDown = 1.0f;

		rWalkOver = 0.15f;
		rWalkUnder= 3.0f;
		rJumpOver = 0.5f;
		rCrouchUnder = 2.0f;
		rMoveableMass= 0.5f;

		rMinUpHillNormalZ = 0.851;	// 31 degree slope.
		rMinDownHillNormalZ = 0.707;	// 45 degree slope.

		edtDinoType = edtRAPTOR;	// Default to raptor.

		// Smarts. 
		rNodesPerSec	= 300;
		iMaxNodes		= 35;
		sWhenToLearn	= gaiSystem.sNow;
		sTimeToForget	= 5;
		rAnimateSensoryRange	= 40.0f;
		rObjectSensoryRange		= 15.0f;
		rTerrainSensoryRange	= 20.0f;
		bUseTerrainPathfinding	= true;
		rMaxPathLength	= rAnimateSensoryRange * 2.0f;
		iMaxAStarSteps  = 800;
		sForgetInfluence= 1.0f;

		rWakeDistance	= 30.0f;
		rSleepDistance	= 30.0f;
		bWakeMe			= false;

		// Not making sounds yet.
		u4AudioID	= 0;
		iVocalizingActivity = -1;

		bOverrideHumanFeeling = false;

		feelDamageFeeling[eptFEAR]		= 0.0f;
		feelDamageFeeling[eptLOVE]		= 0.0f;
		feelDamageFeeling[eptANGER]		= 0.0f;
		feelDamageFeeling[eptHUNGER]	= 0.0f;
		feelDamageFeeling[eptTHIRST]	= 0.0f;
		feelDamageFeeling[eptFATIGUE]	= 0.0f;
		feelDamageFeeling[eptPAIN]		= 2.0f;		// Take 10% damage, pain rises .2
		feelDamageFeeling[eptSOLIDITY]	= 0.0f;
	}

	//****************************************************************************************
	void CBrain::InitDependents()
	{

		//
		//  Essentials.
		//

		//lint -save -e1732 -e1733

		// The World View.
		pwvWorldView = new CWorldView(paniOwner);

		// The pathfinding graph.
		paigGraph = 0;
		//lint -restore

		// Init the message which will talk to physics.
		msgprPhysicsControl = CMessagePhysicsReq(0, paniOwner);

		// For now, all brains share one synthesizer.
		psynSynthesizer		= gaiSystem.psynGlobalSynthesizer;		


		Assert(earArchetype >= 0);
		Assert(earArchetype < earEND);
		SetArchetype(earArchetype);
	}



	//*********************************************************************************
	void CBrain::CreateActivities
	(
	)
	{
	//	CMemCheck	mem("Activity Load", MEM_DIFF_STATS|MEM_DIFF_DUMP);
		AlwaysAssert(iMAX_ACTIVITIES >= eatEND);


		// Init sub-brains
		InitDOSubBrains();

		// Init objectless sub-brains.
		InitSelfSubBrain();

		CActivity* pact = new CActivity("Not implemented");

		// Clear the activity array.
		sapactActivities.Fill(pact);

		// Now create each individual activity and put it in the correct sub-brain.
		CActivityCompound* pag	 = 0;

		// Attacks.
		pag = new CActivityCompound();
		pag->AddActivity(			sapactActivities[(int)eatEAT]					= new CActivityEat());
		pag->AddActivity(			sapactActivities[(int)eatBITE]					= new CActivityBite());
		pag->AddActivity(			sapactActivities[(int)eatFEINT]					= new CActivityFeint());
		pag->AddActivity(			sapactActivities[(int)eatHEAD_BUTT]				= new CActivityRam());
		pag->AddActivity(			sapactActivities[(int)eatDRINK]					= new CActivityDrink());
//		pag->AddActivity(			sapactActivities[(int)eatJUMP_N_BITE]			= new CActivityJumpAndBite());
//		pag->AddActivity(			sapactActivities[(int)eatSHOULDER_CHARGE]		= new CActivityShoulderCharge());
		pag->AddActivity(			sapactActivities[(int)eatTAIL_SWIPE]			= new CActivityTailSwipe());
		pag->AddActivity(			sapactActivities[(int)eatJUMP_BACK]				= new CActivityJumpBack());
//		pag->AddActivity(			sapactActivities[(int)eatCIRCLE]				= new CActivityMoveCommandCircle());

		AddActivity(esbtATTACK, pag);

		// Movements.
		CActivityExclusive* pae = new CActivityExclusive();
		pae->AddActivity(			sapactActivities[(int)eatMOVE_TOWARD]			= new CActivityMoveCommandToward());
//		pae->AddActivity(			sapactActivities[(int)eatMOVE_HERD]				= new CActivityMoveHerd());
//		pae->AddActivity(			sapactActivities[(int)eatMOVE_AWAY]				= new CActivityMoveCommandAway());
//		pae->AddActivity(			sapactActivities[(int)eatDONT_TOUCH]			= new CActivityMoveCommandDontTouch());
//		pae->AddActivity(			sapactActivities[(int)eatSTALK]					= new CActivityMoveCommandStalk());
		pae->AddActivity(			sapactActivities[(int)eatPURSUE]				= new CActivityPursue());
		pae->AddActivity(			sapactActivities[(int)eatFLEE]					= new CActivityFlee());
		pae->AddActivity(			sapactActivities[(int)eatAPPROACH]				= new CActivityApproach());
		pae->AddActivity(			sapactActivities[(int)eatMOVE_BY]				= new CActivityMoveBy());

		AddActivity(esbtMOVEMENT, pae);

		// Head stuff.
//		AddActivity(esbtEMOTION,	sapactActivities[(int)eatLOOK_AT]				= new CActivityLookAt());
//		AddActivity(esbtEMOTION,	sapactActivities[(int)eatTASTE]					= new CActivityTaste());
		AddActivity(esbtEMOTION,	sapactActivities[(int)eatSNIFF_TARGET]			= new CActivitySniffTarget());
		AddActivity(esbtEMOTION,	sapactActivities[(int)eatCOCK_HEAD]				= new CActivityCockHead());
		AddActivity(esbtEMOTION,	sapactActivities[(int)eatGLARE]					= new CActivityGlare());
		AddActivity(esbtEMOTION,	sapactActivities[(int)eatCROON]					= new CActivityVocalCroon());
		AddActivity(esbtEMOTION,	sapactActivities[(int)eatSNARL]					= new CActivityVocalSnarl());

		// Emotions.
		AddActivity(esbtEMOTION,	sapactActivities[(int)eatREAR_BACK]				= new CActivityRearBack());
//		AddActivity(esbtEMOTION,	sapactActivities[(int)eatBACK_AWAY]				= new CActivityBackAway());
		AddActivity(esbtEMOTION,	sapactActivities[(int)eatCOWER]					= new CActivityCower());
//		AddActivity(esbtEMOTION,	sapactActivities[(int)eatSLEEP]					= new CActivitySleep());
//		AddActivity(esbtEMOTION,	sapactActivities[(int)eatSCRATCH]				= new CActivityScratch());
//		AddActivity(esbtEMOTION,	sapactActivities[(int)eatLIE_DOWN]				= new CActivityLieDown());

#if VER_TEST
		// Tests.
		AddObjectlessActivity(		sapactActivities[(int)eatTEST_HEAD_COCK]		= new CActivityTestHeadCock());
		AddObjectlessActivity(		sapactActivities[(int)eatTEST_HEAD_ORIENT]		= new CActivityTestHeadOrient());
		AddObjectlessActivity(		sapactActivities[(int)eatTEST_WAG_TAIL]			= new CActivityTestWagTail());
		AddObjectlessActivity(		sapactActivities[(int)eatTEST_HEAD_POSITION]	= new CActivityTestHeadPosition());
		AddObjectlessActivity(		sapactActivities[(int)eatTEST_MOUTH]			= new CActivityTestMouth());
		
		// Objectless Vocalizations
		AddObjectlessActivity(		sapactActivities[(int)eatOUCH]					= new CActivityVocalOuch());
		AddObjectlessActivity(		sapactActivities[(int)eatHOWL]					= new CActivityVocalHowl());
		AddObjectlessActivity(		sapactActivities[(int)eatHELP]					= new CActivityVocalHelp());
		AddObjectlessActivity(		sapactActivities[(int)eatDIE]					= new CActivityVocalDie());
#endif

		// Objectless activities.

		pae = new CActivityExclusive();
		pae->AddActivity(			sapactActivities[(int)eatWANDER]				= new CActivityMoveHintWander());
		pae->AddActivity(			sapactActivities[(int)eatSTAY_NEAR]				= new CActivityMoveCommandStayNear(gpPlayer, 30.0f, 15.0f));
		pae->AddActivity(			sapactActivities[(int)eatSTAY_AWAY]				= new CActivityMoveCommandStayAway(gpPlayer, 15.0f, 25.0f));
		pae->AddActivity(			sapactActivities[(int)eatLOOK_AROUND]			= new CActivityLookAround());
//		pae->AddActivity(			sapactActivities[(int)eatDASH]					= new CActivityDash());
		pae->AddActivity(			sapactActivities[(int)eatGET_OUT]				= new CActivityGetOut());
		pae->AddActivity(			sapactActivities[(int)eatNOTHING]				= new CActivityNothing());

		AddObjectlessActivity(pae);
		AddObjectlessActivity(		sapactActivities[(int)eatSNIFF]					= new CActivitySniffAir());
//		AddObjectlessActivity(		sapactActivities[(int)eatJUMP]					= new CActivityJump());

		// By default, no activities are active in an AI model.
		for (int i = 0; i < eatEND; ++i)
		{
#if VER_TEST
			AlwaysAssert(sapactActivities[i]);
#endif
			sapactActivities[i]->Activate(false);
		}
	}


	//*****************************************************************************************
	int CBrain::iGetActivityIndex(const CActivity*pact) const
	{
		for (int i = 0; i < eatEND; ++i)
		{
			if (pact == sapactActivities[i])
				return i;
		}
		return -1;
	}
	
	//*********************************************************************************
	void CBrain::SetArchetype
	(
		EArchetype ear
	)
	{
		Assert(ear >= 0);
		Assert(ear < earEND);
		earArchetype = ear;
	}



	//*********************************************************************************
	void CBrain::InitDOSubBrains
	(
	)
	{
		for (int i = esbtEND - 1; i >= 0; i--)
			apasbSubBrain[i] = new CActivityDOSubBrain();		
	}
	
	//*********************************************************************************
	//
	void CBrain::InitSelfSubBrain
	(
	)
	{
		pcactSelf = new CActivityCompound();		
	}


	//****************************************************************************************
	void CBrain::AddActivity 
	(
		const ESubBrainType	esbt,		// Add to this sub-brain.
		CActivity*			pact		// Add this activity.
	)
	{
		Assert(pact);
		Assert(bValid(apasbSubBrain[esbt]));

		apasbSubBrain[esbt]->AddActivity(pact);
	}

	//****************************************************************************************
	//
	void CBrain::AddObjectlessActivity
	(
		CActivity*		pact		// Add this activity.
	)
	{
		Assert(pact);

		if (!pcactSelf)
			pcactSelf = new CActivityCompound();

		pcactSelf->AddActivity(pact);

	}


	//****************************************************************************************
	void CBrain::Activate()
	{
		// And lastly, increase the sleep distance.
		TReal r_dist = (paniOwner->v3Pos() - gpPlayer->v3Pos()).tLen(); 
		r_dist = Max(r_dist, rWakeDistance);
		if (rSleepDistance < r_dist)
		{
			rSleepDistance = r_dist + 10.0f;
		}

		// Make sure we don't start by ignoring everything.
		SetWayPoint();

		sLastSuccessfulPath = gaiSystem.sNow;
		sWhenToLearn		= gaiSystem.sNow;

		// Test the graph.
		AlwaysAssert(paigGraph);
		AlwaysAssert(paigGraph->paniAnimal == paniOwner);
		paigGraph->Reset();

		// And force us to redo terrain query.
		pwvWorldView->v2CenterOfTerrainKnowledge = CVector2<>(-10000, -10000);
	}

	//****************************************************************************************
	void CBrain::Deactivate()
	{
		// Give it a neutral command so that it stops moving.
		msgprPhysicsControl.Reset();

		// And clear out the influences.
		pwvWorldView->RemoveAllInfluences();

		// And the terrain knowlesdge.
		pwvWorldView->ClearTerrainKnowledge();
	}

	//****************************************************************************************
	void CBrain::Think()
	{
		AlwaysAssert(this);
		AlwaysAssert(paigGraph);

		CCycleTimer ctmr;


#if VER_TEST

		CAIGraph* paig = paigGraph;
		{
		// Check all nodes in the graph and see if any of them reference things that aren't in the graph!
		if (paig) for (int i = paig->iMaxNodeIndex() - 1; i >= 0; i--)
		{
			CNodeSource* pns = paig->nNode(i).pnhGetNodeHistory()->pnsReferences();

			if (pns)
			{
				AlwaysAssert(pns->setNodeFlags[ensfIN_GRAPH]);
			}
		}
		}
#endif


		// Let the AI system know that we're thinking.
//		Assert(gaiSystem.pbrGetCurrentBrain() == 0);
		gaiSystem.SetCurrentBrain(this);

		// Kill all old inputs.
		msgprPhysicsControl.Reset();

		// Update the elapsed time.  How long has it been since we thought?
		sElapsedTime = gaiSystem.sNow - sLastTime;

		// Reset the synthesizer and attach to this particular brain.
		psynSynthesizer->Reset(this);

		// Some bookkeeping.
		pwvWorldView->MaybeResetTempInfluenceFlags();			

		// Are we still alive?
		if (!paniOwner->bReallyDead())
		{
			// Yes!  Go ahead and really think.

			// Reset timer clock.
			ctmr();

#if VER_TEST
		{
		// Check all nodes in the graph and see if any of them reference things that aren't in the graph!
		if (paig) for (int i = paig->iMaxNodeIndex() - 1; i >= 0; i--)
		{
			CNodeSource* pns = paig->nNode(i).pnhGetNodeHistory()->pnsReferences();

			if (pns)
			{
				AlwaysAssert(pns->setNodeFlags[ensfIN_GRAPH]);
			}
		}
		}
#endif


			// Examine your surroundings.
			Look();
#if VER_TEST
		{
		// Check all nodes in the graph and see if any of them reference things that aren't in the graph!
		if (paig) for (int i = paig->iMaxNodeIndex() - 1; i >= 0; i--)
		{
			CNodeSource* pns = paig->nNode(i).pnhGetNodeHistory()->pnsReferences();

			if (pns)
			{
				AlwaysAssert(pns->setNodeFlags[ensfIN_GRAPH]);
			}
		}
		}
#endif

			// Give all sub-brains time to think.
			Analyze();

#if VER_TEST
		{
		// Check all nodes in the graph and see if any of them reference things that aren't in the graph!
		if (paig) for (int i = paig->iMaxNodeIndex() - 1; i >= 0; i--)
		{
			CNodeSource* pns = paig->nNode(i).pnhGetNodeHistory()->pnsReferences();

			if (pns)
			{
				AlwaysAssert(pns->setNodeFlags[ensfIN_GRAPH]);
			}
		}
		}
#endif
			// Grab the analysis time.
			psAnalyzer.Add(ctmr());

			// Synthesize a coherent action from all active commands.
			Synthesize();

			// Grab the synthesis time.
			psSynthesizer.Add(ctmr());

			// Now reset all of the activity flags.
			pcactSelf->ResetTempFlags();

			// Each sub-brain....
			for (int iSubBrainIndex = esbtEND - 1; iSubBrainIndex >= 0; iSubBrainIndex--)
			{
				// Reset those temp flags!
				apasbSubBrain[iSubBrainIndex]->ResetTempFlags();
			}


#if VER_TEST
		{
		// Check all nodes in the graph and see if any of them reference things that aren't in the graph!
		if (paig) for (int i = paig->iMaxNodeIndex() - 1; i >= 0; i--)
		{
			CNodeSource* pns = paig->nNode(i).pnhGetNodeHistory()->pnsReferences();

			if (pns)
			{
				AlwaysAssert(pns->setNodeFlags[ensfIN_GRAPH]);
			}
		}
		}
#endif
			// Clear a few rows of debug console.
			MaybeDebugSay("\n************************************************\n");
		}


		// Let emotions settle.
		pmsState->RestoreEmotions(sElapsedTime);

		// But override Fatigue to be relative to damage....
		pmsState->feelEmotions[eptFATIGUE] = Max(0.0f, 1.0f - (paniOwner->fHitPoints / paniOwner->fMaxHitPoints));

		// Done thinking, so let the AI system know.
		gaiSystem.ClearCurrentBrain();

		// Save the timestamp.
		sLastTime = gaiSystem.sNow;

#if VER_TEST
		{
		// Check all nodes in the graph and see if any of them reference things that aren't in the graph!
		if (paig) for (int i = paig->iMaxNodeIndex() - 1; i >= 0; i--)
		{
			CNodeSource* pns = paig->nNode(i).pnhGetNodeHistory()->pnsReferences();

			if (pns)
			{
				AlwaysAssert(pns->setNodeFlags[ensfIN_GRAPH]);
			}
		}
		}
#endif

	}
	
	//****************************************************************************************
	void CBrain::Look()
	{
		// Vision check!
		
		//  Look in the direction of the head.
		CVector3<> v3_vision_center = gaiSystem.v3GetHeadLocation() - gaiSystem.v3GetBodyLocation();
		if (!v3_vision_center.bIsZero())
			v3_vision_center.Normalise(rObjectSensoryRange * 0.75f);
		v3_vision_center += gaiSystem.v3GetBodyLocation();

		CPresence3<> pr3(v3_vision_center );

		// Do we have lots of influences alreadys?
		if (pwvWorldView->inflInfluences.size() < iNUM_INFLUENCES)
		{
			// No!  We have some room, so look for more.

			CWDbQueryAI qai(CBoundVolSphere(rObjectSensoryRange), pr3);
		//		CWDbQueryAI qai(CBoundVolSphere(rObjectSensoryRange), CPresence3<>(gaiSystem.v3GetBodyLocation()));

		//		dout << "Saw " << qai.size() << ", ";

			// Loop through everything you saw.
			foreach(qai)
			{
				CInstance *pins = qai.tGet();

				Assert(pins);

				pwvWorldView->See(pins, gaiSystem.sNow);
			}
		}

		// Now look for any animates you might have missed.  Using different sight distance, so recalc volume.
		//  Look in the direction of the head.
		v3_vision_center = gaiSystem.v3GetHeadLocation() - gaiSystem.v3GetBodyLocation();
		if (!v3_vision_center.bIsZero())
			v3_vision_center.Normalise(rAnimateSensoryRange * 0.75f);
		v3_vision_center += gaiSystem.v3GetBodyLocation();
		TReal r_range_sqr = rAnimateSensoryRange * rAnimateSensoryRange;

#if VER_TEST
		CAIGraph* paig = paigGraph;
		{
		// Check all nodes in the graph and see if any of them reference things that aren't in the graph!
		if (paig) for (int i = paig->iMaxNodeIndex() - 1; i >= 0; i--)
		{
			CNodeSource* pns = paig->nNode(i).pnhGetNodeHistory()->pnsReferences();

			if (pns)
			{
				AlwaysAssert(pns->setNodeFlags[ensfIN_GRAPH]);
			}
		}
		}
#endif


		// Look for the player.
		if ((gpPlayer->v3Pos() - v3_vision_center).tLenSqr() < r_range_sqr)
		{					// Now loop through animal's boundary boxes!
			int i_box = 0;
			for ( ; i_box < ebbEND; ++i_box)
				pwvWorldView->See(gpPlayer->apbbBoundaryBoxes[i_box], gaiSystem.sNow);
		}

		// Loop through all active animals.
		int i;
		for (i = 0; i < iMAX_ACTIVE_ANIMALS; ++i)
		{
			if (gaiSystem.apaniActiveAnimals[i])
			{
				if ((gaiSystem.apaniActiveAnimals[i]->v3Pos() - v3_vision_center).tLenSqr() < r_range_sqr)
				{
					// Now loop through animal's boundary boxes!
					int i_box = 0;
					for ( ; i_box < ebbEND; ++i_box)
						pwvWorldView->See(gaiSystem.apaniActiveAnimals[i]->apbbBoundaryBoxes[i_box], gaiSystem.sNow);
				}
			}
		}


#if VER_TEST
		{
			// Check all nodes in the graph and see if any of them reference things that aren't in the graph!
			if (paig) for (int i = paig->iMaxNodeIndex() - 1; i >= 0; i--)
			{
				CNodeSource* pns = paig->nNode(i).pnhGetNodeHistory()->pnsReferences();

				if (pns)
				{
					AlwaysAssert(pns->setNodeFlags[ensfIN_GRAPH]);
				}
			}
		}
#endif

		LookAtTerrain();
	}


	void CBrain::LookAtTerrain()
	{
		if (!bUseTerrainPathfinding)
		{
			//	Clear old list.
			pwvWorldView->ClearTerrainKnowledge();
			return;
		}

		// Terrain query!
		CWDbQueryTerrain qt;
		if (bUseTerrainPathfinding && qt.tGet())
		{
			// Secondly, update the dangerous terrain poly list.  This can be done fairly infrequently.
			CVector2<> v2_from_knowledge_center = gaiSystem.v2GetBodyLocation() - pwvWorldView->v2CenterOfTerrainKnowledge;
			if (v2_from_knowledge_center.tLenSqr() >
					rTerrainSensoryRange * rTerrainSensoryRange * .25f)
			{
				// Remember the next set...
				CVector2<> v2_new_min = CVector2<>(100000,100000);
				CVector2<> v2_new_max = CVector2<>(-100000,-100000);

				// Initialiser constructor. Queries given world database for the required multiresolution type.
				CWDbQueryTerrainTopology qtt(CBoundVolSphere(rTerrainSensoryRange), 
											CPresence3<>(gaiSystem.v3GetBodyLocation()), 
											0.0f	// This parameter if the frequency filter- ignoring height deltas of less than this value.
											);

				//	Clear old list.
				pwvWorldView->ClearTerrainKnowledge();
				
				// It's not passable if we bother to record it.
				CTerrainKnowledge tk_empty;
				tk_empty.setNodeFlags[ensfIS_PASSABLE]	= false;
				tk_empty.setNodeFlags[ensfCAN_SIMPLIFY] = true;
				tk_empty.MakeSilhouette(0);

				CWDbQueryTerrainTopology::CIterator it = qtt.itBegin();

				// For each quad node...
				for (; it; it.NextQuadNode())
				{
					bool b_entire_node_is_steep = true;
					bool b_entire_node_is_flat	= true;

					// Get the lower two corners of the quad.  We may need them later, but for now we need their width.
					CVector3<> v3_0 = it.pqnqGetNode()->pqvtGetVertex(0)->v3World(it.pqrContainer->pqnqRoot->mpConversions);
					CVector3<> v3_1 = it.pqnqGetNode()->pqvtGetVertex(1)->v3World(it.pqrContainer->pqnqRoot->mpConversions);
					CVector3<> v3_2 = it.pqnqGetNode()->pqvtGetVertex(2)->v3World(it.pqrContainer->pqnqRoot->mpConversions);
					TReal r_width = v3_1.tX - v3_0.tX;


					// Snag the min and max.
					v2_new_min.tX = Min(v2_new_min.tX, v3_0.tX);
					v2_new_min.tY = Min(v2_new_min.tY, v3_0.tY);
					v2_new_max.tX = Max(v2_new_max.tX, v3_2.tX);
					v2_new_max.tY = Max(v2_new_max.tY, v3_2.tY);

					CVector2<> v2_quad_center = (v3_0 + v3_2) * 0.5f;








					// Find the Z extents of the node.
					std::pair<TReal, TReal> pr_minmaxZ = it.pqnqGetNode()->prrGetWorldZLimits(it.pqrContainer->pqnqRoot);

					// Make sure we have (min, max)
					Assert(pr_minmaxZ.first <= pr_minmaxZ.second);

					// Is the maximum possible slope in the quad node below 1/sqrt(3)  (30 degrees)?
					if ((pr_minmaxZ.second - pr_minmaxZ.first) * 2.0f < r_width)
						// Yes!  Skip this node.
						continue;
					else
					{
						int i_sil_index = 0;
						
						// Add the terrain knowledge.
						pwvWorldView->ltkKnowledge.push_front(tk_empty);

						// Get address of new terrain knowledge!
						CTerrainKnowledge* ptk = &(*pwvWorldView->ltkKnowledge.begin());

						// Initialize terrain knowledge radius and center based on the containing quad.
						TReal r_half_width = r_width * 0.5f;
						ptk->rRadius = 1.5f * r_half_width;
						ptk->v2Center = v2_quad_center;

						// Starting point for triangle iteration.  
						// Making it non-const to allow for Init() calls.
						// HACK HACK HACK
						NMultiResolution::CTriangleQuery* ptriCurr = (NMultiResolution::CTriangleQuery*)*it;	// Current triangle (of current quad node) being iterated.

						// Check each triangle.
						for (; i_sil_index < iMAX_SUBMODELS && ptriCurr; ptriCurr = ptriCurr->ptriGetNext())
						{
							ptriCurr->Init(it.pqrContainer->pqnqRoot->mpConversions);

							// Is this triangle too steep?
							if (bTooSteepUpHill(ptriCurr))
							{
								// Yes!  Add the triangle.
								b_entire_node_is_flat = false;

								// Add the triangle!
								ptk->MakeSilhouette(i_sil_index);

								// Snag the next appropriate silhouette pointer.
								CSilhouette* psil = ptk->psilGetSilhouette(i_sil_index);

								// Add the silhouette to the terrain knowledge.
								psil->push_back(ptriCurr->pqvtGetVertex(2)->v3World(it.pqrContainer->pqnqRoot->mpConversions));
								psil->push_back(ptriCurr->pqvtGetVertex(1)->v3World(it.pqrContainer->pqnqRoot->mpConversions));
								psil->push_back(ptriCurr->pqvtGetVertex(0)->v3World(it.pqrContainer->pqnqRoot->mpConversions));

								// Add it to the original data, too.
								ptk->asilOriginalSilhouettes.uLen++;
								ptk->asilOriginalSilhouettes[i_sil_index] = *psil;
								Assert(ptk->asilOriginalSilhouettes[i_sil_index].size() >= 3);

								ptk->ad3Normals.uLen++;
								ptk->ad3Normals[i_sil_index] = ptriCurr->plGetPlaneWorld().d3Normal;

#if VER_DEBUG
								{
									CSilhouette * psil = ptk->psilGetSilhouette(i_sil_index);
									// Make sure that the silhouette points all fall within the terrain sphere.
									for (int i = 0; i < psil->size(); ++i)
									{
										Assert((ptk->v2Center - (*psil)[i]).tLen() < ptk->rRadius);
									}
								}
#endif

								++i_sil_index;
							}
							else
							{
								// No!  Not too steep.  Can't add the whole node.
								b_entire_node_is_steep = false;
							}
						}


#if VER_DEBUG
						//  Did we shortcut the loop because of a silhouette overflow?
						if (ptriCurr)
						{
							// Yes.
							dout << "DANGER!  Terrain too complicated for dinos around " << v2_quad_center.tX << " , " << v2_quad_center.tY << "\n";
						}
#endif

						/*
						// Can we add the whole thing as a unit?
						if (b_entire_node_is_steep)
						{
							// Start over!  We only need a single silhouette.
							ptk->ClearSilhouettes();
							ptk->MakeSilhouette(0);

							// Get the address of that new silhouette.
							CSilhouette* psil = (*pwvWorldView->ltkKnowledge.begin()).psilGetSilhouette(0);

							// Set the silhouette of the terrain segment.
							psil->push_back(it.pqnqGetNode()->pqvtGetVertex(3)->v3World(it.pqrContainer->pqnqRoot->mpConversions));
							psil->push_back(it.pqnqGetNode()->pqvtGetVertex(2)->v3World(it.pqrContainer->pqnqRoot->mpConversions));
							psil->push_back(v3_1);
							psil->push_back(v3_0);

							// Add it to the original data, too.  Make it totally impassable.
							ptk->asilOriginalSilhouettes.uLen = 1;
							ptk->asilOriginalSilhouettes[0] = *psil;
							Assert(ptk->asilOriginalSilhouettes[0].size() >= 3);

							ptk->ad3Normals.uLen = 1;
							ptk->ad3Normals[0] = CDir3<>(1,0,0);

						} else  */
						if (b_entire_node_is_flat)
						{
							// Remove the node entirely.
							pwvWorldView->ltkKnowledge.erase(pwvWorldView->ltkKnowledge.begin());
						}
					}
				}


				CVector2<> v2 = gaiSystem.v3GetBodyLocation();
				pwvWorldView->v2CenterOfTerrainKnowledge = v2;
				pwvWorldView->ResetMergeTerrain();

				// What is the new area?
				pwvWorldView->v2TerrainKnowledgeWorldMax = v2_new_max;
				pwvWorldView->v2TerrainKnowledgeWorldMin = v2_new_min;

				// Clean up old nodes outside the new area.
				pwvWorldView->ClearOutsideTerrainKnowledge();
			}
			else
			{		
				pwvWorldView->MaybeMergeTerrain();
			}
		}   // 		if (qt.tGet())
	}


	//****************************************************************************************
	void CBrain::Analyze()
	{
		// Analyze the world around you.

		// Update waypoints.
		if ((paniOwner->v3Pos() - v3LastWayPoint).tLenSqr() > rWayPointDistance * rWayPointDistance)
		{
			// We've moved far enough to drop a new waypoint!
			SetWayPoint();
		}

		// Clean up a little bit.
		pwvWorldView->RemoveSomeInfluences();

		CInfluenceList* pinfl = &pwvWorldView->inflInfluences;

		CInfluenceList::iterator pinf;

		// Let each sub-brain analyze the entire influence list at one go.

		// Each sub-brain....
		for (int iSubBrainIndex = esbtEND - 1; iSubBrainIndex >= 0; iSubBrainIndex--)
		{
			// Rate each activity, and let it register itself with the synthesizer.
			(void) apasbSubBrain[iSubBrainIndex]->rtRateAndRegisterList
				(
					pmsState->feelEmotions,
					pinfl
				);
		}

		// Update the importance of the influence.
		for (pinf = pinfl->begin(); pinf != pinfl->end(); pinf++)
		{
			((CInfluence*)&(*pinf))->CalculateImportance();
		}


		// Analyze self (run the objectless sub-brain).
		Assert(pcactSelf);

		// Rate each activity, and let it register itself with the synthesizer.
		(void) pcactSelf->rtRateAndRegister(pmsState->feelEmotions, 0);
	}


	//****************************************************************************************
	void CBrain::AnalyzeInfluence(CInfluence* pinf)
	{

		// Rate the importance of the influence.
		TReal r_importance = 0;

		// Each sub-brain....
		for (int iSubBrainIndex = esbtEND - 1; iSubBrainIndex >= 0; iSubBrainIndex--)
		{
			// Rate each activity, and let it register itself with the synthesizer.
			(void) apasbSubBrain[iSubBrainIndex]->rtRateAndRegister
				(
					pmsState->feelEmotions,
					pinf
				);

			for (int i_param = eptEND - 1; i_param >= 0; i_param--)
			{
				r_importance += apasbSubBrain[iSubBrainIndex]->feelAdjustedAttitude[i_param];
			}

#if 0
			// Save the weighted feeling for debugging.
			pinf->feelDBGWeighted = 
				apasbSubBrain[iSubBrainIndex]->feelAdjustedAttitude;
#endif
		} 

		pinf->rImportance = r_importance;

	}


	//****************************************************************************************
	void CBrain::Synthesize()
	{
		// Reset the bio boundary boxes before we begin.
		CBoundaryBox* pbb = paniOwner->apbbBoundaryBoxes[ebbHEAD];
		if (pbb)
			pbb->fDamageMultiplier = 0.0f;

		pbb = paniOwner->apbbBoundaryBoxes[ebbBODY];
		if (pbb)
			pbb->fDamageMultiplier = 0.0f;

		pbb = paniOwner->apbbBoundaryBoxes[ebbTAIL];
		if (pbb)
			pbb->fDamageMultiplier = 0.0f;

		// And then actually synthesize.
		psynSynthesizer->Synthesize();
	}

	
	//****************************************************************************************
	bool CBrain::bTooSteepUpHill(const CPlane& pl)
	{
		// Some magical plane calculations.  Want less than 30 degrees.
		return pl.d3Normal.tZ < rMinUpHillNormalZ;
	}

	//****************************************************************************************
	bool CBrain::bTooSteepUpHill(const NMultiResolution::CTriangleQuery* ptriquery)
	{
		return bTooSteepUpHill(ptriquery->plGetPlaneWorld());
	}

	//****************************************************************************************
	bool CBrain::bTooSteepDownHill(const CPlane& pl)
	{
		// Some magical plane calculations.  Want less than 30 degrees.
		return pl.d3Normal.tZ < rMinDownHillNormalZ;
	}

	//****************************************************************************************
	bool CBrain::bTooSteepDownHill(const NMultiResolution::CTriangleQuery* ptriquery)
	{
		return bTooSteepDownHill(ptriquery->plGetPlaneWorld());
	}

	//****************************************************************************************
	bool CBrain::bFrustrated() const
	{
		return (gaiSystem.sNow - sLastWayPoint) > sPatience;
	}

	//****************************************************************************************
	bool CBrain::bIsVocalizing() const
	{
		return iVocalizingActivity != -1;
	}
	
	//****************************************************************************************
	void CBrain::Startle(const CVector3<>& v3_location)
	{
		if (sAllowStartle < gaiSystem.sNow)
		{
			bStartled = true;
			v3Startle = v3_location;
		}
	}

	//****************************************************************************************
	void CBrain::UnStartle(TSec s_startle_delay)
	{
		bStartled = false;
		
		sAllowStartle = Max(sAllowStartle, gaiSystem.sNow + s_startle_delay);
	}

	//****************************************************************************************
	void CBrain::SetWayPoint()
	{
		sLastWayPoint = gaiSystem.sNow;
		v3LastWayPoint = paniOwner->v3Pos();
	}

	//*********************************************************************************
	const CVector2<> CBrain::v2GetLocation
	(
	) const
	{
		return paniOwner->v3Pos();
	}


		//****************************************************************************************
		void CBrain::HandleMessage(	const CMessageCollision& msgcoll )
		{

		}

		void CBrain::HandleDamage(float f_damage, const CInstance * pins_aggressor)
		{
			if (f_damage <= 0)
			{
				// Prevent physics inputs when you are dead.
				msgprPhysicsControl.Reset();
				return;
			}

			CFeeling feel_damage_reaction = feelDamageFeeling * (f_damage / paniOwner->fMaxHitPoints);

			pmsState->feelEmotions = pmsState->feelEmotions + feel_damage_reaction;

			if (pins_aggressor)
			{
				// See if it's already here.
				CInfluenceList::iterator i = pwvWorldView->inflInfluences.iterFindInfluence((CInstance*)pins_aggressor);

				if (i != pwvWorldView->inflInfluences.end())
				{
					CInfluence* pinf = (CInfluence*)&(*i);
					pinf->feelAboutTarget = pinf->feelAboutTarget + feel_damage_reaction;

					// Stop ignoring the aggressor!
					pinf->Ignore(false);
				}
			}

#if VER_TEST
			char buffer[256];
			sprintf(buffer, "Ouch!  %f hit points lost (%f).\n", f_damage, paniOwner->fGetHitPoints());
			DebugSay(buffer);
#endif

			// Activate the guy.
			gaiSystem.ActivateAnimal(paniOwner);
		}

		//****************************************************************************************
		void CBrain::HandleMessage(	const CMessageMove& msgmv )
		{
			// If we have no graph, our animal is not awake.  Skip this nonsense.
			if (!paigGraph)
				return;

			// Never look at animates, only their bounding boxes!
			if (ptCast<CAnimate>(msgmv.pinsMover))
				return;

			if (msgmv.pinsMover->paiiGetAIInfo() && msgmv.pinsMover->paiiGetAIInfo()->eaiRefType != eaiDONTCARE)
			{
				const CInfluence* pinf = pwvWorldView->pinfInfluenceMoved(msgmv.pinsMover, msgmv.sWhen);
	
				if (pinf && pinf->setNodeFlags[ensfIN_GRAPH])
				{
					paigGraph->InfluenceMoved((CInfluence*)pinf, msgmv);
				}
			}
		}


		//****************************************************************************************
		void CBrain::HandleMessage(	const CMessageDelete& msgdel )
		{
			// Let the world view know about the deletion.
			(void) pwvWorldView->bRemoveInfluence(msgdel.pinsDeleteMe);
		}

		//****************************************************************************************
		void CBrain::HandleMessage(	const CMessageDeath& msgdeath )
		{
			// Let the world view know about the deletion.
			(void) pwvWorldView->bRemoveInfluence(msgdeath.paniDyingThing);

			// If we have bio boxes, get rid of them as well.
			int i;
			for (i = 0; i < ebbEND; ++i)
			{
				if (msgdeath.paniDyingThing->apbbBoundaryBoxes[i])	
				{
					(void) pwvWorldView->bRemoveInfluence(msgdeath.paniDyingThing->apbbBoundaryBoxes[i]);
				}
			}


			// Did we kill it?
			const CInstance* pins_killer = msgdeath.paniDyingThing->pinsKiller;
			const CAnimate* pani_killer = 0;
			if (pins_killer)
			{
				pani_killer = pins_killer->paniGetOwner();
				
				if (pani_killer == paniOwner)
				{
					// Remember that you killed something.
					bTriumph = true;
				}
			}
		}

		//*********************************************************************************
		void CBrain::TellPhysicsMoveTo
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CVector2<>&	v2_location,	// Target location.
			const CRating		rt_speed		// Speed- 1 max, 0 zero.
		)
		{
			AlwaysAssert(!paniOwner->bDead());

			if (rt_importance > msgprPhysicsControl.subMoveBody.rtUrgency)
				msgprPhysicsControl.subMoveBody.Set
				(
					rt_importance,
					rt_speed,
					v2_location
				);
		}

		//*********************************************************************************
		void CBrain::TellPhysicsOpenMouth
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CRating		rt_open_amount,	// Degree to which the mouth should be open.
			const CRating		rt_speed		// Speed- 1 max, 0 zero.
		)
		{
			if (rt_importance > msgprPhysicsControl.subOpenMouth.rtUrgency)
				msgprPhysicsControl.subOpenMouth.Set
				(
					rt_importance,
					rt_speed,
					rt_open_amount
				);
		}

		//*********************************************************************************
		void CBrain::TellPhysicsMoveHeadTo
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CVector3<>&	v3_location,	// Target location.
			const CRating		rt_speed		// Speed- 1 max, 0 zero.
		)
		{
			AlwaysAssert(!paniOwner->bDead());

			if (rt_importance > msgprPhysicsControl.subMoveHead.rtUrgency)
				msgprPhysicsControl.subMoveHead.Set
				(
					rt_importance,
					rt_speed,
					v3_location
				);
		}

		//*********************************************************************************
		void CBrain::TellPhysicsCockHead
		(
			const CRating		rt_importance,	// Urgency of the order.
			float				f_angle,		// Target angle in radians.
			const CRating		rt_speed		// Speed- 1 max, 0 zero.
		)
		{
			if (rt_importance > msgprPhysicsControl.subCockHead.rtUrgency)
				msgprPhysicsControl.subCockHead.Set
				(
					rt_importance,
					rt_speed,
					f_angle
				);
		}

		//*********************************************************************************
		void CBrain::TellPhysicsPointHead
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CVector3<>&	v3_location,	// Target location.
			const CRating		rt_speed		// Speed- 1 max, 0 zero.
		)
		{
			if (rt_importance > msgprPhysicsControl.subPointHead.rtUrgency)
				msgprPhysicsControl.subPointHead.Set
				(
					rt_importance,
					rt_speed,
					v3_location
				);
		}

		//*********************************************************************************
		void CBrain::TellPhysicsWagTail
		(
			const CRating		rt_importance,	// Urgency of the order.
			float				f_amplitude,		// Target angle in radians.
			float				f_frequency		// Speed- 1 max, 0 zero.
		)
		{
		}

		//*********************************************************************************
		void CBrain::TellPhysicsCrouch
		(
			const CRating		rt_importance,	// Urgency of the order.
			const CRating		rt_speed
		)
		{
			AlwaysAssert(!paniOwner->bDead());

			if (rt_importance > msgprPhysicsControl.subCrouch.rtUrgency)
				msgprPhysicsControl.subCrouch.Set
				(
					rt_importance,
					rt_speed,
					true
				);
		}


		//*********************************************************************************
		void CBrain::TellPhysicsJump(
			const CRating		rt_importance,	// Urgency of the order.
			const CRating		rt_speed
		)
		{
			AlwaysAssert(!paniOwner->bDead());

			if (rt_importance > msgprPhysicsControl.subJump.rtUrgency)
				msgprPhysicsControl.subJump.Set
				(
					rt_importance,
					rt_speed,
					true
				);
		}



		//****************************************************************************************
		void CBrain::DrawInfluences
		(
		)
		{
#if VER_DEBUG
			// Get influence list.
			CInfluenceList *pinfl = inflGetInfluences();

			// Draw each influence.
			CInfluenceList::iterator pinf = pinfl->begin();

			if (pinf != pinfl->end())
			{
				for ( ; pinf != pinfl->end(); ++pinf)
				{
	//				DrawFeeling((*pinf).feelDBGWeighted,
	//							(*pinf).v2Location + CVector2<>(2.0,0.0));
					DrawFeeling((*pinf).feelAboutTarget,
								CVector2<>((*pinf).v3Location) + CVector2<>(2.0,0.0));


				// Color of silhouette depends on soldity.
				if ((*pinf).setNodeFlags[ensfBLOCKS_PATH])
				{
					LineColour(255,0,0);
				}
				else if (!(*pinf).setNodeFlags[ensfIS_PASSABLE])
				{
					LineColour(255,0,255);
				}
				else
					LineColour(128,128,128);

					for (int i = (*pinf).iNumSilhouettes() - 1; i >= 0; --i)
					{
						//  Draw each associated silhouette.
						DrawSilhouette((*pinf).psilGetSilhouette(i));
					}
				}
			}


			std::list<CTerrainKnowledge>::iterator ptk;
			ptk = pwvWorldView->ltkKnowledge.begin();

			int i_tri_count = 0;

			if (ptk != pwvWorldView->ltkKnowledge.end())
			{
				for (; ptk != pwvWorldView->ltkKnowledge.end(); ++ptk)
				{
					LineColour(0,255,0);

					for (int i = (*ptk).iNumSilhouettes() - 1; i >= 0; --i)
					{
						//  Draw each associated silhouette.
						DrawSilhouette((*ptk).psilGetSilhouette(i));
					}

					++i_tri_count;
				}

				char buffer[32];
				sprintf(buffer, "TTris: %d\n", i_tri_count);
				DebugSay(buffer);

			}
#endif
		}




		//****************************************************************************************
		void CBrain::Draw3DInfluences(CDraw& draw, CCamera& cam)
		{
			// Get influence list.
			CInfluenceList *pinfl = inflGetInfluences();

			// Draw each influence.
			CInfluenceList::iterator pinf = pinfl->begin();

			for ( ; pinf != pinfl->end(); ++pinf)
			{
				// Get placement and box size (world scale).
				CPresence3<> pr3 = (*pinf).pinsTarget->pr3Presence();

				// Color of silhouette depends on soldity.
				if ((*pinf).setNodeFlags[ensfBLOCKS_PATH])
				{
					draw.Colour(CColour(255,0,0));
				}
				else if (!(*pinf).setNodeFlags[ensfIS_PASSABLE])
				{
					draw.Colour(CColour(255,0,255));
				}
				else
					draw.Colour(CColour(128,128,128));

				
				// But wait!  What if we have a compound guy?
				if ((*pinf).pinsTarget->pphiGetPhysicsInfo() && (*pinf).pinsTarget->pphiGetPhysicsInfo()->pphicCast())
				{
					// We should draw each box individually.
					const CPhysicsInfoCompound* pphic = (*pinf).pinsTarget->pphiGetPhysicsInfo()->pphicCast();

					for (int i = pphic->iNumSubmodels - 1; i >= 0; --i)
					{
						CPresence3<> pr3_submodel = pphic->apr3SubmodelPresences[i] * pr3;

						const CBoundVol* pbv = pphic->apphiSubmodels[i]->pbvGetBoundVol();
						if (!pbv->pbvbCast())
							continue;
						CBoundVolBox bvb = *(CBoundVolBox*)pbv;

						CPlacement3<> p3 = pr3_submodel.p3Placement();
						bvb *= pr3_submodel.rScale;		

						// Draw the box.
						CTransform3<> tf3_shape_screen = p3 * cam.tf3ToHomogeneousScreen();
						draw.Box3D(bvb.tf3Box(tf3_shape_screen));
					}
				}
				else
				{
					const CBoundVol*   pbv = (*pinf).pinsTarget->paiiGetAIInfo()->pbvGetBoundVol((*pinf).pinsTarget);
					if (!pbv->pbvbCast())
						continue;

					CBoundVolBox bvb = *(CBoundVolBox*)pbv;

					CPlacement3<> p3 = pr3.p3Placement();
					bvb *= pr3.rScale;		

					// Draw the box.
					CTransform3<> tf3_shape_screen = p3 * cam.tf3ToHomogeneousScreen();
					draw.Box3D(bvb.tf3Box(tf3_shape_screen));
				}
			}
		}


		//****************************************************************************************
		void CBrain::DrawGraph()
		{
#if VER_DEBUG
			if (!paigGraph)
				return;

			paigGraph->Draw();

			// Highlight the start and stop nodes of the graph.
			LineColour(0,255,0);
			paigGraph->HighlightNode(0);

			// Highlight the start and stop nodes of the graph.
			LineColour(255,0,0);
			paigGraph->HighlightNode(1);

			// Draw the path!
			paigGraph->DrawPath(gpTestPath);
#endif
		}

		//****************************************************************************************
		void CBrain::DrawDebug
		(
		)
		{
#if VER_DEBUG
			Assert(paniOwner);

			bool b_selected_destinations	= IsMenuItemChecked(ID_DEBUG_DESTINATIONS);

			if (gaiSystem.bIsSelected(paniOwner))
				DrawFeeling(pmsState->feelEmotions,
					CVector2<>(paniOwner->v3Pos()) + CVector2<>(2.0,0.0));


			// Tell the brain to draw its influences.
			if (IsMenuItemChecked(ID_DEBUG_INFLUENCES))
			{
				// Make sure that the animal is selected.
				if (gaiSystem.bIsSelected(paniOwner))
					DrawInfluences();  
			}

			// Tell the brain to draw its path graph.
			if (IsMenuItemChecked(ID_DEBUG_GRAPH))
			{
				// Make sure that the animal is selected.
				if (gaiSystem.bIsSelected(paniOwner))
					DrawGraph();  
			}

			if (b_selected_destinations)
			{
				DrawDestination();
			}
#endif
		}

		//****************************************************************************************
		void CBrain::DrawDestination
		(
		)
		{
#if VER_DEBUG
			LineColour(0,0,255);
			Line(v2GetLocation(), v2GetDestination());
#endif
		}

		
		//****************************************************************************************
		CInfluenceList* CBrain::inflGetInfluences()
		{
			return &pwvWorldView->inflInfluences;
		}

		//*********************************************************************************
		const CVector2<>	CBrain::v2GetDestination
		(
		) const
		{
			return pmsState->v2LastDirection + v2GetLocation();
		}


		//****************************************************************************************
		//
		void CBrain::DebugSay
		(
			const char*  ac		// what to say
		)
		{
			gaiSystem.DebugPrint(acName);
			gaiSystem.DebugPrint(": ");
			gaiSystem.DebugPrint(ac);
		}

		//****************************************************************************************
		//
		void CBrain::MaybeDebugSay
		(
			const char*  ac		// what to say
		)
		{
			if (gaiSystem.pinsSelected == paniOwner)
			{
				DebugSay(ac);
			}
		}

#if VER_TEST
		//*****************************************************************************************
		int CBrain::iGetDescription(char *buffer, int i_buffer_length)
		{
			sprintf(buffer, "Brain data:\n\n");

			char local_buffer[512];

			sprintf(local_buffer, "edtDinoType\t%d\n", (int)edtDinoType);
			strcat(buffer, local_buffer);

			sprintf(local_buffer, "iTeam\t%d\n", paniOwner->iTeam);
			strcat(buffer, local_buffer);

			sprintf(local_buffer, "rtBravery\t%f\n", rtBravery.fVal);
			strcat(buffer, local_buffer);

			sprintf(local_buffer, "rWidth\t%f\nrHeight\t%f\n", rWidth, rHeight);
			strcat(buffer, local_buffer);

			sprintf(local_buffer, "rHeadReach\t%f\n", rHeadReach);
			strcat(buffer, local_buffer);
			
			sprintf(local_buffer, "rTailReach\t%f\n", rTailReach);
			strcat(buffer, local_buffer);

			sprintf(local_buffer, "rJumpDistance\t%f\n", rJumpDistance);
			strcat(buffer, local_buffer);
			
			sprintf(local_buffer, "rJumpUp\t%f\nrJumpDown\t%f\n", rJumpUp, rJumpDown);
			strcat(buffer, local_buffer);
			
			sprintf(local_buffer, "rJumpOver\t%f\n", rJumpOver);
			strcat(buffer, local_buffer);

			sprintf(local_buffer, "rCrouchUnder\t%f\n", rCrouchUnder);
			strcat(buffer, local_buffer);

			sprintf(local_buffer, "rWalkOver\t%f\n", rWalkOver);
			strcat(buffer, local_buffer);

			sprintf(local_buffer, "rWalkUnder\t%f\n", rWalkUnder);
			strcat(buffer, local_buffer);

			sprintf(local_buffer, "rMoveableMass\t%f\n", rMoveableMass);
			strcat(buffer, local_buffer);
			
			sprintf(local_buffer, "rWakeDistance\t%f\n", rWakeDistance);
			strcat(buffer, local_buffer);

			sprintf(local_buffer, "rSleepDistance\t%f\n", rSleepDistance);
			strcat(buffer, local_buffer);

			return strlen(buffer);
		}
#endif
		

		//*****************************************************************************************
		char *CBrain::pcSave(char * pc) const
		{
			Assert(pmsState);
			pc = pmsState->pcSave(pc);

			Assert(pwvWorldView);
			pc = pwvWorldView->pcSave(pc);

			Assert(psynSynthesizer);
			pc = psynSynthesizer->pcSave(pc);

///////////////////////////

			pc = pcSaveT(pc, bLastPathSucceeded);
			pc = pcSaveT(pc, sLastSuccessfulPath);
			
	////  WAYPOINTS-
	//  Basically, if the dino hangs out at the same waypoint for too long, he starts making some AI decisions 
	//  to change the way he's acting.
			pc = pcSaveT(pc, v3LastWayPoint);
			pc = pcSaveT(pc, sLastWayPoint);
			pc = pcSaveT(pc, bTriumph);
			pc = pcSaveT(pc, bStartled);
			pc = pcSaveT(pc, sAllowStartle);
			pc = pcSaveT(pc, v3Startle);
			pc = pcSaveT(pc, bJumpEnabled);
			pc = pcSaveT(pc, bCanJump);

	//
	//	Data.
	//

			pc = pcSaveT(pc, sLastTime);

	//
	//	Vocalization data.
	//
			pc = pcSaveT(pc, iVocalizingActivity);
			pc = pcSaveT(pc, sSampleLength);
			pc = pcSaveT(pc, sSampleStart);
			pc = pcSaveT(pc, sOpenMouth);
			pc = pcSaveT(pc, sCloseMouth);
			
//	uint32					u4AudioID;	// Don't save me!


	
	// Pathfinding data.
//	TSec					sWhenToLearn;	// Don't save!

//			pc = pcSaveT(pc, bUseTerrainPathfinding);	// Don't save unless triggers can change this guy.
			


			// save the number of activities for future backwards compatability.
			pc = pcSaveT(pc, (int)eatEND);
			// The activities themselves.
			int i = 0;
			for (; i < eatEND; ++i)
			{
				pc = sapactActivities[i]->pcSave(pc);
			}


///////////////////////////////

			return pc;
		}

		//*****************************************************************************************
		const char *CBrain::pcLoad(const char * pc)
		{
			if (iAnimalVersion >= 8)
			{
				Assert(pmsState);
				pc = pmsState->pcLoad(pc);

				Assert(pwvWorldView);
				pc = pwvWorldView->pcLoad(pc);

				Assert(psynSynthesizer);
				pc = psynSynthesizer->pcLoad(pc);

				///////////////////////////
				//  Actual brain data.
				//

				pc = pcLoadT(pc, &bLastPathSucceeded);
				pc = pcLoadT(pc, &sLastSuccessfulPath);
				
		////  WAYPOINTS-
		//  Basically, if the dino hangs out at the same waypoint for too long, he starts making some AI decisions 
		//  to change the way he's acting.
				pc = pcLoadT(pc, &v3LastWayPoint);
				pc = pcLoadT(pc, &sLastWayPoint);
				pc = pcLoadT(pc, &bTriumph);
				pc = pcLoadT(pc, &bStartled);
				pc = pcLoadT(pc, &sAllowStartle);
				pc = pcLoadT(pc, &v3Startle);
				pc = pcLoadT(pc, &bJumpEnabled);
				pc = pcLoadT(pc, &bCanJump);

		//
		//	Data.
		//

				pc = pcLoadT(pc, &sLastTime);

		//
		//	Vocalization data.
		//
				pc = pcLoadT(pc, &iVocalizingActivity);
				pc = pcLoadT(pc, &sSampleLength);
				pc = pcLoadT(pc, &sSampleStart);
				pc = pcLoadT(pc, &sOpenMouth);
				pc = pcLoadT(pc, &sCloseMouth);
				
	//	uint32					u4AudioID;	// Don't save me!


		
		// Pathfinding data.
	//	TSec					sWhenToLearn;	// Don't save!

	//			pc = pcLoadT(pc, &bUseTerrainPathfinding);	// Don't save unless triggers can change this guy.
				


				// save the number of activities for future backwards compatability.
				int i_num_activities = 0;
				pc = pcLoadT(pc, &i_num_activities);
				// The activities themselves.
				int i = 0;
				for (; i < i_num_activities; ++i)
				{
					if (i < eatEND)
						pc = sapactActivities[i]->pcLoad(pc);
					else
						pc = sapactActivities[eatEND - 1]->pcLoad(pc);
				}

				return pc;
			}
			else
			{
				// Do it the old way.
				Assert(pmsState);
				pc = pmsState->pcLoad(pc);

				Assert(pwvWorldView);
				pc = pwvWorldView->pcLoad(pc);

				// We don't save any graph data.
	//`			pc = paigGraph->pcLoad(pc);

				Assert(psynSynthesizer);
				pc = psynSynthesizer->pcLoad(pc);

		//
		//	Data.
		//
				float f_dummy;

				// Ditch the physics request.  We don't it, really, and its pointers will be bad.
				if (iAnimalVersion < 5)
					pc += 244;	// Skip the old request message size.

				// We don't actually need any of these, so let's ignore them.
				uint32 u4_dummy;

				pc = pcLoadT(pc, &sLastTime);
				pc = pcLoadT(pc, &sElapsedTime);

	//			pc = pcLoadT(pc, &earArchetype);
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rWidth);
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rHeight);	
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rHeadReach);
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rTailReach);
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rClawReach);
				pc = pcLoadT(pc, &u4_dummy);


	//			pc = pcLoadT(pc, &rJumpDistanceSqr);
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rJumpDistance);
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rJumpUp);
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rJumpDown);
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rWalkOver);		// Can step over things this small
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rWalkUnder);		// Can walk under things this high
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rJumpOver);		// Can jump things this small
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &rCrouchUnder);		// Can crouch under things this high
				pc = pcLoadT(pc, &u4_dummy);

				// Don't load moveable mass.
				//pc = pcLoadT(pc, &rMoveableMass);	// Can move things this light
				pc = pcLoadT(pc, &f_dummy);	// Can move things this light

				pc = pcLoadT(pc, &edtDinoType);		// What kind of dino am I, physically?


		//
		//	Vocalization data.
		//
				pc = pcLoadT(pc, &iVocalizingActivity);
				pc = pcLoadT(pc, &sSampleLength);
				pc = pcLoadT(pc, &sSampleStart);
				pc = pcLoadT(pc, &u4AudioID);
				u4AudioID = 0;	// For safety, do not restore the handle!  It has changed.

		//
		//  Together, these variables determine the dino's pathfinding cleverness.
		//

	//			pc = pcLoadT(pc, &rNodesPerSec);
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &iMaxNodes);
				pc = pcLoadT(pc, &u4_dummy);

	//			pc = pcLoadT(pc, &sWhenToLearn);
				pc = pcLoadT(pc, &u4_dummy);
						
	//			pc = pcLoadT(pc, &sTimeToForget);
				pc = pcLoadT(pc, &u4_dummy);


				// Don't load sensory ranges!
				pc = pcLoadT(pc, &f_dummy);	
				pc = pcLoadT(pc, &f_dummy);	
				//pc = pcLoadT(pc, &rObjectSensoryRange);
				//pc = pcLoadT(pc, &rTerrainSensoryRange);

	//			pc = pcLoadT(pc, &rMaxPathLength);
				pc = pcLoadT(pc, &f_dummy);	
				pc = pcLoadT(pc, &sForgetInfluence);

				pc = pcLoadT(pc, &rWakeDistance);	// How far away to wake the dino up?
				pc = pcLoadT(pc, &rSleepDistance);	// How far away to put the dino sleep?

				pc = pcLoadT(pc, &bLastPathSucceeded);
				pc = pcLoadT(pc, &sLastSuccessfulPath);


				// save the number of activities for future backwards compatability.
				int i_num_saved_activities = -1;
				pc = pcLoadT(pc, &i_num_saved_activities);

	// 			Assert(i_num_saved_activities <= eatEND);

				// The activities themselves.


				int i = 0;
				// Danger!  We are assuming that any added acivities are at the end of the list.
				for (; i < i_num_saved_activities; ++i)
				{
					if (i < eatEND)
						pc = sapactActivities[i]->pcLoad(pc);
					else
						pc = sapactActivities[eatEND - 1]->pcLoad(pc);
				}

				// Save some room.
				pc += 128;

				return pc;
			}
		}

